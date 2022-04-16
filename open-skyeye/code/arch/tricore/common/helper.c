/*************************************************************************
	> File Name    : helper.c
	> Author       : jiachao
	> Mail         : jiachao@digiproto.com
	> Created Time : 2021/10/26 10:06:02
************************************************************************/

#include "cpu.h"
#include "helper.h"
#include "softfloat.h"

enum {
    TLBRET_DIRTY = -4,
    TLBRET_INVALID = -3,
    TLBRET_NOMATCH = -2,
    TLBRET_BADADDR = -1,
    TLBRET_MATCH = 0
};

static int get_physical_address(tricore_cpu_t *core, uint64_t *physical,
                                int *prot, generic_address_t address,
                                int rw, int access_type)
{
    int ret = TLBRET_MATCH;

    *physical = address & 0xFFFFFFFF;
    *prot = PAGE_READ | PAGE_WRITE | PAGE_EXEC;

    return ret;
}

/* TODO: Add exeption support*/
static void raise_mmu_exception(tricore_cpu_t *core, generic_address_t address,
                                int rw, int tlb_error)
{
}

int cpu_tricore_handle_mmu_fault(tricore_cpu_t *core, generic_address_t address,
                                 int rw, int mmu_idx)
{
    uint64_t physical;
    int prot;
    int access_type;
    int ret = 0;

    rw &= 1;
    access_type = ACCESS_INT;
    ret = get_physical_address(core, &physical, &prot,
                               address, rw, access_type);
    skyeye_log(Info_log, __FUNCTION__, "address = 0x%x ret %d physical address = 0x%x\n", address, ret, physical);

    if (ret == TLBRET_MATCH)
    {
        /*tlb_set_page(cs, address & TARGET_PAGE_MASK,
                     physical & TARGET_PAGE_MASK, prot | PAGE_EXEC,
                     mmu_idx, TARGET_PAGE_SIZE);
                     */
        ret = 0;
    } else if (ret < 0)
    {
        raise_mmu_exception(core, address, rw, ret);
        ret = 1;
    }

    return ret;
}

void fpu_set_state(tricore_cpu_t *core)
{
    set_float_rounding_mode(core->PSW & MASK_PSW_FPU_RM, &core->fp_status);
    set_flush_inputs_to_zero(1, &core->fp_status);
    set_flush_to_zero(1, &core->fp_status);
    set_default_nan_mode(1, &core->fp_status);
}

uint32_t psw_read(tricore_cpu_t *core)
{
    /* clear all USB bits */
    core->PSW &= 0x6ffffff;
    /* now set them from the cache */
    core->PSW |= ((core->PSW_USB_C != 0) << 31);
    core->PSW |= ((core->PSW_USB_V   & (1 << 31))  >> 1);
    core->PSW |= ((core->PSW_USB_SV  & (1 << 31))  >> 2);
    core->PSW |= ((core->PSW_USB_AV  & (1 << 31))  >> 3);
    core->PSW |= ((core->PSW_USB_SAV & (1 << 31))  >> 4);

    return core->PSW;
}

void psw_write(tricore_cpu_t *core, uint32_t val)
{
    core->PSW_USB_C = (val & MASK_USB_C);
    core->PSW_USB_V = (val & MASK_USB_V) << 1;
    core->PSW_USB_SV = (val & MASK_USB_SV) << 2;
    core->PSW_USB_AV = (val & MASK_USB_AV) << 3;
    core->PSW_USB_SAV = (val & MASK_USB_SAV) << 4;
    core->PSW = val;

    fpu_set_state(core);
}

