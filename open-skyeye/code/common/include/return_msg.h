#ifndef __COMMON_RETURN_MSG_H__
#define __COMMON_RETURN_MSG_H__

typedef enum {
    DEV_PARAM_ERROR=0,
    RUNNING_WARNING,
    RUNNING_STATE_INFO,
    type_number
}return_info_t;


int SKYEYE_return_msg(return_info_t level, char *msg);

#endif
