#include <skyeye_lock.h>
#include <skyeye_types.h>
#include <skyeye_sched.h>
#include <skyeye_signal.h>
#include <skyeye_class.h>
#include <skyeye_interface.h>
#include <skyeye_iface.h>
#include <skyeye_obj.h>
#include <skyeye_mm.h>
#include <memory_space.h>
#include <skyeye_device.h>
#include <skyeye_core.h>
#include <skyeye_signal.h>
#define DEBUG
#include <skyeye_log.h>
#include <skyeye_net_intf.h>
#include <skyeye_system.h>
#include <skyeye_swapendian.h>
#include "eTSEC_8641d.h"

static void miim_read_cycle(eTSEC_8641d_device *etsec)
{
    uint8_t  phy;
    uint8_t  addr;
    uint16_t value;

    phy  = (etsec->regs.miimadd.all >> 8) & 0x1F;
    addr = etsec->regs.miimadd.all & 0x1F;

    switch (addr)
    {
        case MIIM_CONTROL:
            value = etsec->phy_control;
            break;
        case MIIM_STATUS:
            value = etsec->phy_status;
            break;
        case MIIM_T2_STATUS:
            value = 0x1800;           /* Local and remote receivers OK */
            break;
        default:
            value = 0x0;
            break;
    };

    etsec->regs.miimstat = value;
}

static void miim_write_cycle(eTSEC_8641d_device *etsec)
{
    uint8_t  phy;
    uint8_t  addr;
    uint16_t value;

    phy   = (etsec->regs.miimadd.all >> 8) & 0x1F;
    addr  = etsec->regs.miimadd.all & 0x1F;
    value = etsec->regs.miimcon & 0xffff;

    switch (addr)
    {
        case MIIM_CONTROL:
            etsec->phy_control = value & ~(0x8100);
            break;
        default:
            break;
    };
}

static void fill_rx_bd(eTSEC_8641d_device *etsec, ETSEC_DESC *bd, const uint8_t **buf, size_t *size)
{
    uint16_t to_write;
    get_page_t *bufptr_page;
    uint32_t bufptr = bd->bufptr + ((uint32_t)(etsec->regs.tbdbph & 0xF) << 32);
    uint8_t  *bufptr_pbuffer;
    uint8_t  padd[etsec->rx_padding];
    uint8_t  rem;

    bd->bdLen = 0;
    if (etsec->memory_iface)
    {
        bufptr_page = etsec->memory_iface->get_page(etsec->memory, bufptr);
        bufptr_pbuffer = (uint8_t *)(bufptr_page->page_base + (bufptr & 0xfff)); //获取bd->bufptr首地址
    } else
    {
        printf("In %s, line:%d, Not register memory_space iface!\n", __func__, __LINE__);
        return ;
    }

    /* This operation will only write FCB */
    if (etsec->rx_fcb_size != 0)
    {
        etsec->rx_fcb[0] |= ETSEC_RX_FCB_IP | ETSEC_RX_FCB_CIP | ETSEC_RX_FCB_CTU;
        memcpy(bufptr_pbuffer, etsec->rx_fcb, etsec->rx_fcb_size);
        bufptr_pbuffer    += etsec->rx_fcb_size;
        bd->bdLen         += etsec->rx_fcb_size;
        etsec->rx_fcb_size  = 0;
    }

    /* We remove padding from the computation of to_write because it is not
     * allocated in the buffer.
     */
    to_write = MIN(*size - etsec->rx_padding,
            etsec->regs.mrblr - etsec->rx_fcb_size);

    /* This operation can only write packet data and no padding */

    if (to_write > 0)
    {
        memcpy(bufptr_pbuffer, *buf, to_write);
        *buf   += to_write;
        bufptr_pbuffer += to_write;
        *size  -= to_write;

        bd->bdSts  &= ~BD_RX_EMPTY;
        bd->bdLen += to_write;
    }
    if (*size == etsec->rx_padding)
    {
        /* The remaining bytes are only for padding which is not actually
         * allocated in the data buffer.
         */
        rem = MIN(etsec->regs.mrblr - bd->bdLen, etsec->rx_padding);

        if (rem > 0)
        {
            memset(padd, 0x0, sizeof(padd));
            etsec->rx_padding -= rem;
            *size             -= rem;
            bd->bdLen        += rem;
            memcpy(bufptr_pbuffer, padd, rem);
        }
    }
}

static void process_tx_fcb(eTSEC_8641d_device *etsec)
{
    uint8_t flags = (uint8_t)(*etsec->tx_data);
    /* L3 header offset from start of frame */
    uint8_t l3_header_offset = (uint8_t)*(etsec->tx_data + 3);
    /* L4 header offset from start of L3 header */
    uint8_t l4_header_offset = (uint8_t)*(etsec->tx_data + 2);
    /* L3 header */
    uint8_t *l3_header = etsec->tx_data + 8 + l3_header_offset;
    /* L4 header */
    uint8_t *l4_header = l3_header + l4_header_offset;

    /* if packet is IP4 and IP checksum is requested */
    if (flags & FCB_TX_IP && flags & FCB_TX_CIP)
    {
        /* do IP4 checksum (TODO This function does TCP/UDP checksum
         * but not sure if it also does IP4 checksum.) */
    }
    /* TODO Check the correct usage of the PHCS field of the FCB in case the NPH
     * flag is on */

    /* if packet is IP4 and TCP or UDP */
    if (flags & FCB_TX_IP && flags & FCB_TX_TUP)
    {
        /* if UDP */
        if (flags & FCB_TX_UDP) {
            /* if checksum is requested */
            if (flags & FCB_TX_CTU)
            {
                /* do UDP checksum */
            } else {
                /* set checksum field to 0 */
                l4_header[6] = 0;
                l4_header[7] = 0;
            }
        } else if (flags & FCB_TX_CTU)
        { /* if TCP and checksum is requested */
            /* do TCP checksum */
        }
    }
}

static void tx_padding_and_crc(eTSEC_8641d_device *etsec, uint32_t min_frame_len)
{
    int add = min_frame_len - etsec->tx_buffer_len;

    if (add > 0)
    {
        etsec->tx_data = realloc(etsec->tx_data, etsec->tx_buffer_len + add);
        memset(etsec->tx_data+etsec->tx_buffer_len, 0, add);
        etsec->tx_buffer_len += add;
    }
}

static void process_tx_bd(eTSEC_8641d_device *etsec, ETSEC_DESC *bd)
{
    struct registers *regs = &etsec->regs;
    uint32_t tbdbth     = (uint32_t)(regs->tbdbph & 0xF) << 32;
    uint8_t *tmp_buff = NULL;
    get_page_t *bufptr_page;
    uint8_t *bufptr_pbuffer;

    if (bd->bdLen == 0)
    {
        /* ERROR */
        return;
    }

    if (etsec->tx_buffer_len == 0)
    {
        /* It's the first BD */
        etsec->first_bd = *bd;
    }
    /* TODO: if TxBD[TOE/UN] skip the Tx Frame Control Block*/
    /* Load this Data Buffer */
    etsec->tx_data = realloc(etsec->tx_data,
            etsec->tx_buffer_len + bd->bdLen);
    tmp_buff = etsec->tx_data + etsec->tx_buffer_len;
    if (etsec->memory_iface)
    {
        bufptr_page = etsec->memory_iface->get_page(etsec->memory, bd->bufptr);
        bufptr_pbuffer = (uint8_t *)(bufptr_page->page_base + (bd->bufptr & 0xfff));
    } else
    {
        printf("In %s, line:%d, Not register memory_space iface!\n", __func__, __LINE__);
        return ;
    }
#ifdef ETSEC_DEBUG
    printf("##################Tx Data:\n");
    {
        int i;
        for(i=0;i<bd->bdLen;i++)
            printf("%x ", bufptr_pbuffer[i]);
        printf("\n");
    }
#endif
    memcpy(tmp_buff, bufptr_pbuffer+tbdbth, bd->bdLen);
    /* Update buffer length */

    etsec->tx_buffer_len += bd->bdLen;

    if (etsec->tx_buffer_len != 0 && (bd->bdSts & BD_LAST))
    {
        if (regs->maccfg1 & MACCFG1_TX_EN) {
            /* MAC Transmit enabled */
            /* Process offload Tx FCB */
            if (etsec->first_bd.bdSts & BD_TX_TOEUN)
            {
                process_tx_fcb(etsec);
            }

            if (etsec->first_bd.bdSts & BD_TX_PADCRC
                    || regs->maccfg2 & MACCFG2_PADCRC)
            {
                /* Padding and CRC (Padding implies CRC) */
                tx_padding_and_crc(etsec, 64);
            } else if (etsec->first_bd.bdSts & BD_TX_TC
                    || regs->maccfg2 & MACCFG2_CRC_EN)
            {

                /* Only CRC */
            }

            if (etsec->first_bd.bdSts & BD_TX_TOEUN)
            {
                if (etsec->net_iface)
                    etsec->net_iface->net_write(NET_DEVICE(etsec), etsec->tx_data+8, etsec->tx_buffer_len - 8);
            } else
            {
                if (etsec->net_iface)
                    etsec->net_iface->net_write(NET_DEVICE(etsec), etsec->tx_data, etsec->tx_buffer_len);
            }
        }

        etsec->tx_buffer_len = 0;

        if (bd->bdSts & BD_INTERRUPT)
        {
            ievent_set(etsec, IEVENT_TXF);
        }
    } else
    {
        if (bd->bdSts & BD_INTERRUPT)
        {
            ievent_set(etsec, IEVENT_TXB);
        }
    }

    /* Update DB flags */
    /* Clear Ready */
    bd->bdSts &= ~BD_TX_READY;
    /* Clear Defer */
    bd->bdSts &= ~BD_TX_PREDEF;
    /* Clear Late Collision */
    bd->bdSts &= ~BD_TX_HFELC;
    /* Clear Retransmission Limit */
    bd->bdSts &= ~BD_TX_CFRL;
    /* Clear Retry Count */
    bd->bdSts &= ~(BD_TX_RC_MASK << BD_TX_RC_OFFSET);
    /* Clear Underrun */
    bd->bdSts &= ~BD_TX_TOEUN;
    /* Clear Truncation */
    bd->bdSts &= ~BD_TX_TR;
}

static void read_buffer_descriptor(eTSEC_8641d_device *dev, uint32_t addr, ETSEC_DESC *bd)
{
    struct registers *regs = &dev->regs;
    assert(bd != NULL);

    if (dev->memory_iface)
    {
        dev->memory_iface->read(dev->memory, addr, &bd->bdSts, 2);
        dev->memory_iface->read(dev->memory, addr+2, &bd->bdLen, 2);
        dev->memory_iface->read(dev->memory, addr+4, &bd->bufptr, 4);
    }
}

static void write_buffer_descriptor(eTSEC_8641d_device *dev,
        uint32_t addr,
        ETSEC_DESC *bd)
{
    assert(bd != NULL);

    if (dev->memory_iface)
    {
        dev->memory_iface->write(dev->memory, addr, &bd->bdSts, 2);
        dev->memory_iface->write(dev->memory, addr+2, &bd->bdLen, 2);
        dev->memory_iface->write(dev->memory, addr+4, &bd->bufptr, 4);
    }
}

void etsec_walk_tx_ring(eTSEC_8641d_device  *dev, int ring_nbr)
{
    struct registers *regs = &dev->regs; 
    ETSEC_DESC bd;
    int ring_base = 0;
    uint32_t bd_addr = 0;
    uint16_t bd_flags = 0;

    if (!(regs->maccfg1 & MACCFG1_TX_EN))
    {
#ifdef ETSEC_DEBUG
        printf("%s: MAC Transmit not enabled\n", __func__);
#endif
        return;
    }

    ring_base = (uint32_t)(regs->tbaseh & 0xF) << 32;
    ring_base += regs->tbase[ring_nbr] & ~0x7;
    bd_addr    = regs->tbptr[ring_nbr] & ~0x7;

    do
    {
        read_buffer_descriptor(dev, bd_addr, &bd);

        /* Save flags before BD update */
        bd_flags = bd.bdSts;

        if (!(bd_flags & BD_TX_READY))
        {
            break;
        }

        process_tx_bd(dev, &bd);
        /* Write back BD after update */
        write_buffer_descriptor(dev, bd_addr, &bd);

        /* Wrap or next BD */
        if (bd_flags & BD_WRAP)
        {
            bd_addr = ring_base;
        } else {
            bd_addr += sizeof(ETSEC_DESC);
        }
    } while (1);

    /* Save the Buffer Descriptor Pointers to last bd that was not
     * succesfully closed */
    regs->tbptr[ring_nbr] = bd_addr;

    /* Set transmit halt THLTx */
    regs->tstat |= 1 << (31 - ring_nbr);
}

void etsec_walk_rx_ring(eTSEC_8641d_device *etsec, int ring_nbr)
{
    uint32_t         ring_base     = 0;
    uint32_t         bd_addr       = 0;
    uint32_t         start_bd_addr = 0;
    ETSEC_DESC  bd;

    uint16_t       bd_flags;
    uint32_t         remaining_data;
    uint8_t *buf;
    uint8_t       *tmp_buf;
    uint32_t         size;

    if (etsec->rx_buffer_len == 0)
    {
        /* No frame to send */
#ifdef ETSEC_DEBUG
        printf("No frame to send\n");
#endif
        return;
    }

    remaining_data = etsec->rx_remaining_data + etsec->rx_padding;
    buf = etsec->rx_data + (etsec->rx_buffer_len - etsec->rx_remaining_data);
    size = etsec->rx_buffer_len + etsec->rx_padding;

    ring_base = (uint32_t)(etsec->regs.rbaseh & 0xF) << 32;
    ring_base += etsec->regs.rbase[ring_nbr] & ~0x7;
    start_bd_addr = bd_addr = etsec->regs.rbptr[ring_nbr] & ~0x7;

    do
    {
        read_buffer_descriptor(etsec, bd_addr, &bd);

        /* Save flags before BD update */
        bd_flags = bd.bdSts;
        if (bd_flags & BD_RX_EMPTY)
        {
            fill_rx_bd(etsec, &bd, &buf, &remaining_data);
            if (etsec->rx_first_in_frame) {
                bd.bdSts |= BD_RX_FIRST;
                etsec->rx_first_in_frame = 0;
                etsec->rx_first_bd = bd;
            }

            /* Last in frame */
            if (remaining_data == 0)
            {
                /* Clear flags */
                bd.bdSts &= ~0x7ff;
                bd.bdSts |= BD_LAST;
                /* NOTE: non-octet aligned frame is impossible*/
                if (size >= etsec->regs.maxfrm)
                {
                    /* frame length violation */
#ifdef ETSEC_DEBUG
                    printf("%s frame length violation: size:%zu MAXFRM:%d\n",
                            __func__, size, etsec->regs.maxfrm);
#endif
                    bd.bdSts |= BD_RX_LG;
                }

                if (size  < 64)
                {
                    /* Short frame */
                    bd.bdSts |= BD_RX_SH;
                }
                /* TODO: Broadcast and Multicast */
                if (bd.bdSts & BD_INTERRUPT)
                {
                    /* Set RXFx */
                    etsec->regs.rstat |= 1 << (7 - ring_nbr);
                    /* Set IEVENT */
                    ievent_set(etsec, IEVENT_RXF);
                }
            } else
            {
                if (bd.bdSts & BD_INTERRUPT)
                {
                    /* Set IEVENT */
                    ievent_set(etsec, IEVENT_RXB);
                }
            }

            /* Write back BD after update */
            write_buffer_descriptor(etsec, bd_addr, &bd);
        }

        /* Wrap or next BD */
        if (bd_flags & BD_WRAP)
        {
            bd_addr = ring_base;
        } else
        {
            bd_addr += sizeof(ETSEC_DESC);
        }

    } while (remaining_data != 0
            && (bd_flags & BD_RX_EMPTY)
            && bd_addr != start_bd_addr);

    /* Reset ring ptr */
    etsec->regs.rbptr[ring_nbr] = bd_addr;

    /* The frame is too large to fit in the Rx ring */
    if (remaining_data > 0)
    {
        /* Set RSTAT[QHLTx] */
        etsec->regs.rstat |= 1 << (23 - ring_nbr);
        /* Save remaining data to send the end of the frame when the ring will
         * be restarted
         */
        etsec->rx_remaining_data = remaining_data;
        /* Copy the frame */
        tmp_buf = malloc(size);
        memcpy(tmp_buf, etsec->rx_data, size);
        etsec->rx_data = tmp_buf;
#ifdef ETSEC_DEBUG
        printf("no empty RxBD available any more\n");
#endif
    } else
    {
        etsec->rx_buffer_len = 0;
    }
}

static void rx_init_frame(eTSEC_8641d_device *etsec, const uint8_t *buf, size_t size)
{
    uint32_t fcb_size = 0;
    uint8_t  prsdep   = (etsec->regs.rctrl >> RCTRL_PRSDEP_OFFSET) & RCTRL_PRSDEP_MASK;

    if (prsdep != 0)
    {
        /* Prepend FCB (FCB size + RCTRL[PAL]) */
        fcb_size = 8 + ((etsec->regs.rctrl >> 16) & 0x1F);
        etsec->rx_fcb_size = fcb_size;
        /* TODO: fill_FCB(etsec); */
        memset(etsec->rx_fcb, 0x0, sizeof(etsec->rx_fcb));
    } else
    {
        etsec->rx_fcb_size = 0;
    }

    /* Do not copy the frame for now */
    etsec->rx_data     = (uint8_t *)buf;
    etsec->rx_buffer_len = size;

    /* CRC padding (We don't have to compute the CRC) */
    etsec->rx_padding = 4;

    /*
     * Ensure that payload length + CRC length is at least 802.3
     * minimum MTU size bytes long (64)
     */
    if (etsec->rx_buffer_len < 60)
    {
        etsec->rx_padding += 60 - etsec->rx_buffer_len;
    }

    etsec->rx_first_in_frame = 1;
    etsec->rx_remaining_data = etsec->rx_buffer_len;
}

ssize_t etsec_rx_ring_write(eTSEC_8641d_device *etsec, const uint8_t *buf, size_t size)
{
    int ring_nbr = 0;           /* Always use ring0 (no filer) */

    if (etsec->rx_buffer_len != 0)
    {
#ifdef ETSEC_DEBUG
        printf("%s: We can't receive now,"
                " a buffer is already in the pipe\n", __func__);
#endif
        return 0;
    }

    if (etsec->regs.rstat & 1 << (23 - ring_nbr))
    {
#ifdef ETSEC_DEBUG
        printf("%s: The ring is halted\n", __func__);
#endif
        return -1;
    }

    if (etsec->regs.dmactrl & DMACTRL_GRS) {
#ifdef ETSEC_DEBUG
        printf("%s: Graceful receive stop\n", __func__);
#endif
        return -1;
    }

    if (!(etsec->regs.maccfg1 & MACCFG1_RX_EN)) {
#ifdef ETSEC_DEBUG
        printf("%s: MAC Receive not enabled\n", __func__);
#endif
        return -1;
    }

    if (!(etsec->regs.rctrl & RCTRL_RSF) && (size < 60))
    {
        /* CRC is not in the packet yet, so short frame is below 60 bytes */
#ifdef ETSEC_DEBUG
        printf("%s: Drop short frame\n", __func__);
#endif
        //return -1;
    }

    rx_init_frame(etsec, buf, size);
    etsec_walk_rx_ring(etsec, ring_nbr);

    return size;
}

static void ievent_set(eTSEC_8641d_device *etsec, uint32_t  flags)
{
    etsec->regs.ievent |= flags;
}

static void check_etsec_interrupt(conf_object_t* opaque)
{
    eTSEC_8641d_device *dev = opaque->obj;
    uint32_t ievent = dev->regs.ievent;
    uint32_t imask  = dev->regs.imask;
    uint32_t active = ievent & imask;

    if (imask == 0)
        return;
    int tx = !!(active & IEVENT_TX_MASK);
    int rx = !!(active & IEVENT_RX_MASK);
    int err = !!(active & IEVENT_ERR_MASK);

    if(dev->signal.obj && tx)
    {
        dev->signal.intf->raise_signal(dev->signal.obj, dev->signal.tx_num);
    }

    if(dev->signal.obj && rx)
    {
        dev->signal.intf->raise_signal(dev->signal.obj, dev->signal.rx_num);
    }

    if(dev->signal.obj && err)
    {
        dev->signal.intf->raise_signal(dev->signal.obj, dev->signal.err_num);
    }
}

static void write_dmactrl(eTSEC_8641d_device *dev, uint32_t value)
{
    struct registers *regs = &dev->regs;

    if (value & DMACTRL_GRS)
    {
        if (dev->rx_buffer_len != 0)
        {
            /* Graceful receive stop delayed until end of frame */
        } else
        {
            /* Graceful receive stop now */
            regs->ievent |= IEVENT_GRSC;
        }
    }
    if (value & DMACTRL_GTS)
    {
        if (dev->tx_buffer_len != 0)
        {
            /* Graceful transmit stop delayed until end of frame */
        } else
        {
            /* Graceful transmit stop now */
            regs->ievent |= IEVENT_GTSC;
        }
    }
}

static void write_tstat(eTSEC_8641d_device *dev, uint32_t value)
{
    struct registers *regs = &dev->regs;
    int i = 0;

    for (i = 0; i < 8; i++)
    {
        /* Check THLTi flag in TSTAT */
        if (value & (1 << (31 - i)))
        {
            etsec_walk_tx_ring(dev, i);
        }
    }
    regs->tstat &= ~value;
}

static void write_rstat(eTSEC_8641d_device *dev, uint32_t value)
{
    struct registers *regs = &dev->regs;
    int i = 0;

    for (i = 0; i < 8; i++)
    {
        /* Check QHLTi flag in RSTAT */
        if (value & (1 << (23 - i)) && !(regs->rstat & (1 << (23 - i))))
        {
            etsec_walk_rx_ring(dev, i);
        }
    }
    regs->rstat &= ~value;
}

static void etsec_receive(void *opaque)
{
    eTSEC_8641d_device *dev = ((conf_object_t *)opaque)->obj;
    struct ethhdr *hdr_ether;
    int size, channel = 0;

    if (dev->net_iface == NULL) return;

    if ((size = dev->net_iface->net_read(NET_DEVICE(dev), dev->rx_data, 1514)) == -1)
        return;

    hdr_ether = (struct ethhdr *)dev->rx_data;
    if ((dev->regs.rqueue >> (7 - channel)) & 0x1)
    {
        /*RxBD queue is halted, All controller receive activity to this queue is halted*/
        if ((dev->regs.rstat & ETSEC_RSTAT_QHLT(channel)) == ETSEC_RSTAT_QHLT(channel))
            return ;
    } else
    {
        return;
    }

    if (ISBROADCAST(hdr_ether->h_dest))
    {
        if (!(dev->regs.rctrl & ETSEC_RCTRL_BC_REJ))
        {
            //receive broadcast frame
            if (dev->mac_valid)
                etsec_rx_ring_write(dev, dev->rx_data, size);
        } else
        {
            if (dev->regs.rctrl & ETSEC_RCTRL_PROM)
            {
                //receive broadcast frame
                if (dev->mac_valid)
                    etsec_rx_ring_write(dev, dev->rx_data, size);
            }
        }
    } else if (ISMULTICAST(hdr_ether->h_dest))
    {

    } else if (ISUNICAST(hdr_ether->h_dest))
    {
#ifdef ETSEC_DEBUG
        printf("ISUNICAST recv frame size=%d\n", size);
        printf("#########RECV Len = %d\n",size);
        {
            int i;
            for (i=0;i<size;i++)
                printf("%x ", dev->rx_data[i]);
            printf("\n");
        }
#endif
        if (memcmp(hdr_ether->h_dest, dev->macInfo, 6) == 0)
            etsec_rx_ring_write(dev, dev->rx_data, size);
    }
}

static exception_t eTSEC_8641d_read(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
    eTSEC_8641d_device *dev = opaque->obj;
    uint32_t *data = (uint32_t *)buf;
    int tx_channel, rx_channel;

    switch (offset)
    {
        case ID:
            *data = dev->regs.id;
            break;
        case ID2:
            *data = dev->regs.id2;
            break;
        case IEVENT:
            *data = dev->regs.ievent;
            break;
        case IMASK:
            *data = dev->regs.imask;
            break;
        case EDIS:
            *data = dev->regs.edis;
            break;
        case ECNTRL:
            *data = dev->regs.ecntrl;
            break;
        case PTV:
            *data = dev->regs.ptv;
            break;
        case DMACTRL:
            *data = dev->regs.dmactrl;
            break;
        case TBIPA:
            *data = dev->regs.tbipa.all;
            break;
        case TCTRL:
            *data = dev->regs.tctrl;
            break;
        case TSTAT:
            *data = dev->regs.tstat;
            break;
        case DFVLAN:
            *data = dev->regs.dfvlan;
            break;
        case TXIC:
            *data = dev->regs.txic;
            break;
        case TQUEUE:
            *data = dev->regs.tqueue;
            break;
        case TR03WT:
            *data = dev->regs.tr03wt;
            break;
        case TR47WT:
            *data = dev->regs.tr47wt;
            break;
        case TBDBPH:
            *data = dev->regs.tbdbph;
            break;
        case TBPTR0:
        case TBPTR1:
        case TBPTR2:
        case TBPTR3:
        case TBPTR4:
        case TBPTR5:
        case TBPTR6:
        case TBPTR7:
            tx_channel = (offset - TBPTR0) / 8;
            *data = dev->regs.tbptr[tx_channel];
            break;
        case TBASEH:
            *data = dev->regs.tbaseh;
            break;
        case TBASE0:
        case TBASE1:
        case TBASE2:
        case TBASE3:
        case TBASE4:
        case TBASE5:
        case TBASE6:
        case TBASE7:
            tx_channel = (offset - TBASE0) / 8;
            *data = dev->regs.tbase[tx_channel];
            break;
        case RCTRL:
            *data = dev->regs.rctrl;
            break;
        case RSTAT:
            *data = dev->regs.rstat;
            break;
        case RXIC:
            *data = dev->regs.rxic;
            break;
        case RQUEUE:
            *data = dev->regs.rqueue;
            break;
        case RBIFX:
            *data = dev->regs.rbifx;
            break;
        case RQFAR:
            *data = dev->regs.rqfar;
            break;
        case RQFCR:
            *data = dev->regs.rqfcr;
            break;
        case RQFPR:
            *data = dev->regs.rqfpr;
            break;
        case MRBLR:
            *data = dev->regs.mrblr;
            break;
        case RBDBPH:
            *data = dev->regs.rbdbph;
            break;
        case RBPTR0:
        case RBPTR1:
        case RBPTR2:
        case RBPTR3:
        case RBPTR4:
        case RBPTR5:
        case RBPTR6:
        case RBPTR7:
            rx_channel = (offset - RBPTR0) / 8;
            *data = dev->regs.rbptr[rx_channel];
            break;
        case RBASEH:
            *data = dev->regs.rbaseh;
            break;
        case RBASE0:
        case RBASE1:
        case RBASE2:
        case RBASE3:
        case RBASE4:
        case RBASE5:
        case RBASE6:
        case RBASE7:
            rx_channel = (offset - RBASE0) / 8;
            *data = dev->regs.rbase[rx_channel];
            break;
        case MACCFG1:
            *data = dev->regs.maccfg1;
            break;
        case MACCFG2:
            *data = dev->regs.maccfg2;
            break;
        case IPGIFG:
            *data = dev->regs.ipgifg;
            break;
        case HAFDUP:
            *data = dev->regs.hafdup;
            break;
        case MAXFRM:
            *data = dev->regs.maxfrm;
            break;
        case MIIMCFG:
            *data = dev->regs.miimcfg;
            break;
        case MIIMCOM:
            *data = dev->regs.miimcom;
            break;
        case MIIMADD:
            *data = dev->regs.miimadd.all;
            break;
        case MIIMCON:
            *data = dev->regs.miimcon;
            break;
        case MIIMSTAT:
            *data = dev->regs.miimstat;
            break;
        case MIIMIND:
            *data = dev->regs.miimind;
            break;
        case IFSTAT:
            *data = dev->regs.ifstat;
            break;
        case MACSTNADDR1:
            *data = dev->regs.macstnaddr1;
            break;
        case MACSTNADDR2:
            *data = dev->regs.macstnaddr2;
            break;
        case MAC01ADDR1:
            *data = dev->regs.mac01addr1;
            break;
        case MAC01ADDR2:
            *data = dev->regs.mac01addr2;
            break;
        case MAC02ADDR1:
            *data = dev->regs.mac02addr1;
            break;
        case MAC02ADDR2:
            *data = dev->regs.mac02addr2;
            break;
        case MAC03ADDR1:
            *data = dev->regs.mac03addr1;
            break;
        case MAC03ADDR2:
            *data = dev->regs.mac03addr2;
            break;
        case MAC04ADDR1:
            *data = dev->regs.mac04addr1;
            break;
        case MAC04ADDR2:
            *data = dev->regs.mac04addr2;
            break;
        case MAC05ADDR1:
            *data = dev->regs.mac05addr1;
            break;
        case MAC05ADDR2:
            *data = dev->regs.mac05addr2;
            break;
        case MAC06ADDR1:
            *data = dev->regs.mac06addr1;
            break;
        case MAC06ADDR2:
            *data = dev->regs.mac06addr2;
            break;
        case MAC07ADDR1:
            *data = dev->regs.mac07addr1;
            break;
        case MAC07ADDR2:
            *data = dev->regs.mac07addr2;
            break;
        case MAC08ADDR1:
            *data = dev->regs.mac08addr1;
            break;
        case MAC08ADDR2:
            *data = dev->regs.mac08addr2;
            break;
        case MAC09ADDR1:
            *data = dev->regs.mac09addr1;
            break;
        case MAC09ADDR2:
            *data = dev->regs.mac09addr2;
            break;
        case MAC10ADDR1:
            *data = dev->regs.mac10addr1;
            break;
        case MAC10ADDR2:
            *data = dev->regs.mac10addr2;
            break;
        case MAC11ADDR1:
            *data = dev->regs.mac11addr1;
            break;
        case MAC11ADDR2:
            *data = dev->regs.mac11addr2;
            break;
        case MAC12ADDR1:
            *data = dev->regs.mac12addr1;
            break;
        case MAC12ADDR2:
            *data = dev->regs.mac12addr2;
            break;
        case MAC13ADDR1:
            *data = dev->regs.mac13addr1;
            break;
        case MAC13ADDR2:
            *data = dev->regs.mac13addr2;
            break;
        case MAC14ADDR1:
            *data = dev->regs.mac14addr1;
            break;
        case MAC14ADDR2:
            *data = dev->regs.mac14addr2;
            break;
        case MAC15ADDR1:
            *data = dev->regs.mac15addr1;
            break;
        case MAC15ADDR2:
            *data = dev->regs.mac15addr2;
            break;
        case TR64:
            *data = dev->regs.tr64;
            break;
        case TR127:
            *data = dev->regs.tr127;
            break;
        case TR255:
            *data = dev->regs.tr255;
            break;
        case TR511:
            *data = dev->regs.tr511;
            break;
        case TR1K:
            *data = dev->regs.tr1k;
            break;
        case TRMAX:
            *data = dev->regs.trmax;
            break;
        case TRMGV:
            *data = dev->regs.trmgv;
            break;
        case RBYT:
            *data = dev->regs.rbyt;
            break;
        case RPKT:
            *data = dev->regs.rpkt;
            break;
        case RFCS:
            *data = dev->regs.rfcs;
            break;
        case RMCA:
            *data = dev->regs.rmca;
            break;
        case RBCA:
            *data = dev->regs.rbca;
            break;
        case RXCF:
            *data = dev->regs.rxcf;
            break;
        case RXPF:
            *data = dev->regs.rxpf;
            break;
        case RXUO:
            *data = dev->regs.rxuo;
            break;
        case RALN:
            *data = dev->regs.raln;
            break;
        case RFLR:
            *data = dev->regs.rflr;
            break;
        case RCDE:
            *data = dev->regs.rcde;
            break;
        case RCSE:
            *data = dev->regs.rcse;
            break;
        case RUND:
            *data = dev->regs.rund;
            break;
        case ROVR:
            *data = dev->regs.rovr;
            break;
        case RFRG:
            *data = dev->regs.rfrg;
            break;
        case RJBR:
            *data = dev->regs.rjbr;
            break;
        case RDRP:
            *data = dev->regs.rdrp;
            break;
        case TBYT:
            *data = dev->regs.tbyt;
            break;
        case TPKT:
            *data = dev->regs.tpkt;
            break;
        case TMCA:
            *data = dev->regs.tmca;
            break;
        case TBCA:
            *data = dev->regs.tbca;
            break;
        case TXPF:
            *data = dev->regs.txpf;
            break;
        case TDFR:
            *data = dev->regs.tdfr;
            break;
        case TEDF:
            *data = dev->regs.tedf;
            break;
        case TSCL:
            *data = dev->regs.tscl;
            break;
        case TMCL:
            *data = dev->regs.tmcl;
            break;
        case TLCL:
            *data = dev->regs.tlcl;
            break;
        case TXCL:
            *data = dev->regs.txcl;
            break;
        case TNCL:
            *data = dev->regs.tncl;
            break;
        case TDRP:
            *data = dev->regs.tdrp;
            break;
        case TJBR:
            *data = dev->regs.tjbr;
            break;
        case TFCS:
            *data = dev->regs.tfcs;
            break;
        case TXCF:
            *data = dev->regs.txcf;
            break;
        case TOVR:
            *data = dev->regs.tovr;
            break;
        case TUND:
            *data = dev->regs.tund;
            break;
        case TFRG:
            *data = dev->regs.tfrg;
            break;
        case CAR1:
            *data = dev->regs.car1;
            break;
        case CAR2:
            *data = dev->regs.car2;
            break;
        case CAM1:
            *data = dev->regs.cam1;
            break;
        case CAM2:
            *data = dev->regs.cam2;
            break;
        case RREJ:
            *data = dev->regs.rrej;
            break;
        case IGADDR0:
            *data = dev->regs.igaddr0;
            break;
        case IGADDR1:
            *data = dev->regs.igaddr1;
            break;
        case IGADDR2:
            *data = dev->regs.igaddr2;
            break;
        case IGADDR3:
            *data = dev->regs.igaddr3;
            break;
        case IGADDR4:
            *data = dev->regs.igaddr4;
            break;
        case IGADDR5:
            *data = dev->regs.igaddr5;
            break;
        case IGADDR6:
            *data = dev->regs.igaddr6;
            break;
        case IGADDR7:
            *data = dev->regs.igaddr7;
            break;
        case GADDR0:
            *data = dev->regs.gaddr0;
            break;
        case GADDR1:
            *data = dev->regs.gaddr1;
            break;
        case GADDR2:
            *data = dev->regs.gaddr2;
            break;
        case GADDR3:
            *data = dev->regs.gaddr3;
            break;
        case GADDR4:
            *data = dev->regs.gaddr4;
            break;
        case GADDR5:
            *data = dev->regs.gaddr5;
            break;
        case GADDR6:
            *data = dev->regs.gaddr6;
            break;
        case GADDR7:
            *data = dev->regs.gaddr7;
            break;
        case FIFOCFG:
            *data = dev->regs.fifocfg;
            break;
        case ATTR:
            *data = dev->regs.attr;
            break;
        case RQPRM0:
            *data = dev->regs.rqprm0;
            break;
        case RQPRM1:
            *data = dev->regs.rqprm1;
            break;
        case RQPRM2:
            *data = dev->regs.rqprm2;
            break;
        case RQPRM3:
            *data = dev->regs.rqprm3;
            break;
        case RQPRM4:
            *data = dev->regs.rqprm4;
            break;
        case RQPRM5:
            *data = dev->regs.rqprm5;
            break;
        case RQPRM6:
            *data = dev->regs.rqprm6;
            break;
        case RQPRM7:
            *data = dev->regs.rqprm7;
            break;
        case RFBPTR0:
            *data = dev->regs.rfbptr0;
            break;
        case RFBPTR1:
            *data = dev->regs.rfbptr1;
            break;
        case RFBPTR2:
            *data = dev->regs.rfbptr2;
            break;
        case RFBPTR3:
            *data = dev->regs.rfbptr3;
            break;
        case RFBPTR4:
            *data = dev->regs.rfbptr4;
            break;
        case RFBPTR5:
            *data = dev->regs.rfbptr5;
            break;
        case RFBPTR6:
            *data = dev->regs.rfbptr6;
            break;
        case RFBPTR7:
            *data = dev->regs.rfbptr7;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Can not read the register at 0x%x in eTSEC_8641d\n", offset);
            break;
    }
#ifdef ETSEC_DEBUG
    if (!strcmp(opaque->objname, "eTSEC_0"))
        printf("ETSEC %s------------------R---------offset: 0x%x, data=%x\n", opaque->objname, offset, *data);
#endif
    return 0;
}

static exception_t eTSEC_8641d_write(conf_object_t *opaque, generic_address_t offset, void *buf, size_t count)
{
    eTSEC_8641d_device *dev = opaque->obj;
    uint32_t val = *(uint32_t *)buf;
    int phyAddr;
    int tx_channel, rx_channel;

    uint32_t control;
#ifdef ETSEC_DEBUG
    if (!strcmp(opaque->objname, "eTSEC_0"))
        printf("ETSEC %s ------------------W---------offset: 0x%x, val=%x\n", opaque->objname, offset, val);
#endif
    switch (offset)
    {
        case ID:
            dev->regs.id = val;
            break;
        case ID2:
            dev->regs.id2 = val;
            break;
        case IEVENT:
            /* Write 1 to clear */
            dev->regs.ievent &= ~val;
            break;
        case IMASK:
            dev->regs.imask = val;
            break;
        case EDIS:
            dev->regs.edis = val;
            break;
        case ECNTRL:
            dev->regs.ecntrl = val;
            break;
        case PTV:
            dev->regs.ptv = val;
            break;
        case DMACTRL:
            dev->regs.dmactrl = val;
            write_dmactrl(dev, val);
            break;
        case TBIPA:
            dev->regs.tbipa.all = val;
            break;
        case TCTRL:
            dev->regs.tctrl = val;
            break;
        case TSTAT:
            write_tstat(dev, val);
            break;
        case DFVLAN:
            dev->regs.dfvlan = val;
            break;
        case TXIC:
            dev->regs.txic = val;
            break;
        case TQUEUE:
            dev->regs.tqueue = val;
            break;
        case TR03WT:
            dev->regs.tr03wt = val;
            break;
        case TR47WT:
            dev->regs.tr47wt = val;
            break;
        case TBDBPH:
            dev->regs.tbdbph = val;
            break;
        case TBPTR0 ... TBPTR7:
            tx_channel = (offset - TBPTR0) / 8;
            dev->regs.tbptr[tx_channel] = val;
            break;
        case TBASEH:
            dev->regs.tbaseh = val;
            break;
        case TBASE0 ... TBASE7:
            tx_channel = (offset - TBASE0) / 8;
            dev->regs.tbase[tx_channel] = val & ~0x7;
            dev->regs.tbptr[tx_channel] = val & ~0x7;
            break;
        case RCTRL:
            dev->regs.rctrl = val;
            break;
        case RSTAT:
            write_rstat(dev, val);
            break;
        case RXIC:
            dev->regs.rxic = val;
            break;
        case RQUEUE:
            dev->regs.rqueue = val;
            break;
        case RBIFX:
            dev->regs.rbifx = val;
            break;
        case RQFAR:
            dev->regs.rqfar = val;
            break;
        case RQFCR:
            dev->regs.rqfcr = val;
            break;
        case RQFPR:
            dev->regs.rqfpr = val;
            break;
        case MRBLR:
            dev->regs.mrblr = val;
            break;
        case RBDBPH:
            dev->regs.rbdbph = val;
            break;
        case RBPTR0 ... RBPTR7:
            rx_channel = (offset - RBPTR0) / 8;
            dev->regs.rbptr[rx_channel] = val;
            break;
        case RBASEH:
            dev->regs.rbaseh = val;
            break;
        case RBASE0 ... RBASE7:
            rx_channel = (offset - RBASE0) / 8;
            dev->regs.rbase[rx_channel] = val & ~0x7;
            dev->regs.rbptr[rx_channel] = val & ~0x7;
            break;
        case MACCFG1:
            dev->regs.maccfg1 = val;
            if (dev->regs.maccfg1 & MACCFG1_TX_EN)
                dev->regs.maccfg1 |= MACCFG1_SYNC_TX_EN;
            else 
                dev->regs.maccfg1 &= ~MACCFG1_SYNC_TX_EN;

            if (dev->regs.maccfg1 & MACCFG1_RX_EN)
                dev->regs.maccfg1 |= MACCFG1_SYNC_RX_EN;
            else
                dev->regs.maccfg1 &= ~MACCFG1_SYNC_RX_EN;
            break;
        case MACCFG2:
            dev->regs.maccfg2 = val;
            break;
        case IPGIFG:
            dev->regs.ipgifg = val;
            break;
        case HAFDUP:
            dev->regs.hafdup = val;
            break;
        case MAXFRM:
            dev->regs.maxfrm = val;
            break;
        case MIIMCFG:
            dev->regs.miimcfg = val;
            break;
        case MIIMCOM:
            if ((!(dev->regs.miimcom & ETSEC_MIIMCOM_READ)) && (val & ETSEC_MIIMCOM_READ))
            {
                /* Read */
                miim_read_cycle(dev);
            }

            dev->regs.miimcom = val;
            break;
        case MIIMADD:
            dev->regs.miimadd.all = val;
            break;
        case MIIMCON:
            dev->regs.miimcon = val & 0xffff;
            miim_write_cycle(dev);
            break;
        case MIIMSTAT:
            dev->regs.miimstat = val;
            break;
        case MIIMIND:
            dev->regs.miimind = val;
            break;
        case IFSTAT:
            dev->regs.ifstat = val;
            break;
        case MACSTNADDR1:
            dev->regs.macstnaddr1 = val;
            dev->macInfo[2] = val & 0xff; 
            dev->macInfo[3] = (val >> 8) & 0xff; 
            dev->macInfo[4] = (val >> 16) & 0xff; 
            dev->macInfo[5] = (val >> 24) & 0xff; 
            dev->mac_valid = 1;
            break;
        case MACSTNADDR2:
            dev->regs.macstnaddr2 = val;
            dev->macInfo[0] = (val >> 16) & 0xff; 
            dev->macInfo[1] = (val >> 24) & 0xff; 
            break;
        case MAC01ADDR1:
            dev->regs.mac01addr1 = val;
            break;
        case MAC01ADDR2:
            dev->regs.mac01addr2 = val;
            break;
        case MAC02ADDR1:
            dev->regs.mac02addr1 = val;
            break;
        case MAC02ADDR2:
            dev->regs.mac02addr2 = val;
            break;
        case MAC03ADDR1:
            dev->regs.mac03addr1 = val;
            break;
        case MAC03ADDR2:
            dev->regs.mac03addr2 = val;
            break;
        case MAC04ADDR1:
            dev->regs.mac04addr1 = val;
            break;
        case MAC04ADDR2:
            dev->regs.mac04addr2 = val;
            break;
        case MAC05ADDR1:
            dev->regs.mac05addr1 = val;
            break;
        case MAC05ADDR2:
            dev->regs.mac05addr2 = val;
            break;
        case MAC06ADDR1:
            dev->regs.mac06addr1 = val;
            break;
        case MAC06ADDR2:
            dev->regs.mac06addr2 = val;
            break;
        case MAC07ADDR1:
            dev->regs.mac07addr1 = val;
            break;
        case MAC07ADDR2:
            dev->regs.mac07addr2 = val;
            break;
        case MAC08ADDR1:
            dev->regs.mac08addr1 = val;
            break;
        case MAC08ADDR2:
            dev->regs.mac08addr2 = val;
            break;
        case MAC09ADDR1:
            dev->regs.mac09addr1 = val;
            break;
        case MAC09ADDR2:
            dev->regs.mac09addr2 = val;
            break;
        case MAC10ADDR1:
            dev->regs.mac10addr1 = val;
            break;
        case MAC10ADDR2:
            dev->regs.mac10addr2 = val;
            break;
        case MAC11ADDR1:
            dev->regs.mac11addr1 = val;
            break;
        case MAC11ADDR2:
            dev->regs.mac11addr2 = val;
            break;
        case MAC12ADDR1:
            dev->regs.mac12addr1 = val;
            break;
        case MAC12ADDR2:
            dev->regs.mac12addr2 = val;
            break;
        case MAC13ADDR1:
            dev->regs.mac13addr1 = val;
            break;
        case MAC13ADDR2:
            dev->regs.mac13addr2 = val;
            break;
        case MAC14ADDR1:
            dev->regs.mac14addr1 = val;
            break;
        case MAC14ADDR2:
            dev->regs.mac14addr2 = val;
            break;
        case MAC15ADDR1:
            dev->regs.mac15addr1 = val;
            break;
        case MAC15ADDR2:
            dev->regs.mac15addr2 = val;
            break;
        case TR64:
            dev->regs.tr64 = val;
            break;
        case TR127:
            dev->regs.tr127 = val;
            break;
        case TR255:
            dev->regs.tr255 = val;
            break;
        case TR511:
            dev->regs.tr511 = val;
            break;
        case TR1K:
            dev->regs.tr1k = val;
            break;
        case TRMAX:
            dev->regs.trmax = val;
            break;
        case TRMGV:
            dev->regs.trmgv = val;
            break;
        case RBYT:
            dev->regs.rbyt = val;
            break;
        case RPKT:
            dev->regs.rpkt = val;
            break;
        case RFCS:
            dev->regs.rfcs = val;
            break;
        case RMCA:
            dev->regs.rmca = val;
            break;
        case RBCA:
            dev->regs.rbca = val;
            break;
        case RXCF:
            dev->regs.rxcf = val;
            break;
        case RXPF:
            dev->regs.rxpf = val;
            break;
        case RXUO:
            dev->regs.rxuo = val;
            break;
        case RALN:
            dev->regs.raln = val;
            break;
        case RFLR:
            dev->regs.rflr = val;
            break;
        case RCDE:
            dev->regs.rcde = val;
            break;
        case RCSE:
            dev->regs.rcse = val;
            break;
        case RUND:
            dev->regs.rund = val;
            break;
        case ROVR:
            dev->regs.rovr = val;
            break;
        case RFRG:
            dev->regs.rfrg = val;
            break;
        case RJBR:
            dev->regs.rjbr = val;
            break;
        case RDRP:
            dev->regs.rdrp = val;
            break;
        case TBYT:
            dev->regs.tbyt = val;
            break;
        case TPKT:
            dev->regs.tpkt = val;
            break;
        case TMCA:
            dev->regs.tmca = val;
            break;
        case TBCA:
            dev->regs.tbca = val;
            break;
        case TXPF:
            dev->regs.txpf = val;
            break;
        case TDFR:
            dev->regs.tdfr = val;
            break;
        case TEDF:
            dev->regs.tedf = val;
            break;
        case TSCL:
            dev->regs.tscl = val;
            break;
        case TMCL:
            dev->regs.tmcl = val;
            break;
        case TLCL:
            dev->regs.tlcl = val;
            break;
        case TXCL:
            dev->regs.txcl = val;
            break;
        case TNCL:
            dev->regs.tncl = val;
            break;
        case TDRP:
            dev->regs.tdrp = val;
            break;
        case TJBR:
            dev->regs.tjbr = val;
            break;
        case TFCS:
            dev->regs.tfcs = val;
            break;
        case TXCF:
            dev->regs.txcf = val;
            break;
        case TOVR:
            dev->regs.tovr = val;
            break;
        case TUND:
            dev->regs.tund = val;
            break;
        case TFRG:
            dev->regs.tfrg = val;
            break;
        case CAR1:
            dev->regs.car1 = val;
            break;
        case CAR2:
            dev->regs.car2 = val;
            break;
        case CAM1:
            dev->regs.cam1 = val;
            break;
        case CAM2:
            dev->regs.cam2 = val;
            break;
        case RREJ:
            dev->regs.rrej = val;
            break;
        case IGADDR0:
            dev->regs.igaddr0 = val;
            break;
        case IGADDR1:
            dev->regs.igaddr1 = val;
            break;
        case IGADDR2:
            dev->regs.igaddr2 = val;
            break;
        case IGADDR3:
            dev->regs.igaddr3 = val;
            break;
        case IGADDR4:
            dev->regs.igaddr4 = val;
            break;
        case IGADDR5:
            dev->regs.igaddr5 = val;
            break;
        case IGADDR6:
            dev->regs.igaddr6 = val;
            break;
        case IGADDR7:
            dev->regs.igaddr7 = val;
            break;
        case GADDR0:
            dev->regs.gaddr0 = val;
            break;
        case GADDR1:
            dev->regs.gaddr1 = val;
            break;
        case GADDR2:
            dev->regs.gaddr2 = val;
            break;
        case GADDR3:
            dev->regs.gaddr3 = val;
            break;
        case GADDR4:
            dev->regs.gaddr4 = val;
            break;
        case GADDR5:
            dev->regs.gaddr5 = val;
            break;
        case GADDR6:
            dev->regs.gaddr6 = val;
            break;
        case GADDR7:
            dev->regs.gaddr7 = val;
            break;
        case FIFOCFG:
            dev->regs.fifocfg = val;
            break;
        case ATTR:
            dev->regs.attr = val;
            break;
        case RQPRM0:
            dev->regs.rqprm0 = val;
            break;
        case RQPRM1:
            dev->regs.rqprm1 = val;
            break;
        case RQPRM2:
            dev->regs.rqprm2 = val;
            break;
        case RQPRM3:
            dev->regs.rqprm3 = val;
            break;
        case RQPRM4:
            dev->regs.rqprm4 = val;
            break;
        case RQPRM5:
            dev->regs.rqprm5 = val;
            break;
        case RQPRM6:
            dev->regs.rqprm6 = val;
            break;
        case RQPRM7:
            dev->regs.rqprm7 = val;
            break;
        case RFBPTR0:
            dev->regs.rfbptr0 = val;
            break;
        case RFBPTR1:
            dev->regs.rfbptr1 = val;
            break;
        case RFBPTR2:
            dev->regs.rfbptr2 = val;
            break;
        case RFBPTR3:
            dev->regs.rfbptr3 = val;
            break;
        case RFBPTR4:
            dev->regs.rfbptr4 = val;
            break;
        case RFBPTR5:
            dev->regs.rfbptr5 = val;
            break;
        case RFBPTR6:
            dev->regs.rfbptr6 = val;
            break;
        case RFBPTR7:
            dev->regs.rfbptr7 = val;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "Cannot write the register at 0x%x in eTSEC_8641d\n", offset);
            break;
    }
    return 0;
}

static char* eTSEC_8641d_get_regname_by_id(conf_object_t *conf_obj, uint32_t id)
{
    return regs_name[id];
}

static exception_t eTSEC_8641d_set_regvalue_by_id(conf_object_t *conf_obj, uint32_t value, uint32_t id)
{
    eTSEC_8641d_device *dev = conf_obj->obj;
    struct registers *regs = &(dev->regs);
    *((uint32_t*)regs + id) = value;
    return No_exp;
}

static uint32_t eTSEC_8641d_get_regnum(conf_object_t *conf_obj, uint32_t id)
{
    return  sizeof(struct registers) / 4;
}

static uint32_t eTSEC_8641d_get_regvalue_by_id(conf_object_t *conf_obj, uint32_t id)
{
    eTSEC_8641d_device *dev = conf_obj->obj;
    struct registers *regs = &(dev->regs);
    uint32_t* regs_value = (uint32_t*)regs + id;
    return *regs_value;
}

static uint32_t eTSEC_8641d_get_regid_by_name(conf_object_t *conf_obj, char *name)
{
    uint32_t regnum = sizeof(struct registers) / 4, i;
    for(i = 0; i < regnum; i++)
    {
        if(strcmp(name, regs_name[i]) == 0)
            return i;
    }
    return regnum;
}

static void init_regs(conf_object_t *conf_obj)
{
    eTSEC_8641d_device *dev = conf_obj->obj;
    struct registers *regs = &dev->regs;
    regs->id	= 0x1240000;
    regs->id2	= 0x3000f0;
    regs->dfvlan 	= 0x81000000;
    regs->tqueue 	= 0x8000;
    regs->rqueue 	= 0x800080;
    regs->maccfg2 	= 0x7000;
    regs->ipgifg 	= 0x40605060;
    regs->hafdup 	= 0xa1f037;
    regs->maxfrm 	= 0x600;
    regs->miimcfg 	= 0x7;
    regs->cam1 	= 0xfe03ffff;
    regs->cam2 	= 0xffffd;
    regs->fifocfg 	= 0xc0;
    dev->phy_status = MII_SR_EXTENDED_CAPS | MII_SR_LINK_STATUS | MII_SR_AUTONEG_CAPS |
        MII_SR_AUTONEG_COMPLETE | MII_SR_PREAMBLE_SUPPRESS |
        MII_SR_EXTENDED_STATUS  | MII_SR_100T2_HD_CAPS | MII_SR_100T2_FD_CAPS |
        MII_SR_10T_HD_CAPS      | MII_SR_10T_FD_CAPS   | MII_SR_100X_HD_CAPS  |
        MII_SR_100X_FD_CAPS     | MII_SR_100T4_CAPS;
    return ;
}

static exception_t general_net_space_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    eTSEC_8641d_device *dev = obj->obj;

    dev->net = obj2;
    dev->net_iface = (skyeye_net_ops_intf *)SKY_get_iface(dev->net, SKYEYE_NET_INTF_NAME);
    if (dev->net_iface == NULL){
        if (dev->net == NULL)
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object.", SKYEYE_NET_INTF_NAME, obj2);
        else
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", SKYEYE_NET_INTF_NAME, obj2->objname);

        return Not_found_exp;
    }

    dev->net_sim_isok = !dev->net_iface->net_open(NET_DEVICE(dev));
    if (dev->net_sim_isok)
        dev->phy_status |= MII_SR_LINK_STATUS;
    else
        dev->phy_status &= ~MII_SR_LINK_STATUS;

    return No_exp;
}

static exception_t general_net_space_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    eTSEC_8641d_device *dev = obj->obj;

    *obj2 = dev->net;
    *port = NULL;
    return No_exp;
}

static exception_t general_memory_space_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    eTSEC_8641d_device *dev = obj->obj;

    dev->memory = obj2;
    dev->memory_iface = (memory_space_intf *)SKY_get_iface(dev->memory, MEMORY_SPACE_INTF_NAME);
    if (dev->memory_iface == NULL)
    {
        if (dev->memory == NULL)
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from NULL object fail.", MEMORY_SPACE_INTF_NAME, obj2);
        else
            skyeye_log(Error_log, __FUNCTION__, "Can not get %s interface from %s fail.", MEMORY_SPACE_INTF_NAME, obj2->objname);

        return Not_found_exp;
    }
    return No_exp;
}

static exception_t general_memory_space_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    eTSEC_8641d_device *dev = obj->obj;

    *obj2 = dev->memory;
    *port = NULL;
    return No_exp;
}

static conf_object_t* new_eTSEC_8641d(char *obj_name)
{
    eTSEC_8641d_device* dev = skyeye_mm_zero(sizeof(eTSEC_8641d_device));
    dev->obj = new_conf_object(obj_name, dev);
    struct registers *regs = &dev->regs;

    dev->tx_data = skyeye_mm_zero(1514);
    dev->rx_data = skyeye_mm_zero(1514);
    init_regs(dev->obj);
    int sched_receive_id = -1;
    system_register_timer_handler(dev->obj, 100, (time_sched_t)etsec_receive, SCHED_US|SCHED_NORMAL);
    system_register_timer_handler(dev->obj, 100, (time_sched_t)check_etsec_interrupt, SCHED_US|SCHED_NORMAL);
    return dev->obj;
}

static exception_t free_eTSEC_8641d(conf_object_t* opaque)
{
    eTSEC_8641d_device *dev =  opaque->obj;
    skyeye_free(opaque->objname);
    skyeye_free(dev->tx_data);
    skyeye_free(dev->rx_data);
    skyeye_free(dev);
    skyeye_free(opaque);
    return No_exp;
}

static attr_value_t get_attr_tx_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    eTSEC_8641d_device *dev =  conf_obj->obj;
    attr_value_t tx_int_number = SKY_make_attr_uinteger(dev->signal.tx_num);
    return tx_int_number;
}

static exception_t set_attr_tx_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    eTSEC_8641d_device *dev =  conf_obj->obj;
    dev->signal.tx_num = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_rx_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx){
    eTSEC_8641d_device *dev =  conf_obj->obj;
    attr_value_t rx_int_number = SKY_make_attr_uinteger(dev->signal.rx_num);
    return rx_int_number;
}

static exception_t set_attr_rx_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    eTSEC_8641d_device *dev =  conf_obj->obj;
    dev->signal.rx_num = SKY_attr_uinteger(*value);
    return No_exp;
}

static attr_value_t get_attr_err_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* idx)
{
    eTSEC_8641d_device *dev =  conf_obj->obj;
    attr_value_t err_int_number = SKY_make_attr_uinteger(dev->signal.err_num);
    return err_int_number;
}

static exception_t set_attr_err_int_number(void* arg, conf_object_t* conf_obj, attr_value_t* value, attr_value_t* idx)
{
    eTSEC_8641d_device *dev =  conf_obj->obj;
    dev->signal.err_num = SKY_attr_uinteger(*value);
    return No_exp;
}

static exception_t signal_set(conf_object_t *obj, conf_object_t *obj2, const char *port, int index)
{
    eTSEC_8641d_device *dev =  obj->obj;
    dev->signal.obj = obj2;
    dev->signal.intf = (general_signal_intf *)SKY_get_iface(dev->signal.obj, GENERAL_SIGNAL_INTF_NAME);
    if(dev->signal.intf == NULL)
    {
        if (dev->signal.obj == NULL)
            skyeye_log(Error_log, __FUNCTION__, "Get 'general_signal_intf' interface from NULL object fail.", obj2);
        else
            skyeye_log(Error_log, __FUNCTION__, "Get 'general_signal_intf' interface from %s fail.", obj2->objname);
        return Not_found_exp;
    }

    return No_exp;
}

static exception_t signal_get(conf_object_t *obj, conf_object_t **obj2, const char **port, int index)
{
    eTSEC_8641d_device *dev =  obj->obj;
    *obj2 = dev->signal.obj;
    *port = NULL;
    return No_exp;
}

static void class_register_attribute(conf_class_t* clss) {
    SKY_register_attribute(clss, "tx_int_number", get_attr_tx_int_number, NULL, set_attr_tx_int_number, NULL, SKY_Attr_Optional, "uinteger", "tx interrupt number of ETSEC");
    SKY_register_attribute(clss, "rx_int_number", get_attr_rx_int_number, NULL, set_attr_rx_int_number, NULL, SKY_Attr_Optional, "uinteger", "rx interrupt number of ETSEC");
    SKY_register_attribute(clss, "err_int_number", get_attr_err_int_number, NULL, set_attr_err_int_number, NULL, SKY_Attr_Optional, "uinteger", "err interrupt number of ETSEC");
    return;
}
void init_eTSEC_8641d(void)
{
    static skyeye_class_t class_data =
    {
        .class_name = "eTSEC_8641d",
        .class_desc = "eTSEC_8641d",
        .new_instance = new_eTSEC_8641d,
        .free_instance = free_eTSEC_8641d
    };
    conf_class_t* clss = SKY_register_device_class(class_data.class_name, &class_data);
    static memory_space_intf io_memory =
    {
        .read = eTSEC_8641d_read,
        .write = eTSEC_8641d_write
    };
    SKY_register_iface(clss, MEMORY_SPACE_INTF_NAME, &io_memory);
    static skyeye_reg_intf reg_intf =
    {
        .get_regvalue_by_id = eTSEC_8641d_get_regvalue_by_id,
        .get_regname_by_id = eTSEC_8641d_get_regname_by_id,
        .set_regvalue_by_id = eTSEC_8641d_set_regvalue_by_id,
        .get_regnum = eTSEC_8641d_get_regnum,
        .get_regid_by_name = eTSEC_8641d_get_regid_by_name
    };
    SKY_register_iface(clss, SKYEYE_REG_INTF, &reg_intf);
    static const struct InterfaceDescription ifaces[] = 
    {
        (struct InterfaceDescription)
        {
            .name = MEMORY_SPACE_INTF_NAME,
            .iface = &io_memory
        },
        (struct InterfaceDescription)
        {
            .name = SKYEYE_REG_INTF,
            .iface = &reg_intf
        },
    };
    static const struct ConnectDescription connects[] = 
    {
        (struct ConnectDescription) {
            .name = SKYEYE_NET_INTF_NAME,
            .set = general_net_space_set,
            .get = general_net_space_get,
        },
        (struct ConnectDescription) {
            .name = MEMORY_SPACE_INTF_NAME,
            .set = general_memory_space_set,
            .get = general_memory_space_get,
        },
        (struct ConnectDescription ){
            .name = GENERAL_SIGNAL_INTF_NAME,
            .set = signal_set,
            .get = signal_get,
        }
        /* ConnectDescription */
    };
    class_register_ifaces(clss, ifaces);
    class_register_connects(clss, connects);
    class_register_attribute(clss);
}
