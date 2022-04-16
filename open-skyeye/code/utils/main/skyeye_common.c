#include <sim_control.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void python_cli();
extern void backtrace_register();

int main(int argc, char *argv[])
{
    backtrace_register();

    SIM_init();
    python_cli();
    return 0;
}
