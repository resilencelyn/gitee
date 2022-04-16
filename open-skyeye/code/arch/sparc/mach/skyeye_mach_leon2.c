/*
 * =====================================================================================
 *
 *       Filename:  skyeye_mach_leon2.c
 *
 *    Description:  Implementation of the LEON2 processor
 *
 *        Version:  1.0
 *        Created:  15/04/08 15:09:38
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aitor Viana Sanchez (avs), aitor.viana.sanchez@esa.int
 *        Company:  European Space Agency (ESA-ESTEC)
 *
 * =====================================================================================
 */

/*-----------------------------------------------------------------------------
 *  23/06/08 15:24:39 Aitor Viana Sanchez
 *-----------------------------------------------------------------------------*/

#include "skyeye_config.h"
#include "skyeye_addr_space.h"
#include "skyeye_class.h"
#include "skyeye_uart_ops.h"
#include "skyeye_attr.h"
#include "skyeye_obj.h"
#include "skyeye_class.h"
#include "skyeye_interface.h"

#include "../common/types.h"
#include "../common/bits.h"

#include "mach_leon2_regmap.h"
#include "mach_leon2_io.h"
#include "leon2_uart.h"

#define	STR_LEN 128

static exception_t parse_ram(machine_config_t * mach)
{
    char *objname = NULL;
    uint32_t addr, size;
    attr_value_t *value;
    char image_name[STR_LEN];
    int i, ret;

    for (i = 0; i < mach->mem.bank_num; i++)
    {
        /* ram 's name */
        objname = mach->mem.mem_banks[i].objname;
        /* image's name is ram's name + image */
        get_strcat_objname(&image_name[0], objname, "image");
        addr = mach->mem.mem_banks[i].addr;
        size = mach->mem.mem_banks[i].len;
        /* instance a image class */
        conf_object_t *image = pre_conf_obj(image_name, "image");

        value = make_new_attr(Val_UInteger, size);
        ret = set_conf_attr(image, "size", value);
        /* instance a ram class */
        conf_object_t *ram = pre_conf_obj(objname, "ram");

        value = make_new_attr(Val_Object, image);
        ret = set_conf_attr(ram, "image", value);
        printf("In %s Line %d\n", __func__, __LINE__);

        memory_space_intf *ram_io_memory = (memory_space_intf *) SKY_get_interface(ram, MEMORY_SPACE_INTF_NAME);

        ret = add_map(mach->phys_mem, addr, size, 0x0, ram_io_memory, 1, 1);
        if (ret != No_exp)
        {
            skyeye_log(Error_log, __FUNCTION__, "Can not register io memory for system controller\n");
        }
    }

    return No_exp;
}
typedef struct leon2_mach
{
    conf_object_t *obj;
    common_machine_attr_t attr;
} leon2_mach_t;

/*-----------------------------------------------------------------------------
 *  PUBLIC INTERFACE
 *-----------------------------------------------------------------------------*/
exception_t leon2_dev_init(machine_config_t * mach)
{
    exception_t ret;
    attr_value_t *value;

    DBG_leon2("In %s, Line %d init leon2 deivce\n", __func__, __LINE__);
    /* The whole address space */
    //      mach->phys_mem= new_addr_space("leon2_mach_space");

    conf_object_t *sparc_cpu = pre_conf_obj("sparc_cpu0", "sparc_dyncom_cpu");

    mach->phys_mem = (addr_space_t *) SKY_get_interface(sparc_cpu, ADDR_SPACE_INTF_NAME);

    /* register interface for uart */
    conf_object_t *uart_term0 = pre_conf_obj("uart_term_0", "uart_term");
    conf_object_t *uart0 = pre_conf_obj("leon2_uart_0", "leon2_uart");

    value = make_new_attr(Val_Object, uart_term0);
    ret = set_conf_attr(uart0, "term", value);

    memory_space_intf *uart0_io_memory = (memory_space_intf *) SKY_get_interface(uart0, MEMORY_SPACE_INTF_NAME);

    ret = add_map(mach->phys_mem, 0x80000070, 0x10, 0x0, uart0_io_memory, 1, 1);
    if (ret != No_exp)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not register io memory for uart0 controller\n");
    }
    //#ram: name=sdram, map=M, type=RW, addr=0x40000000, size=0x3fffffff
    conf_object_t *image = pre_conf_obj("flash_image", "image");

    value = make_new_attr(Val_UInteger, 0x20000000);
    ret = set_conf_attr(image, "size", value);
    value = make_new_attr(Val_String, "./flash.image");
    ret = set_conf_attr(image, "files", value);

    conf_object_t *image0 = pre_conf_obj("image0", "image");

    value = make_new_attr(Val_UInteger, 0x40000000);
    ret = set_conf_attr(image0, "size", value);
    conf_object_t *ram0 = pre_conf_obj("ram0", "ram");

    value = make_new_attr(Val_Object, image0);
    ret = set_conf_attr(ram0, "image", value);
    memory_space_intf *ram0_io_memory = (memory_space_intf *) SKY_get_interface(ram0, MEMORY_SPACE_INTF_NAME);

    ret = add_map(mach->phys_mem, 0x40000000, 0x40000000, 0x0, ram0_io_memory, 1, 1);

    conf_object_t *flash = pre_conf_obj("amd29lv640d", "amd_flash");

    value = make_new_attr(Val_Object, image);
    ret = set_conf_attr(flash, "image", value);

    memory_space_intf *flash_io_memory = (memory_space_intf *) SKY_get_interface(flash, MEMORY_SPACE_INTF_NAME);

    ret = add_map(mach->phys_mem, 0x0, 0x20000000, 0x0, flash_io_memory, 1, 1);

#if 0
    conf_object_t *uart_term1 = pre_conf_obj("uart_term_1", "uart_term");
    conf_object_t *uart1 = pre_conf_obj("leon2_uart_1", "leon2_uart");

    value = make_new_attr(Val_Object, uart_term1);
    ret = set_conf_attr(uart1, "term", value);
    memory_space_intf *uart1_io_memory = (memory_space_intf *) SKY_get_interface(uart1, MEMORY_SPACE_INTF_NAME);

    ret = add_map(mach->phys_mem, 0x80000070, 0x10, 0x0, uart1_io_memory, 1, 1);
    if (ret != No_exp)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not register io memory for uart1 controller\n");
    }
#endif

#if 0
    conf_object_t *image0 = pre_conf_obj("image0", "image");
    attr_value_t *value = make_new_attr(Val_UInteger, 0x80000000);

    ret = set_conf_attr(image0, "size", value);

    conf_object_t *ram0 = pre_conf_obj("ram0", "ram");

    value = make_new_attr(Val_Object, image0);
    ret = set_conf_attr(ram0, "image", value);
    memory_space_intf *ram0_io_memory = (memory_space_intf *) SKY_get_interface(ram0, MEMORY_SPACE_INTF_NAME);

    ret = add_map(mach->phys_mem, 0x00000000, 0x80000000, 0x0, ram0_io_memory, 1, 1);

    conf_object_t *image1 = pre_conf_obj("image1", "image");

    value = make_new_attr(Val_UInteger, 0x2fffffff);
    ret = set_conf_attr(image1, "size", value);

    conf_object_t *ram1 = pre_conf_obj("ram1", "ram");

    value = make_new_attr(Val_Object, image1);
    ret = set_conf_attr(ram1, "image", value);
    memory_space_intf *ram1_io_memory = (memory_space_intf *) SKY_get_interface(ram1, MEMORY_SPACE_INTF_NAME);

    ret = add_map(mach->phys_mem, 0xd0000000, 0x2fffffff, 0x0, ram1_io_memory, 1, 1);
#else
    parse_ram(mach);
#endif
    return No_exp;
}

conf_object_t *new_leon2_mach(const char *obj_name)
{

    leon2_mach_t *mach = skyeye_mm_zero(sizeof (leon2_mach_t));

    mach->obj = new_conf_object(obj_name, mach);

    mach->attr.core_num = 0;
    skyeye_machine_intf *inface = skyeye_mm_zero(sizeof (skyeye_machine_intf));

    inface->conf_obj = mach->obj;
    inface->set_space = mach_set_space;
    inface->set_core = mach_set_core;
    inface->get_space = mach_get_space;
    SKY_register_interface((void *) inface, obj_name, MACHINE_INTF_NAME);

    return mach->obj;
}

void free_leon2_mach(char *objname)
{
}

void reset_leon2_mach(char *objname)
{
}

void leon2_mach_fini(void *arch_instance, machine_config_t * mach)
{
    space_obj_free(mach->phys_mem);
}

void init_leon2_mach()
{
    static skyeye_class_t class_data = {
        .class_name = "leon2",
        .class_desc = "leon2 machine",
        .new_instance = new_leon2_mach,
        .reset_instance = reset_leon2_mach,
        .free_instance = free_leon2_mach,
        .get_attr = NULL,
        .set_attr = NULL
    };

    SKY_register_soc_class(class_data.class_name, &class_data);
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  leon2_mach_init
 *  Description:  This function initialezes the machine.
 *  In the LEON2 architecture the On-Chip registers are mapped in the address
 *  0x80000000. The io_read/write functions will be the ones reading this
 *  address.
 *  For the rest of the peripherals, the io2_read/write functions are provided.
 * =====================================================================================
 */
#if 0
void leon2_mach_init(void *arch_instance, machine_config_t * mach)
{

    // FIXME!: The state is not treated yet
    // Also some more initializations need to be performed
    machine_config_t *this_mach = mach;

    if (!this_mach)
    {
        SKYEYE_ERR("Error: No machine config structure\n");
        exit(-1);
    } else
    {
        SKYEYE_INFO("%s(): %s initialized\n", __func__, mach->machine_name);
    }

    /*  These routines are defined in the mach_leon2_io.c source file   */
    // read functions
    this_mach->mach_io_read_byte = leon2_io_read_byte;
    this_mach->mach_io_read_halfword = leon2_io_read_word;
    this_mach->mach_io_read_word = leon2_io_read_long;
    // write functions
    this_mach->mach_io_write_byte = leon2_io_write_byte;
    this_mach->mach_io_write_halfword = leon2_io_write_word;
    this_mach->mach_io_write_word = leon2_io_write_long;
    // other functions
    this_mach->mach_io_do_cycle = leon2_io_do_cycle;
    this_mach->mach_io_reset = leon2_io_reset;
    this_mach->mach_set_intr = leon2_set_int;
    this_mach->mach_fini = leon2_mach_fini;
    leon2_dev_init(this_mach);
}
#endif
