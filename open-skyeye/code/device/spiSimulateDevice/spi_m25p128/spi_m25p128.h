#ifndef __SPI_M25P128_H__
#define __SPI_M25P128_H__
#include "m25p128.h"

/* m25p128 cmd head format:BYTE|1 + BYTE|3(option) BYTE|1(option) + BYTES|1-n */
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

struct m25p128_device {
	conf_object_t* obj;
	conf_object_t *spi_bus_obj;
	spi_bus_intf *spi_bus_iface;

	m25p128_t *m25p128;
	transmit_data_t tx_data;

	int is_read_action;
};
typedef struct m25p128_device spi_m25p128_device;
#endif
