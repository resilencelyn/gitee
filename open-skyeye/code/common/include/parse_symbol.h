#ifndef _PARSE_SYMBO_H
#define _PARSE_SYMBO_H

typedef enum {
    POINTER,       // 指针类型数据
    NON_POINTER,   // 非指针类型数据
    ARRAY,         // 数组类型数据
    NON_ARRAY,     // 非数组类型数据
    ARRAY_BOUND,   // 数组越界
    NO_MEMBER,     // 没有该结构体成员
    SYM_OK,        // 解析成功
    NO_CORE,       // 没有这个cpu配置
    ARRAY_NUM_ERR, // 数组下标错误
    NO_GLOBAL_VAR, // 不是一个全局变量
    NO_Bin_PARSE,  // 没有解析二进制文件
    SYM_TOO_LONG,  // 符号长度太长
    NO_EXP,
} sym_info_t;

typedef enum {
    GV_BIT8  = 1,
    GV_BIT16 = 2,
    GV_BIT32 = 4,
    GV_BIT64 = 8,
} sym_type_t;

#define GV_VAL_PTR(g) (void *)(&(g))

#ifdef __cplusplus

extern "C" void free_core_symbol_info_list();
extern "C" void parse_symbol_xml(char *cpu_name, char *xml_filename);
extern "C" int parse_xml_output();
extern "C" int SkyEye_GetSymbol_Addr(char *cpu_name, char *sym_str, uint32_t *global_addr);
extern "C" uint32_t SkyEye_GetFunc_Addr(char *cpu_name, char *func_str);
extern "C" uint32_t SkyEye_GetFunc_Length(char *cpu_name, char *func_str);
extern "C" int SkyEye_GetSymbol_Value(char *cpu_name, char *sym_str, void *value, sym_type_t count);
extern "C" int SkyEye_SetSymbol_Value(char *cpu_name, char *sym_str, void *value, sym_type_t count);
extern "C" int SkyEye_SetSymbol_Value_By_Py(char *cpu_name, char *sym_str, uint64_t value, sym_type_t count);
extern "C" uint64_t SkyEye_GetSymbol_Value_By_Py(char *cpu_name, char *sym_str, sym_type_t count);
extern "C" void SkyEye_GetSymbol_GetErrorMsg(int flag, const char **ret_msg);
extern "C" float SkyEye_GetFloatSymbol_Value(char *cpu_name, char *sym_str, sym_type_t count);
extern "C" double SkyEye_GetDoubleSymbol_Value(char *cpu_name, char *sym_str, sym_type_t count);
extern "C" uint32_t SkyEye_SetFloatSymbol_Value(char *cpu_name, char *sym_str, float value, sym_type_t count);
extern "C" uint32_t SkyEye_SetDoubleSymbol_Value(char *cpu_name, char *sym_str, double value, sym_type_t count);

#else

#ifdef __WIN32__
#define EXPORT_API __declspec (dllexport)
#else
#define EXPORT_API
#endif

EXPORT_API void free_core_symbol_info_list();
EXPORT_API void parse_symbol_xml(char *cpu_name, char *xml_filename);
EXPORT_API int parse_xml_output();
EXPORT_API int SkyEye_GetSymbol_Addr(char *cpu_name, char *sym_str, uint32_t *global_addr);
EXPORT_API uint32_t SkyEye_GetFunc_Addr(char *cpu_name, char *func_str);
EXPORT_API uint32_t SkyEye_GetFunc_Length(char *cpu_name, char *func_str);
EXPORT_API int SkyEye_GetSymbol_Value(char *cpu_name, char *sym_str, void *value, sym_type_t count);
EXPORT_API int SkyEye_SetSymbol_Value(char *cpu_name, char *sym_str, void *value, sym_type_t count);
EXPORT_API void SkyEye_GetSymbol_GetErrorMsg(int flag, const char **ret_msg);

#endif

#endif
