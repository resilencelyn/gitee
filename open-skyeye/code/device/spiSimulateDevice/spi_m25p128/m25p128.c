#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "m25p128.h"
#include "skyeye_sched.h"

static sector_t *get_section_by_addr(m25p128_t *m25p128, uint32_t addr)
{
	uint32_t sec_index;

	if (addr > 0xffffff) return NULL;
	sec_index = addr / 0x040000;
	return &m25p128->sectors[sec_index];
}
static page_t *get_page_by_addr(m25p128_t *m25p128, uint32_t addr)
{
	sector_t *sec;
	uint32_t page_index;

	if (addr > 0xffffff) return NULL;
	sec = get_section_by_addr(m25p128, addr);
	page_index = (addr - sec->start) / 0x100;

	return &sec->pages[page_index];
}
static char get_byte_by_addr(m25p128_t *m25p128, uint32_t addr)
{
	uint32_t byte_index;

	if (addr > 0xffffff) return 0xff;
	m25p128->page = get_page_by_addr(m25p128, addr);
	byte_index = addr - m25p128->page->start;

	return m25p128->page->bytes[byte_index];
}
/* --------------------------------------------------------------------------- */

static void real_read_action(void *m25p128)
{
	char val;
	m25p128_t *m25 = (m25p128_t *)m25p128;

	int addr = m25->data.address;
	uint32_t byte_index;

	m25->page = get_page_by_addr(m25, addr);
	byte_index = addr - m25->page->start;

	m25->data.spi_bus_receive(m25->data.spi_bus, &m25->page->bytes[byte_index], m25->data.length);
#if 0
	if (m25->is_poll_mode && sem_trywait(&m25->sem) == -1) return;

	if (m25->is_poll_mode == 0 && m25->data.index < 4) {
	//if (m25->data.index < 4) {
		m25->data.index++;
		m25->data.spi_bus_receive(m25->data.spi_bus, NULL, 0);
		return;
	}
	//if (m25->data.address > 0xffffff) {
	if (m25->is_poll_mode == 0 && m25->data.address > 0xffffff) {
		m25->data.spi_bus_receive(m25->data.spi_bus, NULL, 0);

		printf("%s: %s Error memory address\n", __FILE__, __FUNCTION__);
		return;
	}

	val = get_byte_by_addr(m25p128, m25->data.address);
	//printf("VAL: 0x%x\taddress:%02x\n", (unsigned char)val, m25->data.address);
	m25->data.spi_bus_receive(m25->data.spi_bus, &val, 1);

	m25->data.address++;
	m25->data.address %= 0xffffff;
#endif
}
static void real_se_action(void *m25p128)
{
	m25p128_t *m25 = (m25p128_t *)m25p128;
	m25->data.spi_bus_receive(m25->data.spi_bus, NULL, 0);
}
static void real_rdsr_action(void *m25p128)
{
	m25p128_t *m25 = (m25p128_t *)m25p128;

	m25->data.spi_bus_receive(m25->data.spi_bus, &m25->status_reg, 1);
}
static void real_pp_action(void *m25p128)
{
	m25p128_t *m25 = (m25p128_t *)m25p128;

	m25->data.spi_bus_receive(m25->data.spi_bus, &m25->status_reg, 0);
}
static void real_wrdi_action(void *m25p128)
{
	m25p128_t *m25 = (m25p128_t *)m25p128;
	m25->data.spi_bus_receive(m25->data.spi_bus, NULL, 0);
}
static void real_rdid_action(void *m25p128)
{
	m25p128_t *m25 = (m25p128_t *)m25p128;
	char jedec[] = {0x20, 0x20, 0x18};

	if (m25->data.index == 0) {
		m25->data.spi_bus_receive(m25->data.spi_bus, NULL, 0);
		m25->data.index = 1;
		return;
	}

	if (m25->data.index == 1) {
		m25->data.spi_bus_receive(m25->data.spi_bus, &jedec[0], 1);
		m25->data.index = 2;
		return;
	}

	if (m25->data.index == 2) {
		m25->data.spi_bus_receive(m25->data.spi_bus, &jedec[1], 1);
		m25->data.index = 3;
		return;
	}

	if (m25->data.index == 3) {
		m25->data.spi_bus_receive(m25->data.spi_bus, &jedec[2], 1);
		m25->data.index = 4;
		return;
	}
	m25->data.spi_bus_receive(m25->data.spi_bus, NULL, 0);
}
static void real_be_action(void *m25p128)
{
	m25p128_t *m25 = (m25p128_t *)m25p128;
	m25->data.spi_bus_receive(m25->data.spi_bus, NULL, 0);
}
static void real_wrsr_action(void *m25p128)
{
	m25p128_t *m25 = (m25p128_t *)m25p128;

	if (m25->data.index == 0) {
		m25->data.spi_bus_receive(m25->data.spi_bus, NULL, 0);
		m25->data.index = 1;
		return;
	}

	if (m25->data.index == 1) {
		m25->data.spi_bus_receive(m25->data.spi_bus, &m25->status_reg, 1);
		m25->data.index = 1;
		return;
	}
	m25->data.spi_bus_receive(m25->data.spi_bus, NULL, 0);
}
/* --------------------------------------------------------------------------- */
static int wren_action(void *m25p128,
		       spi_bus_receive_t spi_bus_receive,
		       void *spi_bus)
{
	m25p128_t *m25 = (m25p128_t *)m25p128;
	SET_BIT(m25->status_reg, WEL);

	spi_bus_receive(spi_bus, NULL, 0);
	return 0;
}
static int rdsr_action(void *m25p128,
		       spi_bus_receive_t spi_bus_receive,
		       void *spi_bus)
{
	m25p128_t *m25 = (m25p128_t *)m25p128;

	m25->data.spi_bus = spi_bus;
	m25->data.spi_bus_receive = spi_bus_receive;

	if (m25->id != -1) {
		del_thread_scheduler(m25->id);
		m25->id = -1;
		m25->data.index = 0;
	}
	create_thread_scheduler(2000, Periodic_sched, real_rdsr_action, (void*)m25, &m25->id);
	return 0;
}
static int read_action(void *m25p128,
		       uint32_t address,
		       spi_bus_receive_t spi_bus_receive,
		       void *spi_bus)
{
	m25p128_t *m25 = (m25p128_t *)m25p128;

	m25->data.spi_bus = spi_bus;
	m25->data.spi_bus_receive = spi_bus_receive;
	m25->data.address = address;

	real_read_action(m25);
#if 0
	if (m25->id != -1) {
		del_thread_scheduler(m25->id);
		m25->id = -1;
		m25->data.index = 0;
	}
	create_thread_scheduler(2000, Periodic_sched, real_read_action, (void*)m25, &m25->id);
#endif	
}
static int pp_action(void *m25p128,
	      uint32_t address,
	      char *buf,
	      int len,
	      spi_bus_receive_t spi_bus_receive,
	      void *spi_bus)
{
	m25p128_t *m25 = (m25p128_t *)m25p128;
	int begin, size;

	if (address > 0xffffff) {
		if (m25->id != -1) {
			del_thread_scheduler(m25->id);
			m25->id = -1;
			m25->data.index = 0;
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

	m25->page = get_page_by_addr(m25p128, address & ~0xffu);

	begin = len > MAXBYTES ? len - MAXBYTES : 0;
	memcpy(&m25->page->bytes[4], &buf[begin], len - begin);
	m25->page->num = len - begin;

	CLEAR_BIT(m25->status_reg, WEL);

	m25->data.spi_bus = spi_bus;
	m25->data.spi_bus_receive = spi_bus_receive;
	m25->data.address = address;

	m25->data.spi_bus_receive(m25->data.spi_bus, m25->page->bytes, m25->page->num);
#if 0	
	if (m25->id != -1) {
		del_thread_scheduler(m25->id);
		m25->id = -1;
		m25->data.index = 0;
	}
	create_thread_scheduler(2000, Periodic_sched, real_pp_action, (void*)m25, &m25->id);
#endif	
	return 0;
}
static int se_action(void *m25p128,
	      uint32_t address,
	      spi_bus_receive_t spi_bus_receive,
	      void *spi_bus)
{
	sector_t *sec;
	m25p128_t *m25 = (m25p128_t *)m25p128;
	int i;

	if (address > 0xffffff) {
		if (m25->id != -1) {
			del_thread_scheduler(m25->id);
			m25->id = -1;
			m25->data.index = 0;
		}
		spi_bus_receive(spi_bus, NULL, 0);

		printf("%s: %s Error memory address\n", __FILE__, __FUNCTION__);
		return -1;
	}

	sec = get_section_by_addr(m25p128, address);
	for (i = 0; i < MAXPAGES; i++) {
		memset(sec->pages[i].bytes, 0xff, MAXBYTES);
	}

	CLEAR_BIT(m25->status_reg, WEL);

	m25->data.spi_bus = spi_bus;
	m25->data.spi_bus_receive = spi_bus_receive;
	m25->data.address = address;

	if (m25->id != -1) {
		del_thread_scheduler(m25->id);
		m25->id = -1;
		m25->data.index = 0;
	}
	create_thread_scheduler(2000, Periodic_sched, real_se_action, (void*)m25, &m25->id);

	return 0;
}
static int wrdi_action(void *m25p128,
		       spi_bus_receive_t spi_bus_receive,
		       void *spi_bus)
{
	m25p128_t *m25 = (m25p128_t *)m25p128;

	CLEAR_BIT(m25->status_reg, WEL);

	m25->data.spi_bus = spi_bus;
	m25->data.spi_bus_receive = spi_bus_receive;

	if (m25->id != -1) {
		del_thread_scheduler(m25->id);
		m25->id = -1;
		m25->data.index = 0;
	}
	create_thread_scheduler(2000, Periodic_sched, real_wrdi_action, (void*)m25, &m25->id);

	return 0;
}
static int rdid_action(void *m25p128,
		       spi_bus_receive_t spi_bus_receive,
		       void *spi_bus)
{
	m25p128_t *m25 = (m25p128_t *)m25p128;

	m25->data.spi_bus = spi_bus;
	m25->data.spi_bus_receive = spi_bus_receive;

	if (m25->id != -1) {
		del_thread_scheduler(m25->id);
		m25->id = -1;
		m25->data.index = 0;
	}
	create_thread_scheduler(2000, Periodic_sched, real_rdid_action, (void*)m25, &m25->id);
	return 0;
}
static int be_action(void *m25p128,
		     spi_bus_receive_t spi_bus_receive,
		     void *spi_bus)
{
	int i, j;
	m25p128_t *pm25 = (m25p128_t *)m25p128;
	sector_t *sec;

	for (i = 0; i < MAXSECTORS; i++) {
		for (j = 0, sec = &pm25->sectors[i]; j < MAXPAGES; j++) {
			memset(sec->pages[j].bytes, 0xff, MAXBYTES);
		}
	}

	CLEAR_BIT(pm25->status_reg, WEL);

	pm25->data.spi_bus = spi_bus;
	pm25->data.spi_bus_receive = spi_bus_receive;

	if (pm25->id != -1) {
		del_thread_scheduler(pm25->id);
		pm25->id = -1;
		pm25->data.index = 0;
	}
	create_thread_scheduler(2000, Periodic_sched, real_be_action, (void*)pm25, &pm25->id);

	return 0;
}
static int wrsr_action(void *m25p128,
		       unsigned char val,
		       spi_bus_receive_t spi_bus_receive,
		       void *spi_bus)
{
	m25p128_t *m25 = (m25p128_t *)m25p128;

	m25->status_reg &= ~(0x7 << 2 | 0x1 << 7);
	m25->status_reg |= val & (0x7 << 2 | 0x1 << 7);

	CLEAR_BIT(m25->status_reg, WEL);

	m25->data.spi_bus = spi_bus;
	m25->data.spi_bus_receive = spi_bus_receive;

	if (m25->id != -1) {
		del_thread_scheduler(m25->id);
		m25->id = -1;
		m25->data.index = 0;
	}
	create_thread_scheduler(2000, Periodic_sched, real_wrsr_action, (void*)m25, &m25->id);

	return 0;
}
static int fast_read_action(void *m25p128,
			    char dummy_byte,
			    uint32_t address,
			    spi_bus_receive_t spi_bus_receive,
			    void *spi_bus)
{
	m25p128_t *m25 = (m25p128_t *)m25p128;

	m25->data.spi_bus = spi_bus;
	m25->data.spi_bus_receive = spi_bus_receive;
	m25->data.address = address;

//	real_read_action(m25);

	if (m25->id != -1) {
		del_thread_scheduler(m25->id);
		m25->id = -1;
		m25->data.index = 0;
	}
	create_thread_scheduler(2000, Periodic_sched, real_read_action, (void*)m25, &m25->id);

	return 0;
}
/* --------------------------------------------------------------------------- */
static int set_cs_line(void *m25p128, int cs)
{
	m25p128_t *m25 = (m25p128_t *)m25p128;
	m25->cs = cs;
	int sval = -1;

	while (sval != 0) {
		sem_getvalue(&m25->sem, &sval);
	}
	if (m25->cs == 0 && m25->id != -1) {
		del_thread_scheduler(m25->id);
		m25->id = -1;
		m25->data.index = 0;
	}
}
static int get_cs_line(void *m25p128)
{
	m25p128_t *m25 = (m25p128_t *)m25p128;
	return m25->cs;
}

m25p128_t *init_m25p128(void)
{
	m25p128_t *m25p128;
	sector_t *sec;
	int i, j;

	m25p128 = malloc(sizeof(m25p128_t));
	if (m25p128 == NULL) {
		printf("%s:%d:malloc faild\n", __FILE__, __LINE__);
		return NULL;
	}
	m25p128->sectors = malloc(sizeof(sector_t) * MAXSECTORS);
	if (m25p128->sectors == NULL) {
		printf("%s:%d:malloc faild\n", __FILE__, __LINE__);
		return NULL;
	}

	for (i = 0; i < MAXSECTORS; i++) {
		m25p128->sectors[i].num = i;
		/* calculate section address */
		m25p128->sectors[i].start = 0x040000 * i;
		m25p128->sectors[i].end = 0x040000 * i + 0x3ffff;
		m25p128->sectors[i].pages = malloc(sizeof(page_t) * MAXPAGES);
		if (m25p128->sectors[i].pages == NULL) {
			printf("%s:%d:malloc faild\n", __FILE__, __LINE__);
			return NULL;
		}

		for (j = 0, sec = &m25p128->sectors[i]; j < MAXPAGES; j++) {
			sec->pages[j].psector = sec;
			/* every page is 256 bytes, calculate page address */
			sec->pages[j].start = sec->start + MAXBYTES * j;
			sec->pages[j].end = sec->start + MAXBYTES * j + 0xff;
			sec->pages[j].num = i * MAXPAGES + j;
		}
	}

	m25p128->wren_action = wren_action;
	m25p128->wrdi_action = wrdi_action;
	m25p128->rdid_action = rdid_action;
	m25p128->rdsr_action = rdsr_action;
	m25p128->wrsr_action = wrsr_action;
	m25p128->read_action = read_action;
	m25p128->fast_read_action = fast_read_action;
	m25p128->pp_action = pp_action;
	m25p128->se_action = se_action;
	m25p128->be_action = be_action;
	m25p128->set_cs_line = set_cs_line;
	m25p128->get_cs_line = get_cs_line;

	m25p128->cs = 1;
	m25p128->id = -1;
	m25p128->data.index = 0;

	sem_init(&m25p128->sem, 0, 0);
	m25p128->is_poll_mode = 0;

	return m25p128;
}
void free_m25p128(m25p128_t *m25p128)
{
	int i, j;
	sector_t *sec;

	for (i = 0; i < MAXSECTORS; i++) {
		sec = &m25p128->sectors[i];
		free(sec->pages);
	}
	free(m25p128->sectors);
}
