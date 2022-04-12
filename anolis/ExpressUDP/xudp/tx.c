/*
 * Copyright (c) 2021 Alibaba Group Holding Limited
 * Express UDP is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 *
 */

#include <sys/shm.h>
#include "xudp_types.h"

#include "packet.h"
#include "ifapi.h"
#include "neigh.h"
#include "route.h"
#include "xsk.h"
#include "kern.h"

static unsigned char noarp_mac[6] = {0xee, 0xff, 0xff, 0xff, 0xff, 0xff};
static unsigned int frame_size;
static u64 frame_mask;

/*
 * TX frame
 *
 * | tx_frame_info | packet header | payload |
 *                 |<----      frame     --->|
 * */
#define payload_size() MIN(MTU, (frame_size - sizeof(struct tx_frame_info) - XUDP_TX_HEAD - XUDP_TX_TAIL))

struct tx_frame_info {
	u32 inuse;
	u32 sent;
	struct txch *txch;
	struct tx_frame_info *next;
};

#define XUDP_TX_TAIL 0

#define frame_info(p) (struct tx_frame_info*)((u64)p & frame_mask)
#define payload_to_packet(p) (p - XUDP_TX_HEAD)
#define frame_to_payload(p) (p + XUDP_TX_HEAD)

static inline void *xq_get_data(struct txch *txch, u64 addr)
{
	return &txch->umem->frames[addr];
}

static inline void xudp_tx_frame_append(struct txch *txch,
					struct tx_frame_info *info)
{
	info->next = txch->frame;

	txch->frame = info;
	txch->frame_queue += 1;
}

static char *xudp_tx_frame_pop(struct txch *txch)
{
	struct tx_frame_info *info;

	if (!txch->frame)
		return NULL;

	info = txch->frame;

	txch->frame = info->next;

	txch->frame_queue -= 1;
	return (char *)(info + 1);
}

int xsk_cq_cache_init(xudp *x, struct txch *txch,
		      struct xdp_umem *umem)
{
	struct tx_frame_info *info;
	u32 index, tx_proc_n;
	int i, num, left;
	char *frame;
	u64 addr;

	tx_proc_n = umem->tx_proc_n;

	num = umem->cq.ring.size / tx_proc_n;
	left = umem->cq.ring.size % tx_proc_n;

	if (0 == txch->xsk.gid)
		num += left;

	for (i = 0; i < num; ++i)
	{
		if (umem->cq_used >= umem->cq.ring.size)
			break;

		index = umem->fq.ring.size + umem->cq_used++;
		addr = index * x->conf.frame_size;

		/*  add offset for save tx_frame_info */
		addr += sizeof(struct tx_frame_info);

		frame = xq_get_data(txch, addr);
		info = frame_info(frame);
		memset(info, 0, sizeof(*info));
		xudp_tx_frame_append(txch, info);
	}

	return 0;
}

void xudp_txch_put(xudp_channel *ch)
{
	free(ch);
}

xudp_channel *xudp_txch_get(xudp *x, int gid)
{
	struct xdpsock *xsk;
	struct txch *txch;

	if (gid > x->conf.group_num)
		return NULL;

	txch = x->tx_xsk + gid;

	xsk = malloc(sizeof(*xsk));
	if (!xsk)
		return NULL;

	memset(xsk, 0, sizeof(*xsk));

	xsk->tx_xsk = &txch->xsk;
	xsk->istx = true;
	xsk->x = x;

	return xsk;
}

static inline int xudp_tx_frame_get_from_cq(struct txch *txch)
{
#define CQ_BATCH_MAX 16
	struct tx_frame_info *info;
	u64 addr[CQ_BATCH_MAX];
	char *frame;
	int num, i;

	/* Limit the number of frames that each tx can independently
	 * hold cannot exceed cq_cache_max, so that no cq event will not
	 * appear.*/
	num = txch->umem->cq_cache_max - txch->frame_alloc - txch->frame_queue;
	if (num <= 0)
		return -1;

	num = MIN(CQ_BATCH_MAX, num);
	num = cq_deq(&txch->umem->cq, addr, num);
	if (!num) {
		++txch->xsk.stats.no_cq;
		return -1;
	}

	txch->frame_sent -= num;
	for (i = 0; i < num; ++i) {
		frame = (char *)xq_get_data(txch, addr[i]);
		info = frame_info(frame);
		info->sent = false;
		xudp_tx_frame_append(txch, info);
	}

	return 0;
}

static inline char *xudp_tx_frame_get(struct txch *txch)
{
	struct tx_frame_info *info;
	char *frame;

try:
	frame = xudp_tx_frame_pop(txch);
	if (!frame) {
		if (xudp_tx_frame_get_from_cq(txch))
			goto err;

		goto try;
	}

	info = frame_info(frame);
	if (info->inuse == XUDP_TX_FRAME_INUSE)
		goto try;

err:
	return frame;
}

static inline void umem_cq_save(struct txch *txch, char *frame)
{
	struct tx_frame_info *info;

	info = frame_info(frame);
	xudp_tx_frame_append(txch, info);
}

#define MSG_XSK_NO_BATCH_LIMIT 0x200
#define MSG_XSK_DEV_QUEUE 0x400
#define XSK_FLAGS (MSG_DONTWAIT | MSG_XSK_NO_BATCH_LIMIT | MSG_XSK_DEV_QUEUE)

static int __kick(struct txch *txch, int flag)
{
	struct xdpsock *xsk = &txch->xsk;
	int ret = 0;

	ret = sendto(xsk->sfd, NULL, 0, flag, NULL, 0);

    	if (ret < 0) {
		goto err;
	}

        txch->need_commit = 0;
	++xsk->stats.send_success;
        return 0;

err:
	// kick will called by send api, errno == EAGAIN may got by user

	switch(errno) {
	case EAGAIN:
		errno = EIO;
		++xsk->stats.send_again;
		break;

	case EBUSY:
		++xsk->stats.send_ebusy;
		break;

	default:
		++xsk->stats.send_err;
		break;
	}

	return -XUDP_ERR_COMMIT_AGAIN;
}

static int kick_xsk(struct txch *txch)
{
	int ret = 0;

    	if (!txch->need_commit)
		goto end;

	ret = __kick(txch, XSK_FLAGS);
end:
	return ret;
}

static inline int xsk_check_need_commit(struct txch *txch)
{
	/* zero copy mode need start dev to consume the tx before,
	 * not wait no cq or no tx, then process may goto sleep */


	/* we should call kick when tx has TX_BATCH_SIZE num,
	 * because too many inside tx then will cause the dev busy
	 * */

	if (txch->need_commit >= txch->xsk.x->tx_batch_num)
		return kick_xsk(txch);

	return 0;
}

static struct route_rule *xudp_route_rule_check(struct route_rule *rule, int ifid, int *err)
{
check:
	if (!rule->data) {
		*err = -XUDP_ERR_ROUTE_INVALID;
		goto invalid_rule;
	}

	if (!rule->ifid) {
		*err = -XUDP_ERR_ROUTE_NOTFOUND;
		goto invalid_rule;
	}

	if (rule->ifid != ifid) {
		*err = -XUDP_ERR_ROUTE_NIC_NOMACTH;
		goto invalid_rule;
	}

	return rule;

invalid_rule:
	if (rule->next) {
		rule= rule->next;
		goto check;
	}

	return NULL;
}

static int xudp_get_route_info(xudp *x, struct xudp_nic *n,
			      struct packet_info *info,
			      struct sockaddr_in *to, int flags)
{
	struct route_rule *rt;
	__be32 next_hop;
	int err = 0, ifid;

	if (n->ni->master)
		ifid = n->ni->master->ifindex;
	else
		ifid = n->ni->ifindex;

	info->smac = n->ni->mac;
	info->to   = to;
	rt = route_lookup(x->route, ntohl(to->sin_addr.s_addr));
	rt = xudp_route_rule_check(rt, ifid, &err);
	if (!rt)
		return err;

	info->from = rt->data; // this include the port

	if (x->noarp) {
		info->dmac = noarp_mac;
		return 0;
	}

	if (rt->next_hop)
		next_hop = rt->next_hop;
	else
		next_hop = to->sin_addr.s_addr;

	info->dmac = neigh_get(next_hop, x->log);
	if (!info->dmac)
		return -XUDP_ERR_ARP_NOTFOUND;

	return 0;
}

static int xudp_send_tx(struct txch *txch, struct xdpsock *xsk,
			char *packet, unsigned int size)
{
	struct tx_frame_info *info;
	struct xdp_desc _desc, *desc;
	int ret = 0;

	info = frame_info(packet);
	info->sent = true;

	desc = &_desc;

	desc->addr = packet - txch->umem->frames;
	desc->len = size;
	desc->options = 0;

	ret = xq_enq(&xsk->ring, desc, 1);
	if (!ret)
		goto success;

	++xsk->stats.no_tx;

    	kick_xsk(txch);

	/* zc mode:
	 * 1. maybe the nospace set fail, because when entry kern, tx is not
	 *    full
	 * so check the tx full again
	 *
	 * */
	ret = xq_enq(&xsk->ring, desc, 1);
	if (!ret)
		goto success;

        errno = EAGAIN;
	ret = -XUDP_ERR_TX_NOSPACE;
	info->sent = false;
	goto err;

success:
	++txch->xsk.stats.tx_npkts;
    	txch->need_commit += 1;

err:
	return ret;
}

static int xudp_xsk_send_one(struct txch *txch, struct packet_info *info,
			     struct xudp_group *g,
			     char *buf, u32 size, int flags)
{
	char *frame;
	int ret;
	u32 l;

	frame = xudp_tx_frame_get(txch);
	if (!frame) {
        	errno = EAGAIN;
		return -XUDP_ERR_CQ_NOSPACE;
    	}

	l = xudp_packet_udp_payload(info, frame, buf, size, txch->xsk.x->log);

	dump_check(txch->xsk.x, g, frame, l);

	ret = xudp_send_tx(txch, &txch->xsk, frame, l);
	if (ret)
		goto err;

    	ret = xsk_check_need_commit(txch);
	if (ret)
		return ret;

	return size;

err:
	umem_cq_save(txch, frame);
	return ret;
}

static int xudp_tx_info_prepare(struct txch *txch, struct xdpsock *xsk,
				struct packet_info *info,
				struct sockaddr_in *to, int flags)
{
	struct sockaddr_in *from;
	int ret;

#define SRC_IPPORT (XUDP_FLAG_SRC_IP  | XUDP_FLAG_SRC_PORT)
#define SRC_INFO   (XUDP_FLAG_SRC_ETH | SRC_IPPORT)
#define ALL_ETH    (XUDP_FLAG_SRC_ETH | XUDP_FLAG_DST_ETH)

	if ((flags & SRC_IPPORT) == SRC_IPPORT) {
		struct xudp_addr *addr;

		addr = (struct xudp_addr *)to;

		info->from = (struct sockaddr_in *)&addr->from;
		info->to   = (struct sockaddr_in *)&addr->to;

		if ((flags & ALL_ETH) == ALL_ETH) {
			info->smac = addr->eth.h_source;
			info->dmac = addr->eth.h_dest;
			return 0;
		}

		if (flags & XUDP_FLAG_DST_ETH) {
			info->dmac = addr->eth.h_dest;
			info->smac = txch->nic->ni->mac;
			return 0;
		}

		if (xsk->x->noarp) {
			info->dmac = noarp_mac;
			if (flags & XUDP_FLAG_SRC_ETH)
				info->smac = addr->eth.h_source;
			else
				info->smac = txch->nic->ni->mac;
			return 0;
		}
	}

	ret = xudp_get_route_info(xsk->x, txch->nic, info, to, flags);
	if (ret) {
		errno = ENODEV;
		return ret;
	}

	if (flags & SRC_INFO) {
		struct xudp_addr *addr;

		addr = (struct xudp_addr *)to;
		from = (struct sockaddr_in *)&addr->from;

		if ((flags & SRC_IPPORT) == SRC_IPPORT) {
			info->from = from;
			return 0;
		}


		info->_from = *info->from;
		info->from = &info->_from;

		if (flags & XUDP_FLAG_SRC_IP)
			info->from->sin_addr = from->sin_addr;

		else if (flags & XUDP_FLAG_SRC_PORT)
			info->from->sin_port = from->sin_port;
	}

	return 0;
}

int xudp_send_channel(struct xdpsock *xsk, char *buf, u32 size,
		      struct sockaddr *to, int flags)
{
	struct packet_info info;
	struct xudp_group *g;
	struct txch *txch;
	int ret;

	g = xsk->group;

	xsk = xsk->tx_xsk;

	txch = (struct txch *)xsk;
	errno = 0;

	if (size > payload_size()) {
		errno = EINVAL;
		return -XUDP_ERR_PACKET_TOO_BIG;
	}

	ret = xudp_tx_info_prepare(txch, xsk, &info,
				   (struct sockaddr_in *)to, flags);
	if (ret)
		goto end;

	ret = xudp_xsk_send_one(txch, &info, g, buf, size, flags);

end:
	return ret;
}

static int xudp_frame_mix_check(struct txch *txch, char *frame)
{
	if (frame < txch->umem->frames)
		return false;

	if (frame >= txch->umem->frames + txch->umem->frames_size)
		return false;

	return true;
}

static int __xudp_frame_send(struct xudp_group *g, struct txch *txch,
			     xudp_msghdr *hdr, char *frame, int size,
			     struct packet_info *info)
{
	int ret;

	xudp_packet_udp(info, frame, size, txch->xsk.x->log);

	dump_check(txch->xsk.x, g, frame, size + XUDP_TX_HEAD);

	ret = xudp_send_tx(txch, &txch->xsk, frame, size + XUDP_TX_HEAD);
	if (ret) {
		hdr->err = ret;
		return -1;
	}

	++hdr->used;

	ret = xsk_check_need_commit(txch);
	if (ret) {
		hdr->err = ret;
		return -1;
	}

	return 0;
}

int xudp_frame_send(xudp_group *g, xudp_msghdr *hdr,
		    struct sockaddr *to, int flags)
{
	struct packet_info info;
	struct xudp_msg *msg;
	struct xdpsock *xsk;
	struct txch *txch;
	int ret, i;
	char *packet;

	xsk = xudp_group_get_tx(g);
	xsk = xsk->tx_xsk;
	txch = (struct txch *)xsk;

	hdr->err = 0;
	hdr->used = 0;

	ret = xudp_tx_info_prepare(txch, xsk, &info,
				   (struct sockaddr_in *)to, flags);
	if (ret) {
		hdr->err = ret;
		return -1;
	}

	for (i = 0; i < hdr->total; ++i) {
		msg = hdr->msg + i;

		if (msg->size > payload_size()) {
			hdr->err = -XUDP_ERR_PACKET_TOO_BIG;
			break;
		}

		packet = payload_to_packet(msg->p);

		if (xudp_frame_mix_check(txch, packet)) {
			if (__xudp_frame_send(g, txch, hdr, packet, msg->size,
						&info))
				break;
		} else {
			ret = xudp_xsk_send_one(txch, &info, g, msg->p,
						msg->size, flags);
			if (ret < 0) {
				/* -XUDP_ERR_COMMIT_AGAIN mean that
				 * packet has been sent */
				if (ret == -XUDP_ERR_COMMIT_AGAIN)
					++hdr->used;

				hdr->err = ret;
				break;
			}
			++hdr->used;
		}
	}

	ret = hdr->used;
	txch->frame_sent += hdr->used;
	if (ret == 0)
		ret = -1;

	return ret;
}

void xudp_frame_free(xudp_group *g, xudp_msghdr *hdr)
{
	struct tx_frame_info *info;
	struct xudp_msg *msg;
	struct xdpsock *xsk;
	struct txch *txch;
	int i;

	xsk = xudp_group_get_tx(g);
	xsk = xsk->tx_xsk;
	txch = (struct txch *)xsk;

	for (i = 0; i < hdr->total; ++i) {
		msg = hdr->msg + i;

		info = frame_info(msg->p);
		info->inuse = 0;

		txch->frame_alloc -= 1;
		if (!info->sent)
			xudp_tx_frame_append(txch, info);
	}
}

int xudp_frame_alloc(xudp_group *g, xudp_msghdr *hdr, int flags)
{
	struct tx_frame_info *info;
	struct xdpsock *xsk;
	struct txch *txch;
	char *frame;
	int n;

	xsk = xudp_group_get_tx(g);

	xsk = xsk->tx_xsk;

	txch = (struct txch *)xsk;
	hdr->err = 0;

	for (n = 0; n < hdr->total; ++n) {
		frame = xudp_tx_frame_get(txch);
		if (!frame) {
			errno = EAGAIN;
			hdr->err = -XUDP_ERR_CQ_NOSPACE;
			if (!n) {
				n = -1;
				goto err;
			}
			goto end;
		}

		info = frame_info(frame);
		info->inuse = XUDP_TX_FRAME_INUSE;
		info->txch = txch;

		hdr->msg[n].p = frame_to_payload(frame);
		hdr->msg[n].size = payload_size();
		hdr->used += 1;

	}

end:
	txch->frame_alloc += n;
err:
	return n;
}

int xudp_commit_channel(xudp_channel *ch)
{
	struct txch *txch;

	ch = ch->tx_xsk;

	txch = (struct txch *)ch;

    	kick_xsk(txch);

	logdebug(ch->x->log, "commit channel. complete: %s\n",
		 txch->need_commit ? "no" : "yes");

    	if (txch->need_commit) {
        	errno = EAGAIN;
        	return -XUDP_ERR_COMMIT_AGAIN;
    	}
    	errno = EXIT_SUCCESS;
	return 0;
}

void xudp_tx_set_frame_size(int size)
{
	frame_size = size;
	frame_mask = size - 1;
	frame_mask = ~frame_mask;
}

