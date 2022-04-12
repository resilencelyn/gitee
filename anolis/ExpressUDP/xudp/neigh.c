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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <linux/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

#include "ifapi.h"
#include "neigh.h"
#include "xudp_types.h"

/* this just for test, aliyun ecs not can set config.noarp. */

struct ipmac {
	__be32 addr;
	unsigned char  mac[6];
};

static struct ipmac cache[10];
static int cache_n;
static pthread_spinlock_t lock;

int ping(const char *ip);

int neigh_init()
{
	return pthread_spin_init(&lock, PTHREAD_PROCESS_SHARED);
}

unsigned char *neigh_get(__be32 addr, struct log *log)
{
	int i;
	struct ipmac *p;
	char ip[20] = {};
	unsigned char mac[6];

	struct in_addr a;

	for (i = 0; i < cache_n; ++i) {
		p = cache + i;

		if (p->addr == addr)
			return p->mac;
	}

	if (cache_n >= sizeof(cache)/sizeof(cache[0])) {
		logdebug(log, "tmp: arp limit %d\n", cache_n);
		return NULL;
	}

	pthread_spin_lock(&lock);

	for (i = 0; i < cache_n; ++i) {
		p = cache + i;

		if (p->addr == addr) {
			pthread_spin_unlock(&lock);
			return p->mac;
		}
	}

	a.s_addr = addr;

	strcpy(ip, inet_ntoa(a));

	logdebug(log, "neigh: get arp. ping sent. for %s\n", ip);
	ping(ip);

	if (nl_neigh(addr, mac, log)) {
		pthread_spin_unlock(&lock);
		logdebug(log, "neigh: get arp fail\n");
		return NULL;
	}


	p = &cache[cache_n];
	p->addr = addr;
	memcpy(p->mac, mac, 6);
	++cache_n;

	pthread_spin_unlock(&lock);

	logdebug(log, "neigh: arp update for %s %x:%x:%x:%x:%x:%x\n",
	       ip,
	       mac[0],
	       mac[1],
	       mac[2],
	       mac[3],
	       mac[4],
	       mac[5]
	       );


	return p->mac;
}
