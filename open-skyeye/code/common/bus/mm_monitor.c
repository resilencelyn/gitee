#include <stdio.h>
#include "mm_monitor.h"
#include "skyeye_mm.h"
#include <string.h>

#define PGD(addr)  ((addr&0xE0000000)>>29)
#define PMD(addr)  ((addr&0x3FF00000)>>20)
#define PTE(addr)  ((addr&0x3FF000)>>12)
#define OFFSET(addr)  ((addr&0x3FF))
#define PGD_SIZE   (0xE0000000>>29)
#define PMD_SIZE   (0x3FF00000>>20)
#define PTE_SIZE   (0x3FF000>>12)
#define PAGE_LEN  (0xFFF)

pgd_t create_pgd()
{
    pgd_t ptr = (pgd_t) skyeye_mm_zero(PGD_SIZE * sizeof (pgd_t));

    return ptr;
}

pmd_t alloc_pmd()
{
    pmd_t ptr = (pmd_t) skyeye_mm_zero(PMD_SIZE * sizeof (pmd_t));

    return ptr;
}

pmd_t alloc_pte()
{
    pte_t ptr = (pmd_t) skyeye_mm_zero(PTE_SIZE * sizeof (pte_t));

    return ptr;
}

addr_info_t *alloc_page()
{
    addr_info_t *ptr = (addr_info_t *) skyeye_mm_zero(PAGE_LEN * sizeof (addr_info_t *));

    return ptr;
}

void insert_pgd_intem(pgd_t pgd, uint32_t addr, addr_info_t * inf)
{
    unsigned pgd_offset, pmd_offset, pte_offset, offset;

    pgd_offset = PGD(addr);
    pmd_offset = PMD(addr);
    pte_offset = PTE(addr);
    offset = OFFSET(addr);
    if (!pgd[pgd_offset])
    {
        pgd[pgd_offset] = alloc_pmd();
    }
    if (!pgd[pgd_offset][pmd_offset])
    {
        pgd[pgd_offset][pmd_offset] = alloc_pmd();
    }
    if (!pgd[pgd_offset][pmd_offset][pte_offset])
    {
        pgd[pgd_offset][pmd_offset][pte_offset] = alloc_pte();
    }
    if (!pgd[pgd_offset][pmd_offset][pte_offset][offset])
    {
        pgd[pgd_offset][pmd_offset][pte_offset][offset] = alloc_page();

    }
    pgd[pgd_offset][pmd_offset][pte_offset][offset] = inf;
}
void clear_pgd_item(pgd_t pgd, unsigned addr)
{
    unsigned pgd_offset, pmd_offset, pte_offset, offset;

    pgd_offset = PGD(addr);
    pmd_offset = PMD(addr);
    pte_offset = PTE(addr);
    offset = OFFSET(addr);
    if (!pgd[pgd_offset])
    {
        return;
    }
    if (!pgd[pgd_offset][pmd_offset])
    {
        return;
    }
    if (!pgd[pgd_offset][pmd_offset][pte_offset])
        return;
    if (!pgd[pgd_offset][pmd_offset][pte_offset][offset])
        return;
    pgd[pgd_offset][pmd_offset][pte_offset][offset] = (addr_info_t *) NULL;
}

addr_info_t *get_pgd_item(pgd_t pgd, unsigned addr)
{
    unsigned pgd_offset, pmd_offset, pte_offset, offset;

    pgd_offset = PGD(addr);
    pmd_offset = PMD(addr);
    pte_offset = PTE(addr);
    offset = OFFSET(addr);
    if (!pgd[pgd_offset])
    {
        return (addr_info_t *) NULL;
    }
    if (!pgd[pgd_offset][pmd_offset])
    {
        return (addr_info_t *) NULL;
    }
    if (!pgd[pgd_offset][pmd_offset][pte_offset])
    {
        return (addr_info_t *) NULL;
    }
    if (!pgd[pgd_offset][pmd_offset][pte_offset][offset])
    {
        return (addr_info_t *) NULL;
    }
    return pgd[pgd_offset][pmd_offset][pte_offset][offset];
}

void hit_pgd_item(pgd_t pgd, unsigned addr, uint8_t val)
{
    addr_info_t *ptr = get_pgd_item(pgd, addr);

    if (ptr)
        ptr->callback_ptr(ptr, addr, val);
}

int get_items(char **its, char *exp)
{
    char sep[] = " ";
    int cnt = 0;
    char *ptr = NULL;

    for (ptr = strtok(exp, sep); ptr != NULL; ptr = strtok(NULL, sep))
    {
        its[cnt++] = ptr;
    }
    return cnt;
}

void decode_cmd(addr_info_t * ptr, char **items)
{

    if (!strcmp(items[3], "=="))
    {
        ptr->logic_exp[0].comp_flag = Equal;
    } else if (!strcmp(items[3], ">"))
    {
        ptr->logic_exp[0].comp_flag = Greater;
    } else if (!strcmp(items[3], ">="))
    {
        ptr->logic_exp[0].comp_flag = GEqual;
    } else if (!strcmp(items[3], "<"))
    {
        ptr->logic_exp[0].comp_flag = Less;
    } else if (!strcmp(items[3], "<="))
    {
        ptr->logic_exp[0].comp_flag = LEqual;
    } else
    {
        ptr->exp_enable = False;
        return;
    }
    //Converts the value of a string type to a value of the corresponding type
    switch (ptr->type)
    {

        case MM_UINT32:
            {
                unsigned v = atoi(items[4]);

                ptr->logic_exp[0].v = v;
                break;
            }
        case MM_INT32:
            {
                int v = atoi(items[4]);

                ptr->logic_exp[0].v = v;
                break;
            }
        case MM_UINT64:
            {
                unsigned long long v = atoi(items[4]);

                ptr->logic_exp[0].v = v;
                break;
            }
        case MM_INT64:
            {
                long long v = atoi(items[4]);

                ptr->logic_exp[0].v = v;
                break;
            }
        case MM_DOUBLE:
        case MM_FLOAT:
            {
                double v = atof(items[4]);

                ptr->logic_exp[0].v = *(unsigned long long *) (&v);
                break;
            }
        default:
            break;
    }
    ptr->exp_enable = True;
}

#define Complare(comp_flag) switch(comp_flag){\
				case Equal:\
					return (tmp1 == tmp2);\
				case Greater:\
					return (tmp1 > tmp2);\
				case Less:\
					return (tmp1 < tmp2);\
				case GEqual:\
					return (tmp1 >= tmp2);\
				case LEqual:\
					return (tmp1 <= tmp2);\
				default:\
					return False;\
	}\

bool_t verif_cmd(addr_info_t * ptr, unsigned long long val)
{
    switch (ptr->type)
    {
        case MM_UINT32:
            {
                unsigned tmp1 = (unsigned) val;
                unsigned tmp2 = (unsigned) (ptr->logic_exp[0].v);

                Complare(ptr->logic_exp[0].comp_flag);
                break;
            }
        case MM_INT32:
            {
                int tmp1 = (int) val;
                int tmp2 = (int) (ptr->logic_exp[0].v);

                Complare(ptr->logic_exp[0].comp_flag);
                break;
            }
        case MM_UINT64:
            {
                unsigned long long tmp1 = (unsigned long long) val;
                unsigned long long tmp2 = (unsigned long long) (ptr->logic_exp[0].v);

                Complare(ptr->logic_exp[0].comp_flag);
                break;
            }
        case MM_INT64:
            {
                long long tmp1 = (long long) val;
                long long tmp2 = (long long) (ptr->logic_exp[0].v);

                Complare(ptr->logic_exp[0].comp_flag);
                break;
            }
        case MM_DOUBLE:
        case MM_FLOAT:
            {
                double tmp1 = *(double *) &val;
                double tmp2 = *(double *) (&(ptr->logic_exp[0].v));

                //printf("tmp1 = %f ,", tmp1);
                //printf("tmp2 = %f\n", tmp2);
                Complare(ptr->logic_exp[0].comp_flag);
                break;
            }

    }

}
