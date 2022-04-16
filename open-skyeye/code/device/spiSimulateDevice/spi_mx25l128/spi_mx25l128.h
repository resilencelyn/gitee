#ifndef __SPI_MX25L128_H__
#define __SPI_MX25L128_H__
#include "mx25l128.h"

/* mx25l128 cmd head format:BYTE|1 + BYTE|3(option) BYTE|1(option) + BYTES|1-n */
typedef struct data_buf {
	unsigned char cmd;
	union {
		unsigned char addr[3];
		unsigned char dummy_byte;
		char data[0];
	};
} data_buf_t;

typedef struct transmit_data {
	int pos;
	data_buf_t *buf;
} transmit_data_t;

typedef struct receive_data {
	int len;
	char *buf;
} receive_data_t;

struct mx25l128_device {
	conf_object_t* obj;
	conf_object_t *spi_bus_obj;
	spi_bus_intf *spi_bus_iface;

	mx25l128_t *mx25l128;
	transmit_data_t tx_data;

	int is_read_action;
};
typedef struct mx25l128_device spi_mx25l128_device;

#define MX25L12805D 0x5D
#define MX25L12855E 0x55E
#endif
