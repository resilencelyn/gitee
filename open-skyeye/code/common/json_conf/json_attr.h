#ifndef __JSON_ATTR_H__
#define __JSON_ATTR_H__

#include <stdio.h>
#include <string.h>
#include "json_parser.h"
#include "json_base.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /* create special attribute list of each base */
#define mach_ATTR_SPEC_LIST(f)
#define cpu_ATTR_SPEC_LIST(f) \
	f(cpu, type, string)
#define device_ATTR_SPEC_LIST(f) \
	f(device, address, uinteger) \
	f(device, length, uinteger)
#define linker_ATTR_SPEC_LIST(f)

    typedef int (*attr_spec_set) (struct obj_desc * obj, struct attr_desc * attr, void *extra);
    typedef struct attr_spec_intf
    {
        const char *key;
        const char *type;
        attr_spec_set set;
    } attr_spec_intf;

#define EXTERN_LIST(base) \
	extern struct attr_spec_intf *base##_attr_spec_list;
      BASE_LIST(EXTERN_LIST)
#undef EXTERN_LIST
#ifdef __cplusplus
}
#endif

#endif
