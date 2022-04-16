/*************************************************************************
	> TriCore simulation for SkyEye: tc27x_core module implement
	> File Name    : tc27x_module.c
	> Author       : jiachao
	> Mail         : jiachao@digiproto.com
	> Created Time : 2021/10/26 09:45:09
************************************************************************/

#include <stdlib.h>
#include "skyeye_module.h"

const char *skyeye_module = "tricore_cpu";

extern void init_tricore_cpu();

void module_init()
{
    init_tricore_cpu();
}

void module_fini()
{
}
