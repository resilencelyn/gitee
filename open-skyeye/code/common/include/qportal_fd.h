#ifndef __QPORTAL_FD_H__
#define __QPORTAL_FD_H__
#ifdef __cplusplus
extern "C" {
#endif
#if 0
}
#endif
typedef struct qportal_fdesc{
	uint8_t qportalDdLioLo;	/* Debug/LIODN bits 0-5 */
	uint8_t qportalBpid;	/* Buffer pool ID */
	uint8_t qportalLioHi;	/* LIODN bits 4-9 */
	uint8_t qportalAddrHi;	/* Buffer address high bits */
	uint32_t qportalAddrLo;	/* Buffer address low bits */
	uint32_t qportalOffLen;	/* Offset/length for stashing */
	uint32_t qportalCmd;	/* User command */
} QPORTAL_FDESC;

struct qportal_enqueue{
    uint8_t qportalVerb;	/* Command verb */
    uint8_t qportalDca;	/* Discrete consumption ack */
    uint16_t qportalSeqNum;	/* Order restoration seqnum */
    uint32_t qportalOrp;	/* Order restoration point ID */
    uint32_t qportalFqId;	/* Frame Queue ID */
    uint32_t qportalTag;	/* Enqueue command tag */
    QPORTAL_FDESC qportalFd;		/* Frame descriptor */
    uint32_t qportalRsvd[8];	
};

struct qportal_dequeue
{
	uint8_t	qportalVerb;	/* Dequeue verb */
	uint8_t	qportalStat;	/* Status */
	uint16_t	qportalSeqNum;	/* Order restore seqnum */
	uint8_t	qportalTok;	/* Dequeue command token */
	uint8_t	qportalRsvd0[3];
	uint32_t	qportalFqId;	/* Frame queue ID */
	uint32_t	qportalCntxB;	/* Queue context B */
	QPORTAL_FDESC	qportalFd;	/* Frame descriptor */
	uint32_t	qportalRsvd1[8];	
};

typedef struct qportal_fd_intf {
	conf_object_t *obj;
	QPORTAL_FDESC* (*get_fd)(conf_object_t *obj);
	int (*set_fd)(conf_object_t *obj, int size,uint32_t Lo_addr);
	int (*init_fd)(conf_object_t *obj, struct qportal_enqueue **en,struct qportal_dequeue **de);
} qportal_fd_intf;
#define QPORTAL_FDESC_INTF_NAME "qportal_fd_intf"

#endif