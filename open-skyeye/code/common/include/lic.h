#ifndef __LIC_H__
#define __LIC_H__
#include<stdbool.h>
#include<skyeye_types.h>
extern char allow_cpu_list[2048];
extern int verify_status;
int local_lic_validation(char* file);
int network_lic_validation(char* ip, int port);
int license_verify();
int check_cpu(char* cpuname);
const char* read_lic_info(char* filename);
const char* get_cpuid();
void close_network_lic();
int dog_lic_validation();
const bool_t check_usb_lic_connect();
#endif

