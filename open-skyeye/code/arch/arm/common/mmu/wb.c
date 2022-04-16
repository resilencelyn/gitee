#include "armdefs.h"
#include "bank_defs.h"
#include <skyeye_mm.h>

/* wb_init
 * @wb_t        :wb_t to init
 * @num         :num of entrys
 * @nb          :num of byte of each entry
 *
 * $    -1:error
 *               0:ok
 * */
int mmu_wb_init(wb_s * wb_t, int num, int nb)
{
    int i;
    wb_entry_t *entrys, *wb;

    entrys = (wb_entry_t *) skyeye_mm_zero(sizeof (*entrys) * num);
    if (entrys == NULL)
    {
        err_msg("malloc size %d\n", sizeof (*entrys) * num);
        goto entrys_malloc_error;
    }

    for (wb = entrys, i = 0; i < num; i++, wb++)
    {
        /*chy 2004-06-06, fix bug found by wenye@cs.ucsb.edu */
        //wb->data = (ARMword *)malloc(sizeof(ARMword) * nb);
        wb->data = (ARMword *) malloc(nb);
        if (wb->data == NULL)
        {
            err_msg("malloc size of %d\n", nb);
            goto data_malloc_error;
        }

    };

    wb_t->first = wb_t->last = wb_t->used = 0;
    wb_t->num = num;
    wb_t->nb = nb;
    wb_t->entrys = entrys;
    return 0;

  data_malloc_error:
    while (--i >= 0)
        free(entrys[i].data);
    free(entrys);
  entrys_malloc_error:
    return -1;
};

/* wb_exit
 * @wb_t :wb_t to exit
 * */
void mmu_wb_exit(wb_s * wb_t)
{
    int i;
    wb_entry_t *wb;

    wb = wb_t->entrys;
    for (i = 0; i < wb_t->num; i++, wb++)
    {
        free(wb->data);
    }
    free(wb_t->entrys);
};

/* wb_write_words :put words in Write Buffer
 * @state:      ARMul_State
 * @wb_t:       write buffer
 * @pa:         physical address
 * @data:       data ptr
 * @n           number of word to write
 *
 * Note: write buffer merge is not implemented, can be done late
 * */
void mmu_wb_write_bytes(ARMul_State * state, wb_s * wb_t, ARMword pa, ARMbyte * data, int n)
{
    int i;
    wb_entry_t *wb;

    while (n)
    {
        if (wb_t->num == wb_t->used)
        {
            /*clean the last wb entry */
            ARMword t;

            wb = &wb_t->entrys[wb_t->last];
            t = wb->pa;
            for (i = 0; i < wb->nb; i++)
            {
                //mem_write_byte (state, t, wb->data[i]);
                //bus_write(8, t, wb->data[i]);
                state->memory_space_iface->write(state->memory_space, t, &(wb->data[i]), 1);
                //t += WORD_SIZE;
                t++;
            }
            wb_t->last++;
            if (wb_t->last == wb_t->num)
                wb_t->last = 0;
            wb_t->used--;
        }

        wb = &wb_t->entrys[wb_t->first];
        i = (n < wb_t->nb) ? n : wb_t->nb;

        wb->pa = pa;
        //pa += i << WORD_SHT;
        pa += i;

        wb->nb = i;
        //memcpy(wb->data, data, i << WORD_SHT);
        memcpy(wb->data, data, i);
        data += i;
        n -= i;
        wb_t->first++;
        if (wb_t->first == wb_t->num)
            wb_t->first = 0;
        wb_t->used++;
    };
}

/* wb_drain_all
 * @wb_t wb_t to drain
 * */
void mmu_wb_drain_all(ARMul_State * state, wb_s * wb_t)
{
    ARMword pa;
    wb_entry_t *wb;
    int i;

    while (wb_t->used)
    {
        wb = &wb_t->entrys[wb_t->last];
        pa = wb->pa;
        for (i = 0; i < wb->nb; i++)
        {
            //mem_write_byte (state, pa, wb->data[i]);
            //bus_write(8, pa, wb->data[i]);
            state->memory_space_iface->write(state->memory_space, pa, &(wb->data[i]), 1);
            //pa += WORD_SIZE;
            pa++;
        }
        wb_t->last++;
        if (wb_t->last == wb_t->num)
            wb_t->last = 0;
        wb_t->used--;
    };
}
