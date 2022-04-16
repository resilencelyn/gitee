/*
 * © Copyright 2014 eJim Lee. All Rights Reserved.
 * File:    json_conf.h
 * Email:   lyjforjob@gmail.com
 * Date:    2014-02-11
 */

#ifndef __JSON_CONF_H__
#define __JSON_CONF_H__

//#include "json_parser.h"

#ifdef __cplusplus
extern "C" {
#endif

#define JSON_CONF_NAME "skyeye.json"
typedef struct obj_desc obj_desc;
int objlist_create(obj_desc *objlist, void *extra);
int objlist_set(obj_desc *objlist, void *extra);

int json_conf(const char *path, void *extra);
int check_current_dir(void);

#ifdef __cplusplus
}
#endif

#endif
