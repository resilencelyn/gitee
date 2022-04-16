#include <stdio.h>
#include <string.h>
#include <memory_space.h>
#include <skyeye_addr_space.h>
#include <memory_monitor.h>

#include <map>
#include <unordered_map>
#include <string>
#include "skyeye_mm.h"

bool verif_memory_cmd(memory_addr_info_t * ptr, void *val);

#define PGD(addr)     ((addr & 0xE0000000) >> 29)
#define PMD(addr)     ((addr & 0x1FF00000) >> 20)
#define PTE(addr)     ((addr & 0x000FF000) >> 12)
#define OFFSET(addr)  ((addr & 0x00000FFF)      )

#define PGD_SIZE      ((0xE0000000 >> 29) + 1)
#define PMD_SIZE      ((0x1FF00000 >> 20) + 1)
#define PTE_SIZE      ((0x000FF000 >> 12) + 1)
#define PAGE_LEN      ((0x00000FFF      ) + 1)

mm_fast_map create_memory_pgd()
{
    mm_fast_map ptr = (mm_fast_map) skyeye_mm_zero(PGD_SIZE * sizeof (void *));

    return ptr;
}

mm_fast_map insert_memory_pgd_intem(mm_fast_map fast_map, unsigned int addr, memory_addr_info_t * data)
{
    uint32_t pgd_offset, pmd_offset, pte_offset, offset;

    pgd_offset = PGD(addr);
    pmd_offset = PMD(addr);
    pte_offset = PTE(addr);
    offset = OFFSET(addr);

    if (fast_map[pgd_offset] == NULL)
    {
        fast_map[pgd_offset] = (void ****) skyeye_mm_zero(PMD_SIZE * sizeof (void *));
    }
    if (fast_map[pgd_offset][pmd_offset] == NULL)
    {
        fast_map[pgd_offset][pmd_offset] = (void ***) skyeye_mm_zero(PTE_SIZE * sizeof (void *));
    }
    if (fast_map[pgd_offset][pmd_offset][pte_offset] == NULL)
    {
        fast_map[pgd_offset][pmd_offset][pte_offset] = (void **) skyeye_mm_zero(PAGE_LEN * sizeof (void *));
    }

    fast_map[pgd_offset][pmd_offset][pte_offset][offset] = (mm_fast_map) data;

    return fast_map;
}

void *get_memory_pgd_item(mm_fast_map fast_map, uint32_t addr)
{
    uint32_t pgd_offset, pmd_offset, pte_offset, offset;

    pgd_offset = PGD(addr);
    pmd_offset = PMD(addr);
    pte_offset = PTE(addr);
    offset = OFFSET(addr);

    if (!fast_map[pgd_offset])
    {
        return (void *) NULL;
    }
    if (!fast_map[pgd_offset][pmd_offset])
    {
        return (void *) NULL;
    }
    if (!fast_map[pgd_offset][pmd_offset][pte_offset])
    {
        return (void *) NULL;
    }
    if (!fast_map[pgd_offset][pmd_offset][pte_offset][offset])
    {
        return (void *) NULL;
    }

    return fast_map[pgd_offset][pmd_offset][pte_offset][offset];
}

int clear_pgd_item(mm_fast_map fast_map, uint32_t addr, uint32_t mm_type)
{
    uint32_t pgd_offset, pmd_offset, pte_offset, offset;

    pgd_offset = PGD(addr);
    pmd_offset = PMD(addr);
    pte_offset = PTE(addr);
    offset = OFFSET(addr);

    if (!fast_map[pgd_offset])
    {
        return 0;
    }
    if (!fast_map[pgd_offset][pmd_offset])
    {
        return 0;
    }
    if (!fast_map[pgd_offset][pmd_offset][pte_offset])
    {
        return 0;
    }
    if (!fast_map[pgd_offset][pmd_offset][pte_offset][offset])
    {
        return 0;
    }

    if (fast_map[pgd_offset][pmd_offset][pte_offset][offset] != NULL)
    {
        free(fast_map[pgd_offset][pmd_offset][pte_offset][offset]);
    }
    fast_map[pgd_offset][pmd_offset][pte_offset][offset] = (mm_fast_map) NULL;
    return 1;
}

void memory_read_hit_pgd_item(mm_fast_map pgd, uint32_t addr, void *val, size_t count)
{
    memory_addr_info_t *ptr = (memory_addr_info_t *) get_memory_pgd_item(pgd, addr);

    uint32_t null_data = 0;

    if (ptr)
    {
        if (ptr->addr_access_lock == 0)
        {
            ptr->addr_access_lock = 1;
            if (ptr->mm_type & MO_READ_OP)
            {
                if (ptr->data_read_width == 0)
                {
                    if (ptr->read_callback_ptr != NULL)
                    {
                        ptr->read_callback_ptr(ptr->conf_obj, MO_READ_OP, ptr->addr, (void *) &null_data, count);
                    }
                    if (ptr->py_read_callback_ptr != NULL)
                    {
                        ptr->py_read_callback_ptr(ptr->py_value[0]);
                    }
                }
                ptr->data_read_width += count;
                if (ptr->data_read_width == ptr->data_width)
                {
                    ptr->data_read_width = 0;
                }
            }
            ptr->addr_access_lock = 0;
        }
    }
}

int check_comparsion(memory_addr_info_t * ptr, uint32_t addr, size_t count)
{
    // Unconditional comparsion
    if (ptr->mm_type & MO_WRITE_OP)
    {
        // memory write operation: Special treatment double type
        if (ptr->write_callback_ptr != NULL)
        {
            ptr->write_callback_ptr(ptr->conf_obj, MO_WRITE_OP, ptr->addr, (void *) &ptr->curr_hex_value, count);
        }
        if (ptr->py_write_callback_ptr != NULL)
        {
            ptr->py_write_callback_ptr(ptr->py_value[1]);
        }
    }
    if (ptr->mm_type & MO_COND_OP)
    {
        if (verif_memory_cmd(ptr, (void *) &ptr->curr_hex_value))
        {
            ptr->cond_callback_ptr(ptr->conf_obj, MO_COND_OP, ptr->addr, (void *) &ptr->curr_hex_value, count);
        }
    }
}

void memory_write_hit_pgd_item(mm_fast_map pgd, uint32_t addr, void *val, size_t count)
{
    memory_addr_info_t *ptr = (memory_addr_info_t *) get_memory_pgd_item(pgd, addr);
    uint32_t temp_data_width,temp_write_data_width;
    uint64_t temp_curr_hex_value;

    if (ptr)
    {
        if (ptr->addr_access_lock == 0)
        {
            ptr->addr_access_lock = 1;

            uint64_t rel_value = 0;

            switch (count)
            {
                case 1:
                    {
                        uint8_t v = *(uint8_t *) val;

                        rel_value = (uint64_t) v;
                        break;
                    }
                case 2:
                    {
                        uint16_t v = *(uint16_t *) val;

                        rel_value = (uint64_t) v;
                        break;
                    }
                case 4:
                    {
                        uint32_t v = *(uint32_t *) val;

                        rel_value = (uint64_t) v;
                        break;
                    }
                case 8:
                    {
                        rel_value = *(uint64_t *) val;
                        break;
                    }
                default:
                    {
                        break;
                    }
            }

            ptr->curr_hex_value |= ((rel_value) << (ptr->data_write_width * 8));
            ptr->data_write_width += count;
            if (ptr->data_write_width == ptr->data_width)
            {
                check_comparsion(ptr, addr, ptr->data_width);
                ptr->data_write_width = 0;
                ptr->curr_hex_value = 0;
                ptr->addr_access_lock = 0;
            }
            else if (ptr->data_write_width > ptr->data_width)
            {
                temp_curr_hex_value = ptr->curr_hex_value <<(ptr->data_width * 8);
                temp_write_data_width = ptr->data_write_width - ptr->data_width;
                temp_data_width = ptr->data_width;
                check_comparsion(ptr, addr, temp_data_width);
                ptr->data_write_width = 0;
                ptr->curr_hex_value = 0;
                ptr->addr_access_lock = 0;
                memory_write_hit_pgd_item(pgd,addr + temp_data_width,&temp_curr_hex_value,temp_write_data_width);
            }
            else
            {
                if (ptr->data_width <= 4)
                {
                    check_comparsion(ptr, addr, ptr->data_width);
                    ptr->data_write_width = 0;
                    ptr->curr_hex_value = 0;
                }
                else
                {
                    ptr->data_write_width = ptr->data_width - count;
                }
            }
            ptr->addr_access_lock = 0;
        }
    }
}

/*
 * OP_INT8
 * OP_UINT8
 * OP_INT16
 * OP_UINT16
 * OP_INT32
 * OP_UINT32
 * OP_INT64
 * OP_UINT64
 * OP_FLOAT
 * OP_DOUBLE
 */
int bit_width[10] = { 1, 1, 2, 2, 4, 4, 8, 8, 4, 8 };

int setting_bit_width(memory_addr_info_t * ptr, OP_DataType_t data_type)
{
    ptr->data_width = bit_width[data_type];
}

int decode_memory_cmd(memory_addr_info_t * ptr, void *value, cond_compare_t cond_compare)
{
    switch (cond_compare)
    {
        case Cond_EQ:
        case Cond_NEQ:
        case Cond_GT:
        case Cond_LT:
        case Cond_GTE:
        case Cond_LTE:
            ptr->logic_exp.comp_flag = cond_compare;
            break;
        case No_Cond:
            ptr->logic_exp.comp_flag = No_Cond;
            return 1;
        default:
            ptr->logic_exp.comp_flag = Cond_Error;
            return -1;
    }

    if (value == NULL)
    {
        fprintf(stderr, "decode_memory_cmd: value is NULL!!\n");
        return -2;
    }

    switch (ptr->type)
    {
        case OP_INT8:
            {
                int8_t v = *(int8_t *) value;

                ptr->logic_exp.value.int8_v = v;
                break;
            }
        case OP_UINT8:
            {
                uint8_t v = *(uint8_t *) value;

                ptr->logic_exp.value.uint8_v = v;
                break;
            }
        case OP_INT16:
            {
                int16_t v = *(int16_t *) value;

                ptr->logic_exp.value.int16_v = v;
                break;
            }
        case OP_UINT16:
            {
                uint16_t v = *(uint16_t *) value;

                ptr->logic_exp.value.uint16_v = v;
                break;
            }
        case OP_INT32:
            {
                int32_t v = *(int32_t *) value;

                ptr->logic_exp.value.int32_v = v;
                break;
            }
        case OP_UINT32:
            {
                uint32_t v = *(uint32_t *) value;

                ptr->logic_exp.value.uint32_v = v;
                break;
            }
        case OP_INT64:
            {
                int64_t v = *(int64_t *) value;

                ptr->logic_exp.value.int64_v = v;
                break;
            }
        case OP_UINT64:
            {
                uint64_t v = *(uint64_t *) value;

                ptr->logic_exp.value.uint64_v = v;
                break;
            }
        case OP_FLOAT:
            {
                float v = *(float *) value;

                ptr->logic_exp.value.fl32_v = v;
                break;
            }
        case OP_DOUBLE:
            {
                double v = *(double *) value;

                ptr->logic_exp.value.fl64_v = v;
                break;
            }
        default:
            return -1;
            break;
    }

    return 1;
}

#define Complare(comp_flag) switch(comp_flag) {\
	case Cond_EQ:\
				 return (tmp1 == tmp2);\
	case Cond_NEQ:\
				  return (tmp1 != tmp2);\
	case Cond_GT:\
				 return (tmp1 > tmp2);\
	case Cond_LT:\
				 return (tmp1 < tmp2);\
	case Cond_GTE:\
				  return (tmp1 >= tmp2);\
	case Cond_LTE:\
				  return (tmp1 <= tmp2);\
	default:\
			return False;\
}\

bool verif_memory_cmd(memory_addr_info_t * ptr, void *val)
{
    switch (ptr->type)
    {
        case OP_INT8:
            {
                int8_t tmp1 = *(int8_t *) val;
                int8_t tmp2 = ptr->logic_exp.value.int8_v;

                Complare(ptr->logic_exp.comp_flag);
                break;
            }
        case OP_UINT8:
            {
                uint8_t tmp1 = *(uint8_t *) val;
                uint8_t tmp2 = ptr->logic_exp.value.uint8_v;

                Complare(ptr->logic_exp.comp_flag);
                break;
            }
        case OP_INT16:
            {
                int16_t tmp1 = *(int16_t *) val;
                int16_t tmp2 = ptr->logic_exp.value.int16_v;

                Complare(ptr->logic_exp.comp_flag);
                break;
            }
        case OP_UINT16:
            {
                uint32_t tmp1 = *(uint32_t *) val;
                uint32_t tmp2 = ptr->logic_exp.value.uint16_v;

                Complare(ptr->logic_exp.comp_flag);
                break;
            }
        case OP_INT32:
            {
                int32_t tmp1 = *(int32_t *) val;
                int32_t tmp2 = ptr->logic_exp.value.int32_v;

                Complare(ptr->logic_exp.comp_flag);
                break;
            }
        case OP_UINT32:
            {
                uint32_t tmp1 = *(uint32_t *) val;
                uint32_t tmp2 = ptr->logic_exp.value.uint32_v;

                Complare(ptr->logic_exp.comp_flag);
                break;
            }
        case OP_INT64:
            {
                int64_t tmp1 = *(int64_t *) val;
                int64_t tmp2 = ptr->logic_exp.value.int64_v;

                Complare(ptr->logic_exp.comp_flag);
                break;
            }
        case OP_UINT64:
            {
                uint64_t tmp1 = *(uint64_t *) val;
                uint64_t tmp2 = ptr->logic_exp.value.uint64_v;

                Complare(ptr->logic_exp.comp_flag);
                break;
            }
        case OP_FLOAT:
            {
                float tmp1 = *(float *) val;
                float tmp2 = ptr->logic_exp.value.fl32_v;

                Complare(ptr->logic_exp.comp_flag);
                break;
            }
        case OP_DOUBLE:
            {
                double tmp1 = *(double *) val;
                double tmp2 = ptr->logic_exp.value.fl64_v;

                Complare(ptr->logic_exp.comp_flag);
                break;
            }
        default:
            break;
    }

    return False;
}

int Py_SetWatchOnMemory(conf_object_t * conf_obj, char *ms_name, int mm_type, uint32_t m_addr,
                        uint32_t data_type, uint32_t m_length, py_memory_callback_t callback, void *user_data)
{
    uint32_t change_mm_status = 0;
    uint32_t old_mm_type = 0;

    conf_object_t *memory_space_obj = (conf_object_t *) get_conf_obj(ms_name);

    if (memory_space_obj == NULL)
    {
        return 0;
    }

    addr_space_t *mem_obj = (addr_space_t *) memory_space_obj->obj;

    memory_addr_info_t *addr_info = (memory_addr_info_t *) get_memory_pgd_item(mem_obj->memory_monitor, m_addr);

    if (addr_info == NULL)
    {
        addr_info = (memory_addr_info_t *) skyeye_mm_zero(sizeof (memory_addr_info_t));
        if (addr_info == NULL)
        {
            return 0;
        }
    } else
    {
        change_mm_status = 1;
    }
    // 改变监视状态
    if (change_mm_status)
    {
        old_mm_type = addr_info->mm_type;
        addr_info->mm_type |= mm_type;
        if ((addr_info->addr != m_addr) || (addr_info->conf_obj != conf_obj))
        {
            return -2;
        }
    } else
    {
        addr_info->mm_type = mm_type;
        addr_info->addr = m_addr;
        addr_info->conf_obj = conf_obj;

        // 该地址第一次设置监视时, 设置其数据的地址宽度
        addr_info->data_width = data_type;
    }

    if ((addr_info->mm_type & MO_READ_OP) && ((old_mm_type & MO_READ_OP) == 0))
    {
        addr_info->py_value[0] = user_data;
        addr_info->py_read_callback_ptr = callback;
    }
    if ((addr_info->mm_type & MO_WRITE_OP) && ((old_mm_type & MO_WRITE_OP) == 0))
    {
        addr_info->py_value[1] = user_data;
        addr_info->py_write_callback_ptr = callback;
    }

    if (addr_info->data_width <= 4)
    {
        if (addr_info->data_width == 4 && (addr_info->mm_type & MO_C28X_OP))
        {
            addr_info->data_width = 2;
            insert_memory_pgd_intem(mem_obj->memory_monitor, addr_info->addr + 1, addr_info);
        } else
            insert_memory_pgd_intem(mem_obj->memory_monitor, addr_info->addr, addr_info);
    } else
    {
        if (addr_info->mm_type & MO_C28X_OP)
        {
            addr_info->data_width = 2;
            insert_memory_pgd_intem(mem_obj->memory_monitor, addr_info->addr + 3, addr_info);
        } else
        {
            insert_memory_pgd_intem(mem_obj->memory_monitor, addr_info->addr, addr_info);
            insert_memory_pgd_intem(mem_obj->memory_monitor, addr_info->addr + 4, addr_info);
        }
    }

    return 1;
}

/*
 * ms_name:    memory space名称
 * m_type:     监视类型
 * m_addr:     监视地址
 * data_type:  数据类型
 * cond:       条件类型
 * value:      条件值, 无条件监视类型忽略
 * callback    监视回调函数
 */
int SkyEye_Memory_Monitor(conf_object_t * conf_obj, char *ms_name, int mm_type, uint32_t m_addr,
                          OP_DataType_t data_type, cond_compare_t cond, void *value, memory_callback_t callback)
{
    uint32_t change_mm_status = 0;
    uint32_t old_mm_type = 0;

    conf_object_t *memory_space_obj = (conf_object_t *) get_conf_obj(ms_name);

    if (memory_space_obj == NULL)
    {
        return 0;
    }

    addr_space_t *mem_obj = (addr_space_t *) memory_space_obj->obj;

    memory_addr_info_t *addr_info = (memory_addr_info_t *) get_memory_pgd_item(mem_obj->memory_monitor, m_addr);

    if (addr_info == NULL)
    {
        addr_info = (memory_addr_info_t *) skyeye_mm_zero(sizeof (memory_addr_info_t));
        if (addr_info == NULL)
        {
            return 0;
        }
    } else
    {
        change_mm_status = 1;
    }

    // 改变监视状态
    if (change_mm_status)
    {
        old_mm_type = addr_info->mm_type;
        addr_info->mm_type |= mm_type;
        if ((addr_info->addr != m_addr) || (addr_info->type != data_type) || (addr_info->conf_obj != conf_obj))
        {
            return -2;
        }
    } else
    {
        addr_info->mm_type = mm_type;
        addr_info->addr = m_addr;
        addr_info->type = data_type;
        addr_info->conf_obj = conf_obj;

        // 该地址第一次设置监视时, 设置其数据的地址宽度
        setting_bit_width(addr_info, data_type);
    }

    if ((addr_info->mm_type & MO_READ_OP) && ((old_mm_type & MO_READ_OP) == 0))
    {
        addr_info->read_callback_ptr = callback;
    }
    if ((addr_info->mm_type & MO_WRITE_OP) && ((old_mm_type & MO_WRITE_OP) == 0))
    {
        addr_info->write_callback_ptr = callback;
    }
    if ((addr_info->mm_type & MO_COND_OP) && ((old_mm_type & MO_COND_OP) == 0))
    {
        addr_info->cond_callback_ptr = callback;
        if (decode_memory_cmd(addr_info, value, cond) == 0)
        {
            return 0;
        }
    }

    if (addr_info->data_width <= 4)
    {
        if (addr_info->data_width == 4 && (addr_info->mm_type & MO_C28X_OP))
        {
            addr_info->data_width = 2;
            insert_memory_pgd_intem(mem_obj->memory_monitor, addr_info->addr + 1, addr_info);
        } else
            insert_memory_pgd_intem(mem_obj->memory_monitor, addr_info->addr, addr_info);
    } else
    {
        if (addr_info->mm_type & MO_C28X_OP)
        {
            addr_info->data_width = 2;
            insert_memory_pgd_intem(mem_obj->memory_monitor, addr_info->addr + 3, addr_info);
        } else
        {
            insert_memory_pgd_intem(mem_obj->memory_monitor, addr_info->addr, addr_info);
            insert_memory_pgd_intem(mem_obj->memory_monitor, addr_info->addr + 4, addr_info);
        }
    }

    return 1;
}

/*
 * cpu_name:   CPU名称
 * m_addr:     监视地址
 */
int SkyEye_Memory_Monitor_Remove(char *cpu_name, int mm_type, uint32_t m_addr)
{
    conf_object_t *memory_space_obj = (conf_object_t *) get_conf_obj(cpu_name);

    if (memory_space_obj == NULL)
    {
        return 0;
    }

    addr_space_t *mem_obj = (addr_space_t *) memory_space_obj->obj;

    memory_addr_info_t *ptr = (memory_addr_info_t *) get_memory_pgd_item(mem_obj->memory_monitor, m_addr);

    if (ptr)
    {
        uint32_t new_mm_type;

        new_mm_type = (mm_type & ptr->mm_type);
        new_mm_type ^= ptr->mm_type;
        if (new_mm_type == 0)
        {
            if (ptr->data_width == 8)
            {
                if (mm_type & MO_C28X_OP)
                {
                    clear_pgd_item(mem_obj->memory_monitor, ptr->addr + 3, mm_type);
                } else
                {
                    clear_pgd_item(mem_obj->memory_monitor, ptr->addr, mm_type);
                    clear_pgd_item(mem_obj->memory_monitor, ptr->addr + 4, mm_type);
                }
            } else
            {
                if (ptr->data_width == 4 && (mm_type & MO_C28X_OP))
                    clear_pgd_item(mem_obj->memory_monitor, ptr->addr + 1, mm_type);
                else
                    clear_pgd_item(mem_obj->memory_monitor, ptr->addr, mm_type);
            }
        } else
        {
            ptr->mm_type = new_mm_type;
        }
    }

    return 0;
}
