#ifndef __M25P128_H__
#define __M25P128_H__
#include <stdint.h>
#include <semaphore.h>

#define MAXSECTORS	64
#define MAXPAGES	1024
#define MAXBYTES	256

#define WREN		0x06
#define WRDI		0x04
#define RDID		0x9f
#define RDSR		0x05
#define WRSR		0x01
#define READ		0x03
#define FAST_READ	0x0b
#define PP		0x02
#define SE		0xd8
#define BE		0xc7

#define WIP		(1 << 0)
#define WEL		(1 << 1)
#define BP0		(1 << 2)
#define BP1		(1 << 3)
#define BP2		(1 << 4)
#define SRWD		(1 << 7)

#define CLEAR_BIT(reg, val) ((reg) &= ~(val))
#define SET_BIT(reg, val) ((reg) |= (val))

typedef struct page {
	char bytes[MAXBYTES];
	struct sector *psector;
	uint32_t start;
	uint32_t end;
	int num;
} page_t;

typedef struct sector {
	int num;
	uint32_t start;
	uint32_t end;
	page_t *pages;
} sector_t;

typedef void (*spi_bus_receive_t)(void *spi_bus, char *buf, int len);

typedef struct m25p128 {
	sector_t *sectors;
	/* cs: 1 is selected, 0 is unselected */
	uint32_t cs;

	int (*wren_action)(void *m25p128,
			   spi_bus_receive_t spi_bus_receive,
			   void *spi_bus);
	int (*rdsr_action)(void *m25p128,
			   spi_bus_receive_t spi_bus_receive,
			   void *spi_bus);
	int (*read_action)(void *m25p128,
			   uint32_t address,
			   spi_bus_receive_t spi_bus_receive,
			   void *spi_bus);
	int (*pp_action)(void *m25p128,
			 uint32_t address,
			 char *buf,
			 int len,
			 spi_bus_receive_t spi_bus_receive,
			 void *spi_bus);
	int (*se_action)(void *m25p128,
			 uint32_t address,
			 spi_bus_receive_t spi_bus_receive,
			 void *spi_bus);
	int (*set_cs_line)(void *m25p128, int cs);
	int (*get_cs_line)(void *m25p128);
	int (*wrdi_action)(void *m25p128,
			   spi_bus_receive_t spi_bus_receive,
			   void *spi_bus);
	int (*rdid_action)(void *m25p128,
			   spi_bus_receive_t spi_bus_receive,
			   void *spi_bus);
	int (*be_action)(void *m25p128,
			 spi_bus_receive_t spi_bus_receive,
			 void *spi_bus);
	int (*wrsr_action)(void *m25p128,
			   unsigned char val,
			   spi_bus_receive_t spi_bus_receive,
			   void *spi_bus);
	int (*fast_read_action)(void *m25p128,
				char dummy_byte,
			        uint32_t address,
				spi_bus_receive_t spi_bus_receive,
				void *spi_bus);

	char status_reg;

	struct private_data {
		void *spi_bus;
		spi_bus_receive_t spi_bus_receive;
		uint32_t address;
		unsigned long long index;
		uint32_t length;
	} data;
	int id;
	sem_t sem;
	int is_poll_mode;
	page_t *page;
} m25p128_t;

m25p128_t *init_m25p128(void);
void free_m25p128(m25p128_t *m25p128);
#endif
