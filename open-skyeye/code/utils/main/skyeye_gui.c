#include <sim_control.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void python_gui();
extern void backtrace_register();

int main(int argc, char *argv[])
{
    backtrace_register();

    SIM_init();
    python_gui();
    return 0;
}
