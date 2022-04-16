/*
 * skyeye_arch.c -  all architecture definition for skyeye
 * Copyright (C) 2003 Skyeye Develop Group
 * for help please send mail to <skyeye-developer@lists.sf.linuxforum.net>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 */
/*
 * 12/16/2006   Michael.Kang  <blackfin.kang@gmail.com>
 */

#include "skyeye_arch.h"
#include "skyeye_config.h"
#include "skyeye_options.h"
#include "skyeye_mm.h"
#include "skyeye_log.h"
#include <stdlib.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
/* the number of supported architecture */
#define MAX_SUPP_ARCH 32

/**
* @brief the architecture supported by skyeye
*/
static arch_config_t *skyeye_archs[MAX_SUPP_ARCH];

/*
 * register a supported arch to skyeye_archs
 */
generic_arch_t *running_arch_list;

/**
* @brief the default architecture
*/
static char *default_arch_name = "arm";

void reset_arch(void)
{
    memset(skyeye_archs, 0, sizeof (skyeye_archs));
    if (running_arch_list)
    {
        skyeye_free(running_arch_list);
        running_arch_list = NULL;
    }
    return;
}

#if 0
void register_core_class(const char *class_name, skyeye_class_t * class_data)
{
    SKY_register_class(class_name, class_data);
}
#endif

/**
* @brief used to register a supported architecture to skyeye
*
* @param arch
*/
#if 0
void register_arch(arch_config_t * arch)
{
    int i;

    for (i = 0; i < MAX_SUPP_ARCH; i++)
    {
        if (skyeye_archs[i] == NULL)
        {
            skyeye_archs[i] = arch;
            return;
        }
    }
    fprintf(stderr, "In %s,can not register the arch since overflow the max arch number.\n", __FUNCTION__);
}
#endif
/*
 * get arch instance in running by its name.
 */

/**
* @brief alloc the current running architecture
*
* @param arch_name
*
* @return 
*/

/**
* @brief The handler of arch option
*
* @param this_option
* @param num_params
* @param params[]
*
* @return 
*/
static int do_arch_option(skyeye_option_t * this_option, int num_params, const char *params[])
{
    int i;

    //arch_config_t *arch = skyeye_config.arch;
    skyeye_config_t *config = get_current_config();

    for (i = 0; i < MAX_SUPP_ARCH; i++)
    {
        if (skyeye_archs[i] == NULL)
            continue;
        if (!strncmp(params[0], skyeye_archs[i]->arch_name, MAX_PARAM_NAME))
        {
            config->arch = skyeye_archs[i];
            SKYEYE_INFO("arch: %s\n", skyeye_archs[i]->arch_name);
            return 0;
        }
    }
    SKYEYE_ERR("Error: Unknowm architecture name \"%s\" or you use low version of skyeye?\n", params[0]);
    return -1;
}

int register_arch_option(const char *arch_name)
{
    int i;

    //arch_config_t *arch = skyeye_config.arch;
    skyeye_config_t *config = get_current_config();

    for (i = 0; i < MAX_SUPP_ARCH; i++)
    {
        if (skyeye_archs[i] == NULL)
            continue;
        if (!strncmp(arch_name, skyeye_archs[i]->arch_name, MAX_PARAM_NAME))
        {
            config->arch = skyeye_archs[i];
            SKYEYE_INFO("arch: %s\n", skyeye_archs[i]->arch_name);
            return 0;
        }
    }
    SKYEYE_ERR("Error: Unknowm architecture name \"%s\" or you use low version of skyeye?\n", arch_name);
    return -1;
}

/**
* @brief get the default architecture supported by skyeye
*
* @return 
*/
generic_arch_t *get_default_arch()
{
    int i;

    for (i = 0; i < MAX_SUPP_ARCH; i++)
    {
        if (skyeye_archs[i] == NULL)
            continue;
        if (!strncmp(default_arch_name, skyeye_archs[i]->arch_name, MAX_PARAM_NAME))
        {
            return skyeye_archs[i];
        }
    }
    skyeye_log(Warning_log, __FUNCTION__, "No default arch is found.\n");
    return NULL;
}

/**
* @brief the initialization of arch
*/
void init_arch()
{
    register_option("arch", do_arch_option, "support different architectures.\n");
}

generic_address_t com_get_pc(void)
{
    //generic_arch_t* arch_instance = get_arch_instance(NULL);
    //return arch_instance->get_pc();
    return 0;
}

#if 0                                   /* we will dynamiclly load all the arch module */
extern void init_arm_arch();
extern void init_bfin_arch();
extern void init_coldfire_arch();
extern void init_mips_arch();

arch_t *extern void init_sparc_arch();

void initialize_all_arch()
{
    int i;

    for (i = 0; i < MAX_SUPP_ARCH; i++)
    {
        skyeye_archs[i] = NULL;
    }
    /* register arm_arch */
    init_arm_arch();

    /* register bfin_arch */
    init_bfin_arch();

    /* register mips_arch */
    init_mips_arch();

    /* register coldfire_arch */
    init_coldfire_arch();

    /* register ppc_arch */
    init_ppc_arch();

    /* register sparc_arch */
    init_sparc_arch();
}
#endif

struct
{
    const char *cpucls;
    arch_t arch;
} arch_list[] =
{
    {
    .cpucls = "cortex_a9_core",.arch = ARM},
    {
.cpucls = "c67x_core",.arch = C6K},};

arch_t get_arch_by_cpucls(char *cpucls)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(arch_list); i++)
    {
        if (strcmp(arch_list[i].cpucls, cpucls) != 0)
            continue;
        return arch_list[i].arch;
    }
    return Unknow;
}
