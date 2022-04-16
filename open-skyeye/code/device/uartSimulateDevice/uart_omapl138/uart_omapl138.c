/* Copyright (C)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*/
/**
* @file uart_omapl138.c
* @brief The implementation of system controller
* @author Kewei Yu keweihk@gmail.com
* @version 78.77
*/

#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_uart_ops.h>
#include <skyeye_iface.h>
#define DEBUG
#include <skyeye_log.h>

#include "uart_omapl138.h"
#include "omapl138_fifo.h"

static char* omapl138_uart_attr[] = {"term", "signal"};

static exception_t serial_write(conf_object_t *obj, uint32_t value)
{
	struct uart_omapl138_device *dev = obj->obj;
	struct registers* regs = &dev->regs;
	unsigned char buf = (uint8_t)value;

	dev->regs.lsr &= ~(1 << 6);
	dev->regs.lsr &= ~(1 << 5);
	if (dev->regs.fcr & 1) {
		/* fifo mode */
		dev->regs.iir |= 1;
		dev->regs.iir &= ~(7 << 1);
		/* translate 'DEL' to 'Backspace' */
		if (buf == 127)
			buf = '\b';

		if (buf == 10 || buf == 13)
			buf = '\r';
		WriteFIFO(dev->read_fifo, &buf, 1);
		if (dev->regs.pwremu_mgmt & 0x2000) {     // URRST enable;
			if (dev->regs.fcr & 0x9) {            // DMAMODE1 enable; FIFOEN enable;
				if (dev->edma_event != NULL) {
					dev->edma_event->write(dev->edma, dev->channel_R, 0);
				}
			}
		}

		/* loop back mode */
		if (dev->regs.mcr & (1 << 4)) {
			if (dev->term_iface != NULL) {
				dev->term_iface->write(dev->term, &buf, 1);
			}
			if (dev->uart_file != NULL){
				dev->uart_file_iface->write(dev->uart_file, &buf, 1);
			}
		}
        /* set the DR bit in LSR */
        //dev->regs.lsr |= 1;
		if (dev->regs.ier & 1) {
			/* set IIR register */
			dev->regs.iir &= ~1;

			dev->regs.iir &= ~(7 << 1);
			dev->regs.iir |= 2 << 1;

			dev->regs.iir &= ~(3 << 6);
			dev->regs.iir |= 3 << 6;

		/* set the DR bit in LSR */
			dev->regs.lsr |= 1;

			if (dev->signal_iface != NULL) {
				dev->signal_iface->raise_signal(
						dev->signal,
						dev->interrupt_number
						);
			}
		} else if (((dev->regs.ier & 1) == 0) && (dev->regs.fcr & (1 << 3))) {
			/* DMA model */
		} else if ((dev->regs.ier & 1) == 0) {
			/* interrupt disable, fifo enable, is the poll mode */
			dev->regs.lsr |= 1;
		}
	} else {
		/* non-fifo mode */
		dev->regs.iir |= 1;
		dev->regs.iir &= ~(7 << 1);

		WriteFIFO(dev->read_fifo, &buf, 1);
		/* loop back mode */
		if (dev->regs.mcr & (1 << 4)) {
			if (dev->term_iface != NULL) {
				dev->term_iface->write(dev->term, &buf, 1);
			}
			if (dev->uart_file != NULL){
				dev->uart_file_iface->write(dev->uart_file, &buf, 1);
			}
		}
		/* set the DR bit in LSR */
		dev->regs.lsr |= 1;

		if (dev->regs.ier & 1) {
			/* refer to the datasheet, we set iir
			 * to here, interrupt is auto cleared
			 * when the character is read from RBR
			 */
			dev->regs.iir &= ~1;

			dev->regs.iir &= ~(7 << 1);
			dev->regs.iir |= 2 << 1;

			dev->regs.iir &= ~(3 << 6);
			dev->regs.iir |= 3 << 6;

			//	/* set the DR bit in LSR */
			//	dev->regs.lsr |= 1;

			if (dev->signal_iface != NULL) {
				dev->signal_iface->raise_signal(
						dev->signal,
						dev->interrupt_number
						);
			}
		}
	}
	return No_exp;
}

void uart_do_cycle(conf_object_t* opaque)
{
	struct uart_omapl138_device *dev = opaque->obj;
	exception_t ret = 0;
	unsigned char buf;

	if (dev->dev_enable == 0)
		return;

	if (dev->regs.lsr & 1)
		return;

	if ((ret = (dev->term_iface->read(dev->term, &buf, 1)) != 0)) {
        dev->regs.lsr &= ~(1 << 6);
		dev->regs.lsr &= ~(1 << 5);
		if (dev->regs.fcr & 1) {
			/* fifo mode */
			dev->regs.iir |= 1;
			dev->regs.iir &= ~(7 << 1);
			/* translate 'DEL' to 'Backspace' */
			if (buf == 127)
				buf = '\b';

			if (buf == 10 || buf == 13)
				buf = '\r';
			WriteFIFO(dev->read_fifo, &buf, 1);
            if (dev->regs.pwremu_mgmt & 0x2000) {     // URRST enable;
                if (dev->regs.fcr & 0x9) {            // DMAMODE1 enable; FIFOEN enable;
                    if (dev->edma_event != NULL) {
                        dev->edma_event->write(dev->edma, dev->channel_R, 0);
                    }
                }
            }

			/* loop back mode */
			if (dev->regs.mcr & (1 << 4)) {
				if (dev->term_iface != NULL) {
					dev->term_iface->write(dev->term, &buf, 1);
				}
                if (dev->uart_file != NULL){
                    dev->uart_file_iface->write(dev->uart_file, &buf, 1);
                }
			}
            /* set the DR bit in LSR */
            //dev->regs.lsr |= 1;
			if (dev->regs.ier & 1) {
				/* set IIR register */
				dev->regs.iir &= ~1;

				dev->regs.iir &= ~(7 << 1);
				dev->regs.iir |= 2 << 1;

				dev->regs.iir &= ~(3 << 6);
				dev->regs.iir |= 3 << 6;

		//		/* set the DR bit in LSR */
				dev->regs.lsr |= 1;

				if (dev->signal_iface != NULL) {
					dev->signal_iface->raise_signal(
							dev->signal,
							dev->interrupt_number
							);
				}
			} else if (((dev->regs.ier & 1) == 0) && (dev->regs.fcr & (1 << 3))) {
				/* DMA model */
			} else if ((dev->regs.ier & 1) == 0) {
				/* interrupt disable, fifo enable, is the poll mode */
				dev->regs.lsr |= 1;
			}
		
		} else {
			/* non-fifo mode */
			dev->regs.iir |= 1;
			dev->regs.iir &= ~(7 << 1);

			WriteFIFO(dev->read_fifo, &buf, 1);
			/* loop back mode */
			if (dev->regs.mcr & (1 << 4)) {
				if (dev->term_iface != NULL) {
					dev->term_iface->write(dev->term, &buf, 1);
				}
                if (dev->uart_file != NULL){
                    dev->uart_file_iface->write(dev->uart_file, &buf, 1);
                }
			}
            /* set the DR bit in LSR */
            dev->regs.lsr |= 1;

			if (dev->regs.ier & 1) {
				/* refer to the datasheet, we set iir
				 * to here, interrupt is auto cleared
				 * when the character is read from RBR
				 */
				dev->regs.iir &= ~1;

				dev->regs.iir &= ~(7 << 1);
				dev->regs.iir |= 2 << 1;

				dev->regs.iir &= ~(3 << 6);
				dev->regs.iir |= 3 << 6;

			//	/* set the DR bit in LSR */
			//	dev->regs.lsr |= 1;

				if (dev->signal_iface != NULL) {
					dev->signal_iface->raise_signal(
							dev->signal,
							dev->interrupt_number
							);
				}
			}
		}
	}
}

static void signal_check(void *opaque)
{
	struct uart_omapl138_device *dev = ((conf_object_t *)opaque)->obj;

	if (dev->dev_enable == 0)
		return;

	if (dev->signal_iface != NULL) {
		dev->signal_iface->raise_signal(
				dev->signal,
				dev->interrupt_number
				);
	}
}

static void uart_edma_send_data(void *opaque)
{
    struct uart_omapl138_device *dev = ((conf_object_t *)opaque)->obj;
    if (dev->regs.pwremu_mgmt & 0x4000) {     // UTRST enable;
        if (dev->regs.fcr & 0x9) {            // DMAMODE1 enable; FIFOEN enable;
            if (CheckCanReadNum(dev->read_fifo) <= 0) {
                if (dev->edma_event != NULL) {
                    dev->edma_event->write(dev->edma, dev->channel_T, 0);
                }
            }
        }
    }
}

static exception_t uart_omapl138_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
	struct uart_omapl138_device *dev = opaque->obj;
	uint32_t *data = (uint32_t *)buf;
	char ch;

	if (offset >= IER && offset < IIR && count == 1) {
		if ((dev->regs.lcr & (1 << 7)) == 0) {
			char *pier = (char *)&dev->regs.ier;
			uint32_t off = offset - IER;

			*data = *(pier + off);
		} else {
			char *pdlh = (char *)&dev->regs.dlh;
			uint32_t off = offset - IER;

			*data = *(pdlh + off);
		}
		return No_exp;
	}
	if (offset >= IIR && offset < LCR && count == 1) {
		char *piir = (char *)&dev->regs.iir;
		uint32_t off = offset - IIR;

		*data = *(piir + off);

		if (dev->sche_id != -1 ) {
			del_thread_scheduler(dev->sche_id);
			dev->sche_id = -1;
		}
		if ((dev->regs.fcr & 1) == 0) {
			/* non-FIFO mode */
			dev->regs.iir |= 1;
			dev->regs.iir &= ~(7 << 1);
		} else {
			/* FIFO mode */
			dev->regs.iir |= 1;
			dev->regs.iir &= ~(7 << 1);
		}
		if (((dev->regs.iir >> 1) & 0x7) == 1) {
			/* clear THR interrupt */
			dev->regs.iir |= 1;
			dev->regs.iir &= ~(7 << 1);
		}
		return No_exp;
	}
	if (offset >= LCR && offset < LSR && count == 1) {
		char *preg = (char *)&dev->regs.lcr;
		uint32_t off = offset - LCR;

		*data = *(preg + off);
		return No_exp;
	}
	if (offset >= LSR && offset < MSR && count == 1) {
		char *plsr = (char *)&dev->regs.lsr;
		uint32_t off = offset - LSR;

		*data = *(plsr + off);
		return No_exp;
	}
	if (offset >= MSR && offset <= (MDR + 4) && count == 1) {
		char *preg = (char *)&dev->regs.msr;
		uint32_t off = offset - MSR;

		*data = *(preg + off);
		return No_exp;
	}
	switch (offset) {
		case 0:
			if ((dev->regs.lcr & (1 << 7)) == 0) {
				if (dev->regs.fcr & 1) {
					/* fifo mode */
					ReadFIFO(dev->read_fifo, &ch, 1);
					*data = ch;

					if (dev->regs.ier & 1) {
						dev->regs.iir |= 1;
						dev->regs.iir &= ~(3 << 1);
						dev->regs.lsr &= ~1;
						dev->regs.lsr |= 1 << 6;
						dev->regs.lsr |= 1 << 5;
					} else if ((dev->regs.ier & 1) == 0) {
						/* fifo poll mode */
						dev->regs.lsr &= ~1;
						dev->regs.lsr |= 1 << 6;
						dev->regs.lsr |= 1 << 5;
					}
				} else {
					/* non-fifo mode */
					if (CheckCanReadNum(dev->read_fifo) > 0) {
						ReadFIFO(dev->read_fifo, &ch, 1);
						*data = ch;
						dev->regs.iir |= 1;
						dev->regs.iir &= ~(3 << 1);
						dev->regs.lsr &= ~1;
                        dev->regs.lsr |= 1 << 6;
						dev->regs.lsr |= 1 << 5;
					}
				}
			} else {
				*data = dev->regs.dll;
			}
			break;
		case IER:
			if ((dev->regs.lcr & (1 << 7)) == 0) {
				*data = dev->regs.ier;
			} else {
				*data = dev->regs.dlh;
			}
			break;
		case 8:
			*data = dev->regs.iir;

			if ((dev->regs.fcr & 1) == 0) {
				/* non-FIFO mode */
				dev->regs.iir |= 1;
				dev->regs.iir &= ~(7 << 1);
			} else {
				/* FIFO mode */
				dev->regs.iir |= 1;
				dev->regs.iir &= ~(7 << 1);
			}
			if (((dev->regs.iir >> 1) & 0x7) == 1) {
				/* clear THR interrupt */
				dev->regs.iir |= 1;
				dev->regs.iir &= ~(7 << 1);
			}
			break;
		case LCR:
			*data = dev->regs.lcr;
			break;
		case MCR:
			*data = dev->regs.mcr;
			break;
		case LSR:
			*data = dev->regs.lsr;
			/* clear OE flag */
			dev->regs.lsr &= ~(1 << 1);
			break;
		case MSR:
			*data = dev->regs.msr;
			break;
		case SCR:
			*data = dev->regs.scr;
			break;
		case DLL:
			*data = dev->regs.dll;
			break;
		case DLH:
			*data = dev->regs.dlh;
			break;
		case REVID1:
			*data = dev->regs.revid1;
			break;
		case REVID2:
			*data = dev->regs.revid2;
			break;
		case PWREMU_MGMT:
			*data = dev->regs.pwremu_mgmt;
			break;
		case MDR:
			*data = dev->regs.mdr;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__,"Can not read the register at 0x%x in uart_omapl138\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}

static exception_t uart_omapl138_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
	struct uart_omapl138_device *dev = opaque->obj;
	uint32_t data = *buf;
	char c;

	if (offset >= IER && offset < IIR && count == 1) {
		char *pier = (char *)&dev->regs.ier;
		char *pdlh = (char *)&dev->regs.dlh;
		uint32_t off = offset - IER;

		if ((dev->regs.lcr & (1 << 7)) == 0) {
			*(pier + off) = data;
		} else {
			*(pdlh + off) = data;
		}
		return No_exp;
	}
	if (offset >= FCR && offset < LCR && count == 1) {
		char *pfcr = (char *)&dev->regs.fcr;
		uint32_t off = offset - FCR;

		*(pfcr + off) = data;
		if (dev->regs.fcr & (1 << 1)) {
			/* FIXME: to reset FIFO and FIFO couter */
			dev->regs.fcr &= ~(1 << 1);
		}
		if (dev->regs.fcr & (1 << 2)) {
			/* FIXME: to reset FIFO and FIFO couter */
			dev->regs.fcr &= ~(1 << 2);
		}

		return No_exp;
	}
	if (offset >= LCR && offset < MCR && count == 1) {
		char *plcr = (char *)&dev->regs.lcr;
		uint32_t off = offset - LCR;

		*(plcr + off) = data;
		return No_exp;
	}
	if (offset >= MCR && offset < LSR && count == 1) {
		char *pmcr = (char *)&dev->regs.mcr;
		uint32_t off = offset - MCR;

		*(pmcr + off) = data;
		if (dev->regs.mcr & (1 << 4)) {
			dev->regs.msr |= ((dev->regs.mcr >> 1) & 0x1) << 4;
			dev->regs.msr |= ((dev->regs.mcr) & 0x1) << 5;
			dev->regs.msr |= ((dev->regs.mcr >> 2) & 0x1) << 6;
			dev->regs.msr |= ((dev->regs.mcr >> 3) & 0x1) << 7;
		}
		return No_exp;
	}
	if (offset >= DLL && offset <= REVID2 && count == 1) {
		char *preg = (char *)&dev->regs.dll;
		uint32_t off = offset - DLL;

		*(preg + off) = data;
		return No_exp;
	}
	if (offset >= PWREMU_MGMT && offset < MDR && count == 1) {
		char *ppwremu_mgmt = (char *)&dev->regs.pwremu_mgmt;
		uint32_t off = offset - PWREMU_MGMT;

		*(ppwremu_mgmt + off) = data;
		if ((dev->regs.pwremu_mgmt & (1 << 13)) == 0) {
			/* receiver disable and reset */
			dev->regs.iir &= ~1;
			dev->regs.iir &= ~(7 << 1);
			dev->regs.fcr &= ~(1 << 3);
			/* FIXME: to reset FIFO and FIFO couter */

			dev->regs.pwremu_mgmt |= 1 << 13;
		}
		if ((dev->regs.pwremu_mgmt & (1 << 14)) == 0) {
			/* transmitter disable and reset */
			dev->regs.iir &= ~1;
			dev->regs.iir &= ~(7 << 1);
			dev->regs.fcr &= ~(1 << 3);
			/* FIXME: to reset FIFO and FIFO couter */

			dev->regs.pwremu_mgmt |= 1 << 14;
		}
		return No_exp;
	}
	if (offset >= MDR && offset <= (MDR + 4) && count == 1) {
		char *pmdr = (char *)&dev->regs.mdr;
		uint32_t off = offset - MDR;

		*(pmdr + off) = data;
		return No_exp;
	}
	if (offset >= THR && offset < IER && count == 1 && (dev->regs.lcr & (1 << 7))) {
		char *pdll = (char *)&dev->regs.dll;
		uint32_t off = offset - THR;

		*(pdll + off) = data;
		return No_exp;
	}
	switch (offset) {
		case 0:
			c = data;
			if ((dev->regs.lcr & (1 << 7)) == 0) {
				if (dev->regs.fcr & 1) {
					/* fifo mode */
					if (dev->term_iface != NULL) {
						dev->term_iface->write(dev->term, &c, 1);
						if (dev->uart_file != NULL)
							dev->uart_file_iface->write(dev->uart_file, &c, 1);
						if (dev->regs.ier & (1 << 1 | 1 << 0)) {
							dev->regs.iir &= ~1;

							dev->regs.iir &= ~(3 << 1);
							dev->regs.iir |= 1 << 1;

							dev->regs.iir &= ~(3 << 6);
							dev->regs.iir |= 3 << 6;

							dev->regs.lsr |= 1 << 6;
							dev->regs.lsr |= 1 << 5;

							if (dev->sche_id == -1) {
								create_thread_scheduler(40000, Periodic_sched, signal_check, opaque, &dev->sche_id);
							}
							if (dev->signal_iface != NULL) {
								dev->signal_iface->raise_signal(
										dev->signal,
										dev->interrupt_number
										);
							}
						} else if ((dev->regs.ier & (1 << 1)) == 0) {
							/* poll mode */
							dev->regs.lsr |= 1 << 6;
							dev->regs.lsr |= 1 << 5;
						}
					}
				} else {
					/* non-fifo mode */
					if (dev->term_iface != NULL) {
						dev->term_iface->write(dev->term, &c, 1);
					if (dev->uart_file != NULL)
						dev->uart_file_iface->write(dev->uart_file, &c, 1);
						if (dev->regs.ier & (1 << 1)) {
							dev->regs.iir &= ~1;

							dev->regs.iir &= ~(3 << 1);
							dev->regs.iir |= 1 << 1;

							dev->regs.iir &= ~(3 << 6);

							dev->regs.lsr |= 1 << 6;
							dev->regs.lsr |= 1 << 5;

							if (dev->sche_id == -1) {
								create_thread_scheduler(40000, Periodic_sched, signal_check, opaque, &dev->sche_id);
							}
							if (dev->signal_iface != NULL) {
								dev->signal_iface->raise_signal(
										dev->signal,
										dev->interrupt_number
										);
							}
						}
					}
				}
			} else {
				dev->regs.dll = data;
			}
			break;
		case IER:
			if ((dev->regs.lcr & (1 << 7)) == 0) {
				dev->regs.ier = data;
			} else {
				dev->regs.dlh = data;
			}
			break;
		case FCR:
			dev->regs.fcr = data;
			if (data & (1 << 1)) {
				/* FIXME: to reset FIFO and FIFO couter */
			}
			if (data & (1 << 2)) {
				/* FIXME: to reset FIFO and FIFO couter */
			}
            if (dev->regs.fcr & 0x8) {
                if (dev->dmamode_sche == -1) {
                    create_thread_scheduler(1000, Periodic_sched, uart_edma_send_data, (void *)opaque, &dev->dmamode_sche);
                }
            } else {
                if (dev->dmamode_sche != -1) {
                    del_thread_scheduler(dev->dmamode_sche);
                    dev->dmamode_sche = -1;
                }
            }
			break;
		case LCR:
			dev->regs.lcr = data;
			break;
		case MCR:
			dev->regs.mcr = data;
			if (dev->regs.mcr & (1 << 4)) {
				dev->regs.msr |= ((dev->regs.mcr >> 1) & 0x1) << 4;
				dev->regs.msr |= ((dev->regs.mcr) & 0x1) << 5;
				dev->regs.msr |= ((dev->regs.mcr >> 2) & 0x1) << 6;
				dev->regs.msr |= ((dev->regs.mcr >> 3) & 0x1) << 7;
			}
			break;
		case DLL:
			dev->regs.dll = data;
			break;
		case DLH:
			dev->regs.dlh = data;
			break;
		case REVID1:
			dev->regs.revid1 = data;
			break;
		case REVID2:
			dev->regs.revid2 = data;
			break;
		case PWREMU_MGMT:
			dev->regs.pwremu_mgmt = data;
			if ((data & (1 << 13)) == 0) {
				/* receiver disable and reset */
				dev->regs.iir &= ~1;
				dev->regs.iir &= ~(7 << 1);
				dev->regs.fcr &= ~(1 << 3);
				/* FIXME: to reset FIFO and FIFO couter */
			}
			if ((data & (1 << 14)) == 0) {
				/* transmitter disable and reset */
				dev->regs.iir &= ~1;
				dev->regs.iir &= ~(7 << 1);
				dev->regs.fcr &= ~(1 << 3);
				/* FIXME: to reset FIFO and FIFO couter */
			}
			break;
		case MDR:
			dev->regs.mdr = data;
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__,"Can not write the register at 0x%x in uart_omapl138\n", offset);
			return Invarg_exp;
	}
	return No_exp;
}
static exception_t omapl138_uart_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
	struct uart_omapl138_device *dev = opaque->obj;
	int index;
	for(index = 0; index < sizeof(omapl138_uart_attr) / sizeof(omapl138_uart_attr[0]); index++) {
		if(!strncmp(attr_name, omapl138_uart_attr[index], strlen(omapl138_uart_attr[index])))
			break;
	}

	switch(index) {
		case 0:
			dev->term = (skyeye_uart_intf*)SKY_get_interface(value->u.object, SKYEYE_UART_INTF);
			break;
		case 1:
			dev->signal = (general_signal_intf*)SKY_get_interface(value->u.object, GENERAL_SIGNAL_INTF_NAME);
			break;
		default:
			skyeye_log(Error_log, __FUNCTION__, "parameter error\n");
			return Invarg_exp;
	}

	return No_exp;
}

static char *uart_omapl138_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
	uart_omapl138_device *dev= conf_obj->obj;
	return dev->regs_name[id];
}

static uint32_t uart_omapl138_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
	uart_omapl138_device *dev = conf_obj->obj;
	uint32_t* regs_value = (uint32_t*)(&dev->regs) + id;
	return *regs_value;
}

static exception_t uart_omapl138_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
	uart_omapl138_device *dev = conf_obj->obj;
	uart_omapl138_reg_t *regs = &dev->regs;
	*((uint32_t*)regs + id) = value;

	return No_exp;
}

static uint32_t uart_omapl138_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
	uart_omapl138_device *dev = conf_obj->obj;
	return  sizeof(dev->regs) / 4;
}

static uint32_t uart_omapl138_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
	uart_omapl138_device *dev = conf_obj->obj;
	uint32_t regnum = sizeof(dev->regs) / 4;
	int i;

	for(i = 0; i < regnum; i++) {
		if(strcmp(name, dev->regs_name[i]) == 0)
			return i;
	}
	return regnum;
}

static conf_object_t* new_uart_omapl138(char* obj_name) {
	uart_omapl138_device* dev = skyeye_mm_zero(sizeof(uart_omapl138_device));
	dev->obj = new_conf_object(obj_name, dev);

	dev->regs.iir		= 0x1;
	dev->regs.lsr		= 0x60;
	dev->regs.revid1	= 0x11020002;
	dev->regs.pwremu_mgmt	= 0x2;

	dev->regs_name = regs_name;
	dev->read_fifo = NULL;

	dev->sche_id = -1;

    // The device number defaults to 0 device
    dev->dev_number = 0;
    dev->channel_R = 8;
    dev->channel_T = 9;
    dev->dmamode_sche = -1;

	//int  pthread_id;
	//create_thread_scheduler(1000, Periodic_sched, uart_do_cycle, dev->obj, &pthread_id);
	//dev->dev_enable = 1;
	return dev->obj;
}

exception_t reset_uart_omapl138(conf_object_t* opaque, const char* parameters) {
	struct uart_omapl138_device *dev = opaque->obj;

	dev->regs.iir		= 0x1;
	dev->regs.lsr		= 0x60;
	dev->regs.revid1	= 0x11020002;
	dev->regs.pwremu_mgmt	= 0x2;

	return No_exp;
}

static exception_t free_uart_omapl138(conf_object_t* opaque) {
	struct uart_omapl138_device *dev = opaque->obj;
	if(dev->read_fifo)
		FreeFIFO(dev->read_fifo);

	skyeye_free(opaque->objname);
	skyeye_free(opaque->obj);
	skyeye_free(opaque);

	return No_exp;
}

static attr_value_t get_attr_signal(void* arg, conf_object_t* conf_obj, attr_value_t* idx) {
	struct uart_omapl138_device *dev = conf_obj->obj;
	return SKY_make_attr_object(dev->signal);
}
static exception_t set_attr_signal(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx) {
	struct uart_omapl138_device *dev = conf_obj->obj;
	conf_object_t* obj = SKY_attr_object(*value);
	dev->signal_iface = (general_signal_intf **)SKY_get_iface(obj, GENERAL_SIGNAL_INTF_NAME);
	dev->signal = obj;
	return No_exp;
}

static attr_value_t get_attr_rfifo(void* arg, conf_object_t* conf_obj, attr_value_t* idx) {
	struct uart_omapl138_device *dev = conf_obj->obj;
	return SKY_make_attr_object(dev->signal);
}
static exception_t set_attr_rfifo(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx) {
	unsigned int length;
	struct uart_omapl138_device *dev = conf_obj->obj;
	length = SKY_attr_uinteger(*value);
	dev->read_fifo = CreateFIFO(length);
	if(dev->read_fifo == NULL)
		skyeye_log(Error_log, __FUNCTION__, "Create Read Fifo Fail.");
	return No_exp;
}

static exception_t term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	struct uart_omapl138_device *dev = obj->obj;
	dev->term = obj2;
	dev->term_iface = (memory_space_intf*)SKY_get_iface(dev->term, SKYEYE_UART_INTF);
	if (dev->term_iface == NULL){
		if (dev->term == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SKYEYE_UART_INTF);
			dev->dev_enable = 0;
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_UART_INTF, obj2->objname);

		return Not_found_exp;
	}

	dev->dev_enable = 1;
	return No_exp;
}

static exception_t term_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	struct uart_omapl138_device *dev = obj->obj;
	*obj2 = dev->term;
	*port = NULL;
	return No_exp;
}

static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	struct uart_omapl138_device *dev = obj->obj;
	dev->uart_file = obj2;
	dev->uart_file_iface = (uart_file_intf*)SKY_get_iface(obj2, UART_FILE_INTF);

	if (dev->uart_file_iface == NULL){
		if (dev->uart_file == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", UART_FILE_INTF);
			dev->dev_enable = 0;
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", UART_FILE_INTF, obj2->objname);

		return Not_found_exp;
	}
	dev->dev_enable = 1;
	return No_exp;
}

static exception_t uart_file_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	struct uart_omapl138_device *dev = obj->obj;
	*obj2 = dev->uart_file;
	*port = NULL;
	return No_exp;
}

static exception_t edma_event_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	struct uart_omapl138_device *dev = obj->obj;
    dev->edma = obj2;
	dev->edma_event = (edma_device_event_t *)SKY_get_iface(dev->edma, EDMA_DEVICE_EVENT_INTERFACE);
	if (dev->edma_event == NULL){
		if (dev->edma == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", EDMA_DEVICE_EVENT_INTERFACE);
			dev->dev_enable = 0;
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", EDMA_DEVICE_EVENT_INTERFACE, obj2->objname);

		return Not_found_exp;
	}
	dev->dev_enable = 1;
    return No_exp;
}

static exception_t edma_event_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	struct uart_omapl138_device *dev = obj->obj;
    *obj2 = dev->edma;
    *port = NULL;
    return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
	struct uart_omapl138_device *dev = obj->obj;
	dev->signal = obj2;
	dev->signal_iface = (general_signal_intf **)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

	if (dev->signal_iface == NULL){
		if (dev->signal == NULL)
		{
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
			dev->dev_enable = 0;
		} else
			skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

		return Not_found_exp;
	}
	dev->dev_enable = 1;
	return No_exp;
}

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
	struct uart_omapl138_device *dev = obj->obj;
	*obj2 = dev->signal;
	*port = NULL;
	return No_exp;
}
static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	struct uart_omapl138_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->interrupt_number);
	return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	struct uart_omapl138_device *dev = conf_obj->obj;
	dev->interrupt_number = SKY_attr_uinteger(*value);
	return No_exp;
}

static attr_value_t get_attr_device_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
	struct uart_omapl138_device *dev = conf_obj->obj;
	attr_value_t int_number = SKY_make_attr_uinteger(dev->dev_number);
	return int_number;
}

static exception_t set_attr_device_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
	struct uart_omapl138_device *dev = conf_obj->obj;
	dev->dev_number = SKY_attr_uinteger(*value);
    if (dev->dev_number == 0) {
        dev->channel_R = 8;
        dev->channel_T = 9;
    } else if (dev->dev_number == 1) {
        dev->channel_R = 12;
        dev->channel_T = 13;
    } else if (dev->dev_number == 2) {
        dev->channel_R = 30;
        dev->channel_T = 31;
    } else {
        skyeye_log(Error_log, __FUNCTION__, "The device McbSP_FIFO is only 0 and 1 only, from %s fail.", conf_obj->objname);
    }
	return No_exp;
}

void uart_omapl138_register_attribute(conf_class_t* clss){
	SKY_register_attribute(clss, "rfifo", get_attr_rfifo, NULL, set_attr_rfifo, NULL, SKY_Attr_Optional, "uinteger", "fifo length for omapl138 uart");
	SKY_register_attribute(clss, "int_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of systimer");
    SKY_register_attribute(clss, "device_number", get_attr_device_number, NULL, set_attr_device_number, NULL, SKY_Attr_Optional, "uinteger", "McBSP device number");
}

void init_uart_omapl138(){
	static skyeye_class_t class_data = {
		.class_name = "omapl138_uart",
		.class_desc = "omapl138_uart",
		.new_instance = new_uart_omapl138,
		.free_instance = free_uart_omapl138,
		.reset_instance = reset_uart_omapl138,
		.get_attr = NULL,
		.set_attr = omapl138_uart_set_attr,
		.module_type = SKYML_ONCHIP(SC_omapl138),
	};

	conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

	static const memory_space_intf io_memory = {
		.read = uart_omapl138_read,
		.write = uart_omapl138_write
	};
	SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

	static const skyeye_reg_intf reg_intf = {
		.get_regvalue_by_id = uart_omapl138_get_regvalue_by_id,
		.get_regname_by_id = uart_omapl138_get_regname_by_id,
		.set_regvalue_by_id = uart_omapl138_set_regvalue_by_id,
		.get_regnum = uart_omapl138_get_regnum,
		.get_regid_by_name = uart_omapl138_get_regid_by_name,
	};
	SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

	static const skyeye_serial_device_t serial_device = {
		.write = serial_write,
		.receive_ready = NULL,
		.write_chars = NULL,
	};
	SKY_register_iface(clss, SERIAL_DEVICE_INTERFACE, &serial_device);
	static const struct InterfaceDescription ifaces[] = {
		(struct InterfaceDescription) {
			.name = MEMORY_SPACE_INTF_NAME,
			.iface = &io_memory,
		},
		(struct InterfaceDescription) {
			.name = SKYEYE_REG_INTF,
			.iface = &reg_intf,
		},
		(struct InterfaceDescription) {
			.name = SERIAL_DEVICE_INTERFACE,
			.iface = &serial_device,
		},
 	};
	static const struct ConnectDescription connects[] = {
		(struct ConnectDescription ){
			.name = SKYEYE_UART_INTF,
			.set = term_set,
			.get = term_get,
		},
		(struct ConnectDescription) {
			.name = GENERAL_SIGNAL_INTF_NAME,
			.set = general_signal_set,
			.get = general_signal_get,
		},
		(struct ConnectDescription ){
			.name = UART_FILE_INTF,
			.set = uart_file_set,
			.get = uart_file_get,
		},
        (struct ConnectDescription){
            .name = EDMA_DEVICE_EVENT_INTERFACE,
            .set = edma_event_set,
            .get = edma_event_get,
        },
	};
	class_register_ifaces(clss, ifaces);
	class_register_connects(clss, connects);

	uart_omapl138_register_attribute(clss);
}
