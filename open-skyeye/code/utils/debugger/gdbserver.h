#ifndef __GDBSERVER__
#define __GDBSERVER__

typedef struct gdbserver
{
    conf_object_t *obj;
    conf_object_t *target_cpu;
    char *ip;
    int port;
    register_defs_t *current_reg_type;
    char *client_ip;
    int remote_debug;
    int remote_desc;
    int extended_protocol;
    int general_thread;
    int cont_thread;
    pthread_t remote_thread_id;
    bool_t SIM_remote_stopped;
    unsigned char *registers;
    jmp_buf toplevel;
} gdbserver_device;

#define TARGET_BYTE_WIDTH(SERVER) ((SERVER)->current_reg_type->byte_width)
#endif
