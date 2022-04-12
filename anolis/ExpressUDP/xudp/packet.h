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

#ifndef  __PACKET_H__
#define __PACKET_H__
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/udp.h>

#include <arpa/inet.h>
#include "log.h"
typedef __u64 u64;
typedef __u32 u32;
typedef __u16 u16;

struct packet_info {
	unsigned char *dmac;
	unsigned char *smac;
	struct sockaddr_in *to;
	struct sockaddr_in *from;
	struct sockaddr_in _from;
};

void *xudp_packet_udp(struct packet_info *info, char *buff, u32 size, struct log *log);
int xudp_packet_udp_payload(struct packet_info *info, char *buff, char *payload, u32 size,
			    struct log *log);

#define XUDP_TX_HEAD (sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(struct udphdr))
#define XUDP_TX_FRAME_INUSE 0xAFCD

#endif


