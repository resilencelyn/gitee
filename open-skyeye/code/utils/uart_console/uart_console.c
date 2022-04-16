/*
 * ========== Copyright Header Begin ==========================================
 * 
 * OpenSPARC T2 Processor File: netcons.c
 * Copyright (c) 2006 Sun Microsystems, Inc.  All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES.
 * 
 * The above named program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License version 2 as published by the Free Software Foundation.
 * 
 * The above named program is distributed in the hope that it will be 
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 * 
 * ========== Copyright Header End ============================================
 */
/*
 * Copyright 2006 Sun Microsystems, Inc.  All rights reserved.
 * Use is subject to license terms.
 */
#pragma ident	"@(#)netcons.c	1.2	06/08/25 SMI"

        /*
         * Simple program - essentially functions as telnet, but without
         * the NVT arbitration, or the annoying cooked terminal mode
         */
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <sim_control.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/termios.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <termio.h>
#include <unistd.h>

#ifndef __MINGW32__
#include <poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#else
#include <windows.h>
#include <winsock2.h>
#include <process.h>
#endif

#ifdef __svr4__
#include <sys/systeminfo.h>
#include <sys/stream.h>
#include <thread.h>
#else //no define __svr4__
#if !defined POLLRDNORM
const uint32_t POLLRDNORM = 0x040;
#endif
#if !defined POLLRDBAND
const uint32_t POLLRDBAND = 0x080;
#endif
#if !defined POLLWRNORM
const uint32_t POLLWRNORM = 0x100;
#endif
#if !defined POLLWRBAND
const uint32_t POLLWRBAND = 0x200;
#endif
#endif //__svr4__

static int setup_term();
static int setup_netlink(char *hostp, char *portp);

static void tcparams(int fd);
static void dump_buf(char *strp, uint8_t * bufp, int len);
static void poll_flags(char *strp, int flags);

/* redefine exit to be while (1) when debugging to read exit status on console */
#if 1                                   /* { */
#define	EXIT(_s)	exit(_s)
#else /* } */
#define	DBG(s)	do { } while (0)
#define	EXIT(_s)	while (1)
#endif

#ifdef __MINGW32__
DWORD WINAPI Monitor_socket(LPVOID fd_socket)
{
    int res;
    fd_set fdwrite;

    FD_ZERO(&fdwrite);
    FD_SET((int) fd_socket, &fdwrite);
    int fd_stdout = setup_term();

    uint8_t buf[1024];

    do
    {
        select(0, NULL, &fdwrite, NULL, NULL);
        if (FD_ISSET((int) fd_socket, &fdwrite))
        {
            res = recv((int) fd_socket, buf, 1024, 0);
            fflush(stdout);
            if (res == 0)
            {
                perror("recvout: ");
                EXIT(0);
            } else if (res < 0)
            {
                perror("recvout: ");
                exit(0);
            } else
            {
                write(fd_stdout, buf, res);
            }
        }
    } while (1);

    return 0;
}
#endif

int main(int argc, char **argv)
{
    uint8_t buf[1024];
    int i;

    for (i = 1; i < argc; i++)
    {
        if (argv[i][0] != '-')
            break;
        switch (argv[i][1])
        {
        }
    }

    if ((argc - i) != 2)
    {
        fprintf(stderr, "usage: %s <hostname> <port>\n", argv[0]);
        //while(1);
        EXIT(1);
    }
    char *hostp = argv[i];
    char *portp = argv[i + 1];

#ifndef __MINGW32__
    struct pollfd fds[2];

    fds[0].fd = setup_term();
    fds[1].fd = setup_netlink(hostp, portp);
    fds[0].events = POLLIN | POLLRDNORM;
    fds[1].events = POLLIN | POLLRDNORM;
#define	POLL_TIMEOUT	-1              /* wait forever ? FIXME ? */
    do
    {
        int res;
        int count;

        fds[0].revents = 0;
        fds[1].revents = 0;

        res = poll(fds, 2, POLL_TIMEOUT);

        DBG(poll_flags("ttyin:", fds[0].revents);
            tcparams(fds[0].fd);
            );
        DBG(poll_flags("sktin:", fds[1].revents);
            );

        if (fds[0].revents & POLLIN)
        {
            res = read(fds[0].fd, buf, 1024);
            if (res == 0)
            {
                /* if we read an EOF then the terminal went away
                 * so we close the socket and quit
                 */
                close(fds[1].fd);
                EXIT(0);
            } else if (res < 0)
            {
                perror("read");
            } else
            {
                DBG(dump_buf("ttyin:", buf, res);
                    );
                write(fds[1].fd, buf, res);
            }
        }

        if (fds[1].revents & POLLIN)
        {
            res = read(fds[1].fd, buf, 1024);
            if (res == 0)
            {
                /* if we read an EOF from the socket then
                 * the simulator went away - so simply quit
                 */
                EXIT(0);
            } else if (res < 0)
            {
                perror("read");
            } else
            {
                DBG(dump_buf("socketin:", buf, res);
                    );
                write(fds[0].fd, buf, res);
            }
        }

    } while (1);
#else
#include <conio.h>
    DWORD ThreadID;
    int fd_socket = setup_netlink(hostp, portp);
    HANDLE handle = CreateThread(NULL, 0, Monitor_socket, fd_socket, 0, &ThreadID);

    if (handle == 0)
        printf("Failed to create thread\n");
    char ch;

    while (1)
    {
        ch = getch();
        send(fd_socket, &ch, 1, 0);
    }
#endif
    return 0;
}

        /*
         * For some reason the old termio ioctl interface seems to be broken on S9
         * ... so we move over to the termios version .... why am I not surprised ?!
         */

int setup_term()
{
    struct termios tio;
    int fd;

    fd = 1;                             /* stdout */

    DBG(printf("\nstdin:\n");
        tcparams(fd);
        );

    if (tcgetattr(fd, &tio) < 0)
    {
        perror("setup_term: tcgetattr");
        EXIT(1);
    }

    tio.c_iflag |= IGNBRK | IGNPAR | ICRNL;
    tio.c_lflag &= ~ISIG & ~ICANON & ~ECHO & ~ECHOE & ~ECHOK & ~ECHONL & ~ECHOCTL & ~ECHOKE;
#ifndef __MINGW32__
    tio.c_iflag &= ~ECHOPRT;
#endif
    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 0;

    //if (tcsetattr(fd, TCSANOW, &tio)<0) {
    if (tcsetattr(fd, TCSADRAIN, &tio) < 0)
    {
        perror("setup_term: tcsetattr");
        EXIT(1);
    }

    DBG(tcparams(fd);
        printf("\nstdout:\n");
        tcparams(1);
        );

    return fd;
}

        /*--------------------------------------------------------------//
	//  	Connect to the simulator/socket interface		//
	//--------------------------------------------------------------*/

int setup_netlink(char *hostp, char *portp)
{
    int port;
    int num;

#ifndef __MINGW32__
    int skt;

#else
    int err;
    SOCKET skt;
    WSADATA wsaData;

    /*
     * initiates use of the Winsock DLL by a process
     * shenoubang modified it 2012-12-4
     */
    err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0)
    {
        /*
         * Tell the user that we could not find a usable
         * Winsock DLL.
         */
        printf("WSAStartup failed with error: %d\n", err);
        return 1;
    }
#endif

    printf("Connecting to %s:%s\n", hostp, portp);
    if (sscanf(portp, "%d", &port) != 1 || port < 1 || port > 65535)
    {
        fprintf(stderr, "Connection port must be a number from 1 to 65535");
        EXIT(1);
    }
    struct sockaddr_in target;

    target.sin_family = AF_INET;
#ifndef __MINGW32__
    target.sin_addr.s_addr = INADDR_ANY;
#else
    target.sin_addr.s_addr = inet_addr("127.0.0.1");
#endif
    target.sin_port = htons(port);

    for (num = 0; num < 3; num++)
    {

        skt = socket(AF_INET, SOCK_STREAM, 0);
        if (skt < 0)
        {
            perror("opening stream socket");
            EXIT(1);
        }
#ifndef __MINGW32__
        if (connect(skt, (struct sockaddr *) &target, sizeof (target)) != -1)
            goto gotit;
#else
        if (connect(skt, (SOCKADDR *) & target, sizeof (target)) != -1)
            goto gotit;
#endif
        printf("connecting error: %s\n", strerror(errno));
        fflush(stdout);
#ifndef __MINGW32__
        sleep(1);
#else
        Sleep(1);
#endif
        close(skt);
    }

    return -1;

  gotit:
    fflush(stdout);

    return skt;
}

#ifndef __MINGW32__
void tcparams(int fd)
{
    int mlines;
    struct termios tio;

#define	test(_b, _f)	if ((_b)&(_f)) printf("\t"#_f);

    mlines = 0;
    memset(&tio, 0, sizeof (tio));

    ioctl(fd, TIOCMGET, &mlines);
    if (tcgetattr(fd, &tio) < 0)
    {
        perror("tcgetattr");
        EXIT(1);
    }

    printf("modem lines : ");
    test(mlines, TIOCM_LE);
    test(mlines, TIOCM_DTR);
    test(mlines, TIOCM_RTS);
    test(mlines, TIOCM_ST);
    test(mlines, TIOCM_SR);
    test(mlines, TIOCM_CTS);
    test(mlines, TIOCM_CAR);
    test(mlines, TIOCM_RNG);
    test(mlines, TIOCM_DSR);
    printf("\n");

    printf("input modes: ");
    test(tio.c_iflag, IGNBRK);
    test(tio.c_iflag, BRKINT);
    test(tio.c_iflag, IGNPAR);
    test(tio.c_iflag, PARMRK);
    test(tio.c_iflag, INPCK);
    test(tio.c_iflag, ISTRIP);
    test(tio.c_iflag, INLCR);
    test(tio.c_iflag, IGNCR);
    test(tio.c_iflag, ICRNL);
    test(tio.c_iflag, IUCLC);
    test(tio.c_iflag, IXON);
    test(tio.c_iflag, IXANY);
    test(tio.c_iflag, IXOFF);
    test(tio.c_iflag, IMAXBEL);
    printf("\n");

    printf("output modes: ");
    test(tio.c_oflag, OPOST);
    test(tio.c_oflag, OLCUC);
    test(tio.c_oflag, ONLCR);
    test(tio.c_oflag, OCRNL);
    test(tio.c_oflag, ONOCR);
    test(tio.c_oflag, ONLRET);
    test(tio.c_oflag, OFILL);
    test(tio.c_oflag, OFDEL);
    printf("\n");

    /*      text(tio.c_cflag,  */

    printf("local flags: ");
    test(tio.c_lflag, ISIG);
    test(tio.c_lflag, ICANON);
    test(tio.c_lflag, XCASE);
    test(tio.c_lflag, ECHO);
    test(tio.c_lflag, ECHOE);
    test(tio.c_lflag, ECHOK);
    test(tio.c_lflag, ECHONL);
    test(tio.c_lflag, NOFLSH);
    test(tio.c_lflag, TOSTOP);
    test(tio.c_lflag, ECHOCTL);
    test(tio.c_lflag, ECHOPRT);
    test(tio.c_lflag, ECHOKE);
    test(tio.c_lflag, FLUSHO);
    test(tio.c_lflag, PENDIN);
    test(tio.c_lflag, IEXTEN);
    printf("\n");
}
#endif

#ifndef __MINGW32__
void poll_flags(char *strp, int flags)
{
    printf("%s", strp);

    test(flags, POLLIN) test(flags, POLLRDNORM);
    test(flags, POLLRDBAND);
    test(flags, POLLOUT);
    test(flags, POLLWRNORM);
    test(flags, POLLWRBAND);
    test(flags, POLLERR);
    test(flags, POLLHUP);
    test(flags, POLLNVAL);

    printf("\n");
    fflush(stdout);
}
#endif

void dump_buf(char *strp, uint8_t * bufp, int len)
{
    int i;

    printf("%s read %d bytes: ", strp, len);
    for (i = 0; i < len; i++)
        printf("0x%02x [%c]", bufp[i], bufp[i] >= 32 && bufp[i] < 127 ? bufp[i] : '.');
    printf("\n");
}
