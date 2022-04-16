#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "mx25l128.h"
#include "skyeye_sched.h"

static sector_t *get_section_by_addr(mx25l128_t *mx25l128, uint32_t addr)
{
	uint32_t sec_index;

	if (addr > 0xffffff) return NULL;
	sec_index = addr / 0x040000;
	return &mx25l128->sectors[sec_index];
}

static page_t *get_page_by_addr(mx25l128_t *mx25l128, uint32_t addr)
{
	sector_t *sec;
	uint32_t page_index;

	if (addr > 0xffffff) return NULL;
	sec = get_section_by_addr(mx25l128, addr);
	page_index = (addr - sec->start) / 0x100;

	return &sec->pages[page_index];
}

static char get_byte_by_addr(mx25l128_t *mx25l128, uint32_t addr)
{
	uint32_t byte_index;

	if (addr > 0xffffff) return 0xff;
	mx25l128->page = get_page_by_addr(mx25l128, addr);
	byte_index = addr - mx25l128->page->start;

	return mx25l128->page->bytes[byte_index];
}

static void real_read_action(void *mx25l128)
{
	char val;
	mx25l128_t *mx25 = (mx25l128_t *)mx25l128;

	int addr = mx25->data.address;
	uint32_t byte_index;

	mx25->page = get_page_by_addr(mx25, addr);
	byte_index = addr - mx25->page->start;
	mx25->data.length = PAGE_SIZE;
	int i;
	for (i = 0; i < PAGE_NUM; i++)
	{
		mx25->data.spi_bus_receive(mx25->data.spi_bus, &mx25->page->bytes[byte_index + i * 16] , mx25->data.length);
		byte_index += PAGE_SIZE;
	}

}

static void real_se_action(void *mx25l128)
{
	mx25l128_t *mx25 = (mx25l128_t *)mx25l128;
	mx25->data.spi_bus_receive(mx25->data.spi_bus, NULL, 0);
}

static void real_rdsr_action(void *mx25l128)
{
	mx25l128_t *mx25 = (mx25l128_t *)mx25l128;
	unsigned char status = mx25->status_reg & 0xff;

	mx25->data.spi_bus_receive(mx25->data.spi_bus, &status, 1);
}

static void real_rdsr1_action(void *mx25l128)
{
	mx25l128_t *mx25 = (mx25l128_t *)mx25l128;
	unsigned char status = mx25->status_reg >> 8;

	mx25->data.spi_bus_receive(mx25->data.spi_bus, &status, 1);
}

static void real_pp_action(void *mx25l128)
{
	mx25l128_t *mx25 = (mx25l128_t *)mx25l128;

	mx25->data.spi_bus_receive(mx25->data.spi_bus, &mx25->status_reg, 0);
}

static void real_wrdi_action(void *mx25l128)
{
	mx25l128_t *mx25 = (mx25l128_t *)mx25l128;
	mx25->data.spi_bus_receive(mx25->data.spi_bus, NULL, 0);
}

static void real_rdid_action(void *mx25l128)
{
	mx25l128_t *mx25 = (mx25l128_t *)mx25l128;
	unsigned char idcode[5] = {0};
	idcode[0] = 0xc2;
	idcode[1] = mx25->flash_id[0];
	idcode[2] = mx25->flash_id[1];
	idcode[3] = 0;
	idcode[4] = 0;

	if (mx25->data.index == 0) {
		mx25->data.spi_bus_receive(mx25->data.spi_bus, &idcode[0], 1);
		mx25->data.index = 1;
		return;
	}

	if (mx25->data.index == 1) {
		mx25->data.spi_bus_receive(mx25->data.spi_bus, &idcode[1], 1);
		mx25->data.index = 2;
		return;
	}

	if (mx25->data.index == 2) {
		mx25->data.spi_bus_receive(mx25->data.spi_bus, &idcode[2], 1);
		mx25->data.index = 3;
		return;
	}

	if (mx25->data.index == 3) {
		mx25->data.spi_bus_receive(mx25->data.spi_bus, &idcode[3], 1);
		mx25->data.index = 4;
		return;
	}

	if (mx25->data.index == 4)
	{
		mx25->data.spi_bus_receive(mx25->data.spi_bus, &idcode[4], 1);
		mx25->data.index = 5;
		return;
	}
}

static void real_be_action(void *mx25l128)
{
	mx25l128_t *mx25 = (mx25l128_t *)mx25l128;
	mx25->data.spi_bus_receive(mx25->data.spi_bus, NULL, 0);
}

static void real_wrsr_action(void *mx25l128)
{
	mx25l128_t *mx25 = (mx25l128_t *)mx25l128;
	char status = 0;

	if (mx25->data.index == 0) {
		mx25->data.spi_bus_receive(mx25->data.spi_bus, &status, 0);
		mx25->data.index = 1;
		return;
	}

	if (mx25->data.index == 1) {
		mx25->data.spi_bus_receive(mx25->data.spi_bus, &mx25->status_reg, 1);
		mx25->data.index = 1;
		return;
	}
	mx25->data.spi_bus_receive(mx25->data.spi_bus, &status, 0);
}

static int wren_action(void *mx25l128,
		spi_bus_receive_t spi_bus_receive,
		void *spi_bus)
{
	mx25l128_t *mx25 = (mx25l128_t *)mx25l128;
	SET_BIT(mx25->status_reg, WEL);

	spi_bus_receive(spi_bus, NULL, 0);
	return 0;
}

static int rdsr_action(void *mx25l128,
		spi_bus_receive_t spi_bus_receive,
		void *spi_bus)
{
	mx25l128_t *mx25 = (mx25l128_t *)mx25l128;

	mx25->data.spi_bus = spi_bus;
	mx25->data.spi_bus_receive = spi_bus_receive;

	real_rdsr_action(mx25);
	return 0;
}

static int rdsr1_action(void *mx25l128,
		spi_bus_receive_t spi_bus_receive,
		void *spi_bus)
{
	mx25l128_t *mx25 = (mx25l128_t *)mx25l128;

	mx25->data.spi_bus = spi_bus;
	mx25->data.spi_bus_receive = spi_bus_receive;

	real_rdsr1_action(mx25);
	return 0;
}

static int read_action(void *mx25l128,
		uint32_t address,
		spi_bus_receive_t spi_bus_receive,
		void *spi_bus)
{
	mx25l128_t *mx25 = (mx25l128_t *)mx25l128;

	mx25->data.spi_bus = spi_bus;
	mx25->data.spi_bus_receive = spi_bus_receive;
	mx25->data.address = address;

	real_read_action(mx25);
#if 0
	if (mx25->id != -1) {
		del_thread_scheduler(mx25->id);
		mx25->id = -1;
		mx25->data.index = 0;
	}
	create_thread_scheduler(2000, Periodic_sched, real_read_action, (void*)mx25, &mx25->id);
#endif	
}

static int pp_action(void *mx25l128,
		uint32_t address,
		char *buf,
		int len,
		spi_bus_receive_t spi_bus_receive,
		void *spi_bus)
{
	mx25l128_t *mx25 = (mx25l128_t *)mx25l128;
	int begin, size;

	if (address > 0xffffff) {
		if (mx25->id != -1) {
			del_thread_scheduler(mx25->id);
			mx25->id = -1;
			mx25->data.index = 0;
		}
		spi_bus_receive(spi_bus, NULL, 0);

		printf("%s: %s Error memory address\n", __FILE__, __FUNCTION__);
		return -1;
	}

	/* If the 8 least significant address bits (A7-A0) are
	 * not all zero, all transmitted data that goes beyond
	 * the end of the current page are programmed from the
	 * start address of the same.
	 * FIXME:What's meaning? Here force address align by 256 ?
	 */

	mx25->page = get_page_by_addr(mx25l128, address & ~0xffu);

	if (len < 0)
	{
		len = 0;
		begin = 0;
	} else
		begin = len > MAXBYTES ? len - MAXBYTES : 0;
	memcpy(&mx25->page->bytes[0], &buf[begin], len - begin);
	fwrite(&buf[begin], 1, len - begin, mx25->fp);
	mx25->page->num = len - begin;

	CLEAR_BIT(mx25->status_reg, WEL);

	mx25->data.spi_bus = spi_bus;
	mx25->data.spi_bus_receive = spi_bus_receive;
	mx25->data.address = address;

	mx25->data.spi_bus_receive(mx25->data.spi_bus, mx25->page->bytes, mx25->page->num);
#if 0	
	if (mx25->id != -1) {
		del_thread_scheduler(mx25->id);
		mx25->id = -1;
		mx25->data.index = 0;
	}
	create_thread_scheduler(2000, Periodic_sched, real_pp_action, (void*)mx25, &mx25->id);
#endif	
	return 0;
}

static int se_action(void *mx25l128,
		uint32_t address,
		spi_bus_receive_t spi_bus_receive,
		void *spi_bus)
{
	sector_t *sec;
	mx25l128_t *mx25 = (mx25l128_t *)mx25l128;
	int i;

	if (address > 0xffffff) {
		if (mx25->id != -1) {
			del_thread_scheduler(mx25->id);
			mx25->id = -1;
			mx25->data.index = 0;
		}
		spi_bus_receive(spi_bus, NULL, 0);

		printf("%s: %s Error memory address\n", __FILE__, __FUNCTION__);
		return -1;
	}

	sec = get_section_by_addr(mx25l128, address);
	for (i = 0; i < MAXPAGES; i++) {
		memset(sec->pages[i].bytes, 0xff, MAXBYTES);
	}

	CLEAR_BIT(mx25->status_reg, WEL);

	mx25->data.spi_bus = spi_bus;
	mx25->data.spi_bus_receive = spi_bus_receive;
	mx25->data.address = address;

	if (mx25->id != -1) {
		del_thread_scheduler(mx25->id);
		mx25->id = -1;
		mx25->data.index = 0;
	}
	create_thread_scheduler(2000, Periodic_sched, real_se_action, (void*)mx25, &mx25->id);

	return 0;
}

static int wrdi_action(void *mx25l128,
		spi_bus_receive_t spi_bus_receive,
		void *spi_bus)
{
	mx25l128_t *mx25 = (mx25l128_t *)mx25l128;

	CLEAR_BIT(mx25->status_reg, WEL);

	mx25->data.spi_bus = spi_bus;
	mx25->data.spi_bus_receive = spi_bus_receive;

	if (mx25->id != -1) {
		del_thread_scheduler(mx25->id);
		mx25->id = -1;
		mx25->data.index = 0;
	}
	create_thread_scheduler(2000, Periodic_sched, real_wrdi_action, (void*)mx25, &mx25->id);

	return 0;
}

static int rdid_action(void *mx25l128,
		spi_bus_receive_t spi_bus_receive,
		void *spi_bus)
{
	mx25l128_t *mx25 = (mx25l128_t *)mx25l128;

	mx25->data.spi_bus = spi_bus;
	mx25->data.spi_bus_receive = spi_bus_receive;

	if (mx25->id != -1) {
		del_thread_scheduler(mx25->id);
		mx25->id = -1;
		mx25->data.index = 0;
	}
	create_thread_scheduler(2000, Periodic_sched, real_rdid_action, (void*)mx25, &mx25->id);
	return 0;
}

static int be_action(void *mx25l128,
		spi_bus_receive_t spi_bus_receive,
		void *spi_bus)
{
	int i, j;
	mx25l128_t *pmx25 = (mx25l128_t *)mx25l128;
	sector_t *sec;

	for (i = 0; i < MAXSECTORS; i++) {
		for (j = 0, sec = &pmx25->sectors[i]; j < MAXPAGES; j++) {
			memset(sec->pages[j].bytes, 0xff, MAXBYTES);
		}
	}

	CLEAR_BIT(pmx25->status_reg, WEL);

	pmx25->data.spi_bus = spi_bus;
	pmx25->data.spi_bus_receive = spi_bus_receive;

	if (pmx25->id != -1) {
		del_thread_scheduler(pmx25->id);
		pmx25->id = -1;
		pmx25->data.index = 0;
	}
	create_thread_scheduler(2000, Periodic_sched, real_be_action, (void*)pmx25, &pmx25->id);

	return 0;
}

static int wrsr_action(void *mx25l128,
		unsigned char val,
		spi_bus_receive_t spi_bus_receive,
		void *spi_bus)
{
	mx25l128_t *mx25 = (mx25l128_t *)mx25l128;

	mx25->status_reg &= ~(0x7 << 2 | 0x1 << 7);
	mx25->status_reg |= val & (0x7 << 2 | 0x1 << 7);

	CLEAR_BIT(mx25->status_reg, WEL);

	mx25->data.spi_bus = spi_bus;
	mx25->data.spi_bus_receive = spi_bus_receive;

	if (mx25->id != -1) {
		del_thread_scheduler(mx25->id);
		mx25->id = -1;
		mx25->data.index = 0;
	}
	create_thread_scheduler(2000, Periodic_sched, real_wrsr_action, (void*)mx25, &mx25->id);

	return 0;
}

static int fast_read_action(void *mx25l128,
		char dummy_byte,
		uint32_t address,
		spi_bus_receive_t spi_bus_receive,
		void *spi_bus)
{
	mx25l128_t *mx25 = (mx25l128_t *)mx25l128;

	mx25->data.spi_bus = spi_bus;
	mx25->data.spi_bus_receive = spi_bus_receive;
	mx25->data.address = address;

	//	real_read_action(mx25);

	if (mx25->id != -1) {
		del_thread_scheduler(mx25->id);
		mx25->id = -1;
		mx25->data.index = 0;
	}
	create_thread_scheduler(2000, Periodic_sched, real_read_action, (void*)mx25, &mx25->id);

	return 0;
}

static int set_cs_line(void *mx25l128, int cs)
{
	mx25l128_t *mx25 = (mx25l128_t *)mx25l128;
	mx25->cs = cs;
	int sval = -1;

	while (sval != 0) {
		sem_getvalue(&mx25->sem, &sval);
	}
	if (mx25->cs == 0 && mx25->id != -1) {
		del_thread_scheduler(mx25->id);
		mx25->id = -1;
		mx25->data.index = 0;
	}
}

static int get_cs_line(void *mx25l128)
{
	mx25l128_t *mx25 = (mx25l128_t *)mx25l128;
	return mx25->cs;
}

mx25l128_t *init_mx25l128(void)
{
	mx25l128_t *mx25l128;
	sector_t *sec;
	int i, j;

	mx25l128 = malloc(sizeof(mx25l128_t));
	if (mx25l128 == NULL) {
		printf("%s:%d:malloc faild\n", __FILE__, __LINE__);
		return NULL;
	}
	mx25l128->sectors = malloc(sizeof(sector_t) * MAXSECTORS);
	if (mx25l128->sectors == NULL) {
		printf("%s:%d:malloc faild\n", __FILE__, __LINE__);
		return NULL;
	}

	for (i = 0; i < MAXSECTORS; i++) {
		mx25l128->sectors[i].num = i;
		/* calculate section address */
		mx25l128->sectors[i].start = 0x040000 * i;
		mx25l128->sectors[i].end = 0x040000 * i + 0x3ffff;
		mx25l128->sectors[i].pages = malloc(sizeof(page_t) * MAXPAGES);
		if (mx25l128->sectors[i].pages == NULL) {
			printf("%s:%d:malloc faild\n", __FILE__, __LINE__);
			return NULL;
		}

		for (j = 0, sec = &mx25l128->sectors[i]; j < MAXPAGES; j++) {
			sec->pages[j].psector = sec;
			/* every page is 256 bytes, calculate page address */
			sec->pages[j].start = sec->start + MAXBYTES * j;
			sec->pages[j].end = sec->start + MAXBYTES * j + 0xff;
			sec->pages[j].num = i * MAXPAGES + j;
		}
	}

	mx25l128->wren_action = wren_action;
	mx25l128->wrdi_action = wrdi_action;
	mx25l128->rdid_action = rdid_action;
	mx25l128->rdsr_action = rdsr_action;
	mx25l128->rdsr1_action = rdsr1_action;
	mx25l128->wrsr_action = wrsr_action;
	mx25l128->read_action = read_action;
	mx25l128->fast_read_action = fast_read_action;
	mx25l128->pp_action = pp_action;
	mx25l128->se_action = se_action;
	mx25l128->be_action = be_action;
	mx25l128->set_cs_line = set_cs_line;
	mx25l128->get_cs_line = get_cs_line;

	mx25l128->cs = 1;
	mx25l128->id = -1;
	mx25l128->data.index = 0;

	sem_init(&mx25l128->sem, 0, 0);
	mx25l128->is_poll_mode = 0;

	return mx25l128;
}

void free_mx25l128(mx25l128_t *mx25l128)
{
	int i, j;
	sector_t *sec;

	for (i = 0; i < MAXSECTORS; i++) {
		sec = &mx25l128->sectors[i];
		free(sec->pages);
	}
	free(mx25l128->sectors);
}
