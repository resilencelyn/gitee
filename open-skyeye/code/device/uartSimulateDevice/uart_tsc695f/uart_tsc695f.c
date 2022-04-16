/*
 * =====================================================================================
 *
 *       Filename:  tsc695f_uart.c
 *
 *    Description:  This file implements the LEON2 UART on-chip device.
 *
 *        Version:  1.0
 *        Created:  24/06/08 10:51:22
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aitor Viana Sanchez (avs), aitor.viana.sanchez@esa.int
 *        Company:  European Space Agency (ESA-ESTEC)
 *
 * =====================================================================================
 */

#include <skyeye_config.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_attr.h>
#include <skyeye_uart_ops.h>

#include "uart_tsc695f.h"
#define UARTA_INT 4
#define UARTB_INT 5

static uint32_t bswap_word(uint32_t data)
{
    return (data<<24)|((data<<8) & 0xff0000) | (data>>24) | ((data>>8) & 0xff00);
}

FIFO* CreateFIFO(unsigned int  FIFOLength)
{
    FIFO *fifo = malloc(sizeof(FIFO));
    if(fifo == NULL)
        return NULL;
    unsigned char  *pfifo;
    pfifo = (unsigned char  *)malloc(FIFOLength);
    if(pfifo == NULL)
        return NULL;
    fifo->pFirst = pfifo;
    fifo->pLast = pfifo + FIFOLength-1;
    fifo->Length = FIFOLength;
    fifo->pIn     = fifo->pFirst;
    fifo->pOut    = fifo->pFirst;
    fifo->Enteres = 0;
    return fifo;
}

unsigned int  WriteFIFO(FIFO *fifo, unsigned char  *pSource,unsigned int  WriteLength)
{
    unsigned int  i;

    for (i = 0; i < WriteLength; i++)
    {
        if (fifo->Enteres >= fifo->Length)
        {
            return i;//如果已经写入FIFO的数据两超过或者等于FIFO的长度，就返回实际写入FIFO的数据个�?
        }
        *(fifo->pIn) = *(pSource ++);
        if (fifo->pIn == fifo->pLast)
        {
            fifo->pIn = fifo->pFirst;
        }
        else
        {
            fifo->pIn++;
        }
        fifo->Enteres ++;
    }
    return i;
}

unsigned int  ReadFIFO(FIFO *fifo, unsigned char  *pAim,unsigned int  ReadLength)
{
    unsigned int  i;
    for (i = 0; i < ReadLength; i++)
    {
        if (fifo->Enteres <= 0){
            return i;//返回从FIFO中读取到的数据个�?
        }
        *(pAim ++) = *(fifo->pOut);
        if (fifo->pOut == fifo->pLast)
        {
            fifo->pOut = fifo->pFirst;
        }
        else
        {
            fifo->pOut ++;
        }
        fifo->Enteres -- ;
    }
    return i;
}

unsigned int  CheckFIFOLength(FIFO *fifo)
{
    return fifo->Length;
}

unsigned char * CheckCurrentWritePoint(FIFO *fifo)
{
    return (fifo->pIn);
}

unsigned char * CheckCurrentReadPoint(FIFO *fifo)
{
    return (fifo->pOut);
}

void FreeFIFO(FIFO *fifo)
{
    free(fifo->pFirst);
    free(fifo);
}

unsigned int  CheckCanWriteNum(FIFO *fifo)
{
    return (fifo->Length - fifo->Enteres);
}

unsigned int  CheckCanReadNum(FIFO *fifo)
{
    return fifo->Enteres;
}

void send_file_data(conf_object_t* opaque, unsigned char *buf, int length)
{
    tsc695f_uart_dev *dev = opaque->obj;

    WriteFIFO(dev->read_fifo, buf, length);
    dev->regs.status.flag.dra = 1;
    dev->regs.status.flag.drb = 1;
    if(dev->signal_iface)
    {
        dev->signal_iface->raise_signal(dev->signal, UARTA_INT);
        dev->signal_iface->raise_signal(dev->signal, UARTB_INT);
    }
}

void uart_do_cycle(conf_object_t* opaque)
{
    tsc695f_uart_dev *dev = opaque->obj;
    exception_t ret = 0;
    unsigned char buf;

    if (dev->term_iface == NULL)
    {
        skyeye_log(Error_log, __func__, "The term iface is not connect...");
    }

    ret = dev->term_iface->read(dev->term, &buf, 1);
    if (ret != 0)
    {
        /* translate 'DEL' to 'Backspace' */
        if (buf == 127)
            buf = '\b';
        //	printf("In %s, ret :%x, buf:%x\n", __func__,ret, buf);
        WriteFIFO(dev->read_fifo, &buf, 1);
        dev->regs.status.flag.dra = 1;
        dev->regs.status.flag.drb = 1;
        if(dev->signal_iface)
        {
            dev->signal_iface->raise_signal(dev->signal, UARTA_INT);
            dev->signal_iface->raise_signal(dev->signal, UARTB_INT);
        }
    }
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  tsc695f_uart_write
 *  Description:  This function perform the writing in the UART device
 * =====================================================================================
 */

static exception_t tsc695f_uart_write(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
    tsc695f_uart_dev *dev = opaque->obj;
    uint32_t data = *(uint32_t *)buf;

    switch(offset)
    {
        case UARTA_DATA_REGISTER:
            dev->regs.uarta_data = data;
            if (dev->term_iface)
            {
                dev->term_iface->write(dev->term, buf, 1);
            }
            if (dev->uart_file)
            {
                dev->uart_file_iface->write(dev->uart_file, buf, 1);
            }
            //	dev->regs.status.flag.dra |= DRA; 

            break;
        case UARTB_DATA_REGISTER:
            dev->regs.uartb_data = data;
            dev->regs.status.value &= ~DRB;
            if (dev->term_iface)
            {
                dev->term_iface->write(dev->term, buf, 1);
                //	dev->regs.status.value |= DRB;
            }
            if (dev->uart_file)
            {
                dev->uart_file_iface->write(dev->uart_file, buf, 1);
            }

            break;
        case UART_STATUS_REGISTER:
            break;
        default:
            printf("write %s error offset %d : 0x%x\n",
                    dev->obj->objname, offset, data);
            break;
    }
    return No_exp;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  tsc695f_uart_read
 *  Description:  This function performs the reading in the UART device.
 * =====================================================================================
 */
static exception_t tsc695f_uart_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
    tsc695f_uart_dev *dev = opaque->obj;
    uint32_t *data = (uint32_t *)buf;
    char ch;
    //	skyeye_log(Error_log, __FUNCTION__, "read : %s, %x, %x\n", opaque->objname, offset, *(uint32_t *)buf);

    if (offset >= UARTA_DATA_REGISTER && offset < UARTB_DATA_REGISTER && count == 1)
    {
        if(dev->regs.status.flag.dra == 1)
        {
            ReadFIFO(dev->read_fifo, &ch, 1);
            *data = ch;
        }
        uint32_t tmp = bswap_word(*data);
        int m = (offset % 4) * 8;
        *(uint8_t *)buf = (tmp>>m) & 0xff;
        dev->regs.status.flag.dra = 0;
        return No_exp;
    }

    if (offset >= UARTB_DATA_REGISTER && offset <= UART_STATUS_REGISTER && count == 1)
    {
        if(dev->regs.status.flag.drb == 1)
        {
            ReadFIFO(dev->read_fifo, &ch, 1);
            *data = ch;
        }
        uint32_t tmp = bswap_word(*data);
        int m = (offset % 4) * 8;
        *(uint8_t *)buf = (tmp>>m) & 0xff;
        dev->regs.status.flag.drb = 0;
        return No_exp;
    }

    switch(offset)
    {
        case UARTA_DATA_REGISTER:
            if(dev->regs.status.flag.dra == 1)
            {
                ReadFIFO(dev->read_fifo, &ch, 1);
                *data = ch;
                dev->regs.status.flag.dra = 0;
            }
            break;
        case UARTB_DATA_REGISTER:
            if(dev->regs.status.flag.drb == 1)
            {
                ReadFIFO(dev->read_fifo, &ch, 1);
                *data = ch;
            }
            break;
        case UART_STATUS_REGISTER:
            *(uint32*)buf =dev->regs.status.value;
            break;
        default:
            break;
    }
    //	printf("In %s, offset:%x, buf:%x\n", __func__, offset, *(uint32_t *)buf);

    return No_exp;
}

char* tsc695f_uart_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
    tsc695f_uart_dev *dev = conf_obj->obj;
    return regs_name[id];
}

uint32_t tsc695f_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
    tsc695f_uart_dev *dev = conf_obj->obj;
    uint32_t* regs_value = (uint32_t*)&(dev->regs) + id;
    return *regs_value;
}

static exception_t tsc695f_uart_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
    tsc695f_uart_dev *dev = conf_obj->obj;
    struct registers* regs = &(dev->regs);
    *((uint32_t*)regs + id) = value;

    return No_exp;
}

static uint32_t tsc695f_uart_get_regnum(conf_object_t* conf_obj)
{
    tsc695f_uart_dev *dev = conf_obj->obj;
    struct registers* regs = &(dev->regs);
    return  sizeof(struct registers) / 4;
}

static uint32_t tsc695f_uart_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
    tsc695f_uart_dev *dev = conf_obj->obj;
    struct registers* regs = &(dev->regs);
    uint32_t regnum;
    regnum = sizeof(struct registers) / 4;;
    int i;
    for(i = 0; i < regnum; i++)
    {
        if(strcmp(name, regs_name[i]) == 0)
            return i;
    }
    return regnum;
}
/*
 * ===  FUNCTION  ======================================================================
 *         Name:  new_tsc695f_uart
 *  Description:  instance tsc695f_uart.
 *
 *  The function returns base class of point which point to the tsc695f_uart.
 * =====================================================================================
 */

static conf_object_t* new_tsc695f_uart(const char* obj_name)
{
    tsc695f_uart_dev* tsc695f_uart = skyeye_mm_zero(sizeof(tsc695f_uart_dev));
    /*  Clear the UART register   */
    tsc695f_uart->obj = new_conf_object(obj_name, tsc695f_uart);
    tsc695f_uart->read_fifo = CreateFIFO(0x100);

    tsc695f_uart->regs.status.flag.dra = 0;
    tsc695f_uart->regs.status.flag.drb = 0;
    tsc695f_uart->regs.status.flag.tsea = 1;
    tsc695f_uart->regs.status.flag.thea = 1;
    tsc695f_uart->regs.status.flag.tseb = 1;
    tsc695f_uart->regs.status.flag.theb = 1;

    int pthread_id;
    create_thread_scheduler(1000, Periodic_sched, uart_do_cycle, tsc695f_uart->obj, &pthread_id);

    return tsc695f_uart->obj;
}

static exception_t reset_tsc695f_uart(conf_object_t* opaque, const char* args)
{
    tsc695f_uart_dev *dev = opaque->obj;
    memset(&(dev->regs), 0, sizeof(dev->regs));
    /*  Enable the transmitter and receiver by default  */

    dev->regs.status.flag.tsea = 1;
    dev->regs.status.flag.thea = 1;
    dev->regs.status.flag.tseb = 1;
    dev->regs.status.flag.theb = 1;

    return No_exp;
}

static exception_t free_tsc695f_uart(conf_object_t* conf_obj)
{
    tsc695f_uart_dev* dev = conf_obj->obj;
    if(dev->read_fifo)
        FreeFIFO(dev->read_fifo);
    skyeye_free(dev->obj);
    skyeye_free(dev);
    return No_exp;
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    tsc695f_uart_dev* dev = (tsc695f_uart_dev*)(conf_obj->obj);
    attr_value_t int_number = SKY_make_attr_uinteger(dev->int_number);
    return int_number;
}

static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    tsc695f_uart_dev* dev = (tsc695f_uart_dev*)(conf_obj->obj);
    dev->int_number = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_rfifo(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    tsc695f_uart_dev *dev = conf_obj->obj;
    return SKY_make_attr_uinteger(dev->fifo_length);
}

static exception_t set_attr_rfifo(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    tsc695f_uart_dev *dev = conf_obj->obj;
    dev->fifo_length = SKY_attr_uinteger(*value);
    if(dev->read_fifo == NULL)
    {
        dev->read_fifo = CreateFIFO(dev->fifo_length);
    }
    return No_exp;
}

static void tsc695f_register_attribute(conf_class_t* clss)
{
    SKY_register_attribute(clss, "rfifo", get_attr_rfifo, NULL, set_attr_rfifo, NULL, SKY_Attr_Optional, "uinteger", "fifo length for tsc695f uart");
    SKY_register_attribute(clss, "irq_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of tsc695f uart");
    return;
}

static exception_t term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    tsc695f_uart_dev* dev = (tsc695f_uart_dev*)(obj->obj);
    dev->term = obj2;
    dev->term_iface = (skyeye_uart_intf*)SKY_get_iface(dev->term, SKYEYE_UART_INTF);

    if (dev->term_iface == NULL)
    {
        if (dev->term == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", SKYEYE_UART_INTF);
        } else
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_UART_INTF, obj2->objname);

        return Not_found_exp;
    }

    return No_exp;
}

static exception_t term_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    tsc695f_uart_dev* dev = (tsc695f_uart_dev*)(obj->obj);
    *obj2 = dev->term;
    *port = NULL;
    return No_exp;
}

static exception_t general_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    tsc695f_uart_dev* dev = (tsc695f_uart_dev*)(obj->obj);
    dev->signal = obj2;
    dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

    if (dev->signal_iface == NULL)
    {
        if (dev->signal == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", GENERAL_SIGNAL_INTF_NAME);
        } else
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

        return Not_found_exp;
    }
    return No_exp;
}

static exception_t general_signal_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    tsc695f_uart_dev* dev = (tsc695f_uart_dev*)(obj->obj);
    *obj2 = dev->signal;
    *port = NULL;
    return No_exp;
}

static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    tsc695f_uart_dev* dev = (tsc695f_uart_dev*)(obj->obj);
    dev->uart_file = obj2;
    dev->uart_file_iface = (uart_file_intf*)SKY_get_iface(obj2, UART_FILE_INTF);

    if (dev->uart_file_iface == NULL)
    {
        if (dev->uart_file == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", UART_FILE_INTF);
        } else
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", UART_FILE_INTF, obj2->objname);

        return Not_found_exp;
    }
    return No_exp;
}

static exception_t uart_file_get(conf_object_t *obj, conf_object_t **obj2, char **port, int index)
{
    tsc695f_uart_dev* dev = (tsc695f_uart_dev*)(obj->obj);
    *obj2 = dev->uart_file;
    *port = NULL;
    return No_exp;
}

void init_tsc695f_uart()
{
    static skyeye_class_t class_data =
    {
        .class_name = "tsc695f_uart",
        .class_desc = "tsc695f uart",
        .new_instance = new_tsc695f_uart,
        .reset_instance = reset_tsc695f_uart,
        .free_instance = free_tsc695f_uart,
        .module_type = SKYML_ONCHIP(SC_tsc695f),
    };
    conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);

    static const memory_space_intf io_memory =
    {
        .read = tsc695f_uart_read,
        .write = tsc695f_uart_write,
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

    static const skyeye_reg_intf reg_intf =
    {
        .get_regvalue_by_id = tsc695f_get_regvalue_by_id,
        .get_regname_by_id = tsc695f_uart_get_regname_by_id,
        .set_regvalue_by_id = tsc695f_uart_set_regvalue_by_id,
        .get_regnum = tsc695f_uart_get_regnum,
        .get_regid_by_name = tsc695f_uart_get_regid_by_name,
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

    static const struct InterfaceDescription ifaces[] =
    {
        (struct InterfaceDescription)
        {
            .name = SKYEYE_REG_INTF,
            .iface = &reg_intf,
        },
        (struct InterfaceDescription)
        {
            .name = MEMORY_SPACE_INTF_NAME,
            .iface = &io_memory,
        }
    };
    static const struct ConnectDescription connects[] =
    {
        (struct ConnectDescription )
        {
            .name = SKYEYE_UART_INTF,
            .set = term_set,
            .get = term_get,
        },
        (struct ConnectDescription )
        {
            .name = UART_FILE_INTF,
            .set = uart_file_set,
            .get = uart_file_get,
        },
        (struct ConnectDescription )
        {
            .name = GENERAL_SIGNAL_INTF_NAME,
            .set = general_signal_set,
            .get = general_signal_get,
        },

    };
    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);

    tsc695f_register_attribute(clss);
    return;
}
