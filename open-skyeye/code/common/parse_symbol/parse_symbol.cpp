#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>
// SkyEye api header file
#include <skyeye_class.h>
#include <sim_control.h>
#include <skyeye_mach.h>
#include <skyeye_core.h>
#include <skyeye_mm.h>
#include <skyeye_addr_space.h>
#include <skyeye_core_intf.h>
#include <skyeye_cell.h>
#include <skyeye_iface.h>
#include <skyeye_interface.h>
#include <skyeye_types.h>
#include <parse_symbol.h>

#include "tinyxml2.h"

#define DEBUG_PARSE_SYMBOL 0

#if DEBUG_PARSE_SYMBOL
void debug_printf_func(int level, const char *format, ...)
{
    int i;

    for (i = 0; i < level; i++)
    {
        printf(" ");
    }

    char TempString[1024] = { '\0' };
    memset(TempString, 0, 1024);

    va_list args;

    va_start(args, format);
    vsprintf(TempString, format, args);
    va_end(args);

    printf("%s", TempString);
    fflush(stdout);
}

#define debug_printf debug_printf_func
#else
#define debug_printf
#endif

using namespace tinyxml2;

//using namespace std;

typedef enum
{
    DW_TAG_variable,                    // 全局变量
    DW_TAG_base_type,                   // 基础类型
    DW_TAG_pointer_type,                // 指针类型
    DW_TAG_structure_type,              // 机构体类型
    DW_TAG_array_type,                  // 数组类型
    DW_TAG_union_type,                  // 联合体类型
    DW_TAG_enumeration_type,            // 枚举类型
    DW_TAG_typedef,
    DW_TAG_volatile_type,
} tag_type_t;

typedef struct struct_s
{
    int id_base_type;                   // 变量类型的Die id
    int member_size;                    // 位域的宽度
    int member_offset;                  // 当前元素在当前结构体的偏移位置
} struct_t;

// 成员元素名称 --> struct_t
typedef std::vector < std::pair < std::string, struct_t * >>structure_map_t;

typedef struct die_node_s
{
    int id;                             // Die id
    tag_type_t tag;                     // Die 类型
    char *type_name;                    // 基础数据类型名称
    char *name;                         // 全局变量、结构体、枚举、联合体的名称
    int addr;                           // 全局变量的基地址
    int idref;                          // 变量类型的Die id
    int data_member_size;               // 数据成员的大小(基础类型、结构体、数组、枚举、联合体)
    structure_map_t *structure_type_map;        // 结构体、数组、枚举、联合体内部成员列表
} die_node_t;

typedef struct func_info
{
    uint32_t start_addr;
    uint32_t length;
} func_info_t;

// Die id --> die_node_t
typedef std::unordered_map < int, die_node_t * >sym_table_map_t;

//sym_table_map_t sym_table_map;

// 存放全局变量名 --> 对应的Die id
typedef std::unordered_map < std::string, int >global_table_map_t;

//global_table_map_t global_table_map;

//存放 <函数名，地址信息>
typedef std::unordered_map < std::string, func_info_t > func_map_t;

// 基础类型数据
#define   ST_INT         4
#define   ST_UINT        4
#define   ST_CHAR        1
#define   ST_UCHAR       1
#define   ST_USHORT      2
#define   ST_SHORT       2
#define   ST_FLOAT       4
#define   ST_DOUBLE      8
#define   ST_LONGDOUBLE  8

typedef std::unordered_map < std::string, int >base_type_map_t;
base_type_map_t base_type_map;

typedef struct
{
    conf_object_t *core;
    memory_space_intf *iface;
} core_memory_obj_t;

typedef struct
{
    char cpu_name;
    char xml_filename;
    core_memory_obj_t core_memory;

    // 符号信息表
    sym_table_map_t sym_table_map;

    // 存放全局变量名
    global_table_map_t global_table_map;

    // 存放函数名
    func_map_t func_map;
} core_symbol_info_t;

// cpu_name --> core_symbol_info_t
typedef std::unordered_map < std::string, core_symbol_info_t > core_symbol_info_list_t;

// 存放所有CPU核对应符号的列表
core_symbol_info_list_t core_symbol_info_list;

void free_core_symbol_info_list()
{
    core_symbol_info_list.clear();
    return;
}

void init_base_type()
{
    base_type_map["signed"] = ST_INT;
    base_type_map["signed int"] = ST_INT;
    base_type_map["int"] = ST_INT;
    base_type_map["long"] = ST_INT;
    base_type_map["long long"] = ST_INT;

    base_type_map["unsigned long"] = ST_UINT;

    base_type_map["unsigned int"] = ST_UINT;

    base_type_map["signed char"] = ST_CHAR;
    base_type_map["char"] = ST_CHAR;
    base_type_map["wchar_t"] = ST_CHAR;

    base_type_map["unsigned char"] = ST_UCHAR;

    base_type_map["unsigned short"] = ST_USHORT;
    base_type_map["short"] = ST_SHORT;

    base_type_map["float"] = ST_FLOAT;

    base_type_map["double"] = ST_DOUBLE;
    base_type_map["long double"] = ST_LONGDOUBLE;
}

void init_symbol_error_msg();
void Second_Layer_Die(core_symbol_info_t * core_symbol_info, XMLElement * elmtDIE_Child);
// 处理第三层die, 收集结构体map中存储的相关信息:
// name, member_size, member_offset, ref_id
void Third_Layer_Die(core_symbol_info_t * core_symbol_info,XMLElement * Die_child, structure_map_t * structure_map)
{
    int flag1 = 0;
    int flag2 = 0;
    int flag3 = 0;
    int flag4 = 0;
    char *name;
    struct_t *p_struct;

    XMLElement *Attribute_Die_die = Die_child->FirstChildElement();

    while (Attribute_Die_die)
    {
        const char *content = Attribute_Die_die->Name();

        if (!strncmp(content, "tag", 100))
        {
            const char *content_Tag = Attribute_Die_die->GetText();

            if (!strncmp(content_Tag, "DW_TAG_member", 100))
            {
                flag1 = 1;
                p_struct = (struct_t *) skyeye_mm_zero(sizeof (struct_t));
            }
            else
            {
                Second_Layer_Die(core_symbol_info,Die_child);
            }
        } else if (!strncmp(content, "attribute", 100) && flag1 == 1)
        {
            XMLElement *Attribute_Type = Attribute_Die_die->FirstChildElement();

            while (Attribute_Type)
            {
                const char *content_Value = Attribute_Type->Name();

                if (!strncmp(content_Value, "type", 100))
                {
                    const char *content_type = Attribute_Type->GetText();

                    if (!strncmp(content_type, "DW_AT_name", 100))
                    {
                        flag2 = 1;
                    } else if (!strncmp(content_type, "DW_AT_type", 100))
                    {
                        flag2 = 1;
                    } else if (!strncmp(content_type, "DW_AT_bit_size", 100))
                    {
                        flag2 = 1;
                        flag3 = 1;
                    } else if (!strncmp(content_type, "DW_AT_data_member_location", 100))
                    {
                        flag2 = 1;
                        flag4 = 1;
                    } else
                    {
                        break;
                    }
                } else if ((!strncmp(content_Value, "value", 100)) && flag2 == 1)
                {
                    XMLElement *Attribute_Value = Attribute_Type->FirstChildElement();
                    const char *content_Name = Attribute_Value->Name();

                    if (!strncmp(content_Name, "string", 100))
                    {
                        name = strdup(Attribute_Value->GetText());
                    } else if (!strncmp(content_Name, "const", 100) && flag3 == 1)
                    {
                        const char *content_size = Attribute_Value->GetText();
                        int m_size = 0;

                        sscanf(content_size, "0x%x", &m_size);
                        flag3 = 0;
                        p_struct->member_size = m_size;
                    } else if (!strncmp(content_Name, "block", 100) && flag4 == 1)
                    {
                        const char *content_instance = Attribute_Value->GetText();
                        int member_offset_tmp = 0;

                        sscanf(content_instance, "DW_OP_plus_uconst 0x%x", &member_offset_tmp);
                        p_struct->member_offset = member_offset_tmp;
                    } else if (!strncmp(content_Name, "ref", 100))
                    {
                        const XMLAttribute *attributeOfSurface = Attribute_Value->FirstAttribute();
                        const char *content_ref = attributeOfSurface->Value();
                        int ref_id;

                        sscanf(content_ref, "%*[^:]:%x", &ref_id);
                        p_struct->id_base_type = ref_id;
                        structure_map->push_back(std::make_pair(name, p_struct));
                    }
                }
                Attribute_Type = Attribute_Type->NextSiblingElement();
            }
        }
        Attribute_Die_die = Attribute_Die_die->NextSiblingElement();
    }
}

/*分析第二层的attribute */
void attribute_Analysis(XMLElement * Attribute_Type, die_node_t * die_node)
{
    int flag = 0;

    while (Attribute_Type)
    {
        const char *content_Value = Attribute_Type->Name();

        if (!strncmp(content_Value, "type", 100))
        {
            const char *content_type = Attribute_Type->GetText();

            if (!strncmp(content_type, "DW_AT_location", 100))
                flag = 1;
            else if (!strncmp(content_type, "DW_AT_type", 100))
                flag = 1;
            else if (!strncmp(content_type, "DW_AT_name", 100))
                flag = 1;
            else if (!strncmp(content_type, "DW_AT_byte_size", 100))
                flag = 1;
        } else if ((!strncmp(content_Value, "value", 100)) && flag == 1)
        {
            // 从value提取有用的block, ref, string内容
            XMLElement *Attribute_Value = Attribute_Type->FirstChildElement();
            if (Attribute_Value != 0)
            {
                const char *content_Name = Attribute_Value->Name();

                if (!strncmp(content_Name, "block", 100))
                {
                    // 切割出全局变量的地址
                    const char *content_addr = Attribute_Value->GetText();
                    int addr;

                    sscanf(content_addr, "%*s %x", &addr);
                    die_node->addr = addr;
                } else if (!strncmp(content_Name, "ref", 100))
                {
                    const XMLAttribute *attributeOfSurface = Attribute_Value->FirstAttribute();
                    const char *content_ref = attributeOfSurface->Value();
                    int ref_id;

                    sscanf(content_ref, "%*[^:]:%x", &ref_id);
                    die_node->idref = ref_id;
                } else if (!strncmp(content_Name, "string", 100))
                {
                    die_node->name = strdup(Attribute_Value->GetText());
                } else if (!strncmp(content_Name, "const", 100))
                {
                    const char *struct_size_t = Attribute_Value->GetText();
                    int data_member_size_s;

                    sscanf(struct_size_t, " %x", &data_member_size_s);
                    die_node->data_member_size = data_member_size_s;
                }
            }
        }
        Attribute_Type = Attribute_Type->NextSiblingElement();

    }
}

void check_global_valid(core_symbol_info_t * core_symbol_info, const char *global_name)
{
    //全局变量列表
    global_table_map_t::iterator gtm_iter;

    //基础类型，变量，结构体等列表
    sym_table_map_t::iterator sym_iter;

    //要查找的Key，全局变量的名称
    gtm_iter = core_symbol_info->global_table_map.find(global_name);
    if (gtm_iter != core_symbol_info->global_table_map.end())
    {
        //printf("Find, the value is: 0x%x\n", gtm_iter->second);

        sym_iter = core_symbol_info->sym_table_map.find(gtm_iter->second);
        if (sym_iter != core_symbol_info->sym_table_map.end())
        {
            die_node_t *die_node;

            die_node = core_symbol_info->sym_table_map[sym_iter->first];

            //printf("\ndie id: 0x%x\n",die_node->id);
            //printf("addr:     0x%x\n",die_node->addr);
            //printf("idref:    0x%x\n",die_node->idref);

            if (die_node->addr == 0)
            {
                core_symbol_info->global_table_map.erase(global_name);
            }
        }
    }
}

// 解析第二层die信息, 并收集全局变量名和地址
void Second_Layer_Die(core_symbol_info_t * core_symbol_info, XMLElement * elmtDIE_Child)
{
    int flag = 0;
    int flag2 = 0;
    int flag3 = 0;
    int flag4 = 0;
    int flag5 = 0;
    int flag6 = 0;
    int flag7 = 0;
    int global_id = 0;

    // 第二层：遇到一个id申请一个空间
    die_node_t *die_node;

    die_node = (die_node_t *) skyeye_mm_zero(sizeof (die_node_t));

    const XMLAttribute *attributeOfSurface = elmtDIE_Child->FirstAttribute();

    // 收集第二层die中的id
    const char *die_die_id = attributeOfSurface->Value();
    int ref_id;

    sscanf(die_die_id, "%*[^:]:%x", &ref_id);

    structure_map_t *structure_map;
    XMLElement *Die_child = elmtDIE_Child->FirstChildElement();

    while (Die_child)
    {
        XMLElement *Attribute_Type = Die_child->FirstChildElement();
        const char *content = Die_child->Name();

        // 解析第二层中的tag, 并筛选对应需要的信息
        if (!strncmp(content, "tag", 100))
        {
            const char *content_Tag = Die_child->GetText();

            if (!strncmp(content_Tag, "DW_TAG_variable", 100))
            {
                flag = 1;
                flag3 = 1;
                die_node->tag = DW_TAG_variable;
                die_node->idref = 0;
            } else if (!strncmp(content_Tag, "DW_TAG_base_type", 100))
            {
                die_node->tag = DW_TAG_base_type;
                flag5 = 1;

            } else if (!strncmp(content_Tag, "DW_TAG_pointer_type", 100))
            {
                die_node->tag = DW_TAG_pointer_type;
                flag = 1;
            } else if (!strncmp(content_Tag, "DW_TAG_structure_type", 100))// 当第二层die中tag为DW_TAG_structure_type, 申请存储结构体信息的map表空间
            {
                flag = 1;
                flag2 = 1;
                die_node->tag = DW_TAG_structure_type;
                structure_map = (structure_map_t *) skyeye_mm_zero(sizeof (structure_map_t));
                die_node->structure_type_map = structure_map;
                memset(structure_map, 0, sizeof (structure_map_t));
            } else if (!strncmp(content_Tag, "DW_TAG_union_type", 100))
            {
                flag = 1;
                flag2 = 1;
                die_node->tag = DW_TAG_union_type;
                structure_map = (structure_map_t *) skyeye_mm_zero(sizeof (structure_map_t));
                die_node->structure_type_map = structure_map;
                memset(structure_map, 0, sizeof (structure_map_t));
            } else if (!strncmp(content_Tag, "DW_TAG_enumeration_type", 100))
            {
                flag = 1;
                flag2 = 1;
                die_node->tag = DW_TAG_enumeration_type;
                structure_map = (structure_map_t *) skyeye_mm_zero(sizeof (structure_map_t));
                die_node->structure_type_map = structure_map;
                memset(structure_map, 0, sizeof (structure_map_t));
            } else if (!strncmp(content_Tag, "DW_TAG_array_type", 100))
            {
                die_node->tag = DW_TAG_array_type;
                flag = 1;
            } else if (!strncmp(content_Tag, "DW_TAG_typedef", 100))
            {
                die_node->tag = DW_TAG_typedef;
                flag = 1;
            } else if (!strncmp(content_Tag, "DW_TAG_volatile_type", 100))
            {
                die_node->tag = DW_TAG_volatile_type;
                flag = 1;
            }
            else
            {
                break;
            }
        } else if ((!strncmp(content, "attribute", 100)) && flag == 1)
        {
            attribute_Analysis(Attribute_Type, die_node);
            if (flag3 == 1)
            {
                global_id = ref_id;
                while (Attribute_Type)
                {
                    const char *global_value = Attribute_Type->Name();

                    if (!strncmp(global_value, "type", 100))
                    {
                        const char *global_type = Attribute_Type->GetText();

                        if (!strncmp(global_type, "DW_AT_name", 100))
                            flag4 = 1;
                        else if (!strncmp(global_type, "DW_AT_type", 100))
                            flag6 = 1;
                    } else if ((!strncmp(global_value, "value", 100)) && flag4 == 1)
                    {
                        XMLElement *Attribute_global = Attribute_Type->FirstChildElement();
                        const char *global_name = Attribute_global->GetText();

                        check_global_valid(core_symbol_info, global_name);
                        core_symbol_info->global_table_map.insert(std::make_pair(global_name, global_id));
                        flag4 = 0;
                    } else if ((!strncmp(global_value, "value", 100)) && flag6 == 1)
                    {
                        XMLElement *Attribute_global = Attribute_Type->FirstChildElement();
                        const char *global_name = Attribute_global->GetText();
                        const XMLAttribute *attributeOfSurface = Attribute_global->FirstAttribute();
                        const char *global_ref = attributeOfSurface->Value();
                        int global_ref_id;

                        sscanf(global_ref, "%*[^:]:%x", &global_ref_id);
                        die_node->idref = global_ref_id;
                        flag6 = 0;
                    }
                    Attribute_Type = Attribute_Type->NextSiblingElement();
                }
            }
        } else if ((!strncmp(content, "attribute", 100)) && flag5 == 1)
        {
            while (Attribute_Type)
            {
                const char *global_value = Attribute_Type->Name();

                if (!strncmp(global_value, "type", 100))
                {
                    const char *global_type = Attribute_Type->GetText();

                    if (!strncmp(global_type, "DW_AT_byte_size", 100))
                    {
                        flag4 = 1;
                    }
                } else if ((!strncmp(global_value, "value", 100)) && flag4 == 1)
                {
                    /*
                     *<attribute>
                     <type>DW_AT_byte_size</type>
                     <value>
                     <const>0x4</const>
                     </value>
                     </attribute>
                     */
                    XMLElement *Attribute_global = Attribute_Type->FirstChildElement();
                    const char *content_size = Attribute_global->GetText();
                    int m_size = 0;

                    /*获取成员变量的数据宽度 C28x特殊: char:int:short都是1个字节 ,
                     * double和unsigned long是2个字节，unsigned long long是4个字节*/
                    sscanf(content_size, "0x%x", &m_size);
                    die_node->data_member_size = m_size;
                    flag4 = 0;
                }
                Attribute_Type = Attribute_Type->NextSiblingElement();
            }
        } else if ((!strncmp(content, "die", 100)) && flag2 == 1)// 处理第三层die, 只有在tag == DW_TAG_structure_type时, 才会有第三层die
        {
            Third_Layer_Die(core_symbol_info,Die_child,structure_map);
        }
        Die_child = Die_child->NextSiblingElement();
    }
    // 每遍历完一次第二层die, 在大结构体中记录一次id值
    if (flag == 1 || flag5 == 1)
    {
        die_node->id = ref_id;
        core_symbol_info->sym_table_map[die_node->id] = die_node;
    }
}

// 遍历分析第一层die的tag为DW_TAG_compile_unit中的die单元
void parse_dwarf_die_tree(core_symbol_info_t * core_symbol_info, XMLElement * elmtDIE)
{
    int flag = 0;

    if (elmtDIE)
    {
        const char *content;
        XMLElement *elmtDIE_Child = elmtDIE->FirstChildElement();

        while (elmtDIE_Child)
        {
            content = elmtDIE_Child->Name();
            if (!strncmp(content, "tag", 100))
            {
                const char *content_Tag = elmtDIE_Child->GetText();

                if (!strncmp(content_Tag, "DW_TAG_compile_unit", 100))
                    flag = 1;
                else
                    flag = 0;
            } else if (!strncmp(content, "die", 100))// 进入处理第二层 Die
            {
                Second_Layer_Die(core_symbol_info, elmtDIE_Child);
            }
            elmtDIE_Child = elmtDIE_Child->NextSiblingElement();
        }
        elmtDIE = elmtDIE->NextSiblingElement();
    }
}

// 进入compile_unit, 调用parse_dwarf_die_tree, 对die进行遍历
void parse_dwarf_compile_unit(core_symbol_info_t * core_symbol_info, XMLElement * elmtCompile_Unit_Child)
{
    const char *pContent;
    XMLElement *elmtDIE = elmtCompile_Unit_Child->FirstChildElement("die");

    const XMLAttribute *attributeOfSurface = elmtDIE->FirstAttribute();

    //XMLElement *elmtDIE_Tag = elmtDIE->FirstChildElement("tag");

    parse_dwarf_die_tree(core_symbol_info, elmtDIE);
}

void parse_dwarf_fde(core_symbol_info_t * core_symbol_info, XMLElement * elmtFDE_Child)
{
    const char *pContent;
    func_info_t *func_info_tmp;

    func_info_tmp = (func_info_t *) skyeye_mm_zero(sizeof (func_info_t));
    XMLElement *elmtFDE_name = elmtFDE_Child->FirstChildElement("name");
    XMLElement *elmtFDE_location = elmtFDE_Child->FirstChildElement("initial_location");
    XMLElement *elmtFED_length = elmtFDE_Child->FirstChildElement("address_range");

    if (elmtFDE_name)
    {
        std::string str_name = elmtFDE_name->GetText();
        func_info_tmp->start_addr = std::strtoul(elmtFDE_location->GetText(), NULL, 0);
        func_info_tmp->length = std::strtoul(elmtFED_length->GetText(), NULL, 0);
        core_symbol_info->func_map.insert(std::make_pair(str_name, *func_info_tmp));
    }
}

// 处理遍历compile_unit, 调用parse_dwarf_compile_unit进行第二层遍历
void parse_symbol_xml(char *cpu_name, char *xml_filename)
{
    // 重复符号信息
    core_symbol_info_t core_symbol_info;

    core_memory_obj_t *core_memory = &(core_symbol_info.core_memory);

    core_memory->core = (conf_object_t *) get_conf_obj(cpu_name);

    if (core_memory->core == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not find obj [%s]\n", cpu_name);
        return;
    }

    core_memory->iface = (memory_space_intf *) SKY_get_iface(core_memory->core, MEMORY_SPACE_INTF_NAME);

    // 初始化基础类型数据长度
    init_base_type();
    init_symbol_error_msg();

    XMLDocument docXml;
    XMLError errXml = docXml.LoadFile(xml_filename);

    if (XML_SUCCESS == errXml)
    {
        XMLElement *elmtRoot = docXml.RootElement();
        XMLElement *elmtDwarf = elmtRoot->FirstChildElement("object_file")->FirstChildElement("dwarf");

        //const char *pContent;
        //XMLElement *elmtXml_Version = elmtDwarf->FirstChildElement("xml_version");
        //pContent = elmtXml_Version->GetText();

        // 遍历所有的dwarf section
        XMLElement *elmtSection = elmtDwarf->FirstChildElement("section");

        while (elmtSection)
        {
            const char *pSContent;

            XMLElement *elmtSectionName = elmtSection->FirstChildElement("name");

            pSContent = elmtSectionName->GetText();

            // 处理".debug_info"信息
            if (!(strncmp(pSContent, ".debug_info", 100)))
            {
                XMLElement *elmtCompile_Unit = elmtSection->FirstChildElement("compile_unit");

                while (elmtCompile_Unit)
                {
                    const char *content;
                    const XMLAttribute *attributeOfSurface = elmtCompile_Unit->FirstAttribute();

                    parse_dwarf_compile_unit(&core_symbol_info, elmtCompile_Unit);
                    elmtCompile_Unit = elmtCompile_Unit->NextSiblingElement();
                }
            }
            // 处理".debug_frame"信息
            if (!(strncmp(pSContent, ".debug_frame", 100)))
            {
                XMLElement *elmtFDE = elmtSection->FirstChildElement("fde");

                while (elmtFDE)
                {
                    const char *content;
                    const XMLAttribute *attributeOfSurface = elmtFDE->FirstAttribute();

                    parse_dwarf_fde(&core_symbol_info, elmtFDE);
                    elmtFDE = elmtFDE->NextSiblingElement();
                }
            }

            elmtSection = elmtSection->NextSiblingElement();
        }
    }
    // 插入表中
    core_symbol_info_list_t::iterator core_symbol_iter;
    core_symbol_iter = core_symbol_info_list.find(cpu_name);
    if (core_symbol_iter == core_symbol_info_list.end())
    {
        core_symbol_info_list.insert(std::make_pair(cpu_name, core_symbol_info));
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "Core %s repeated addition parsing\n", cpu_name);
        return;
    }

    // 清除子节点
    docXml.Clear();
}

int get_global_var_die(core_symbol_info_t * core_symbol_info, char *global_name, die_node_t ** ret_die_node)
{
    // 结构体成员列表
    structure_map_t::iterator struct_iter;
    // 基础类型、变量、结构体等列表
    sym_table_map_t::iterator sym_iter;
    // 全局变量列表
    global_table_map_t::iterator gtm_iter;

    // 要查找的Key, 全局变量的名称
    gtm_iter = core_symbol_info->global_table_map.find(global_name);
    if (gtm_iter != core_symbol_info->global_table_map.end())
    {
        //printf("Find, the value is: 0x%x\n", gtm_iter->second);

        sym_iter = core_symbol_info->sym_table_map.find(gtm_iter->second);
        if (sym_iter != core_symbol_info->sym_table_map.end())
        {
            die_node_t *die_node;

            die_node = core_symbol_info->sym_table_map[sym_iter->first];

            //printf("\ndie id: 0x%x\n", die_node->id);
            //printf("addr:   0x%x\n", die_node->addr);
            //printf("idref:  0x%x\n\n", die_node->idref);

            if (die_node->tag == DW_TAG_variable)
            {
                *ret_die_node = die_node;
                return 1;
            } else
            {
                return 0;
            }

        }
    }
    // 获取失败
    return 0;
}

int get_AT_type_die(core_symbol_info_t * core_symbol_info, die_node_t * search_die_node, die_node_t ** ret_die_node)
{
    // 结构体成员列表
    structure_map_t::iterator struct_iter;
    // 基础类型、变量、结构体等列表
    sym_table_map_t::iterator sym_iter;
    // 全局变量列表
    global_table_map_t::iterator gtm_iter;

    sym_iter = core_symbol_info->sym_table_map.find(search_die_node->idref);
    if (sym_iter != core_symbol_info->sym_table_map.end())
    {
        die_node_t *die_node;

        die_node = core_symbol_info->sym_table_map[sym_iter->first];

        //printf("\ndie id: 0x%x\n", die_node->id);
        //printf("addr:   0x%x\n", die_node->addr);
        //printf("idref:  0x%x\n\n", die_node->idref);

        *ret_die_node = die_node;
        return 1;
    }

    return 0;
}

int get_AT_type_die_byid(core_symbol_info_t * core_symbol_info, int die_id, die_node_t ** ret_die_node)
{
    // 结构体成员列表
    structure_map_t::iterator struct_iter;
    // 基础类型、变量、结构体等列表
    sym_table_map_t::iterator sym_iter;
    // 全局变量列表
    global_table_map_t::iterator gtm_iter;

    sym_iter = core_symbol_info->sym_table_map.find(die_id);
    if (sym_iter != core_symbol_info->sym_table_map.end())
    {
        die_node_t *die_node;

        die_node = core_symbol_info->sym_table_map[sym_iter->first];

        //printf("\ndie id: 0x%x\n", die_node->id);
        //printf("addr:   0x%x\n", die_node->addr);
        //printf("idref:  0x%x\n\n", die_node->idref);

        *ret_die_node = die_node;
        return 1;
    }

    return 0;
}

int get_member_sym_die(char *member_name, die_node_t * die_node, struct_t ** p_struct)
{
    // 结构体成员列表
    structure_map_t::iterator struct_iter;

    if ((die_node->tag == DW_TAG_structure_type) || (die_node->tag == DW_TAG_union_type))
    {
        for (struct_iter = (die_node->structure_type_map)->begin();
                struct_iter != (die_node->structure_type_map)->end(); struct_iter++)
        {
            std::string temp_str(member_name);
            if (temp_str.compare((*struct_iter).first) == 0)
            {
                //cout << "  struct_member: " << (*struct_iter).first << endl;
                //printf("[xxx2]:offset: 0x%x\n", (*struct_iter).second->member_offset);

                *p_struct = (*struct_iter).second;
                return 1;
            }
        }
    } else
    {
        return 0;
    }

    return 0;
}

// 解析c语言语句
int get_next_token(char **src)
{
    return *(*src)++;
}

int is_number(char *num_str)
{
    int i;

    for (i = 0; num_str[i] != 0; i++)
    {
        if (!isdigit(num_str[i]))
        {
            return 0;
        }
    }

    return 1;
}

void check_node_type(core_symbol_info_t * core_symbol_info,die_node_t *die_node, die_node_t ** ret_die_node)
{
    while(1)
    {
        if (((die_node)->tag == DW_TAG_typedef) || ((die_node)->tag == DW_TAG_volatile_type))
        {
            get_AT_type_die(core_symbol_info, (die_node), &die_node);
            debug_printf(3, "volatile or typedef : id = 0x%x\n", (die_node)->id);
        }
        else
        {
            debug_printf(3, "check type = %d,id = 0x%x\n", (die_node)->tag,(die_node)->id);
            *ret_die_node = die_node;
            break;
        }
    }
}


int err_index_position;

sym_info_t symbol_pasrse_program(core_symbol_info_t * core_symbol_info, char *g_src_sym, uint32_t * ret_global_addr)
{
    sym_info_t result;

    result = NO_EXP;

    // 结构体成员列表
    structure_map_t::iterator struct_iter;
    // 基础类型、变量、结构体等列表
    sym_table_map_t::iterator sym_iter;
    // 全局变量列表
    global_table_map_t::iterator gtm_iter;

    die_node_t *curr_die_node;

    // 计算全局变量的内存地址
    uint32_t global_addr = 0;

    // 下面的变量是解析c语言
    char token;                         // current token

    char *src_sym = g_src_sym;

    int src_sym_len = strlen(src_sym);

    char tmp_buff[200] = { 0 };         // 临时符号缓存
    int tmp_index = 0;                  // 临时符号索引
    int is_num = 0;
    int is_global = 1;
    int is_symbol = 1;
    int array_width = 0;

    core_memory_obj_t *core_memory = &(core_symbol_info->core_memory);

    // get next token
    token = get_next_token(&src_sym);

    int i;

    for (i = 0; i < src_sym_len + 1; i++)
    {
        // 报错位置
        err_index_position = i;
        debug_printf(0, "token is: %c 0x%x\n", token, token);
        switch (token)
        {
            case '[':
                tmp_buff[tmp_index++] = 0;
                debug_printf(1, "[case]: %c tmp_buff: %s\n", token, tmp_buff);
                tmp_index = 0;

                is_num = 1;             // 开始读取数组下标数字

                if (is_global == 1)
                {
                    debug_printf(2, "tmp_buff: %s is global!\n", tmp_buff);
                    is_global = 0;

                    if (get_global_var_die(core_symbol_info, tmp_buff, &curr_die_node))
                    {
                        debug_printf(3, "===: 0x%x\n", curr_die_node->addr);
                        // 全局变量地址赋值
                        global_addr = curr_die_node->addr;

                        die_node_t *type_die_node;

                        if (get_AT_type_die(core_symbol_info, curr_die_node, &type_die_node))
                        {
                            check_node_type(core_symbol_info,type_die_node,&type_die_node);

                            if (type_die_node->tag == DW_TAG_pointer_type)
                            {
                                debug_printf(3, "============= this is DW_TAG_pointer_type\n");

                                curr_die_node = type_die_node;

                                // 读取指针指向的地址
                                uint32_t value = 0;

                                if (core_memory->iface)
                                {
                                    core_memory->iface->read(core_memory->core, global_addr, &value, 4);
                                }
                                global_addr = value;
                                debug_printf(3, "value: 0x%x\n\n", value);

                                // 获取指针结构体的结构体大小
                                die_node_t *type_die_node;

                                if (get_AT_type_die(core_symbol_info, curr_die_node, &type_die_node))
                                {
                                    check_node_type(core_symbol_info,type_die_node,&type_die_node);
                                    debug_printf(3, "==== id: 0x%x\n", type_die_node->id);
                                    debug_printf(3, "==== size: 0x%x\n", type_die_node->data_member_size);
                                    array_width = type_die_node->data_member_size;
                                }
                            } else if (type_die_node->tag == DW_TAG_array_type)
                            {
                                debug_printf(3, "============= this is DW_TAG_array_type\n");

                                curr_die_node = type_die_node;

                                if (get_AT_type_die(core_symbol_info, curr_die_node, &type_die_node))
                                {
                                    check_node_type(core_symbol_info,type_die_node,&type_die_node);
                                    array_width = type_die_node->data_member_size;
                                    debug_printf(3, "data_member_size: 0x%x\n", type_die_node->data_member_size);
                                    debug_printf(3, "tag: %d id: 0x%0x\n", type_die_node->tag, type_die_node->id);
                                }
                            } else
                            {
                                // 不是指针数据也不是数组
                                return NON_POINTER;
                            }
                        }
                    } else
                    {
                        // 不是一个全局变量
                        return NO_GLOBAL_VAR;
                    }
                } else
                {
                    // 获取子元素
                    get_AT_type_die(core_symbol_info, curr_die_node, &curr_die_node);

                    check_node_type(core_symbol_info,curr_die_node,&curr_die_node);

                    struct_t *struct_insn;

                    if (get_member_sym_die(tmp_buff, curr_die_node, &struct_insn))
                    {
                        debug_printf(3, "id: 0x%x\n", curr_die_node->id);
                        debug_printf(3, "offset: %d\n\n", struct_insn->member_offset);
                        global_addr += struct_insn->member_offset;

                        die_node_t *type_die_node;

                        if (get_AT_type_die_byid(core_symbol_info, struct_insn->id_base_type, &type_die_node))
                        {
                            debug_printf(3, "==== id: 0x%x\n", type_die_node->id);

                            check_node_type(core_symbol_info,type_die_node,&type_die_node);

                            if (type_die_node->tag == DW_TAG_array_type)
                            {
                                curr_die_node = type_die_node;
                                if (get_AT_type_die(core_symbol_info, type_die_node, &type_die_node))
                                {

                                    check_node_type(core_symbol_info,type_die_node,&type_die_node);

                                    debug_printf(3, "==== id: 0x%x\n", type_die_node->id);
                                    debug_printf(3, "==== size: 0x%x\n", type_die_node->data_member_size);
                                    array_width = type_die_node->data_member_size;
                                }
                            } else if (type_die_node->tag == DW_TAG_pointer_type)
                            {
                                debug_printf(3, "============= this is DW_TAG_pointer_type\n");

                                curr_die_node = type_die_node;

                                // 读取指针指向的地址
                                uint32_t value = 0;

                                if (core_memory->iface)
                                {
                                    core_memory->iface->read(core_memory->core, global_addr, &value, 4);
                                }
                                global_addr = value;
                                debug_printf(3, "value: 0x%x\n\n", value);

                                // 获取指针结构体的结构体大小
                                die_node_t *type_die_node;

                                if (get_AT_type_die(core_symbol_info, curr_die_node, &type_die_node))
                                {
                                    debug_printf(3, "==== id: 0x%x\n", type_die_node->id);
                                    debug_printf(3, "==== size: 0x%x\n", type_die_node->data_member_size);
                                    array_width = type_die_node->data_member_size;
                                }
                            } else
                            {
                                // 不是指针数据也不是数组
                                return NON_POINTER;
                            }
                        }
                        else
                        {
                            return NO_MEMBER;
                        }
                    }
                    else
                    {
                        return NO_MEMBER;
                    }
                }

                break;
            case ']':
                tmp_buff[tmp_index++] = 0;
                debug_printf(1, "[case]: %c tmp_buff: %s\n", token, tmp_buff);

                int value;

                if (is_number(tmp_buff))
                {
                    sscanf(tmp_buff, "%d", &value);
                    debug_printf(2, "===> value: %d\n", value);

                    global_addr += array_width * value;
                    debug_printf(2, "global_addr: 0x%x array_width: 0x%x\n", global_addr, array_width);

                    // 数据结尾的元素
                    result = SYM_OK;
                } else
                {
                    // 数组不是数字的错误
                    debug_printf(2, "===> tmp_buff: %s not is number!\n", tmp_buff);
                    return ARRAY_NUM_ERR;
                }

                tmp_index = 0;
                is_num = 0;             // 读取数据完毕
                break;
            case '.':
                tmp_buff[tmp_index++] = 0;
                debug_printf(1, "[case]: %c tmp_buff: %s\n", token, tmp_buff);
                tmp_index = 0;

                if (is_symbol == 1)
                {
                    if (is_global == 1)
                    {
                        debug_printf(2, "tmp_buff: %s is global!\n", tmp_buff);
                        is_global = 0;

                        // 普通成员变量
                        if (get_global_var_die(core_symbol_info, tmp_buff, &curr_die_node))
                        {
                            debug_printf(3, "===: 0x%x\n", curr_die_node->addr);
                            global_addr = curr_die_node->addr;
                        } else
                        {
                            // 不是一个全局变量
                            return NO_GLOBAL_VAR;
                        }
                    } else
                    {
                        // 获取子元素
                        get_AT_type_die(core_symbol_info, curr_die_node, &curr_die_node);

                        check_node_type(core_symbol_info,curr_die_node,&curr_die_node);
                        struct_t *struct_insn;

                        if (get_member_sym_die(tmp_buff, curr_die_node, &struct_insn))
                        {
                            debug_printf(3, "id: 0x%x\n", curr_die_node->id);
                            debug_printf(3, "offset: %d\n\n", struct_insn->member_offset);
                            global_addr += struct_insn->member_offset;

                            die_node_t *type_die_node;

                            if (get_AT_type_die_byid(core_symbol_info, struct_insn->id_base_type, &type_die_node))
                            {
                                debug_printf(3, "==== id: 0x%x\n", type_die_node->id);
                                debug_printf(3, "///// tag: %d\n\n", type_die_node->tag);
                                if (type_die_node->tag == DW_TAG_array_type)
                                {
                                    debug_printf(3, "            DW_TAG_array_type\n");
                                    curr_die_node = type_die_node;
                                } else if (type_die_node->tag == DW_TAG_structure_type)
                                {
                                    debug_printf(3, "            DW_TAG_structure_type\n");
                                    curr_die_node = type_die_node;
                                } else if (type_die_node->tag == DW_TAG_typedef)
                                {
                                    debug_printf(3, "            DW_TAG_typedef\n");
                                    curr_die_node = type_die_node;
                                } else
                                {
                                    return NO_MEMBER;
                                }
                            }
                        } else
                        {
                            // 报错, 没有该结构体成员
                            return NO_MEMBER;
                        }
                    }
                }
                break;
            case '-':
                tmp_buff[tmp_index++] = 0;
                debug_printf(1, "[case]: %c tmp_buff: %s\n", token, tmp_buff);
                tmp_index = 0;

                if (is_symbol == 1)
                {
                    if (is_global == 1)
                    {
                        debug_printf(2, "tmp_buff: %s is global!\n", tmp_buff);
                        is_global = 0;

                        if (get_global_var_die(core_symbol_info, tmp_buff, &curr_die_node))
                        {
                            debug_printf(3, "===: 0x%x\n", curr_die_node->addr);
                            global_addr = curr_die_node->addr;

                            die_node_t *type_die_node;

                            if (get_AT_type_die(core_symbol_info, curr_die_node, &type_die_node))
                            {
                                if (type_die_node->tag == DW_TAG_pointer_type)
                                {
                                    debug_printf(3, "============= this is DW_TAG_pointer_type\n");

                                    curr_die_node = type_die_node;

                                    // 读取指针指向的地址
                                    uint32_t value = 0;

                                    if (core_memory->iface)
                                    {
                                        core_memory->iface->read(core_memory->core, global_addr, &value, 4);
                                    }
                                    global_addr = value;
                                    debug_printf(3, "value: 0x%x\n", value);
                                } else if (type_die_node->tag == DW_TAG_array_type)
                                {
                                    debug_printf(3, "============= this is DW_TAG_array_type\n");

                                    curr_die_node = type_die_node;

                                    if (get_AT_type_die(core_symbol_info, curr_die_node, &type_die_node))
                                    {
                                        array_width = type_die_node->data_member_size;
                                        debug_printf(3, "data_member_size: 0x%x\n", type_die_node->data_member_size);
                                        debug_printf(3, "tag: %d id: 0x%0x\n", type_die_node->tag, type_die_node->id);
                                    }
                                } else
                                {
                                    // 不是指针数据
                                    return NON_POINTER;
                                }
                            }
                        } else
                        {
                            // 不是一个全局变量
                            return NO_GLOBAL_VAR;
                        }
                    } else
                    {
                        // 结构体成员
                        // 获取子元素
                        get_AT_type_die(core_symbol_info, curr_die_node, &curr_die_node);

                        check_node_type(core_symbol_info,curr_die_node,&curr_die_node);
                        struct_t *struct_insn;

                        if (get_member_sym_die(tmp_buff, curr_die_node, &struct_insn))
                        {
                            debug_printf(3, "id: 0x%x\n", curr_die_node->id);
                            debug_printf(3, "offset: %d\n\n", struct_insn->member_offset);
                            global_addr += struct_insn->member_offset;

                            die_node_t *type_die_node;

                            if (get_AT_type_die_byid(core_symbol_info, struct_insn->id_base_type, &type_die_node))
                            {
                                debug_printf(3, "==== id: 0x%x\n", type_die_node->id);
                                debug_printf(3, "///// tag: %d\n\n", type_die_node->tag);

                                if (type_die_node->tag == DW_TAG_pointer_type)
                                {
                                    debug_printf(3, "============= this is DW_TAG_pointer_type\n");

                                    curr_die_node = type_die_node;

                                    // 读取指针指向的地址
                                    uint32_t value = 0;

                                    if (core_memory->iface)
                                    {
                                        core_memory->iface->read(core_memory->core, global_addr, &value, 4);
                                    }
                                    global_addr = value;
                                    debug_printf(3, "value: 0x%x\n", value);
                                } else
                                {
                                    return NON_POINTER;
                                }
                            }
                        } else
                        {
                            // 报错, 没有该结构体成员
                            return NO_MEMBER;
                        }
                    }
                } else
                {
                }

                break;
            case '>':
                break;
            case 0:
                {
                    // 结束符号
                    tmp_buff[tmp_index++] = 0;
                    debug_printf(1, "[case]: 0x0 tmp_buff: %s\n", tmp_buff);

                    if (strlen(tmp_buff))
                    {
                        // 全局变量
                        if (is_global == 1)
                        {
                            debug_printf(2, "tmp_buff: %s is global!\n", tmp_buff);
                            is_global = 0;

                            if (get_global_var_die(core_symbol_info, tmp_buff, &curr_die_node))
                            {
                                debug_printf(3, "===: 0x%x\n", curr_die_node->addr);
                                // 全局变量地址赋值
                                global_addr = curr_die_node->addr;

                                result = SYM_OK;
                            }
                        } else
                        {
                            // 结构体成员
                            struct_t *struct_insn;

                            debug_printf(3, "id: 0x%x\n", curr_die_node->id);

                            // 获取子元素
                            get_AT_type_die(core_symbol_info, curr_die_node, &curr_die_node);
                            debug_printf(3, "id: 0x%x\n", curr_die_node->id);

                            check_node_type(core_symbol_info,curr_die_node,&curr_die_node);

                            if (get_member_sym_die(tmp_buff, curr_die_node, &struct_insn))
                            {
                                debug_printf(3, "offset: %d\n", struct_insn->member_offset);
                                global_addr += struct_insn->member_offset;

                                result = SYM_OK;
                            }
                            else
                            {
                                return NO_MEMBER;
                            }
                        }
                    }
                    break;
                }
            default:
                {
                    // 判断是否读数组数字
                    if (is_num)
                    {
                        is_symbol = 0;
                    } else
                    {
                        is_symbol = 1;
                    }
                    // 接收字符串、数组字符
                    tmp_buff[tmp_index++] = token;
                    break;
                }
        }
        token = get_next_token(&src_sym);
    }

    *ret_global_addr = global_addr;
    return result;
}

void format_symbol_src(char *c_str, char *g_src_sym)
{
    char *p;

    p = g_src_sym;

    while (*c_str)
    {
        if (*c_str != ' ')
        {
            *p++ = *c_str++;
        } else
        {
            c_str++;
        }
    }

    // 添加结尾
    *p = '\0';
    p = 0;
}

std::string global_sym_str;
std::string global_sym_err_str;
typedef std::unordered_map < int, std::string > sym_err_map_t;
sym_err_map_t sym_err_map;

void init_symbol_error_msg()
{
    sym_err_map[POINTER] = "A pointer type data";
    sym_err_map[NON_POINTER] = "Not pointer type data";
    sym_err_map[ARRAY] = "A rray type data";
    sym_err_map[NON_ARRAY] = "Not Array type data";
    sym_err_map[ARRAY_BOUND] = "Array crossing";
    sym_err_map[NO_MEMBER] = "No member of this structure";
    sym_err_map[SYM_OK] = "Analytic symbol success";
    sym_err_map[NO_CORE] = "No CPU core";
    sym_err_map[ARRAY_NUM_ERR] = "Array subscript error";
    sym_err_map[NO_GLOBAL_VAR] = "Not a global variable";
    sym_err_map[NO_Bin_PARSE] = "No binary file parsed";
    sym_err_map[SYM_TOO_LONG] = "symbol length too long";
    sym_err_map[NO_EXP] = "No exceedingly";
}

void SkyEye_GetSymbol_GetErrorMsg(int flag, const char **ret_msg)
{
    sym_err_map_t::iterator sym_err_iter;
    sym_err_iter = sym_err_map.find(flag);
    if (sym_err_iter != sym_err_map.end())
    {
        global_sym_err_str.clear();

        if (flag == NO_CORE)
        {
            global_sym_err_str.append("Error: ");
            global_sym_err_str.append((*sym_err_iter).second);
            global_sym_err_str.append(", please check whether the processor core is correct.");
        } else if (flag == NO_EXP)
        {
            global_sym_err_str.append("Parse symbol NO_EXP: ");
            global_sym_err_str.append((*sym_err_iter).second);
        } else if (flag == SYM_OK)
        {
            global_sym_err_str.append("Parse symbol SYM_OK: ");
            global_sym_err_str.append((*sym_err_iter).second);
        } else
        {
            global_sym_err_str.append("Parse symbol error: ");
            global_sym_err_str.append((*sym_err_iter).second);
            global_sym_err_str.append("\n");
            global_sym_err_str.append(global_sym_str);
            global_sym_err_str.append("\n");

            for (int i = 0; i < err_index_position - 1; i++)
            {
                global_sym_err_str.append(" ");
            }
            global_sym_err_str.append("^");
        }

        *ret_msg = global_sym_err_str.data();
    }
}

uint32_t SkyEye_GetFunc_Addr(char *cpu_name, char *func_str)
{
    uint32_t result = 0;

    char g_src_sym[1024] = { 0 };

    // 去掉空格符号
    format_symbol_src(func_str, g_src_sym);

    global_sym_str.clear();
    global_sym_str.append(g_src_sym);

    core_symbol_info_t *core_symbol_info;

    core_symbol_info_list_t::iterator core_symbol_iter;
    core_symbol_iter = core_symbol_info_list.find(cpu_name);
    if (core_symbol_iter != core_symbol_info_list.end())
    {
        core_symbol_info = &((*core_symbol_iter).second);
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "%s no binary file parsed\n", cpu_name);
        return result;
    }
    debug_printf(3, "\n[Function]: %s\n\n", func_str);
    uint32_t func_addr;

    func_map_t::iterator func_map_iter;
    func_map_iter = core_symbol_info->func_map.find(func_str);
    if (func_map_iter != core_symbol_info->func_map.end())
    {
        func_addr = (*func_map_iter).second.start_addr;
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "function %s not found\n", func_str);
        return result;
    }
    result = func_addr;

    return result;
}

uint32_t SkyEye_GetFunc_Length(char *cpu_name, char *func_str)
{
    uint32_t result = 0;

    char g_src_sym[1024] = { 0 };

    // 去掉空格符号
    format_symbol_src(func_str, g_src_sym);

    global_sym_str.clear();
    global_sym_str.append(g_src_sym);

    core_symbol_info_t *core_symbol_info;

    core_symbol_info_list_t::iterator core_symbol_iter;
    core_symbol_iter = core_symbol_info_list.find(cpu_name);
    if (core_symbol_iter != core_symbol_info_list.end())
    {
        core_symbol_info = &((*core_symbol_iter).second);
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "%s no binary file parsed\n", cpu_name);
        return result;
    }
    debug_printf(3, "\n[Function]: %s\n\n", func_str);
    uint32_t func_length;

    func_map_t::iterator func_map_iter;
    func_map_iter = core_symbol_info->func_map.find(func_str);
    if (func_map_iter != core_symbol_info->func_map.end())
    {
        func_length = (*func_map_iter).second.length;
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "function %s not found\n", func_str);
        return result;
    }
    result = func_length;

    return result;
}

int SkyEye_GetSymbol_Addr(char *cpu_name, char *sym_str, uint32_t * global_addr)
{
    int result;

    //char *g_src_sym = (char *)skyeye_mm_zero(strlen(sym_str));
    char g_src_sym[1024] = { 0 };
    if (strlen(sym_str) >= 1024)
    {
        return SYM_TOO_LONG;
    }
    // 去掉空格符号
    format_symbol_src(sym_str, g_src_sym);

    global_sym_str.clear();
    global_sym_str.append(g_src_sym);

    core_symbol_info_t *core_symbol_info;

    core_symbol_info_list_t::iterator core_symbol_iter;
    core_symbol_iter = core_symbol_info_list.find(cpu_name);
    if (core_symbol_iter != core_symbol_info_list.end())
    {
        core_symbol_info = &((*core_symbol_iter).second);
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "%s no binary file parsed\n", cpu_name);
        return NO_Bin_PARSE;
    }

    debug_printf(3, "\n[Symbol]: %s\n\n", sym_str);
    result = symbol_pasrse_program(core_symbol_info, g_src_sym, global_addr);

    return result;
}

int SkyEye_GetSymbol_Value(char *cpu_name, char *sym_str, void *value, sym_type_t count)
{
    int result;
    uint32_t global_addr = 0;

    core_symbol_info_t *core_symbol_info;
    conf_object_t *core = (conf_object_t *) get_conf_obj(cpu_name);

    core_symbol_info_list_t::iterator core_symbol_iter;
    core_symbol_iter = core_symbol_info_list.find(cpu_name);
    if (core_symbol_iter != core_symbol_info_list.end())
    {
        core_symbol_info = &((*core_symbol_iter).second);
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "%s no binary file parsed\n", cpu_name);
        return NO_Bin_PARSE;
    }

    core_memory_obj_t *core_memory = &(core_symbol_info->core_memory);

    result = SkyEye_GetSymbol_Addr(cpu_name, sym_str, &global_addr);

    if (core_memory->iface != NULL)
    {
        switch (count)
        {
            case GV_BIT8:
            case GV_BIT16:
            case GV_BIT32:
                core_memory->iface->read(core_memory->core, global_addr, value, count);
                break;
            case GV_BIT64:
                {
                    uint64_t bit64_data = 0;
                    uint64_t bit64_h_data = 0;
                    uint64_t bit64_l_data = 0;

                    if (!strncmp(cpu_name, "f28335_core", strlen("f28335_core"))
                            || !strncmp(cpu_name, "c2812_core", strlen("c2812_core")))
                    {
                        core_memory->iface->read(core_memory->core, global_addr, &bit64_l_data, 4);
                        core_memory->iface->read(core_memory->core, global_addr + 2, &bit64_h_data, 4);
                    } else
                    {
                        core_info_intf *iface_core = (core_info_intf *) SKY_get_iface(core, CORE_INFO_INTF_NAME);
                        char *arch_tmp = iface_core->get_architecture(core);

                        if (!strcmp(arch_tmp, "powerpc"))
                        {
                            core_memory->iface->read(core_memory->core, global_addr, &bit64_h_data, 4);
                            core_memory->iface->read(core_memory->core, global_addr + 4, &bit64_l_data, 4);
                        } else
                        {
                            core_memory->iface->read(core_memory->core, global_addr, &bit64_l_data, 4);
                            core_memory->iface->read(core_memory->core, global_addr + 4, &bit64_h_data, 4);
                        }
                    }

                    bit64_data = (bit64_h_data << 32) | bit64_l_data;

                    *(uint64_t *) value = bit64_data;
                }
                break;
            default:
                return -2;
        }
    }

    return result;
}

uint32_t SkyEye_SetFloatSymbol_Value(char *cpu_name, char *sym_str, float value, sym_type_t count)
{
    SkyEye_SetSymbol_Value(cpu_name, sym_str, &value, count);
}

uint32_t SkyEye_SetDoubleSymbol_Value(char *cpu_name, char *sym_str, double value, sym_type_t count)
{
    SkyEye_SetSymbol_Value(cpu_name, sym_str, &value, count);
}

float SkyEye_GetFloatSymbol_Value(char *cpu_name, char *sym_strvoid, sym_type_t count)
{
    uint32_t F_Val = 0;
    float result = 0;

    SkyEye_GetSymbol_Value(cpu_name, sym_strvoid, &(F_Val), count);
    result = *(float *) (&F_Val);

    return result;
}

double SkyEye_GetDoubleSymbol_Value(char *cpu_name, char *sym_strvoid, sym_type_t count)
{
    uint64_t D_Val = 0;
    double result = 0;

    SkyEye_GetSymbol_Value(cpu_name, sym_strvoid, &(D_Val), count);
    result = *(double *) (&D_Val);

    return result;
}

uint64_t SkyEye_GetSymbol_Value_By_Py(char *cpu_name, char *sym_str, sym_type_t count)
{
    int result;
    uint64_t value = 0;
    uint32_t global_addr = 0;

    core_symbol_info_t *core_symbol_info;

    core_symbol_info_list_t::iterator core_symbol_iter;
    core_symbol_iter = core_symbol_info_list.find(cpu_name);
    if (core_symbol_iter != core_symbol_info_list.end())
    {
        core_symbol_info = &((*core_symbol_iter).second);
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "%s no binary file parsed\n", cpu_name);
        return NO_Bin_PARSE;
    }

    core_memory_obj_t *core_memory = &(core_symbol_info->core_memory);

    result = SkyEye_GetSymbol_Addr(cpu_name, sym_str, &global_addr);

    if (core_memory->iface != NULL)
    {
        switch (count)
        {
            case GV_BIT8:
            case GV_BIT16:
            case GV_BIT32:
                core_memory->iface->read(core_memory->core, global_addr, &value, count);
                break;
            case GV_BIT64:
                {
                    uint64_t bit64_data = 0;
                    uint64_t bit64_h_data = 0;
                    uint64_t bit64_l_data = 0;

                    if (!strncmp(cpu_name, "f28335_core", strlen("f28335_core"))
                            || !strncmp(cpu_name, "c2812_core", strlen("c2812_core")))
                    {
                        core_memory->iface->read(core_memory->core, global_addr, &bit64_l_data, 4);
                        core_memory->iface->read(core_memory->core, global_addr + 2, &bit64_h_data, 4);
                    } else
                    {
                        core_memory->iface->read(core_memory->core, global_addr, &bit64_l_data, 4);
                        core_memory->iface->read(core_memory->core, global_addr + 4, &bit64_h_data, 4);
                    }

                    bit64_data = (bit64_h_data << 32) | bit64_l_data;

                    value = bit64_data;
                }
                break;
            default:
                return value;
        }
    }

    return value;
}

int SkyEye_SetSymbol_Value_By_Py(char *cpu_name, char *sym_str, uint64_t value, sym_type_t count)
{
    int result;
    uint32_t global_addr = 0;

    core_symbol_info_t *core_symbol_info;

    core_symbol_info_list_t::iterator core_symbol_iter;
    core_symbol_iter = core_symbol_info_list.find(cpu_name);
    if (core_symbol_iter != core_symbol_info_list.end())
    {
        core_symbol_info = &((*core_symbol_iter).second);
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "%s no binary file parsed\n", cpu_name);
        return NO_Bin_PARSE;
    }

    core_memory_obj_t *core_memory = &(core_symbol_info->core_memory);

    result = SkyEye_GetSymbol_Addr(cpu_name, sym_str, &global_addr);
    if (core_memory->iface != NULL)
    {
        switch (count)
        {
            case GV_BIT8:
            case GV_BIT16:
            case GV_BIT32:
                core_memory->iface->write(core_memory->core, global_addr, &value, count);
                break;
            case GV_BIT64:
                {
                    uint64_t bit64_data = 0;
                    uint32_t bit64_h_data = 0;
                    uint32_t bit64_l_data = 0;

                    bit64_data = value;

                    bit64_h_data = (bit64_data >> 32) & 0xFFFFFFFF;
                    bit64_l_data = (bit64_data) & 0xFFFFFFFF;

                    if (!strncmp(cpu_name, "f28335_core", strlen("f28335_core"))
                            || !strncmp(cpu_name, "c2812_core", strlen("c2812_core")))
                    {
                        core_memory->iface->write(core_memory->core, global_addr, &bit64_l_data, 4);
                        core_memory->iface->write(core_memory->core, global_addr + 2, &bit64_h_data, 4);
                    } else
                    {
                        core_memory->iface->write(core_memory->core, global_addr, &bit64_l_data, 4);
                        core_memory->iface->write(core_memory->core, global_addr + 4, &bit64_h_data, 4);
                    }
                }
                break;
            default:
                return -2;
        }
    }
    return result;
}

int SkyEye_SetSymbol_Value(char *cpu_name, char *sym_str, void *value, sym_type_t count)
{
    int result;
    uint32_t global_addr = 0;

    core_symbol_info_t *core_symbol_info;

    core_symbol_info_list_t::iterator core_symbol_iter;
    core_symbol_iter = core_symbol_info_list.find(cpu_name);
    if (core_symbol_iter != core_symbol_info_list.end())
    {
        core_symbol_info = &((*core_symbol_iter).second);
    } else
    {
        skyeye_log(Error_log, __FUNCTION__, "%s no binary file parsed\n", cpu_name);
        return NO_Bin_PARSE;
    }

    core_memory_obj_t *core_memory = &(core_symbol_info->core_memory);

    result = SkyEye_GetSymbol_Addr(cpu_name, sym_str, &global_addr);
    if (core_memory->iface != NULL)
    {
        switch (count)
        {
            case GV_BIT8:
            case GV_BIT16:
            case GV_BIT32:
                core_memory->iface->write(core_memory->core, global_addr, value, count);
                break;
            case GV_BIT64:
                {
                    uint64_t bit64_data = 0;
                    uint32_t bit64_h_data = 0;
                    uint32_t bit64_l_data = 0;

                    bit64_data = *(uint64_t *) value;

                    bit64_h_data = (bit64_data >> 32) & 0xFFFFFFFF;
                    bit64_l_data = (bit64_data) & 0xFFFFFFFF;

                    if (!strncmp(cpu_name, "f28335_core", strlen("f28335_core"))
                            || !strncmp(cpu_name, "c2812_core", strlen("c2812_core")))
                    {
                        core_memory->iface->write(core_memory->core, global_addr, &bit64_l_data, 4);
                        core_memory->iface->write(core_memory->core, global_addr + 2, &bit64_h_data, 4);
                    } else
                    {
                        core_memory->iface->write(core_memory->core, global_addr, &bit64_l_data, 4);
                        core_memory->iface->write(core_memory->core, global_addr + 4, &bit64_h_data, 4);
                    }
                }
                break;
            default:
                return -2;
        }
    }
    return result;
}

#if 1
int parse_xml_output()
{
    sym_table_map_t::iterator it;
    structure_map_t::iterator iter;
    global_table_map_t::iterator it2;
    core_symbol_info_list_t::iterator it3;
    func_map_t::iterator itFunc;
    for (it3 = core_symbol_info_list.begin(); it3 != core_symbol_info_list.end(); ++it3)
    {
        for (itFunc = it3->second.func_map.begin(); itFunc != it3->second.func_map.end(); ++itFunc)
        {
            std::cout << "Function: " << (*itFunc).first << std::endl;
            printf("address:   0x%x\n", (*itFunc).second.start_addr);
            printf("length:   0x%x\n", (*itFunc).second.length);
            printf("\n");
        }
        for (it2 = it3->second.global_table_map.begin(); it2 != it3->second.global_table_map.end(); ++it2)
        {
            std::cout << "global_name:" << (*it2).first << std::endl;
            printf("global_id:   0x%x\n", (*it2).second);
            printf("\n");
        }
        printf("\n");
        for (it = it3->second.sym_table_map.begin(); it != it3->second.sym_table_map.end(); ++it)
        {
            die_node_t *die_node;

            die_node = it3->second.sym_table_map[it->first];

            printf("die id: 0x%x\n", die_node->id);
            switch (die_node->tag)
            {
                case DW_TAG_variable:
                    printf("DW_TAG_variable\n");
                    break;
                case DW_TAG_base_type:
                    printf("DW_TAG_base_type\n");
                    break;
                case DW_TAG_pointer_type:
                    printf("DW_TAG_pointer_type\n");
                    break;
                case DW_TAG_structure_type:
                    printf("DW_TAG_structure_type\n");
                    break;
                case DW_TAG_array_type:
                    printf("DW_TAG_array_type\n");
                    break;
            }
            printf("type_name:   %s\n", die_node->type_name);
            printf("name:   %s\n", die_node->name);
            printf("addr:   0x%x\n", die_node->addr);
            printf("idref:  0x%x\n", die_node->idref);
            //std::cout << std::endl;
            int flag = 0;

            /*当遇到结构体时 */
            if ((die_node->structure_type_map) != NULL)
            {
                for (iter = (die_node->structure_type_map)->begin(); iter != (die_node->structure_type_map)->end();
                        iter++)
                {
                    std::cout << "struct_member: " << (*iter).first << std::endl;
                    printf("id_base_type: 0x%x\n", (*iter).second->id_base_type);
                    printf("member_size: 0x%x\n", (*iter).second->member_size);
                    //printf("member_instance: 0x%x\n",(*iter).second->member_instance);
                    std::cout << std::endl;
                }
                std::cout << std::endl;
            }
        }
    }
    return 0;
}
#else
int parse_xml_output()
{
    return 0;
}
#endif
