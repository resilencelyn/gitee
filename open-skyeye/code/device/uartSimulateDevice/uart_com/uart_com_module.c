#include "skyeye_module.h"
#include <stdio.h>

const char* skyeye_module = "skyeye_uart_com";

extern void init_uart_com();

void module_init(){
    init_uart_com();
}

void module_fini(){
}
