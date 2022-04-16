/* Copyright (C) 
* 2013 - Michael.Kang blackfin.kang@gmail.com
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
* @file skyeye_class.h
* @brief 
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-09-29
*/

#ifndef __SKYEYE_CLASS_H__
#define __SKYEYE_CLASS_H__
#include <skyeye_obj.h>
#include <skyeye_attr.h>

typedef enum{
	/* Need to be saved for checkpointing */
	Class_Persistent,
	/* Do not need to be saved for checkpointing */
	Class_temp
}class_kind_t;
typedef exception_t (*connect_set_t)(conf_object_t *dev1, conf_object_t *dev2, const char *port, int idx);
typedef exception_t (*connect_get_t)(conf_object_t *dev1, conf_object_t **dev2, char **port, int idx);

struct ConnectDescription {
   const char *name;
   connect_set_t set;
   connect_get_t get;
};

struct InterfaceDescription {
   const char *name;
   void const *iface;
};

typedef struct ifaces_list{
	const struct InterfaceDescription *ifaces; 
	uint32_t cnt;
}ifaces_t;

typedef struct connects_list{
	const struct ConnectDescription *connects;
	uint32_t cnt;
}connects_t;

typedef enum {
	ST_OFFCHIP = 0,
	ST_ONCHIP  = 1,
} chip_type_t;

typedef struct {
	chip_type_t  c_type;
	const char * c_soc;
} module_types_t;

#define MAKE_CHIP_SOC(sc) #sc"|"

// arm
#define SC_am335x MAKE_CHIP_SOC(am335x)
#define SC_am3359 MAKE_CHIP_SOC(am3359)
#define SC_at91rm9200 MAKE_CHIP_SOC(at91rm9200)
#define SC_imx6 MAKE_CHIP_SOC(imx6)
#define SC_omap4460_mach MAKE_CHIP_SOC(omap4460_mach)
#define SC_omapl138 MAKE_CHIP_SOC(omapl138)
#define SC_s3c2410x1 MAKE_CHIP_SOC(s3c2410x1)
#define SC_s3c6410 MAKE_CHIP_SOC(s3c6410)
#define SC_stm32 MAKE_CHIP_SOC(stm32)

//c6kplus
#define SC_c6415 MAKE_CHIP_SOC(c6415)
#define SC_c6416 MAKE_CHIP_SOC(c6416)
#define SC_c6446 MAKE_CHIP_SOC(c6446)
#define SC_c6455 MAKE_CHIP_SOC(c6455)
#define SC_c6457 MAKE_CHIP_SOC(c6457)
#define SC_c6474 MAKE_CHIP_SOC(c6474)
#define SC_c6678 MAKE_CHIP_SOC(c6678)
#define SC_c6713 MAKE_CHIP_SOC(c6713)
#define SC_c6746 MAKE_CHIP_SOC(c6746)
#define SC_c6748 MAKE_CHIP_SOC(c6748)

//c28
#define SC_c2812 MAKE_CHIP_SOC(c2812)
#define SC_f28335 MAKE_CHIP_SOC(f28335)

//c54
#define SC_c5416 MAKE_CHIP_SOC(c5416)

//c55
#define SC_c55 MAKE_CHIP_SOC(c55)
#define SC_c5510 MAKE_CHIP_SOC(c5510)

//ppc
#define SC_mpc755 MAKE_CHIP_SOC(mpc755)
#define SC_mpc5121 MAKE_CHIP_SOC(mpc5121)
#define SC_mpc8245 MAKE_CHIP_SOC(mpc8245)
#define SC_mpc8378 MAKE_CHIP_SOC(mpc8378)
#define SC_mpc8560 MAKE_CHIP_SOC(mpc8560)
#define SC_mpc8572_machine MAKE_CHIP_SOC(mpc8572_machine)
#define SC_mpc8641d_mach MAKE_CHIP_SOC(mpc8641d_mach)
#define SC_p1010 MAKE_CHIP_SOC(p1010)
#define SC_t2080 MAKE_CHIP_SOC(t2080)

//sparc
#define SC_leon2 MAKE_CHIP_SOC(leon2)
#define SC_tsc695f MAKE_CHIP_SOC(tsc695f)

// Off-chip device
#define SKYML_OFFCHIP {ST_OFFCHIP, NULL}

// On-chip device
#define SKYML_ONCHIP(soc_list) {ST_ONCHIP, soc_list}

typedef struct skyeye_class{
	const char* class_name;
	char* class_desc;
	conf_object_t* (*new_instance)(const char* obj_name);
	exception_t (*free_instance)(conf_object_t* obj);
	exception_t (*reset_instance)(conf_object_t* obj, const char* parameter);
	attr_value_t* (*get_attr)(const char* attr_name, conf_object_t* obj);
	exception_t (*set_attr)(conf_object_t* obj, const char* attr_name, attr_value_t* value);
	exception_t (*config_instance)(conf_object_t* obj);
	module_types_t module_type;
	class_kind_t kind;
	char** interface_list;
	ifaces_t iface_list;
	connects_t connect_list;
	const struct InterfaceDescription *ifaces; 
	const struct ConnectDescription *connects;
	char *class_type;
}skyeye_class_t;

typedef struct skyeye_class class_data_t;
typedef struct log_group log_group_t;
typedef struct attribute_list attribute_list_t;
typedef struct interface_list interface_list_t;

typedef struct conf_class {
	struct skyeye_class class_data;
	log_group_t        *log_group;
	attribute_list_t   *attr;
	interface_list_t	*iface;
}conf_class_t;


#ifdef __cplusplus
 extern "C" {
#endif

conf_class_t* SKY_register_class(const char* name, class_data_t* skyeye_class);
conf_class_t* SKY_register_cpu_class(const char* name, class_data_t* skyeye_class);
conf_class_t* SKY_register_device_class(const char* name, class_data_t* skyeye_class);
conf_class_t* SKY_register_soc_class(const char* name, class_data_t* skyeye_class);
conf_class_t* SKY_register_linker_class(const char* name, class_data_t* skyeye_class);
conf_class_t* SKY_get_object_class(conf_object_t* obj);
conf_object_t* SKY_alloc_conf_object(const char *objname, const char *class_name);
conf_object_t* pre_conf_obj(const char* objname, const char* class_name);
exception_t reset_conf_obj(conf_object_t* obj);
exception_t config_conf_obj(conf_object_t* obj);
void free_conf_obj(conf_object_t* obj);
char **SKY_get_classname_list();
exception_t SKY_free_class_list();
void SKY_print_class_list();
conf_class_t* SKY_get_class(const char* name);
uint32_t SKY_get_obj_count();
conf_object_t *SKY_get_obj_index(int index);
exception_t SKY_delete_all_obj(void);
char **class_get_iface_list(conf_class_t *clss);
char **class_get_connect_list(conf_class_t *clss);


exception_t _class_register_ifaces(conf_class_t*clss,  const struct InterfaceDescription *ifaces, uint32_t count);

exception_t _class_register_connects(conf_class_t*clss,  const struct ConnectDescription *connects, uint32_t count);
#define IFACES_NULL  NULL
#define CONNECTS_NULL NULL

#define class_register_ifaces(clss, ifaces) _class_register_ifaces((clss), (ifaces), ifaces ? (sizeof(ifaces) / sizeof(ifaces[0])) : 0)

#define class_register_connects(clss, connects) _class_register_connects((clss), (connects), connects ? (sizeof(connects) / sizeof(connects[0])) : 0)



#ifdef __cplusplus
}
#endif

//SIM_register_typed_attribute

#endif
