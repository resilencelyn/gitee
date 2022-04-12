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

#include <string.h>

#include "packet.h"
#include "checksum.h"

#define IP_XUDP_TTL 64
#define IP_DF 0x4000
#define IP_VIT ((4 << 12) | (5 << 8) | (0 & 0xff))

typedef unsigned char u8;

//#ifdef __x86_64__
//static inline void *__movsb(void *d, const void *s, size_t n)
//{
//  	asm volatile ("rep movsb"
//                      : "=D" (d),
//                      "=S" (s),
//                      "=c" (n)
//                      : "0" (d),
//                      "1" (s),
//                      "2" (n)
//                      : "memory");
//  	return d;
//}
//#define memcpy __movsb
//#endif

static void xudp_checksum_half(struct iphdr *iph)
{
#define IP_HALF_SUM (ntohs(IP_VIT) +                           \
		     ntohs((IP_XUDP_TTL << 8) + IPPROTO_UDP) + \
		     ntohs(IP_DF));
	u32 sum;
	u16 *p;

	sum = IP_HALF_SUM;

	sum += iph->tot_len;

	p = (u16 *)&iph->saddr;

	sum += *p++;
	sum += *p++;
	sum += *p++;
	sum += *p;

	sum = (sum & 0xFFFF) + (sum >> 16);
	sum += sum >> 16;

	iph->check = (u16)~sum;
}

static void iph_build(struct iphdr *iph, u32 size, __be32 saddr, __be32 daddr)
{
	/* ip */
	// version, ihl, tos
	*((__be16 *)iph) = htons(IP_VIT);

	iph->tot_len  = htons(sizeof(*iph) + size);
	iph->id       = 0;
	iph->frag_off = htons(IP_DF);
	iph->ttl      = IP_XUDP_TTL;
	iph->protocol = IPPROTO_UDP;
	iph->saddr    = saddr;
	iph->daddr    = daddr;

	xudp_checksum_half(iph);
}

static void udp_build(struct udphdr *udp, u32 size, __be16 sport, __be16 dport)
{
	u16 udp_len;

	udp_len = sizeof(*udp) + size;

	udp->source = sport;
	udp->dest = dport;
	udp->len = htons(udp_len);

	udp->check = 0; // must

#if XUDP_UDP_CHECKSUM
    	u32 sum = 0;
    	sum = do_csum((char *)udp, udp_len);
    	udp->check = csum_tcpudp_magic(saddr, daddr, udp_len, IPPROTO_UDP, sum);
#endif
}

static void copy_eth(unsigned char *dst, unsigned char *src)
{
	u32 *d1 = (u32 *)dst;
	u32 *s1 = (u32 *)src;
	u16 *d2 = (uint16_t *)(dst + sizeof(u32));
	u16 *s2 = (uint16_t *)(src + sizeof(u32));

	*d1 = *s1;
	*d2 = *s2;
}

static void eth_build(struct ethhdr *eth,
		      unsigned char *smac, unsigned char *dmac)
{
	/* eth */
	copy_eth(eth->h_source, smac);
	copy_eth(eth->h_dest, dmac);

	eth->h_proto = htons(ETH_P_IP);
}

void *xudp_packet_udp(struct packet_info *info, char *buff, u32 size,
		      struct log *log)
{
	struct ethhdr *eth;
	struct iphdr  *iph;
    	struct udphdr *udp;
	struct sockaddr_in *src, *dst;

	src = info->from;
	dst = info->to;

	eth = (void *)buff;
	iph = (void *)(eth + 1);
	udp = (void *)(iph + 1);

	eth_build(eth, info->smac, info->dmac);
	iph_build(iph, sizeof(*udp) + size, src->sin_addr.s_addr,
		  dst->sin_addr.s_addr);
	udp_build(udp, size, src->sin_port, dst->sin_port);

	return udp + 1;
}

int xudp_packet_udp_payload(struct packet_info *info, char *buff, char *payload, u32 size,
			    struct log *log)
{
	char *p;

	p = xudp_packet_udp(info, buff, size, log);
	memcpy(p, payload, size);
	return size + XUDP_TX_HEAD;
}
