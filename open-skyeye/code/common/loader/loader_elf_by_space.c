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
 * * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

/**
* @file loader_elf_by_obj.c
* @brief Load the elf by an object
* @author Michael.Kang blackfin.kang@gmail.com
* @version 7849
* @date 2013-09-05
*/
#include <config.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "portable/mman.h"
#include "bank_defs.h"
#include "skyeye_pref.h"
#include "skyeye_config.h"
#include "skyeye_ram.h"
#include "coff.h"
#include "hex.h"
#include "elf.h"
#include <skyeye_interface.h>
#include <skyeye_core_intf.h>
#include <skyeye_iface.h>
#include <bfd.h>
#include <skyeye_loader.h>
#include <skyeye_mm.h>
#include <skyeye_swapendian.h>
// magic numbers for big-endian headers
#if 0
const uint16_t BEH_COFF_V1_MAGIC = 0xc100;
const uint16_t BEH_COFF_V2_MAGIC = 0xc200;
const uint16_t BEH_AOUT_MAGIC = 0x0801;

// TI's IDs for big-endian headers
const uint16_t BEH_TARGET_ID_TMS320_C1X_C2X_C5X = 0x9200;
const uint16_t BEH_TARGET_ID_TMS320_C3X_C4X = 0x9300;
const uint16_t BEH_TARGET_ID_C80 = 0x9500;
const uint16_t BEH_TARGET_ID_TMS320_C54X = 0x9800;
const uint16_t BEH_TARGET_ID_TMS320_C6X = 0x9900;
const uint16_t BEH_TARGET_ID_TMS320_C28X = 0x9d00;
#endif

/*
 * ---------------------------------------------------------
 HEX FILE FORMAT
 |0 bit|   ':'First Head  
 |1-2bit|  Data_Len 
 |3-6bit|  OFFSET_ADDR_ADDR 
 |7-8bit|  Data Type 
 '00' :Data Record  
 '01' :End of File Record
 '02' :End of File Record
 '03' :Start Segment Address Record
 '04' :Extended Linear Address Record
 '05' :Start Linear Address Record
 |9-12bit| if (Data Type == '04')  is BASE_ADDR else instruction Data  
 |last bit| every line CheckSum 
 * ---------------------------------------------------------
 */

#define BUF_SIZE        64
#define DATA_LEN       2
#define BASE_ADDR_LEN   8
#define OFFSET_ADDR_LEN 4
#define DATA_TYPE_LEN    2
#define TEMP_LEN        2
static inline exception_t
sky_write_phys_by_space(conf_object_t * obj, memory_space_intf * memory_space, uint32 addr, uint8_t * buffer, int size)
{

    if (!SKY_has_attribute(obj, "load_mask"))
    {
        attr_value_t attr = SKY_get_attribute(obj, "load_mask");
        uint32_t load_mask = 0;

        load_mask = SKY_attr_uinteger(attr);
        if (load_mask != 0)
            addr = (addr & load_mask);
    }
    if (!SKY_has_attribute(obj, "load_addr"))
    {
        attr_value_t attr = SKY_get_attribute(obj, "load_addr");
        uint32_t load_addr = 0;

        load_addr = SKY_attr_uinteger(attr);
        if (load_addr != 0)
            addr = addr | load_addr;
    }
    int i, fault;

    memory_space_intf *iface = (memory_space_intf *) SKY_get_iface(obj, MEMORY_SPACE_INTF_NAME);

    if (iface == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Get memory_space interface from %s error!!!\n", obj->objname);
        return Not_found_exp;
    }
    for (i = 0; i < size; i++)
    {
        if (iface->write(obj, addr + i, &buffer[i], 1) != No_exp)
        {
            skyeye_log(Error_log, __FUNCTION__, "SKYEYE: write physical address 0x%x error!!!\n", addr + i);
            return Access_exp;
        }
    }

    return No_exp;
}
static inline exception_t
c28x_sky_write_phys_by_space(conf_object_t * obj, memory_space_intf * memory_space, uint32 addr, uint16_t * buffer,
                             int size)
{

    if (!SKY_has_attribute(obj, "load_mask"))
    {
        attr_value_t attr = SKY_get_attribute(obj, "load_mask");
        uint32_t load_mask = 0;

        load_mask = SKY_attr_uinteger(attr);
        if (load_mask != 0)
            addr = (addr & load_mask);
    }
    if (!SKY_has_attribute(obj, "load_addr"))
    {
        attr_value_t attr = SKY_get_attribute(obj, "load_addr");
        uint32_t load_addr = 0;

        load_addr = SKY_attr_uinteger(attr);
        if (load_addr != 0)
            addr = addr | load_addr;
    }
    int i, fault;

    //emory_space_intf* mem_iface = memory_space;
    memory_space_intf *iface = (memory_space_intf *) SKY_get_iface(obj, MEMORY_SPACE_INTF_NAME);

    if (iface == NULL)
    {
        SKYEYE_ERR("can not load the data in %s", __FUNCTION__);
        return Not_found_exp;
    }
    for (i = 0; i < size; i++)
    {
        if (iface->write(obj, addr + i, &buffer[i], 2) != No_exp)
        {
            skyeye_log(Error_log, __FUNCTION__, "SKYEYE: write physical address 0x%x error!!!\n", addr + i);
            return Access_exp;
        }
    }
    return No_exp;
}
static inline exception_t
c55x_sky_write_phys_by_space(conf_object_t * obj, memory_space_intf * memory_space, uint32 addr, uint16_t * buffer,
                             int size)
{

    if (!SKY_has_attribute(obj, "load_mask"))
    {
        attr_value_t attr = SKY_get_attribute(obj, "load_mask");
        uint32_t load_mask = 0;

        load_mask = SKY_attr_uinteger(attr);
        if (load_mask != 0)
            addr = (addr & load_mask);
    }
    if (!SKY_has_attribute(obj, "load_addr"))
    {
        attr_value_t attr = SKY_get_attribute(obj, "load_addr");
        uint32_t load_addr = 0;

        load_addr = SKY_attr_uinteger(attr);
        if (load_addr != 0)
            addr = addr | load_addr;
    }
    int i, fault;

    //emory_space_intf* mem_iface = memory_space;
    memory_space_intf *iface = (memory_space_intf *) SKY_get_iface(obj, MEMORY_SPACE_INTF_NAME);

    if (iface == NULL)
    {
        SKYEYE_ERR("can not load the data in %s", __FUNCTION__);
        return Not_found_exp;
    }
    for (i = 0; i < size; i++)
    {
        uint16_t data = half_from_BE(buffer[i]);

        if (iface->write(obj, addr + i, &data, 2) != No_exp)
        {
            skyeye_log(Error_log, __FUNCTION__, "SKYEYE: write physical address 0x%x error!!!\n", addr + i);
            return Access_exp;
        }
    }
    return No_exp;
}

static inline void write_phys_by_space(memory_space_intf * memory_space, uint32 addr, uint8_t * buffer, int size)
{
    int i, fault;

    memory_space_intf *io_memory = memory_space;

    if (io_memory == NULL)
    {
        SKYEYE_ERR("can not load the data in %s", __FUNCTION__);
        return;
    }
    for (i = 0; i < size; i++)
    {

        if (io_memory->write(memory_space->conf_obj, addr + i, &buffer[i], 1) != 0)
        {
            skyeye_log(Error_log, __FUNCTION__, "SKYEYE: write physical address 0x%x error!!!\n", addr + i);
            return;
        }
    }
}

exception_t sky_load_coff_by_space(conf_object_t * obj, memory_space_intf * memory_space, const char *filename)
{
    struct coff_header *coffFile;
    struct stat coff_fstat;
    int tmp_fd = open(filename, O_RDONLY);
    exception_t exp = No_exp;

    if (tmp_fd == -1)
    {
        fprintf(stderr, "open %s error: %s\n", filename, strerror(errno));
        goto out;
    }
    fstat(tmp_fd, &coff_fstat);
    /* malloc */
    coffFile = mmap(NULL, coff_fstat.st_size, PROT_READ, MAP_PRIVATE, tmp_fd, 0);
    if (coffFile == NULL || coffFile == MAP_FAILED)
    {
        fprintf(stderr, "mmap error: %s\n", strerror(errno));
        goto out;
    }
    filehdr_v12 *filehdr = ((char *) coffFile + 0x0);
    aouthdr *aout_header = ((char *) coffFile + sizeof (filehdr_v12));
    scnhdr_v2 *section_header;
    int section_num = filehdr->f_nscns;

    int i;

    for (i = 0; i < section_num; i++)
    {
        section_header =
            (scnhdr_v2 *) ((char *) coffFile + sizeof (filehdr_v12) + sizeof (aouthdr) + i * sizeof (scnhdr_v2));

        // s_vaddr  : 虚拟地址
        // s_paddr  : 物理地址
        // s_scnptr : 文件偏移地址
        // s_size   : 段的大小
        // check_s_flags: 用来检查当前段的属性是否要加载到内存
        if (section_header->s_size != 0 && section_header->s_scnptr != 0 && check_s_flags(section_header))
        {
            uint8_t *start = (uint8_t *) ((char *) coffFile + section_header->s_scnptr);
            int j = 0;

            /* target_id = 0x9d : C28x  C54x : 0x98 */
            if (filehdr->f_target_id == 0x9d || filehdr->f_target_id == 0x98)
            {
                exp =
                    c28x_sky_write_phys_by_space(obj, memory_space, section_header->s_vaddr, start,
                                                 section_header->s_size);
            } else if (filehdr->f_target_id == 0x9c)
            {                           /* target_id == 0x9c : C55x */
                /* .cinit section flags :0x140 ; .const section flags :0x240 */
                if (section_header->s_flags == 0x140 || section_header->s_flags == 0x240)
                {
                    int new_vaddr = section_header->s_vaddr >> 1;
                    int new_size = section_header->s_size >> 1;

                    exp = c55x_sky_write_phys_by_space(obj, memory_space, new_vaddr, start, new_size);
                } else
                {
                    exp =
                        sky_write_phys_by_space(obj, memory_space, section_header->s_vaddr, start,
                                                section_header->s_size);
                }
            } else
            {
                /* .bootload section flags: 0xa20; .text section flags :0x520 ; .cinit section flags :0x340 ; .const section flags :0x240 */
                if (section_header->s_flags == 0xa20 || section_header->s_flags == 0x520
                    || section_header->s_flags == 0x340 || section_header->s_flags == 0x240)
                    exp =
                        sky_write_phys_by_space(obj, memory_space, section_header->s_vaddr, start,
                                                section_header->s_size);
            }
        }
    }
  out:
    close(tmp_fd);
    //sleep(10);
    return exp;
}

exception_t sky_load_hex_by_space(conf_object_t * obj, memory_space_intf * memory_space, const char *filename)
{
    char buf[BUF_SIZE];
    char *len, *haddr, *faddr, *data_type, *temp;
    int i, j = 0;
    int paddr, addr, off_addr;
    int *data;

    /* malloc corresponding size */
    len = calloc(DATA_LEN, sizeof (char));      /* data len */
    haddr = calloc(BASE_ADDR_LEN, sizeof (char));       /* Base address */
    faddr = calloc(OFFSET_ADDR_LEN, sizeof (char));     /* offset address */
    data_type = calloc(DATA_TYPE_LEN, sizeof (char));   /* Data Type */
    temp = calloc(TEMP_LEN, sizeof (char));     /* temp variable */

    memory_space_intf *mem_iface = (memory_space_intf *) SKY_get_iface(obj, MEMORY_SPACE_INTF_NAME);

    if (mem_iface == NULL)
    {
        SKYEYE_ERR("can not load the data in %s", __FUNCTION__);
        return;
    }

    FILE *tmp_fd = fopen(filename, "r");

    if (tmp_fd == NULL)
    {
        fprintf(stderr, "open %s error: %s\n", filename, strerror(errno));
        goto out;
    }
    /* Analytical every line of the file */
    while ((fgets(buf, BUF_SIZE, tmp_fd)) != NULL)
    {
        for (i = 1; i < strlen(buf); i++)
        {
            if (i < 3)
                len[i - 1] = buf[i];    /* Get length of Data */
            if (i > 2 && i < 7)
                faddr[i - 3] = buf[i];  /* Get the offset address */
            if (i > 6 && i < 9)
                data_type[i - 7] = buf[i];      /* Get the Data Type */
        }
        len[2] = '\0';
        faddr[4] = '\0';

        if (strncmp(data_type, "04", 2) == 0)
        {
            for (i = 9; i < strtol(len, NULL, 16) * 2 + 9; i++)
            {
                haddr[i - 9] = buf[i];  /* Get the Base address */
            }
            haddr[i - 8] = '\0';
            paddr = strtol(haddr, NULL, 16) << 16;
        } else if (strncmp(data_type, "00", 2) == 0)
        {
            off_addr = strtol(faddr, NULL, 16);
            addr = paddr + off_addr;

            if (!SKY_has_attribute(obj, "load_mask"))
            {
                attr_value_t attr = SKY_get_attribute(obj, "load_mask");
                uint32_t load_mask = 0;

                load_mask = SKY_attr_uinteger(attr);
                if (load_mask != 0)
                    addr = (addr & load_mask);
            }
            if (!SKY_has_attribute(obj, "load_addr"))
            {
                attr_value_t attr = SKY_get_attribute(obj, "load_addr");
                uint32_t load_addr = 0;

                load_addr = SKY_attr_uinteger(attr);
                if (load_addr != 0)
                    addr = addr | load_addr;
            }

            data = calloc(strtol(len, NULL, 16), sizeof (int));
            for (i = 9; i < strtol(len, NULL, 16) * 2 + 9; i++)
            {
                temp[j] = buf[i];
                temp[2] = '\0';
                if (j == 1)
                {
                    data[(i - 10) / 2] = strtol(temp, NULL, 16);
                    j = -1;
                }
                j++;
            }

            for (i = 0; i < strtol(len, NULL, 16); i++)
            {
                if (mem_iface->write(obj, addr + i, &data[i], 1) != 0)
                {
                    skyeye_log(Error_log, __FUNCTION__, "SKYEYE: write physical address 0x%x error!!!\n", addr + i);
                    return;
                }
            }
        } else if (strncmp(data_type, "05", 2) == 0)
        {
            data = calloc(strtol(len, NULL, 16), sizeof (int));
            for (i = 9; i < strtol(len, NULL, 16) * 2 + 9; i++)
            {
                temp[j] = buf[i];
                temp[9] = '\0';
                if (j == 1)
                {
                    data[(i - 10) / 2] = strtol(temp, NULL, 16);
                    j = -1;
                }
                j++;
            }
        } else if (strncmp(data_type, "01", 2) == 0)
        {
            goto out;
        }
    }

  out:
    close(tmp_fd);
    return No_exp;
}

exception_t get_hex_start_addr(const char *filename, generic_address_t * start_addr,
                               generic_address_t * code_start_addr, generic_address_t * code_end_addr)
{
    char buf[BUF_SIZE];
    char *len, *haddr, *faddr, *data_type;
    int i = 0;
    int base_addr;

    len = calloc(DATA_LEN, sizeof (char));
    haddr = calloc(BASE_ADDR_LEN, sizeof (char));
    faddr = calloc(OFFSET_ADDR_LEN, sizeof (char));
    data_type = calloc(DATA_TYPE_LEN, sizeof (char));

    FILE *tmp_fd = fopen(filename, "r");

    if (tmp_fd == NULL)
    {
        fprintf(stderr, "open %s error: %s\n", filename, strerror(errno));
        goto out;
    }

    while ((fgets(buf, BUF_SIZE, tmp_fd)) != NULL)
    {
        for (i = 1; i < strlen(buf); i++)
        {
            if (i < 3)
                len[i - 1] = buf[i];
            if (i > 2 && i < 7)
                faddr[i - 3] = buf[i];
            if (i > 6 && i < 9)
                data_type[i - 7] = buf[i];
        }
        if (strncmp(data_type, "04", 2) == 0)
        {
            for (i = 9; i < strtol(len, NULL, 16) * 2 + 9; i++)
            {
                haddr[i - 9] = buf[i];
            }
            base_addr = strtol(haddr, NULL, 16) << 16;
            *start_addr = base_addr;
            goto out;
        }
    }
  out:
    close(tmp_fd);
    return No_exp;
}

exception_t get_coff_start_addr(const char *filename, generic_address_t * start_addr,
                                generic_address_t * code_start_addr, generic_address_t * code_end_addr)
{
    struct coff_header *coffFile;
    struct stat coff_fstat;
    int tmp_fd = open(filename, O_RDONLY);

    if (tmp_fd == -1)
    {
        fprintf(stderr, "open %s error: %s\n", filename, strerror(errno));
        goto out;
    }
    fstat(tmp_fd, &coff_fstat);
    /* malloc */
    coffFile = mmap(NULL, coff_fstat.st_size, PROT_READ, MAP_PRIVATE, tmp_fd, 0);
    if (coffFile == NULL || coffFile == MAP_FAILED)
    {
        fprintf(stderr, "mmap error: %s\n", strerror(errno));
        goto out;
    }
    filehdr_v12 *filehdr = ((char *) coffFile + 0x0);
    aouthdr *aout_header = ((char *) coffFile + sizeof (filehdr_v12));

    *start_addr = aout_header->o_entry;

    *code_start_addr = aout_header->o_text_start;
    *code_end_addr = aout_header->o_text_start + aout_header->o_tsize;

    close(tmp_fd);
  out:
    return No_exp;
}

exception_t SKY_load_elf_by_space(conf_object_t * obj, memory_space_intf * memory_space, const char *filename)
{
    exception_t ret = Exec_format_exp;
    bfd *tmp_bfd = NULL;
    asection *s;
    char *tmp_str = NULL;

    /* open */
    tmp_bfd = bfd_openr(filename, NULL);
    if (tmp_bfd == NULL)
    {
        fprintf(stderr, "open %s error: %s\n", filename, bfd_errmsg(bfd_get_error()));
        goto out;
    }
    if (!bfd_check_format(tmp_bfd, bfd_object))
    {
        /* FIXME:In freebsd, if bfd_errno is bfd_error_file_ambiguously_recognized,
         * though bfd can't recognize this format, we should try to load file.*/
        if (bfd_get_error() != bfd_error_file_ambiguously_recognized)
        {
            fprintf(stderr, "check format of %s error: %s\n", filename, bfd_errmsg(bfd_get_error()));
            goto out;
        }
    }
    //big_endian = bfd_big_endian(tmp_bfd);

    skyeye_log(Info_log, __FUNCTION__, "exec file \"%s\"'s format is %s.\n", filename, tmp_bfd->xvec->name);
#if 1
    sky_pref_t *pref = get_skyeye_pref();

    if (pref->user_mode_sim)
    {
        /* trying a direct mmap access, where user application
         * will be mapped to 0x8000 of skyeye memory space. */

        uint32_t ret_mmap = -1;
        sky_exec_info_t *info = get_skyeye_exec_info();

        if (info->mmap_access)
        {
            uint32_t prog_top = info->brk;

            /* For now, the only protection needed is write as we are going to copy data */
            /* contains text, rodata and bss as well */
            ret_mmap = (uint32_t) mmap((void *) 0x8000, prog_top - 0x4000,
                                       PROT_WRITE, MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS, -1, 0);
            if (ret_mmap == (uint32_t) MAP_FAILED)
            {
#include <errno.h>
                printf("Direct mmap access failed, mmap error %d\nExecute application without -m argument.\n", errno);
                exit(-1);
            } else
            {
                printf("Direct mmap access success, at 0x%08x-0x%08x\n", ret_mmap, ret_mmap + prog_top - 0x4000);
            }
        }
    }                                   //if(pref->user_mode_sim)
#endif
    /* load the corresponding section to memory */
    for (s = tmp_bfd->sections; s; s = s->next)
    {
        // To load not only [SEC_LOAD], but all segments with [SEC_ALLOC]
        if (bfd_get_section_flags(tmp_bfd, s) & (SEC_LOAD))
        {
            if (bfd_section_lma(tmp_bfd, s) != bfd_section_vma(tmp_bfd, s))
            {
#if ELF_LOADING_INFO
                printf("load section %s: lma = 0x%08x (vma = 0x%08x)  size = 0x%08x.\n", bfd_section_name(tmp_bfd, s),
                       (unsigned int) bfd_section_lma(tmp_bfd, s), (unsigned int) bfd_section_vma(tmp_bfd, s),
                       (unsigned int) bfd_section_size(tmp_bfd, s));
#endif
            } else
            {
#if ELF_LOADING_INFO
                printf("load section %s: addr = 0x%08x  size = 0x%08x.\n", bfd_section_name(tmp_bfd, s),
                       (unsigned int) bfd_section_lma(tmp_bfd, s), (unsigned int) bfd_section_size(tmp_bfd, s));
#endif
            }

            if (bfd_section_size(tmp_bfd, s) > 0)
            {
                tmp_str = (char *) skyeye_mm(bfd_section_size(tmp_bfd, s));
                if (!tmp_str)
                {
                    fprintf(stderr, "alloc memory to load session %s error.\n", bfd_section_name(tmp_bfd, s));
                    goto out;
                }
                if (!bfd_get_section_contents(tmp_bfd, s, tmp_str, 0, bfd_section_size(tmp_bfd, s)))
                {
                    fprintf(stderr,
                            "get session %s content error: %s\n",
                            bfd_section_name(tmp_bfd, s), bfd_errmsg(bfd_get_error()));
                    goto out;
                }
                if (bfd_section_lma(tmp_bfd, s) != bfd_section_vma(tmp_bfd, s))
                {
                    ret = sky_write_phys_by_space(obj, memory_space, bfd_section_lma(tmp_bfd, s),
                                                  tmp_str, bfd_section_size(tmp_bfd, s));

                } else
                    ret = sky_write_phys_by_space(obj, memory_space, bfd_section_vma(tmp_bfd, s),
                                                  tmp_str, bfd_section_size(tmp_bfd, s));
#if 0
                /* If only run in user mode, we load section with physical address */
                if (addr_type == Phys_addr || get_user_mode())
                {
                    write_phys(memory_space, bfd_section_vma(tmp_bfd, s), tmp_str, bfd_section_size(tmp_bfd, s));
                } else if (addr_type == Virt_addr)
                {
                    write_virt(memory_space, bfd_section_lma(tmp_bfd, s), tmp_str, bfd_section_size(tmp_bfd, s));
                } else
                {
                    fprintf(stderr, "wrong address type %d\n", addr_type);
                    goto out;
                }
#endif
                if (ret != No_exp)
                    goto out;
                skyeye_free(tmp_str);
                tmp_str = NULL;
            }
        } else
        {
            /* clear .bss section if simulate applications */
            if (get_user_mode())
            {
                //if(strcmp(bfd_section_name (tmp_bfd, s), ".bss") == 0){
                if (strcmp(bfd_section_name(tmp_bfd, s), ".bss") == 0 ||
                    strcmp(bfd_section_name(tmp_bfd, s), ".sbss") == 0 ||
                    strcmp(bfd_section_name(tmp_bfd, s), "__libc_freeres_pt") == 0 ||
                    strcmp(bfd_section_name(tmp_bfd, s), ".tbss") == 0)
                {
                    unsigned int bss_addr = (unsigned int) bfd_section_vma(tmp_bfd, s);
                    unsigned int bss_size = (unsigned int) bfd_section_size(tmp_bfd, s);
                }
            } else
            {
#if ELF_LOADING_INFO
                printf("not load section %s: addr = 0x%08x  size = 0x%08x .\n", bfd_section_name(tmp_bfd, s),
                       (unsigned int) bfd_section_vma(tmp_bfd, s), (unsigned int) bfd_section_size(tmp_bfd, s));
#endif
            }
        }
    }

    ret = No_exp;
  out:
    if (tmp_str)
        skyeye_free(tmp_str);
    if (tmp_bfd)
        bfd_close(tmp_bfd);
    return (ret);
}

exception_t SKY_load_file(conf_object_t * obj, memory_space_intf * memory_space, const char *filename)
{
    if (is_elf(filename) == True)
    {
        return SKY_load_elf_by_space(obj, memory_space, filename);
    } else if (is_coff(filename))
    {
        return sky_load_coff_by_space(obj, memory_space, filename);
    } else if (is_hex(filename))
    {
        return sky_load_hex_by_space(obj, memory_space, filename);
    } else
        /* something wrong */
        return Exec_format_exp;
}
