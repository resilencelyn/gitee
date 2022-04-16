/*
 * =====================================================================================
 *
 *       Filename:  leon2_uart.c
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
#include <simics/base_types.h>
#include <skyeye_dev_intf.h>
#include "skyeye_uart.h"

#include "uart_leon2.h"

static char* leon2_uart_attr[] = {"term"};
static exception_t leon2_uart_set_attr(conf_object_t* opaque, const char* attr_name, attr_value_t* value)
{
    leon2_uart_dev *dev = opaque->obj;
    int index;
    //parse the parameter
    for(index = 0; index < 3; index++){
        if(!strncmp(attr_name, leon2_uart_attr[index], strlen(leon2_uart_attr[index])))
            break;
    }
    switch(index){
        case 0:
            dev->term = (skyeye_uart_intf*)SKY_get_iface(value->u.object, SKYEYE_UART_INTF);
            break;
        default:
            printf("parameter error\n");
            return Invarg_exp;
    }
    return No_exp;
}
/*
 * ===  FUNCTION  ======================================================================
 *         Name:  leon2_uart_write
 *  Description:  This function perform the writing in the UART device
 * =====================================================================================
 */
static exception_t leon2_uart_write(conf_object_t *opaque, generic_address_t offset, uint32_t* buf, size_t count)
{
    leon2_uart_dev *dev = opaque->obj;
    skyeye_uart_intf* skyeye_uart = dev->term;
    //	skyeye_log(Error_log, __FUNCTION__, "write : %x, %s, %d\n", offset, opaque->objname, *buf);
    char* name = NULL;
    uint32_t data = 0;

    switch(offset)
    {
        case UART_DATA_REGISTER:
            dev->regs.data = *buf;
            if (dev->term_dev.obj) {
                dev->term_dev.term->write(dev->term_dev.obj, buf, 1);
            }
            if (dev->uart_file){
                dev->uart_file_iface->write(dev->uart_file, buf, 1);
            }
            if (dev->uart_com){
                dev->uart_com_iface->write(dev->uart_com, buf, 1);  //output to rs232
            }

            dev->regs.status.value |= TH;

            if((UCONTROL&TE) && (UCONTROL&TI) && (USTATUS& TH) && (dev->signal_iface)){
                dev->signal_iface->raise_signal(dev->signal, dev->int_number);
            }

            break;
        case UART_STATUS_REGISTER:
            break;
        case UART_CONTROL_REGISTER:
            dev->regs.control.value = *buf;
            if((*buf) & TI){
                dev->regs.status.value |= TH;
                if(dev->signal_iface)
                    dev->signal_iface->raise_signal(dev->signal, dev->int_number);
            }
            break;
        case UART_SCALER_REGISTER:
            dev->regs.scaler = *buf;
            break;
        case UART_FP1_REGISTER:
            dev->regs.fp1 = *buf;
            break;
        case UART_FP2_REGISTER:
            dev->regs.fp2 = *buf;
            uint64_t f = dev->regs.fp1;
            f = (f << 32);
            f |= dev->regs.fp2;
            double result = *(double *)&f;
            char buf[100];
            sprintf(buf, "%lf", result);
            char c;
            int i = 0;
            while(i <= strlen(buf)){
                c = buf[i];
                if(skyeye_uart)
                    skyeye_uart->write(skyeye_uart->conf_obj, &c, 1);
                if(dev->uart_term.serial_dev)
                    //	printf("%s %s : %c\n", opaque->objname, __func__, *(char*)buf);
                    dev->uart_term.serial_dev->write(dev->uart_term.obj, c);
                i++;
            }
            break;
        default:
            //printf("write %s error offset %d : 0x%x\n",
            //		dev->obj->objname, offset, *buf);
            break;
    }
    return No_exp;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  leon2_uart_read
 *  Description:  This function performs the reading in the UART device.
 * =====================================================================================
 */
static exception_t leon2_uart_read(conf_object_t *opaque, generic_address_t offset, void* buf, size_t count)
{
    leon2_uart_dev *dev = opaque->obj;
    //	skyeye_log(Error_log, __FUNCTION__, "read : %s, %x\n", opaque->objname, offset);

    //*buf = ((uint32 *)(&(dev->regs)))[offset];

    switch(offset)
    {
        case UART_DATA_REGISTER:
            if(dev->regs.control.flag.receiver_enable == 1){
                *(uint32*)buf = dev->fifo.buf[dev->fifo.start++];
            }
            if(dev->fifo.start >= dev->fifo.end){
                dev->regs.status.flag.data_ready = 0;
                dev->fifo.start = 0;
                dev->fifo.end = 0;
            }
            break;
        case UART_STATUS_REGISTER:
            *(uint32*)buf =dev->regs.status.value;
            break;
        case UART_CONTROL_REGISTER:
            *(uint32*)buf =dev->regs.control.value;
            break;
        case UART_SCALER_REGISTER:
            *(uint32*)buf =dev->regs.scaler;
            break;
        default:
            break;
    }
    DBG_LEON2_UART("read leon2 uart %s offset %d : 0x%x\n", dev->obj->objname, offset, *(uint32*)buf);

    return No_exp;
}

static exception_t serial_write(conf_object_t *obj, uint32_t value)
{
    leon2_uart_dev *dev = obj->obj;
    dev->fifo.buf[dev->fifo.end++] = (char)value;
    dev->regs.status.flag.data_ready = 1;

    return No_exp;
}
static uint32_t  serial_write_chars(conf_object_t *obj, char *buf, uint32_t count)
{
    leon2_uart_dev *dev = obj->obj;
    memcpy(dev->fifo.buf + dev->fifo.end, buf, count);
    dev->fifo.end += count;
    dev->regs.status.flag.data_ready = 1;
    if(UCONTROL & RI && dev->signal_iface && dev->fifo.start < dev->fifo.end){
        dev->signal_iface->raise_signal(dev->signal, 3);
    }
    return count;
}

char* leon2_uart_get_regname_by_id(conf_object_t* conf_obj, uint32_t id)
{
    leon2_uart_dev *dev = conf_obj->obj;
    return dev->regs_name[id];
}

uint32_t leon2_get_regvalue_by_id(conf_object_t* conf_obj, uint32_t id)
{
    leon2_uart_dev *dev = conf_obj->obj;
    uint32_t* regs_value = (uint32_t*)&(dev->regs) + id;
    return *regs_value;
}


static char* leon2_uart_set_regvalue_by_id(conf_object_t* conf_obj, uint32_t value, uint32_t id)
{
    leon2_uart_dev *dev = conf_obj->obj;
    struct registers* regs = &(dev->regs);
    *((uint32_t*)regs + id) = value;

    return NULL;
}

static uint32_t leon2_uart_get_regnum(conf_object_t* conf_obj, uint32_t id)
{
    leon2_uart_dev *dev = conf_obj->obj;
    struct registers* regs = &(dev->regs);
    return  6;


}

static uint32_t leon2_uart_get_regid_by_name(conf_object_t* conf_obj, char *name)
{
    leon2_uart_dev *dev = conf_obj->obj;
    struct registers* regs = &(dev->regs);
    uint32_t regnum;
    regnum = sizeof(struct registers) / 4;;
    int i;
    for(i =0; i < regnum; i++){
        if(strcmp(name, regs_name[i]) == 0)
            return i;
    }
    return regnum;


}
/*
 * ===  FUNCTION  ======================================================================
 *         Name:  new_leon2_uart
 *  Description:  instance leon2_uart.
 *
 *  The function returns base class of point which point to the leon2_uart.
 * =====================================================================================
 */
static conf_object_t* new_leon2_uart(char* obj_name)
{
    leon2_uart_dev* leon2_uart = skyeye_mm_zero(sizeof(leon2_uart_dev));
    leon2_uart->regs_name = &regs_name;
    /*  Clear the UART register   */
    leon2_uart->obj = new_conf_object(obj_name, leon2_uart);
    leon2_uart->fifo.start = 0;
    leon2_uart->fifo.end = 0;

    leon2_uart->regs.status.flag.data_ready = 0;

    return leon2_uart->obj;
}

static exception_t reset_leon2_uart(conf_object_t* opaque, const char* args)
{
    leon2_uart_dev *dev = opaque->obj;
    memset(&(dev->regs), 0, sizeof(dev->regs));
    /*  Enable the transmitter and receiver by default  */
    dev->regs.control.flag.transmitter_enable = 0;
    dev->regs.control.flag.receiver_enable = 0;

    /*  Indicate that the transmitter hold register is EMPTY    */
    dev->regs.status.flag.transmitter_hold_register_empty = 1;
    dev->regs.status.flag.transmitter_shift_register_empty = 1;

    return No_exp;
}

static void free_leon2_uart(conf_object_t* conf_obj)
{
    leon2_uart_dev* dev = conf_obj->obj;
    skyeye_free(dev->obj);
    skyeye_free(dev);
}

static attr_value_t get_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
    leon2_uart_dev* dev = (leon2_uart_dev*)(conf_obj->obj);
    attr_value_t int_number = SKY_make_attr_uinteger(dev->int_number);
    return int_number;
}
static exception_t set_attr_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx){
    leon2_uart_dev* dev = (leon2_uart_dev*)(conf_obj->obj);
    dev->int_number = SKY_attr_uinteger(*value);
    return No_exp;
}


static exception_t uart_term_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    leon2_uart_dev* dev = (leon2_uart_dev*)(obj->obj);
    dev->term_dev.obj = obj2;
    dev->term_dev.term = (skyeye_uart_intf*)SKY_get_iface(obj2, SKYEYE_UART_INTF);
    if (dev->term_dev.term == NULL){
        if (dev->term_dev.obj == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.\n", SKYEYE_UART_INTF);
        } else
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.\n", SKYEYE_UART_INTF, obj2->objname);

        return Not_found_exp;
    }

    return No_exp;
}

static exception_t uart_term_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    leon2_uart_dev* dev = (leon2_uart_dev*)(obj->obj);
    *obj2 = dev->term_dev.obj;
    *port = NULL;
    return No_exp;
}


static exception_t uart_file_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    leon2_uart_dev* dev = (leon2_uart_dev*)(obj->obj);
    dev->uart_file = obj2;
    dev->uart_file_iface = (uart_file_intf*)SKY_get_iface(obj2, UART_FILE_INTF);
    if (dev->uart_file_iface == NULL){
        if (dev->uart_file == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.\n", UART_FILE_INTF);
        } else
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.\n", UART_FILE_INTF, obj2->objname);

        return Not_found_exp;
    }
    return No_exp;
}

static exception_t uart_file_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    leon2_uart_dev* dev = (leon2_uart_dev*)(obj->obj);
    *obj2 = dev->uart_file;
    *port = NULL;
    return No_exp;
}

static exception_t uart_signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    leon2_uart_dev* dev = (leon2_uart_dev*)(obj->obj);
    dev->signal = obj2;
    dev->signal_iface = (general_signal_intf *)SKY_get_iface(dev->signal, GENERAL_SIGNAL_INTF_NAME);

    if (dev->signal_iface == NULL){
        if (dev->signal == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.\n", GENERAL_SIGNAL_INTF_NAME);
        } else
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.\n", GENERAL_SIGNAL_INTF_NAME, obj2->objname);

        return Not_found_exp;
    }

    return No_exp;
}

static exception_t uart_signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    leon2_uart_dev* dev = (leon2_uart_dev*)(obj->obj);
    *obj2 = dev->signal;
    *port = NULL;
    return No_exp;
}

static exception_t uart_com_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    leon2_uart_dev* dev = (leon2_uart_dev*)(obj->obj);
    dev->uart_com = obj2;
    dev->uart_com_iface = (uart_com_intf*)SKY_get_iface(obj2, UART_COM_INTF);

    if (dev->uart_com_iface == NULL){
        if (dev->uart_com == NULL)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.\n", UART_COM_INTF);
        } else
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.\n", UART_COM_INTF, obj2->objname);

        return Not_found_exp;
    }
    return No_exp;
}

static exception_t uart_com_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    leon2_uart_dev* dev = (leon2_uart_dev*)(obj->obj);
    *obj2 = dev->uart_com;
    *port = NULL;
    return No_exp;
}


static void leon2_register_attribute(conf_class_t* clss) {
    SKY_register_attribute(clss, "irq_number", get_attr_int_number, NULL, set_attr_int_number, NULL, SKY_Attr_Optional, "uinteger", "interrupt number of leon2 uart");
    return;
}



void init_leon2_uart(){
    static skyeye_class_t class_data = {
        .class_name = "leon2_uart",
        .class_desc = "leon2 uart",
        .new_instance = new_leon2_uart,
        .reset_instance = reset_leon2_uart,
        .free_instance = free_leon2_uart,
        .get_attr = NULL,
        .set_attr = leon2_uart_set_attr,
        .module_type = SKYML_ONCHIP(SC_leon2),
    };
    conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
    static const skyeye_serial_device_t serial_device = {
        .write = serial_write,
        .receive_ready = NULL,
        .write_chars = serial_write_chars,
    };
    SKY_register_iface(clss, SERIAL_DEVICE_INTERFACE, &serial_device);

    static const memory_space_intf io_memory = {
        .read = leon2_uart_read,
        .write = leon2_uart_write,
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);

    static const skyeye_reg_intf reg_intf = {
        .get_regvalue_by_id = leon2_get_regvalue_by_id,
        .get_regname_by_id = leon2_uart_get_regname_by_id,
        .set_regvalue_by_id = leon2_uart_set_regvalue_by_id,
        .get_regnum = leon2_uart_get_regnum,
        .get_regid_by_name = leon2_uart_get_regid_by_name,
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);

    static const struct InterfaceDescription ifaces[] = {
        (struct InterfaceDescription) {
            .name = SERIAL_DEVICE_INTERFACE,
            .iface = &serial_device,
        },
        (struct InterfaceDescription) {
            .name = SKYEYE_REG_INTF,
            .iface = &reg_intf,
        },
        (struct InterfaceDescription) {
            .name = MEMORY_SPACE_INTF_NAME, 
            .iface = &io_memory,
        },
    };
    static const struct ConnectDescription connects[] = {
        (struct ConnectDescription ){
            .name = SKYEYE_UART_INTF,
            .set = uart_term_set,
            .get = uart_term_get,
        },
        (struct ConnectDescription ){
            .name = UART_FILE_INTF,
            .set = uart_file_set,
            .get = uart_file_get,
        },
        (struct ConnectDescription) {
            .name = GENERAL_SIGNAL_INTF_NAME,
            .set = uart_signal_set,
            .get = uart_signal_get,
        },
        (struct ConnectDescription) {
            .name = UART_COM_INTF,
            .set = uart_com_set,
            .get = uart_com_get,
        }

    };
    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);

    leon2_register_attribute(clss);
    return;
}
