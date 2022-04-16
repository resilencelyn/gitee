/*
 * © Copyright 2014 eJim Lee. All Rights Reserved.
 * File:    skyeye_checkpoint.h
 * Email:   lyjforjob@gmail.com
 * Date:    2014-03-27
 */

#ifndef __CHECKPOINT_H__
#define __CHECKPOINT_H__

#include "skyeye_types.h"

#ifdef __cplusplus
extern "C" {
#endif

int init_chp();
void add_chp_data(void *data, int size, char *name);

typedef uint32_t save_flag_t;
void write_configuration_to_file(const char *file, save_flag_t flag);
void read_configuration(const char *file);
exception_t store_to_configure(char *file);
exception_t recover_from_configure(char *file);

typedef struct x86_checkpoint{
	exception_t (*save_configure)(conf_object_t *obj, char *filename);
	exception_t (*load_configure)(conf_object_t *obj, char *filename);
}x86_checkpoint_intf;
#define X86_CHECKPOINT_INTF_NAME "x86_checkpoint"

#ifdef __cplusplus
}
#endif

#endif
