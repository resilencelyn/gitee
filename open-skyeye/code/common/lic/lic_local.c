#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lic.h"
#include "skyeye_internal.h"
#ifdef __WIN32__
#include "rsa_lic.h"
#endif

int local_lic_validation(char *filename)
{
#ifndef __MINGW32__
    return 3;
#else
    int ret = 0;
    ret = licensecmp(filename);
    switch (ret)
    {
        case 1:
            skyeye_log(Error_log, __FUNCTION__, "Outside the scope of the allowed time\n");
            return 1;
        case 2:
            skyeye_log(Error_log, __FUNCTION__, "Outside the scope of the allowed MAC addresses\n");
            return 2;
        case 3:
            strcpy(allow_cpu_list, (const char *) get_cpu_list());
            return 3;
        case 4:
            skyeye_log(Error_log, __FUNCTION__, "Could not find the Key\n");
            return 4;
        case 5:
            skyeye_log(Error_log, __FUNCTION__, "Could not find the license\n");
            return 5;
        case 26:
            skyeye_log(Error_log, __FUNCTION__, "Invalid license file with incorrect length\n");
            return 26;
        case 27:
            skyeye_log(Error_log, __FUNCTION__, "License file does not match key\n");
            return 27;
        case 28:
            skyeye_log(Error_log, __FUNCTION__, "CPU information does not match\n");
            return 28;
        default:
            return ret;
    }
#endif
}
const char *read_lic_info(char *filename)
{
#ifndef __MINGW32__
    return NULL;
#else
    const char *ret;
    ret = read_license_info(filename);
    return ret;
#endif
}

const char *get_cpuid()
{
#ifndef __MINGW32__
    return NULL;
#else
    const char *ret;

    ret = get_cpuid_info();

    return ret;
#endif
}
