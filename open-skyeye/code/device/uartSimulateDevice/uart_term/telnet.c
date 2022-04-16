/*
 * © Copyright 2013 eJim Lee. All Rights Reserved.
 * File:    telnet.c
 * Email:   lyjforjob@gmail.com
 * Date:    2013-12-01
 */


#include <stdio.h>
#include <unistd.h>
#include "telnet.h"

#define telnet_write(fd, buf, len)          \
    do                                      \
    {                                       \
        int t;                              \
        t = term_write((fd), (buf), (len)); \
        if (t < 0)                          \
        {                                   \
            perror("telnet_write");         \
        }                                   \
    } while (0)

#define TELCMDS(X)                                          \
    X(IAC,  255)    /* interpret as command: */             \
    X(DONT, 254)    /* you are not to use option */         \
    X(DO,   253)    /* please, you use option */            \
    X(WONT, 252)    /* I won't use option */                \
    X(WILL, 251)    /* I will use option */                 \
    X(SB,   250)    /* interpret as subnegotiation */       \
    X(SE,   240)    /* end sub negotiation */               \
    X(GA,   249)    /* you may reverse the line */          \
    X(EL,   248)    /* erase the current line */            \
    X(EC,   247)    /* erase the current character */       \
    X(AYT,  246)    /* are you there */                     \
    X(AO,   245)    /* abort output--but let prog finish */ \
    X(IP,   244)    /* interrupt process--permanently */    \
    X(BREAK,243)    /* break */                             \
    X(DM,   242)    /* data mark--for connect. cleaning */  \
    X(NOP,  241)    /* nop */                               \
    X(EOR,  239)    /* end of record (transparent mode) */  \
    X(ABORT,238)    /* Abort process */                     \
    X(SUSP, 237)    /* Suspend process */                   \
    X(xEOF, 236)    /* End of file: EOF is already used... */

#define TELOPTS(X)                                                         \
    X(BINARY, 0)          /* 8-bit data path */                            \
    X(ECHO, 1)            /* echo */                                       \
    X(RCP, 2)             /* prepare to reconnect */                       \
    X(SGA, 3)             /* suppress go ahead */                          \
    X(NAMS, 4)            /* approximate message size */                   \
    X(STATUS, 5)          /* give status */                                \
    X(TM, 6)              /* timing mark */                                \
    X(RCTE, 7)            /* remote controlled transmission and echo */    \
    X(NAOL, 8)            /* negotiate about output line width */          \
    X(NAOP, 9)            /* negotiate about output page size */           \
    X(NAOCRD, 10)         /* negotiate about CR disposition */             \
    X(NAOHTS, 11)         /* negotiate about horizontal tabstops */        \
    X(NAOHTD, 12)         /* negotiate about horizontal tab disposition */ \
    X(NAOFFD, 13)         /* negotiate about formfeed disposition */       \
    X(NAOVTS, 14)         /* negotiate about vertical tab stops */         \
    X(NAOVTD, 15)         /* negotiate about vertical tab disposition */   \
    X(NAOLFD, 16)         /* negotiate about output LF disposition */      \
    X(XASCII, 17)         /* extended ascic character set */               \
    X(LOGOUT, 18)         /* force logout */                               \
    X(BM, 19)             /* byte macro */                                 \
    X(DET, 20)            /* data entry terminal */                        \
    X(SUPDUP, 21)         /* supdup protocol */                            \
    X(SUPDUPOUTPUT, 22)   /* supdup output */                              \
    X(SNDLOC, 23)         /* send location */                              \
    X(TTYPE, 24)          /* terminal type */                              \
    X(EOR, 25)            /* end or record */                              \
    X(TUID, 26)           /* TACACS user identification */                 \
    X(OUTMRK, 27)         /* output marking */                             \
    X(TTYLOC, 28)         /* terminal location number */                   \
    X(3270REGIME, 29)     /* 3270 regime */                                \
    X(X3PAD, 30)          /* X.3 PAD */                                    \
    X(NAWS, 31)           /* window size */                                \
    X(TSPEED, 32)         /* terminal speed */                             \
    X(LFLOW, 33)          /* remote flow control */                        \
    X(LINEMODE, 34)       /* Linemode option */                            \
    X(XDISPLOC, 35)       /* X Display Location */                         \
    X(OLD_ENVIRON, 36)    /* Old - Environment variables */                \
    X(AUTHENTICATION, 37) /* Authenticate */                               \
    X(ENCRYPT, 38)        /* Encryption option */                          \
    X(NEW_ENVIRON, 39)    /* New - Environment variables */                \
    X(TN3270E, 40)        /* TN3270 enhancements */                        \
    X(XAUTH, 41)                                                           \
    X(CHARSET, 42)         /* Character set */                             \
    X(RSP, 43)             /* Remote serial port */                        \
    X(COM_PORT_OPTION, 44) /* Com port control */                          \
    X(SLE, 45)             /* Suppress local echo */                       \
    X(STARTTLS, 46)        /* Start TLS */                                 \
    X(KERMIT, 47)          /* Automatic Kermit file transfer */            \
    X(SEND_URL, 48)                                                        \
    X(FORWARD_X, 49)                                                       \
    X(PRAGMA_LOGON, 138)                                                   \
    X(SSPI_LOGON, 139)                                                     \
    X(PRAGMA_HEARTBEAT, 140)                                               \
    X(EXOPL, 255) /* extended-options-list */

#define telnet_enum(x, y) TELOPT_##x = y,
enum
{
    TELOPTS(telnet_enum) dummy = 0
};
#undef telnet_enum

#define telnet_enum(x, y) x = y,
enum
{
    TELCMDS(telnet_enum)
};
#undef telnet_enum

#define CR 13
#define LF 10
#define NUL 0

//#define TELNET_DEBUG
/* only for debug */
#ifdef TELNET_DEBUG

static char *telcmd(int cmd)
{
#define telnet_str(x, y) \
    case x:              \
        return #x;
    switch (cmd)
    {
        TELCMDS(telnet_str)
    default:
        return "<unknown>";
    }
#undef telnet_str
}

static char *telopt(int opt)
{
#define telnet_str(x, y) \
    case TELOPT_##x:     \
        return #x;
    switch (opt)
    {
        TELOPTS(telnet_str)
    default:
        return "<unknown>";
    }
#undef telnet_str
}

#define TDBG(FMT, ...)              \
    do                              \
    {                               \
        printf(FMT, ##__VA_ARGS__); \
    } while (0)

#else // TELNET_DEBUG

#define TDBG(FMT, ...) \
    do                 \
    {                  \
    } while (0)

#endif // TELNET_DEBUG

/* return commad length */
/* telnet command:
 * 1) (IAC|DO|TELOPT), (IAC|WONT|TELOPT) etc
 * 2) (IAC|GA), (IAC|EL) etc
 * 3) (IAC|SB|TTYPE){0}xterm(IAC|SE) etc
 */
static int cmd_process(telnet_fd fd, const unsigned char *cmd, int len)
{
    unsigned char tcmd[256] = {IAC};
#define SEND_CMD(c, op, n)             \
    do                                 \
    {                                  \
        tcmd[1] = c;                   \
        tcmd[2] = op;                  \
        telnet_write(fd, tcmd, 2 + n); \
    } while (0)
#define SEND_CMD_SB(op, n)             \
    do                                 \
    {                                  \
        tcmd[1] = SB;                  \
        tcmd[2] = op;                  \
        tcmd[3 + n] = IAC;             \
        tcmd[4 + n] = SE;              \
        telnet_write(fd, tcmd, 5 + n); \
    } while (0)

    TDBG("Command: (%s|%s|%s)\n", "IAC", telcmd(cmd[1]), telopt(cmd[2]));

    switch (cmd[1])
    {
    case DO: /* please, you use option */
        if (len < 3)
        {
            return 0;
        }
        if ((cmd[2] == TELOPT_ECHO) || (cmd[2] == TELOPT_SGA))
        {
            SEND_CMD(WILL, cmd[2], 1);
        }
        else
        {
            SEND_CMD(WONT, cmd[2], 1);
        }
        return 2;
    case WILL: /* I will use option */
        if (len < 3)
        {
            return 0;
        }
        if ((cmd[2] == TELOPT_ECHO) || (cmd[2] == TELOPT_SGA))
        {
            SEND_CMD(DO, cmd[2], 1);
        }
        else
        {
            SEND_CMD(DONT, cmd[2], 1);
        }
        return 2;
    case SB:
    {
        int j;
        for (j = 0; j < len && cmd[j] != SE; j++)
        {
            ;
        }
        if (j >= len)
        {
            return 0;
        }
        return j;
    }
    case IAC:
        /* if go here, maybe something wrong */
    case DONT:
    case WONT:
        if (len < 3)
        {
            return 0;
        }
        return 2;
    case SE:
    case GA:
    case EL:
    case EC:
    case AYT:
    case AO:
    case IP:
    case BREAK:
    case DM:
    case NOP:
    case EOR:
    case ABORT:
    case SUSP:
    case xEOF:
        if (len < 2)
        {
            return 0;
        }
        return 1;
    default:
        /* if go here, maybe something wrong */
        return 0;
    }
#undef SEND_CMD
#undef SEND_CMD_SB
}

int telnet_process(telnet_fd fd, void *str, int len)
{
    unsigned char *buf = str;
    int i, cmd_len;
    int idx; // for buffer after process

#ifdef TELNET_DEBUG
    TDBG("before telnet process: \n");
    for (i = 0; i < len; i++)
    {
        unsigned char c = buf[i];
        if (32 <= c && c < 128)
        {
            printf("%c", c);
        }
        else
        {
            printf("{%hhu}", c);
        }
    }
    TDBG("\n*********************\n");
#endif
    for (i = idx = 0; i < len; i++)
    {
        if (buf[i] == IAC)
        {
            cmd_len = cmd_process(fd, buf + i, len - i);
            if (cmd_len < 0)
            {
                fprintf(stderr, "Get a wrong telnet command.\n");
            }
            else
            {
                i += cmd_len;
            }
        }
        else if (buf[i] == CR && i + 1 < len && buf[i + 1] == LF)
        {
            buf[idx++] = LF;
            i += 1;
        }
        else
        {
            buf[idx++] = buf[i];
        }
    }
#ifdef TELNET_DEBUG
    TDBG("after telnet process: \n");
    for (i = 0; i < idx; i++)
    {
        unsigned char c = buf[i];
        if (32 <= c && c < 128)
        {
            printf("%c", c);
        }
        else
        {
            printf("{%hhu}", c);
        }
    }
    TDBG("\n=====================\n");
#endif
    return idx;
}

int telnet_out_process(telnet_fd fd, void *str, int len, unsigned char *lastchar)
{
    unsigned char *buf = str, c = CR;
    int i;

    for (i = 0; i < len; i++)
    {
        if (buf[i] == LF)
        {
            if (lastchar != CR)
            {
                telnet_write(fd, &c, 1);
            }
        }
        *lastchar = buf[i];
    }
    return len;
}
