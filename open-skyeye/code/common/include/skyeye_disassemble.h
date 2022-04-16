#ifndef __SKYEYE_DISASSEMBLE__
#define  __SKYEYE_DISASSEMBLE__
typedef char *(*disassemble_t)(conf_object_t *core, uint32_t addr);

exception_t register_disassemble_func(disassemble_t disassemble);
char *skyeye_disassemble(conf_object_t *core, int32_t addr);
#endif
