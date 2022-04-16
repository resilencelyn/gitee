/* debugger: Remote server for GDB.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "arch_regdefs.h"
#include "sim_control.h"
#include "skyeye2gdb.h"
#include "skyeye_arch.h"
#include "skyeye_callback.h"
#include "skyeye_core.h"
#include "skyeye_dev_intf.h"
#include "skyeye_iface.h"
#include "skyeye_interface.h"
#include "skyeye_mm.h"
#include "skyeye_pref.h"
#include "skyeye_thread.h"
#include "skyeye_types.h"
#include <errno.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <unistd.h>
#ifdef __BEOS__
#include <BeBuild.h>
#endif
#include "gdbserver.h"

// koodailar remove it for mingw 2005.12.18--------------------------------------
#ifndef __MINGW32__
// Anthony Lee 2007-02-02 : for BeOS R5.0.x
#if (defined(__BEOS__) && B_BEOS_VERSION < 0x510)
#include <net/socket.h>
#define PF_INET AF_INET
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#endif
#endif
// end --------------------------------------------------------------------------

#include <fcntl.h>
#include <signal.h>

// Anthony Lee 2006-08-22 : for WinSock2
#ifdef __MINGW32__
#undef WORD
#undef byte
#include <pthread.h>
#include <windows.h>
#include <winsock2.h>
#endif

#if (defined(__MINGW32__) || (defined(__BEOS__) && B_BEOS_VERSION < 0x510))
#define Read(a, b, c)  recv(a, b, c, 0)
#define Write(a, b, c) send(a, b, c, 0)
#define Close(a)       closesocket(a)
#else
#define Read(a, b, c)  read(a, b, c)
#define Write(a, b, c) write(a, b, c)
#define Close(a)       close(a)
#endif

#define DBG_RDI(args...)       \
    {                          \
        if (dev->remote_debug) \
        {                      \
            printf(args);      \
            fflush(stdout);    \
        }                      \
    }

bool_t socket_is_open = False;

void set_skyeye_debug(conf_object_t *obj, int isEnable)
{
    gdbserver_device *dev = obj->obj;
    dev->remote_debug = isEnable;
    printf("dev->remote_debug = %d\n", dev->remote_debug);
    fflush(stdout);
}

char *ArchTable[][2] = {
    {"c67x_core", "c6k"},
    {"c64x_core", "c6k"},
    {"c64xplus_core", "c6k"},
    {"c674x_core", "c6k"},
    {"c66x_core", "c6k"},
    {"x86", "x86"},
    {"c54x_core", "tic54x"},
    {"c5510_core", "tic55x"},
    {"sparc_v7", "sparc"},
    {"sparc_v8", "sparc"},
    {"f28335_core", "c28x"},
    {"c2812_core", "c28x"},
    {"arm11_core", "arm"},
    {"arm920t_core", "arm"},
    {"cortex_a8_core", "arm"},
    {"cortex_a9_core", "arm"},
    {"arm926ejs_core", "arm"},
    {"cortex_m0_core", "arm"},
    {"cortex_m3", "arm"},
    {"cortex_r4_core", "arm"},
    {"ftc662_core", "arm"},
    {"e200_core", "powerpc"},
    {"e300_core", "powerpc"},
    {"e500_core", "powerpc"},
    {"e600_core", "powerpc"},
    {"g2_core", "powerpc"},
    {"gs32eb1_core", "mips"},
    {"ls132r_core", "mips"},
    {NULL, NULL}
};
typedef unsigned long CORE_ADDR;
struct SkyEye_ICE skyeye_ice;

jmp_buf toplevel;

/*
 * The pthread_cancel can't be used in win32,
 * So Using the globle value SIM_remote_stoped
 * to judge the remote's status, If remote is
 * stopped, We let the thread return.
 */

/* Open a connection to a remote debugger.
 * NAME is the filename used for communication.  */

int remote_open(conf_object_t *obj)
{

    gdbserver_device *dev = obj->obj;
    int save_fcntl_flags;

    int port = dev->port;
    char *ip = dev->ip;
    struct sockaddr_in sockaddr;
    int tmp;
    struct protoent *protoent;
    int tmp_desc;

#ifndef __MINGW32__
    if ((tmp_desc = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can't open socket\n");
        return 0;
    }
#else
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        skyeye_log(Error_log, __FUNCTION__, "Create init error!\n");
        return 0;
    }
    if ((tmp_desc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
    {
        skyeye_log(Error_log, __FUNCTION__, "Create Socket error!\n");
        return 0;
    }
    socket_is_open = True;
#endif

    /* Allow rapid reuse of this port. */
    tmp = 1;
    setsockopt(tmp_desc, SOL_SOCKET, SO_REUSEADDR, (char *)&tmp, sizeof(tmp));

    sockaddr.sin_family = PF_INET;
    sockaddr.sin_port = htons(port);
    sockaddr.sin_addr.s_addr = inet_addr(ip);
    if (bind(tmp_desc, (struct sockaddr *)&sockaddr, sizeof(sockaddr)))
    {
        /* We can't bind the port and the address once more
         * before server accepts the gdb client */
        skyeye_log(Warning_log, __FUNCTION__, "Can't bind address\n");
        return 0;
    }
    if (listen(tmp_desc, 1))
    {
        skyeye_log(Warning_log, __FUNCTION__, "Can't Listen address\n");
        return 0;
    }
    skyeye_log(Info_log, __FUNCTION__, "Listen success for connect!\n");

    tmp = sizeof(sockaddr);
    /* It is a best way that I thought to close the socket(tmp_desc)
     * before gdb server accepts a client refer to the function : remote_close */
    dev->remote_desc = tmp_desc;

    dev->remote_desc = accept(tmp_desc, (struct sockaddr *)&sockaddr, &tmp);
    if (dev->remote_desc == -1)
    {
        skyeye_log(Info_log, __FUNCTION__, "Accept failed\n");
        return 0;
    }
    char *client_ip = inet_ntoa(sockaddr.sin_addr);

    dev->client_ip = skyeye_mm(strlen(client_ip) + 1);
    strncpy(dev->client_ip, client_ip, strlen(client_ip) + 1);
    skyeye_log(Info_log, __FUNCTION__, "Wait for client connect\n");

    /* 2007-02-02 disabled on BeOS R5.0.x by Anthony Lee */
#if !(defined(__BEOS__) && B_BEOS_VERSION < 0x510)
    /* Enable TCP keep alive process. */
    tmp = 1;
    setsockopt(tmp_desc, SOL_SOCKET, SO_KEEPALIVE, (char *)&tmp, sizeof(tmp));

    /* Tell TCP not to delay small packets.  This greatly speeds up
     * interactive response. */
    tmp = 1;
    setsockopt(dev->remote_desc, 6 /* PROTO_TCP */, TCP_NODELAY, (char *)&tmp, sizeof(tmp));
#endif

    Close(tmp_desc);                    /* No longer need this */

#ifndef __MINGW32__
    signal(SIGPIPE, SIG_IGN);           /* If we don't do this, then gdbserver simply
                                         * exits when the remote side dies.  */
#endif

    skyeye_log(Info_log, __FUNCTION__, "Remote debugging IP:  %s, Port:   %d\n", ip, port);
    return 1;
}

extern register_defs_t *register_types[9];

// void remote_interrupt(generic_arch_t* arch_instance);
void com_remote_gdb(conf_object_t *obj, char *archname)
{
    gdbserver_device *dev = obj->obj;
    int i;

    for (i = 0; i < MAX_SUPP_ARCH; i++)
    {
        if (register_types[i] != NULL)
            if (!strcmp(archname, register_types[i]->name))
                dev->current_reg_type = register_types[i];
    }
    if (!dev->current_reg_type)
    {
        skyeye_log(Info_log, __FUNCTION__, "Can not find register type for current arch!\n");
        return -1;
    }
    // register_callback(remote_interrupt, Step_callback);
    /* run gdbserver in a single thread */
    dev->SIM_remote_stopped = False;
    create_thread(sim_debug, NULL, &dev->remote_thread_id);
}

/* The function will close remote_desc after accepting the client,
 * it will close the tmp_desc before accepting the client */
void remote_close(conf_object_t *obj)
{

    gdbserver_device *dev = obj->obj;

#ifndef __MINGW32__
    Close(dev->remote_desc);
#else
    closesocket(dev->remote_desc);
    if (socket_is_open == True)
    {
        WSACleanup();
        socket_is_open = False;
    }
#endif
}

void close_remote_gdb(conf_object_t *obj)
{
    gdbserver_device *dev = obj->obj;

    dev->SIM_remote_stopped = True;
    remote_close(obj);
    destory_threads_by_id(dev->remote_thread_id);
    return;
}

/* Convert hex digit A to a number.  */

static int fromhex(int a)
{
    if (a >= '0' && a <= '9')
        return a - '0';
    else if (a >= 'a' && a <= 'f')
        return a - 'a' + 10;
    else if (a >= 'A' && a <= 'F')
        return a - 'A' + 10;
    else
        perror("Reply contains invalid hex digit");

    return -1;
}

/* Convert number NIB to a hex digit.  */

static int tohex(int nib)
{
    if (nib < 10)
        return '0' + nib;
    else
        return 'a' + nib - 10;
}

/* Send a packet to the remote machine, with error checking.
 * The data of the packet is in BUF.  Returns >= 0 on success, -1 otherwise. */

int putpkt(conf_object_t *obj, char *buf)
{

    gdbserver_device *dev = obj->obj;
    int i;
    unsigned char csum = 0;
    char buf2[2000];
    char buf3[1];
    int cnt = strlen(buf);
    char *p;

    /* Copy the packet into buffer BUF2, encapsulating it
     * and giving it a checksum.  */

    p = buf2;
    *p++ = '$';

    for (i = 0; i < cnt; i++)
    {
        csum += buf[i];
        *p++ = buf[i];
    }
    *p++ = '#';
    *p++ = tohex((csum >> 4) & 0xf);
    *p++ = tohex(csum & 0xf);

    *p = '\0';

    /* Send it over and over until we get a positive ack.  */

    do
    {
        int cc;

        if (Write(dev->remote_desc, buf2, p - buf2) != p - buf2)
        {
            perror("putpkt(write)");
            return -1;
        }

        DBG_RDI("putpkt (\"%s\"); [looking for ack]\n", buf2);
        cc = Read(dev->remote_desc, buf3, 1);
        DBG_RDI("[received '%c' (0x%x)]\n", buf3[0], buf3[0]);
        if (cc <= 0)
        {
            if (cc == 0)
                fprintf(stderr, "putpkt(read): Got EOF\n");
            else
                perror("putpkt(read)");

            return -1;
        }
    }
    while (buf3[0] != '+');

    return 1;                           /* Success! */
}

/* Returns next char from remote GDB.  -1 if error.  */

static int readchar(conf_object_t *obj)
{
    gdbserver_device *dev = obj->obj;
    static char buf[BUFSIZ];
    static int bufcnt = 0;
    static char *bufp;

    if (bufcnt-- > 0)
        return *bufp++ & 0x7f;

    bufcnt = Read(dev->remote_desc, buf, sizeof(buf));

    if (bufcnt <= 0)
    {
        if (bufcnt == 0)
            fprintf(stderr, "readchar: Got EOF\n");
        else
            perror("readchar");

        return -1;
    }

    bufp = buf;
    bufcnt--;
    return *bufp++ & 0x7f;
}

/* Read a packet from the remote machine, with error checking,
 * and store it in BUF.  Returns length of packet, or negative if error. */

int getpkt(conf_object_t *obj, char *buf)
{

    gdbserver_device *dev = obj->obj;
    char *bp;
    unsigned char csum, c1, c2;
    int c;

    while (1)
    {
        csum = 0;

        while (1)
        {
            c = readchar(obj);
            if (c == '$')
                break;
            DBG_RDI("[getpkt: discarding char '%c']\n", c);
            if (c < 0)
                return -1;
        }

        bp = buf;
        while (1)
        {
            c = readchar(obj);
            if (c < 0)
                return -1;
            if (c == '#')
                break;
            *bp++ = c;
            csum += c;
        }
        *bp = 0;

        c1 = fromhex(readchar(obj));
        c2 = fromhex(readchar(obj));

        if (csum == (c1 << 4) + c2)
            break;

        fprintf(stderr, "Bad checksum, sentsum=0x%x, csum=0x%x, buf=%s\n", (c1 << 4) + c2, csum, buf);
        Write(dev->remote_desc, "-", 1);
    }

    DBG_RDI("getpkt (\"%s\");  [sending ack] \n", buf);

    Write(dev->remote_desc, "+", 1);

    DBG_RDI("[sent ack]\n");
    return bp - buf;
}

void write_ok(char *buf)
{
    buf[0] = 'O';
    buf[1] = 'K';
    buf[2] = '\0';
}

void write_enn(char *buf)
{
    buf[0] = 'E';
    buf[1] = '0';
    buf[2] = '1';
    buf[3] = '\0';
}
static void convert_int_to_ascii(char *from, char *to, int n)
{
    int nib;
    char ch;

    while (n--)
    {
        ch = *from++;
        nib = ((ch & 0xf0) >> 4) & 0x0f;
        *to++ = tohex(nib);
        nib = ch & 0x0f;
        *to++ = tohex(nib);
    }
    *to++ = 0;
}

static void convert_ascii_to_int(char *from, char *to, int n)
{
    int nib1, nib2;

    while (n--)
    {
        nib1 = fromhex(*from++);
        nib2 = fromhex(*from++);
        *to++ = (((nib1 & 0x0f) << 4) & 0xf0) | (nib2 & 0x0f);
    }
}

static char *outreg(conf_object_t *obj, int regno, char *buf)
{

    gdbserver_device *dev = obj->obj;
    int regsize = dev->current_reg_type->register_raw_size(regno);

    *buf++ = tohex(regno >> 4);
    *buf++ = tohex(regno & 0xf);
    *buf++ = ':';
    convert_int_to_ascii(&(dev->registers[dev->current_reg_type->register_byte(regno)]), buf, regsize);
    buf += 2 * regsize;
    *buf++ = ';';

    return buf;
}

void prepare_resume_reply(conf_object_t *obj, char *buf, char status, unsigned char signo)
{

    gdbserver_device *dev = obj->obj;
    int nib;

    *buf++ = status;

    /* FIXME!  Should be converting this signal number (numbered
     * according to the signal numbering of the system we are running on)
     * to the signal numbers used by the gdb protocol (see enum target_signal
     * in gdb/target.h).  */
    nib = ((signo & 0xf0) >> 4);
    *buf++ = tohex(nib);
    nib = signo & 0x0f;
    *buf++ = tohex(nib);

    if (status == 'T')
    {
        buf = outreg(obj, dev->current_reg_type->pc_regnum, buf);
        buf = outreg(obj, dev->current_reg_type->fp_regnum, buf);
        buf = outreg(obj, dev->current_reg_type->sp_regnum, buf);
    }
    /* For W and X, we're done.  */
    *buf++ = 0;
}

static void decode_m_packet(char *from, CORE_ADDR *mem_addr_ptr, unsigned int *len_ptr)
{
    int i = 0, j = 0;
    char ch;

    *mem_addr_ptr = *len_ptr = 0;

    while ((ch = from[i++]) != ',')
    {
        *mem_addr_ptr = *mem_addr_ptr << 4;
        *mem_addr_ptr |= fromhex(ch) & 0x0f;
    }

    for (j = 0; j < 4; j++)
    {
        if ((ch = from[i++]) == 0)
            break;
        *len_ptr = *len_ptr << 4;
        *len_ptr |= fromhex(ch) & 0x0f;
    }
}

static void decode_M_packet(char *from, CORE_ADDR *mem_addr_ptr, unsigned int *len_ptr, char *to)
{
    int i = 0;
    char ch;

    *mem_addr_ptr = *len_ptr = 0;

    while ((ch = from[i++]) != ',')
    {
        *mem_addr_ptr = *mem_addr_ptr << 4;
        *mem_addr_ptr |= fromhex(ch) & 0x0f;
    }

    while ((ch = from[i++]) != ':')
    {
        *len_ptr = *len_ptr << 4;
        *len_ptr |= fromhex(ch) & 0x0f;
    }

    convert_ascii_to_int(&from[i++], to, *len_ptr);
}

static void decode_DP_playload(char *buffer, int tp_id, action *parent_action)
{

    int i = 0;
    char ch;
    action *current_action;
    unsigned int remaining_step;
    unsigned int pass_count, remaining_pass;
    unsigned int mask;
    unsigned offset, length;
    int base_reg;

    switch (buffer[i])
    {
        case 'E':
        {
            i = i + 2;
            remaining_step = 0;
            while ((ch = buffer[i++]) != ':')
            {
                remaining_step = remaining_step << 4;
                remaining_step |= fromhex(ch) & 0x0f;
            }

            pass_count = 0;
            while (((ch = buffer[i]) != '-') && ((ch = buffer[i]) != '\0'))
            {
                pass_count = pass_count << 4;
                pass_count |= fromhex(ch) & 0x0f;
                i++;
            }

            set_tracepoint_status(tp_id, TRACEPOINT_ENABLED);
            set_tracepoint_remaining_step(tp_id, remaining_step);
            set_tracepoint_pass_count(tp_id, pass_count);
            set_tracepoint_remaining_pass(tp_id, pass_count);
            break;
        }
        case 'D':
        {
            i = i + 2;
            remaining_step = 0;
            while ((ch = buffer[i++]) != ':')
            {
                remaining_step = remaining_step << 4;
                remaining_step |= fromhex(ch) & 0x0f;
            }

            pass_count = 0;
            while (((ch = buffer[i]) != '-') && ((ch = buffer[i]) != '\0'))
            {
                pass_count = pass_count << 4;
                pass_count |= fromhex(ch) & 0x0f;
                i++;
            }

            set_tracepoint_status(tp_id, TRACEPOINT_DISABLED);
            set_tracepoint_remaining_step(tp_id, remaining_step);
            set_tracepoint_pass_count(tp_id, pass_count);
            set_tracepoint_remaining_pass(tp_id, pass_count);
            break;
        }
        case 'R':
        {
            i++;

            mask = 0;
            while (((ch = buffer[i]) != '-') && ((ch = buffer[i]) != '\0'))
            {
                mask = mask << 4;
                mask |= fromhex(ch) & 0x0f;
                i++;
            }
            current_action = prepare_action(tp_id, parent_action);
            set_action_type(current_action, ACTION_COLLECT);
            set_action_data_type(current_action, COLLECT_REGISTERS);
            set_rc_action_mask(current_action, mask);
            break;
        }
        case 'M':
        {
            i++;

            // get base_reg
            if (buffer[i] == '-')
            {
                base_reg = -1;
                while ((ch = buffer[i++]) != ',')
                    ;
            } else
            {
                base_reg = 0;
                i++;
                while (((ch = buffer[i]) != ',') && ((ch = buffer[i]) != '\0'))
                {
                    base_reg = base_reg << 4;
                    base_reg |= fromhex(ch) & 0x0f;
                    i++;
                }
            }

            // get offset
            offset = 0;
            while (((ch = buffer[i]) != ',') && ((ch = buffer[i]) != '\0'))
            {
                offset = offset << 4;
                offset |= fromhex(ch) & 0x0f;
                i++;
            }

            // get_length
            length = 0;
            i++;
            while (((ch = buffer[i]) != '-') && ((ch = buffer[i]) != '\0'))
            {
                length = length << 4;
                length |= fromhex(ch) & 0x0f;
                i++;
            }

            current_action = prepare_action(tp_id, parent_action);
            set_action_type(current_action, ACTION_COLLECT);
            set_action_data_type(current_action, COLLECT_MEMORY);
            set_mc_action_base_reg(current_action, base_reg);
            set_mc_action_offset(current_action, offset);
            set_mc_action_length(current_action, length);
            break;
        }
        case 'X':
            break;
        case 'S':
        {
            i++;
            current_action = prepare_action(tp_id, parent_action);
            set_action_type(current_action, ACTION_WHILE);
            set_wa_step_count(current_action, get_tracepoint_remaining_step(tp_id));
            set_wa_remaining_steps(current_action, get_tracepoint_remaining_step(tp_id));

            decode_DP_playload(&buffer[i], tp_id, current_action);
            break;
        }
        default:;
    }
}

static void decode_DP_packet(char *buffer, char *response)
{
    int i = 0;
    int cont = 0;
    char ch;
    unsigned int tp_number = 0;
    unsigned int tp_address = 0;
    int tp_id = -1; // tp_id is the position of the tracepoint on the skyeye_ice.tps array

    if (buffer[0] == '-')
    {
        i = 1;
        cont = 1;
    }
    // getting tp_number
    while ((ch = buffer[i++]) != ':')
    {
        tp_number = tp_number << 4;
        tp_number |= fromhex(ch) & 0x0f;
    }
    // getting tp_address
    while ((ch = buffer[i++]) != ':')
    {
        tp_address = tp_address << 4;
        tp_address |= fromhex(ch) & 0x0f;
    }

    if ((tp_id = find_tp_id(tp_number, tp_address)) != -1)
    {
    } else
    {
        tp_id = add_tracepoint(tp_number, tp_address);
    }

    decode_DP_playload(&buffer[i], tp_id, NULL);
    return;
}

static void decode_Frame_packet(char *buffer, char *response)
{
    int i;
    char ch;
    unsigned int frame_number;
    unsigned int tracepoint_number;

    start_trace_focusing();
    switch (buffer[0])
    {
        case 'p': // pc
        {
            i = 3;
            break;
        }
        case 't': // tdp
        {
            i = 4;
            tracepoint_number = 0;
            while ((ch = buffer[i]) != '\0')
            {
                tracepoint_number = tracepoint_number << 4;
                tracepoint_number |= fromhex(ch) & 0x0f;
                i++;
            }
            if (select_frame_buffer_by_tpn(tracepoint_number, &frame_number))
            {
                sprintf(response, "F%xT%x", frame_number, tracepoint_number);
            } else
            {
                sprintf(response, "F-1");
            }
            break;
        }
        case 'r': // range
        {
            i = 6;
            break;
        }
        case 'o': // outside
        {
            i = 8;
            break;
        }
        default: // a frame number or an error
        {
            i = 0;

            frame_number = 0;
            while ((ch = buffer[i]) != '\0')
            {
                frame_number = frame_number << 4;
                frame_number |= fromhex(ch) & 0x0f;
                i++;
            }

            // is it a tfind start none?
            if (frame_number == 0xFFFFFFFF)
            {
                stop_trace_focusing();

                sprintf(response, "OK");
            } else
            {
                if (select_frame_buffer_by_fn(&tracepoint_number, frame_number))
                {
                    sprintf(response, "F%xT%x", frame_number, tracepoint_number);
                } else
                {
                    sprintf(response, "F-1");
                }
            }
        }
    }
}

static void decode_ro_packet(char *buffer, char *response)
{
    char ch;
    int i;
    unsigned int *address;
    unsigned int start_address;
    unsigned int end_address;

    i = 0;
    start_address = 0;
    end_address = 0;

    address = &start_address;
    while ((ch = buffer[i]) != '\0')
    {
        switch (ch)
        {
            case ',':
            {
                address = &end_address;
                i++;
                break;
            }
            case ':':
            {
                address = &start_address;
                add_ro_region(start_address, end_address);
                start_address = 0;
                end_address = 0;

                address = &start_address;
                i++;
                break;
            }
            default:
            {
                *address = *address << 4;
                *address |= fromhex(ch) & 0x0f;
                i++;
            }
        }
    }
    if ((ch == 0) && ((start_address != 0) || (end_address != 0)))
    {
        add_ro_region(start_address, end_address);
    }
    sprintf(response, "OK");
}

static int decode_Q_packet(char *buffer, char *response)
{
    int i = 0;

    // init

    if (buffer[i] != 'T') // a valid trace request
    {
        return (0);
    } else
    {
        i = 1;
    }

    switch (buffer[i])
    {
        case 'i': // init
        {
            init_tracepoints();
            write_ok(response);
            return (1);
        }
        case 'S': // start or stop
        {
            if (buffer[i + 2] == 'a') // start
            {
                start_trace_recording();
                write_ok(response);
                return (1);
            } else if (buffer[i + 2] == 'o') // stop
            {
                stop_trace_recording();
                write_ok(response);
                return (1);
            } else
            {
                return (0);
            }
        }
        case 'D': // DP
        {
            decode_DP_packet(&(buffer[i + 3]), response);
            write_ok(response);
            return (1);
        }
        case 'F': // frame
        {
            decode_Frame_packet(&(buffer[i + 6]), response);
            return (1);
        }
        case 'r': // ro
        {
            decode_ro_packet(&(buffer[i + 3]), response);
            return (1);
        }
        default:
        {
            return (0);
        }
    }

    return (0);
}

void fetch_inferior_registers(conf_object_t *obj, int regno, unsigned char *memory)
{
    gdbserver_device *dev = obj->obj;

    if (regno >= 0 && regno < dev->current_reg_type->num_regs)
    {
        dev->current_reg_type->fetch_register(dev->target_cpu, regno, &memory[dev->current_reg_type->register_byte(regno)]);
    } else if (regno == -1)
    {
        for (regno = 0; regno < dev->current_reg_type->num_regs; regno++)
        {
            dev->current_reg_type->fetch_register(dev->target_cpu, regno, &memory[dev->current_reg_type->register_byte(regno)]);
        }
    }
}

static void store_inferior_registers(conf_object_t *obj, int regno, unsigned char *memory)
{
    gdbserver_device *dev = obj->obj;

    if (regno >= 0 && regno < dev->current_reg_type->num_regs)
    {
        dev->current_reg_type->store_register(dev->target_cpu, regno, &memory[dev->current_reg_type->register_byte(regno)]);
    } else if (regno == -1)
    {
        for (regno = 0; regno < dev->current_reg_type->num_regs; regno++)
        {
            dev->current_reg_type->store_register(dev->target_cpu, regno, &memory[dev->current_reg_type->register_byte(regno)]);
        }
    }
}

/*
 * stop when press Ctrl+c
 */
#if 1
void remote_interrupt(conf_object_t *conf_obj)
{
    fd_set rfds;
    struct timeval tv;

    gdbserver_device *dev = conf_obj->obj;

    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&rfds);
    FD_SET(dev->remote_desc, &rfds);

    if (select(dev->remote_desc + 1, &rfds, NULL, NULL, &tv) == 1)
    {
        int n;
        char buf[100];

        n = Read(dev->remote_desc, buf, sizeof(buf));
        if (n > 0)
        {
            DBG_RDI("Got async char");
        }

        if (buf[0] == '\003')
        {
            DBG_RDI(":Ctrl+C, interrupted.\n");
            SIM_stop(NULL);
        }
    }
}
#endif

int sim_debug(conf_object_t *obj)
{

    gdbserver_device *dev = obj->obj;
    static char own_buf[8000], mem_buf[8000];
    char *p;
    char ch, status;
    int i = 0;
    unsigned char signal;
    unsigned int len, addr;
    CORE_ADDR mem_addr;
    int type, size;

    // set num_regs for diff tic6x arch
    if (dev->current_reg_type->register_num)
    {
        dev->current_reg_type->num_regs = dev->current_reg_type->register_num(dev->target_cpu);
        dev->current_reg_type->register_bytes = dev->current_reg_type->register_byte(dev->current_reg_type->num_regs);
    }

    dev->registers = (unsigned char *)malloc(dev->current_reg_type->register_bytes);

    if (setjmp(toplevel))
    {
        fprintf(stderr, "Exiting\n");
        skyeye_exit(1);
    }
    sky_pref_t *pref = get_skyeye_pref();

    while (1)
    {
        i = remote_open(obj);
        if (dev->SIM_remote_stopped == True || i == 0)
        {
            /* Let the thread return if remote is stoped
             * or the bind the port failed */
            pthread_exit(0);
        }
    restart:
        setjmp(toplevel);
        while (getpkt(obj, own_buf) > 0)
        {
            if (dev->SIM_remote_stopped == True)
            {
                /* Let the thread return if remote is stoped */
                pthread_exit(0);
            }
            unsigned char sig;

            i = 0;
            ch = own_buf[i++];

            DBG_RDI("In %s line:%d [getpkt] ch = %c\n", __func__, __LINE__, ch);
            switch (ch)
            {
                case 'd':
                {
                    dev->remote_debug = !dev->remote_debug;
                    break;
                }
                case '?':
                {
                    status = 'S';
                    signal = 5;
                    prepare_resume_reply(obj, own_buf, status, signal);
                    break;
                }
                case 'H':
                {
                    switch (own_buf[1])
                    {
                        case 'g':
                            strtol(&own_buf[2], NULL, 16);
                            write_ok(own_buf);
                            // fetch_inferior_registers (0);
                            break;
                        case 'c':
                            strtol(&own_buf[2], NULL, 16);
                            write_ok(own_buf);
                            break;
                        default:
                            /* Silently ignore it so that gdb can extend the protocol
                             * without compatibility headaches.  */
                            own_buf[0] = '\0';
                            break;
                    }
                    break;
                }
                case 'g':
                {
                    if (get_trace_status() == TRACE_FOCUSING)
                        trace_fetch_registers(-1, dev->registers);
                    else
                        fetch_inferior_registers(obj, -1, dev->registers);
                    convert_int_to_ascii(dev->registers, own_buf, dev->current_reg_type->register_bytes);
                    break;
                }
                case 'G':
                {
                    convert_ascii_to_int(&own_buf[1], dev->registers, dev->current_reg_type->register_bytes);
                    store_inferior_registers(obj, -1, dev->registers);
                    write_ok(own_buf);
                    break;
                }
                case 'm':
                {
                    decode_m_packet(&own_buf[1], &mem_addr, &len);

#if 0
                    if ((get_trace_status() == TRACE_FOCUSING) && (is_in_ro_region(mem_addr, len) == 0))
                        size = trace_read(mem_addr, mem_buf, len);
                    else
                        size = sim_read(dev->target_cpu, mem_addr, mem_buf, len);
                    if (size != -1)
                        convert_int_to_ascii(mem_buf, own_buf, len);
                    else
                        write_enn(own_buf);
#endif
                    size = sim_read(dev->target_cpu, mem_addr, mem_buf, len, TARGET_BYTE_WIDTH(dev));
                    if (size != -1)
                        convert_int_to_ascii(mem_buf, own_buf, size);
                    else
                        write_enn(own_buf);
                    break;
                }
                case 'M':
                {
                    decode_M_packet(&own_buf[1], &mem_addr, &len, mem_buf);
                    // chy 2005-07-30  ARM_BREAKPOINT          0xe7ffdefe
                    // bp_opcode_conv( (unsigned int) mem_addr, (unsigned int*)mem_buf );
                    // chy 2006-04-12 debug
                    // printf("SKYEYE, debugger M  addr 0x%x, word 0x%x, len 0x%x\n", mem_addr, *((unsigned int *)mem_buf), len);

                    if (sim_write(dev->target_cpu, mem_addr, mem_buf, len, TARGET_BYTE_WIDTH(dev)) == len)
                        write_ok(own_buf);
                    else
                        write_enn(own_buf);
                    break;
                }
                case 'Q':
                {
                    if (decode_Q_packet(&own_buf[1], own_buf) == 0)
                        own_buf[0] = '\0';
                    break;
                }
                case 'q':
                {
                    if (strstr(own_buf, "qSupported"))
                    {
                        char tmp_buf[1024] = "\0";

                        // add support for feature query
                        if (strstr(own_buf, "swbreak+"))
                            strcat(tmp_buf, "swbreak+");
                        if (strstr(own_buf, "hwbreak+") && *tmp_buf)
                            strcat(tmp_buf, ";hwbreak+");
                        strcat(tmp_buf, "PacketSize=");
                        char buf_size[10];

                        sprintf(buf_size, "%x", dev->current_reg_type->register_bytes);
                        strcat(tmp_buf, buf_size);
                        strcpy(own_buf, tmp_buf);
                        // other feature need to be add too

                        // set debug_mode attr to 1
                        SIM_set_core_debug_mode(dev->target_cpu);
                        // SIM_set_core_mode(dev->target_cpu,0);

                    } else
                    {
                        switch (own_buf[1])
                        {
                            case 'C':
                                // set debug_mode attr for sparc gdb because of gdb6.4 donot support qSupported
                                SIM_set_core_debug_mode(dev->target_cpu);
                                // SIM_set_core_mode(dev->target_cpu,0);
                                break;
                            case 'T':
                                // Status
                                if (own_buf[2] == 'f') // Shielding the packet of qTfV and qTfp ,so we don't support the features request data about tracepoints that are being used by the target temporarily.We can refer the web "http://sourceforge.net/tracker/?func=detail&aid=3122955&group_id=85554&atid=576533."
                                {
                                    own_buf[0] = '\0';
                                } else if (get_trace_status() == TRACE_STARTED)
                                {
                                    own_buf[0] = 'T';
                                    own_buf[1] = '1';
                                    own_buf[2] = '\0';
                                } else
                                {
                                    own_buf[0] = 'T';
                                    own_buf[1] = '0';
                                    own_buf[2] = '\0';
                                }
                                break;
                            default:
                                own_buf[0] = '\0';
                                break;
                        }
                    }
                    break;
                    /* chy 2005-07-28
                     * case 'q':
                     * switch (own_buf[1]) {
                     * case 'C':
                     * own_buf[0] = '\0';
                     * break;
                     * case 'O':
                     * send_area(own_buf);
                     * break;
                     * default:
                     * own_buf[0] = '\0';
                     * break;
                     * }
                     * break;
                     * case 'C':
                     * convert_ascii_to_int (own_buf + 1, &sig, 1);
                     * myresume (0, sig);
                     * signal = mywait (&status);
                     * prepare_resume_reply (own_buf, status, signal);
                     * break;
                     * case 'S':
                     * convert_ascii_to_int (own_buf + 1, &sig, 1);
                     * myresume (1, sig);
                     * signal = mywait (&status);
                     * prepare_resume_reply (own_buf, status, signal);
                     * break;
                     * chy */
                }
                case 'c':
                {
                    // chy 2005-07-30
                    // ARMul_DoProg (state);
                    // sim_resume(0);

                    gdbserver_cont(obj);
                    prepare_resume_reply(obj, own_buf, status, signal);
                    break;
                }
                case 's':
                {
                    gdbserver_step(dev->target_cpu);

                    int signal_2 = 5;

                    signal_status_t sig_status = SIGNAL_Inactive;

                    sig_status = sim_get_signal_status(dev->target_cpu);
                    switch (sig_status)
                    {
                        case SIGNAL_Inactive:
                            // printf("[Signal]: SIGNAL_Inactive\n");
                            signal_2 = 5;
                            break;
                        case SIGNAL_Pending:
                            // printf("[Signal]: SIGNAL_Pending\n");
                            signal_2 = 5;
                            break;
                        case SIGNAL_Active:
                            // printf("[Signal]: SIGNAL_Active\n");
                            signal_2 = 2;
                            break;
                        default:
                            printf("[Signal]: No Signal\n");
                            break;
                    }

                    prepare_resume_reply(obj, own_buf, status, signal_2);
                    break;
                }
                    /* get a register value by its number */
                case 'p':
                {
                    p = &(own_buf[1]);
                    /* Get the register number */
                    addr = strtoul(p, (char **)&p, 16);
                    // printf("addr=0x%x\n", addr);
                    if (addr < dev->current_reg_type->num_regs)
                    {
                        fetch_inferior_registers(obj, addr, dev->registers);
                        convert_int_to_ascii(&dev->registers[dev->current_reg_type->register_byte(addr)], own_buf, dev->current_reg_type->register_raw_size(addr));
                    } else
                    {
                        strcpy(own_buf, "xxxxxxxx"); // register is unfetchable
                    }
                    break;
                }
                case 'z':
                {
                    p = &(own_buf[1]);
                    type = strtoul(p, (char **)&p, 16);
                    if (*p == ',')
                        p++;
                    addr = strtoul(p, (char **)&p, 16);
                    if (*p == ',')
                        p++;
                    len = strtoul(p, (char **)&p, 16);
                    // printf("SKYEYE:gdbserver z, type %d, addr %x, len %x\n",type, addr,len);
                    if (type == 0 || type == 1)
                    {
                        if (sim_ice_breakpoint_remove(dev->target_cpu, addr) < 0)
                            goto remove_breakpoint_error;
                        write_ok(own_buf);
                    } else
                    {
                    remove_breakpoint_error:
                        write_enn(own_buf);
                    }
                    break;
                }
                case 'Z':
                {
                    p = &(own_buf[1]);
                    type = strtoul(p, (char **)&p, 16);
                    if (*p == ',')
                        p++;
                    addr = strtoul(p, (char **)&p, 16);
                    if (*p == ',')
                        p++;
                    len = strtoul(p, (char **)&p, 16);
                    // printf("SKYEYE:gdbserver Z, type %d, addr %x, len %x\n",type, addr,len);
                    if (type == 0 || type == 1)
                    {
                        if (sim_ice_breakpoint_insert(dev->target_cpu, addr) < 0)
                            goto insert_breakpoint_error;
                        write_ok(own_buf);
                    } else
                    {
                    insert_breakpoint_error:
                        write_enn(own_buf);
                    }
                    break;
                }
                case 'D':
                {
                    write_ok(own_buf);
                    remote_close(obj);

                    break;
                }
                case 'k':
                {
                    remote_close(obj);
                    /* Let the thread return if remote is stoped */
                    // pthread_exit(0);
                    DBG_RDI("In %s line:%d skyeye exit.\n", __func__, __LINE__);
                    break;
                }
                default:
                {
                    /* It is a request we don't understand.  Respond with an
                     * empty packet so that gdb knows that we don't support this
                     * request.  */
                    DBG_RDI("unknown command: %c\n", ch);
                    own_buf[0] = '\0';
                    break;
                }
            }

            putpkt(obj, own_buf);

            if (status == 'W')
            {
                DBG_RDI("\nChild exited with status %d\n", sig);
            }
            if (status == 'X')
            {
                DBG_RDI("\nChild terminated with signal = 0x%x\n", sig);
            }
            if (status == 'W' || status == 'X')
            {
                if (dev->extended_protocol)
                {
                    // chy 2005-07-30
                    DBG_RDI("\nCHY SkyEye: not Killing inferior\n");
                    // kill_inferior ();
                    write_ok(own_buf);
                    DBG_RDI("\nGDBserver restarting\n");

                    /* Wait till we are at 1st instruction in prog.  */
                    // chy 20050729 go to restart
                    // signal = start_inferior (&argv[2], &status);
                    goto restart;
                    break;
                } else
                {
                    DBG_RDI("\nGDBserver exiting\n");
                }
            }
        }

        /* We come here when getpkt fails.
         *
         * For the extended remote protocol we exit (and this is the only
         * For the traditional remote protocol close the connection,
         * and re-open it at the top of the loop.  */
        if (dev->extended_protocol)
        {
            DBG_RDI("In %s line:%d skyeye_exit.\n", __func__, __LINE__);
            remote_close(obj);
        } else
        {
            DBG_RDI("Remote side has terminated connection.  GDBserver will reopen the connection.\n");
            remote_close(obj);
        }
    }
}

void print_supportlist()
{
    int i;

    printf("gdb server support arch list:\n");
    for (i = 0; ArchTable[i][0] != NULL; i++)
        printf("\t%s\n", ArchTable[i][0]);
}

char *parse_archname(char *cpucls)
{
    int i;

    for (i = 0; ArchTable[i][0] != NULL; i++)
    {
        if (strncmp(ArchTable[i][0], cpucls, strlen(cpucls)) == 0)
            return ArchTable[i][1];
    }
    return NULL;
}

int gdb_server_is_connected(conf_object_t *obj)
{
    gdbserver_device *dev = obj->obj;

    if (dev->client_ip == NULL)
        return 0;
    else
        return 1;
}

char *gdb_server_get_client_ip(conf_object_t *obj)
{
    gdbserver_device *dev = obj->obj;

    return dev->client_ip;
}

int gdb_server_start(conf_object_t *obj, char *cpuname, int port, char *ip)
{
    gdbserver_device *dev = obj->obj;
    conf_object_t *cpu = get_conf_obj(cpuname);

    dev->target_cpu = cpu;
    int i;
    char *archname = NULL;

    if (cpu == NULL)
        return GDB_SERVER_ERROR;
    archname = parse_archname(cpu->class_name);
    if (archname == NULL)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not parese the archname for CPU class\n", cpu->class_name);
        return GDB_SERVER_ERROR;
    }

    for (i = 0; i < MAX_SUPP_ARCH; i++)
    {
        if (register_types[i] != NULL)
            if (!strcmp(archname, register_types[i]->name))
                dev->current_reg_type = register_types[i];
    }

    if (!dev->current_reg_type)
    {
        skyeye_log(Error_log, __FUNCTION__, "Can not parese the archname for CPU class:%s.\n", cpu->class_name);
        print_supportlist();
        return GDB_SERVER_ERROR;
    }
    dev->ip = skyeye_mm(strlen(ip) + 1);
    strncpy(dev->ip, ip, strlen(ip) + 1);
    dev->port = port;

    create_thread(sim_debug, obj, &(dev->remote_thread_id));

    dev->SIM_remote_stopped = False;

    return GDB_SERVER_SUCCESS;
}

static conf_object_t *gdbserver_new(char *obj_name)
{
    gdbserver_device *dev = (gdbserver_device *)skyeye_mm_zero(sizeof(gdbserver_device));

    dev->obj = new_conf_object(obj_name, dev);

    dev->SIM_remote_stopped = True;
    dev->client_ip = NULL;
    return dev->obj;
}

static exception_t gdbserver_free(conf_object_t *obj)
{
    gdbserver_device *dev = obj->obj;

    close_remote_gdb(obj);
    skyeye_free(dev->client_ip);
    skyeye_free(dev->ip);
    skyeye_free(dev);
    return No_exp;
}

static exception_t gdbserver_reset(conf_object_t *obj)
{
    gdbserver_device *dev = obj->obj;

    dev->client_ip = NULL;
    return No_exp;
}

static exception_t gdbserver_set_attr(conf_object_t *obj, const char *attr_name, attr_value_t *value)
{
    gdbserver_device *dev = obj->obj;

    return No_exp;
}

static attr_value_t *gdbserver_get_attr(const char *attr_name, conf_object_t *obj)
{
    return NULL;
}

void gdbserver_register_interface(conf_class_t *clss)
{
    static const skyeye_gdbserver_intf gdbserver_inface = {
        .GdbServerStart = gdb_server_start,
        .GdbServerIsConnected = gdb_server_is_connected,
        .GdbServerGetClientIp = gdb_server_get_client_ip,
    };
    SKY_register_iface(clss, SKYEYE_GDBSERVER_INTF_NAME, &gdbserver_inface);

    static const skyeye_debug_intf debug_iface = {
        .set_skyeye_debug = set_skyeye_debug,
    };
    SKY_register_iface(clss, SKYEYE_DEBUG_INTF, &debug_iface);

    static const struct InterfaceDescription ifaces[] = {
        (struct InterfaceDescription) {
            .name = SKYEYE_GDBSERVER_INTF_NAME,
            .iface = &gdbserver_inface,
        },
        (struct InterfaceDescription) {
            .name = SKYEYE_DEBUG_INTF,
            .iface = &debug_iface,
        }
    };
    class_register_ifaces(clss, ifaces);

    return;
}

void init_gdbserver()
{
    static skyeye_class_t class_data = {
        .class_name = "gdbserver",
        .class_desc = "gdbserver",
        .new_instance = gdbserver_new,
        .free_instance = gdbserver_free,
        .reset_instance = gdbserver_reset,
        .get_attr = gdbserver_get_attr,
        .set_attr = gdbserver_set_attr
    };
    conf_class_t *clss = SKY_register_device_class(class_data.class_name, &class_data);

    gdbserver_register_interface(clss);
}
