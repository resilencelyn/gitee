#ifndef __SPARC_ARCH_INTERFACE_H__
#define __SPARC_ARCH_INTERFACE_H__
#ifdef __cplusplus
extern "C"
{
#endif

#include "iu.h"

    iu_config_t *iu;

    void sparc_init_state(conf_object_t * conf_obj);

    conf_object_t *new_sparc(const char *obj_name);
    exception_t free_sparc(conf_object_t * obj);
    exception_t config_sparc(conf_object_t * obj);
    exception_t reset_sparc(conf_object_t * conf_obj, const char *parameter);

    char *sparc_get_architecture(conf_object_t * obj);
    char *sparc_disassemble(conf_object_t * obj, generic_address_t addr);

    int sparc_insert_bp(conf_object_t * conf_obj, generic_address_t addr);
    int sparc_delete_bp(conf_object_t * conf_obj, generic_address_t addr);
    uint32_t sparc_get_bp_numbers(conf_object_t * conf_obj);
    uint32_t sparc_get_bp_addr_by_id(conf_object_t * conf_obj, int id);
    int sparc_get_trigger(conf_object_t * conf_obj);
    bool_t sparc_check_bp_trigger(conf_object_t * conf_obj);
    int sparc_clear_bp_trigger(conf_object_t * conf_obj);
    uint32_t sparc_get_regvalue_by_id(conf_object_t * conf_obj, uint32_t id);
    char *sparc_get_regname_by_id(conf_object_t * conf_obj, uint32_t id);
    exception_t sparc_set_regvalue_by_id(conf_object_t * conf_obj, uint32 value, uint32 id);

    uint32_t sparc_core_run(conf_object_t * conf_obj);
    void sparc_stepi(conf_object_t * conf_obj, uint64_t steps);
    void sparc_power(conf_object_t * conf_obj, int run);
    void sparc_stepi_check(conf_object_t * conf_obj);
    int sparc_breakpoint_check(conf_object_t * conf_obj);
    uint32 sparc_get_regid_by_name(conf_object_t * conf_obj, char *name);
    uint32 sparc_get_regnum(conf_object_t * opaque);

     ;
    uint32_t *sparc_get_reg_window(conf_object_t * obj);
    attr_value_t get_attr_int_regw_index(void *arg, conf_object_t * conf_obj, attr_value_t * idx);
    exception_t set_attr_int_regw_index(void *arg, conf_object_t * conf_obj, attr_value_t * value, attr_value_t * idx);
    uint32_t sparc_get_address_width(conf_object_t * obj);

    exception_t sparc_space_write(conf_object_t * obj, generic_address_t addr, void *buf, size_t count);
    exception_t sparc_space_read(conf_object_t * obj, generic_address_t addr, void *buf, size_t count);
    get_page_t *sparc_get_page(conf_object_t * obj, generic_address_t addr);
    exception_t memory_space_get(conf_object_t * obj, conf_object_t ** obj2, char **port, int index);
    exception_t memory_space_set(conf_object_t * obj, conf_object_t * obj2, const char *port, int index);
    exception_t sparc_signal(conf_object_t * conf_obj, interrupt_signal_t * signal);

    void sparc_register_attribute(conf_class_t * clss);
    int sparc_get_cpu_exception_num(conf_object_t * obj);
    void sparc_set_cpu_exception_by_id(conf_object_t * obj, int exception_id);
    char *sparc_get_cpu_exception_name_by_id(conf_object_t * obj, int exception_id);
#ifdef __cplusplus
}
#endif

#endif
