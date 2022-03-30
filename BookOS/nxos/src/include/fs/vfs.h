/**
 * Copyright (c) 2018-2022, NXOS Development Team
 * SPDX-License-Identifier: Apache-2.0
 * 
 * Copyright(c) 2007-2022 Jianjun Jiang <8192542@qq.com>
 *          Official site: http://xboot.org
 *          Mobile phone: +86-18665388956
 *          QQ: 8192542
 * 
 * Contains: virtual file system form xboot.
 * 
 * Change Logs:
 * Date           Author            Notes
 * 2022-3-26      JasonHu           Port from xboot
 */

#ifndef __FS_VFS__
#define __FS_VFS__

#include <xbook.h>
#include <sched/mutex.h>
#include <xbook/atomic.h>
#include <utils/list.h>

#ifdef CONFIG_NX_VFS_MAX_PATH 
#define NX_VFS_MAX_PATH CONFIG_NX_VFS_MAX_PATH
#else
#define NX_VFS_MAX_PATH		    (1024)
#endif

#ifdef CONFIG_NX_VFS_MAX_NAME
#define NX_VFS_MAX_NAME CONFIG_NX_VFS_MAX_NAME
#else
#define NX_VFS_MAX_NAME		    (256)
#endif

#ifdef CONFIG_NX_VFS_MAX_FD
#define NX_VFS_MAX_FD CONFIG_NX_VFS_MAX_FD
#else
#define NX_VFS_MAX_FD		    (256)
#endif

#ifdef CONFIG_NX_VFS_NODE_HASH_SIZE
#define NX_VFS_NODE_HASH_SIZE CONFIG_NX_VFS_NODE_HASH_SIZE
#else
#define NX_VFS_NODE_HASH_SIZE   (256)
#endif

#define NX_VFS_O_RDONLY			(1 << 0)
#define NX_VFS_O_WRONLY			(1 << 1)
#define NX_VFS_O_RDWR			(NX_VFS_O_RDONLY | NX_VFS_O_WRONLY)
#define NX_VFS_O_ACCMODE		(NX_VFS_O_RDWR)

#define NX_VFS_O_CREAT			(1 << 8)
#define NX_VFS_O_EXCL			(1 << 9)
#define NX_VFS_O_NOCTTY			(1 << 10)
#define NX_VFS_O_TRUNC			(1 << 11)
#define NX_VFS_O_APPEND			(1 << 12)
#define NX_VFS_O_DSYNC		    (1 << 13)
#define NX_VFS_O_NONBLOCK		(1 << 14)
#define NX_VFS_O_SYNC			(1 << 15)

#define NX_VFS_S_IXOTH			(1 << 0)
#define NX_VFS_S_IWOTH			(1 << 1)
#define NX_VFS_S_IROTH			(1 << 2)
#define NX_VFS_S_IRWXO			(NX_VFS_S_IROTH | NX_VFS_S_IWOTH | NX_VFS_S_IXOTH)

#define NX_VFS_S_IXGRP			(1 << 3)
#define NX_VFS_S_IWGRP			(1 << 4)
#define NX_VFS_S_IRGRP			(1 << 5)
#define NX_VFS_S_IRWXG			(NX_VFS_S_IRGRP | NX_VFS_S_IWGRP | NX_VFS_S_IXGRP)

#define NX_VFS_S_IXUSR			(1 << 6)
#define NX_VFS_S_IWUSR			(1 << 7)
#define NX_VFS_S_IRUSR			(1 << 8)
#define NX_VFS_S_IRWXU			(NX_VFS_S_IRUSR | NX_VFS_S_IWUSR | NX_VFS_S_IXUSR)

#define	NX_VFS_S_IFDIR			(1 << 16)
#define	NX_VFS_S_IFCHR			(1 << 17)
#define	NX_VFS_S_IFBLK			(1 << 18)
#define	NX_VFS_S_IFREG			(1 << 19)
#define	NX_VFS_S_IFLNK			(1 << 20)
#define	NX_VFS_S_IFIFO		    (1 << 21)
#define	NX_VFS_S_IFSOCK			(1 << 22)
#define	NX_VFS_S_IFMT			(NX_VFS_S_IFDIR | NX_VFS_S_IFCHR | NX_VFS_S_IFBLK | NX_VFS_S_IFREG | NX_VFS_S_IFLNK | NX_VFS_S_IFIFO | NX_VFS_S_IFSOCK)

#define NX_VFS_S_ISDIR(m)		((m) & NX_VFS_S_IFDIR )
#define NX_VFS_S_ISCHR(m)		((m) & NX_VFS_S_IFCHR )
#define NX_VFS_S_ISBLK(m)		((m) & NX_VFS_S_IFBLK )
#define NX_VFS_S_ISREG(m)		((m) & NX_VFS_S_IFREG )
#define NX_VFS_S_ISLNK(m)		((m) & NX_VFS_S_IFLNK )
#define NX_VFS_S_ISFIFO(m)		((m) & NX_VFS_S_IFIFO )
#define NX_VFS_S_ISSOCK(m)		((m) & NX_VFS_S_IFSOCK )

#define	NX_VFS_R_OK				(1 << 2)
#define NX_VFS_W_OK				(1 << 1)
#define NX_VFS_X_OK				(1 << 0)

#define NX_VFS_SEEK_SET		    (0)
#define NX_VFS_SEEK_CUR		    (1)
#define NX_VFS_SEEK_END		    (2)

struct NX_VfsStatInfo;
struct NX_VfsDirent;
struct NX_VfsNode;
struct NX_VfsMount;
struct NX_VfsFileSystem;

typedef struct NX_VfsStatInfo
{
	NX_U64 ino;
	NX_I64 size;
	NX_U32 mode;
	NX_U64 dev;
	NX_U32 uid;
	NX_U32 gid;
	NX_U64 ctime;
	NX_U64 atime;
	NX_U64 mtime;
} NX_VfsStatInfo;

enum NX_VfsDirentTtpe
{
	NX_VFS_DIR_TYPE_UNK,
	NX_VFS_DIR_TYPE_DIR,
	NX_VFS_DIR_TYPE_CHR,
	NX_VFS_DIR_TYPE_BLK,
	NX_VFS_DIR_TYPE_REG,
	NX_VFS_DIR_TYPE_LNK,
	NX_VFS_DIR_TYPE_FIFO,
	NX_VFS_DIR_TYPE_SOCK,
};

typedef struct NX_VfsDirent
{
	NX_U64 off;
	NX_U32 reclen;
	enum NX_VfsDirentTtpe type;
	char name[NX_VFS_MAX_NAME];
} NX_VfsDirent;

enum NX_VfsNodeFlag
{
	NX_VFS_NODE_FLAG_NONE,
	NX_VFS_NODE_FLAG_ROOT,
};

enum NX_VfsNodeType
{
	NX_VFS_NODE_TYPE_UNK,
	NX_VFS_NODE_TYPE_DIR,
	NX_VFS_NODE_TYPE_CHR,
	NX_VFS_NODE_TYPE_BLK,
	NX_VFS_NODE_TYPE_REG,
	NX_VFS_NODE_TYPE_LNK,
	NX_VFS_NODE_TYPE_FIFO,
	NX_VFS_NODE_TYPE_SOCK,
};

typedef struct NX_VfsNode
{
	NX_List link;
	struct NX_VfsMount * mount;
	NX_Atomic refcnt;
	char path[NX_VFS_MAX_PATH];
	enum NX_VfsNodeFlag flags;
	enum NX_VfsNodeType type;
	NX_Mutex lock;
	NX_U64 ctime;
	NX_U64 atime;
	NX_U64 mtime;
	NX_U32 mode;
	NX_I64 size;
	void * data;
} NX_VfsNode;

typedef struct NX_VfsFile
{
	NX_Mutex lock;
	struct NX_VfsNode * node;
	NX_I64 offset;
	NX_U32 flags;
} NX_VfsFile;

typedef struct NX_VfsFileTable
{
	struct NX_VfsFile file[NX_VFS_MAX_FD];
    NX_Mutex file_lock;
} NX_VfsFileTable;

void NX_VfsFileTableInit(NX_VfsFileTable *ft);
NX_VfsFileTable *NX_VfsGetDefaultFileTable(void);

enum
{
	NX_VFS_MOUNT_RW	    = (0x0 << 0),
	NX_VFS_MOUNT_RO	    = (0x1 << 0),
	NX_VFS_MOUNT_MASK	= (0x1 << 0),
};

typedef struct NX_VfsMount
{
	NX_List link;
	struct NX_VfsFileSystem * fs;
	void * dev;
	char path[NX_VFS_MAX_PATH];
	NX_U32 flags;
	NX_Atomic refcnt;
	struct NX_VfsNode * root;
	struct NX_VfsNode * covered;
	NX_Mutex lock;
	void * data;
} NX_VfsMount;

typedef struct NX_VfsFileSystem
{
	NX_List list;
	const char * name;

	NX_Error (*mount)(NX_VfsMount *, const char *);
	NX_Error (*unmount)(NX_VfsMount *);
	NX_Error (*msync)(NX_VfsMount *);
	NX_Error (*vget)(NX_VfsMount *, NX_VfsNode *);
	NX_Error (*vput)(NX_VfsMount *, NX_VfsNode *);

	NX_U64 (*read)(NX_VfsNode *, NX_I64, void *, NX_U64, NX_Error *outErr);
	NX_U64 (*write)(NX_VfsNode *, NX_I64, void *, NX_U64, NX_Error *outErr);
	NX_Error (*truncate)(NX_VfsNode *, NX_I64);
	NX_Error (*sync)(NX_VfsNode *);
	NX_Error (*readdir)(NX_VfsNode *, NX_I64, NX_VfsDirent *);
	NX_Error (*lookup)(NX_VfsNode *, const char *, NX_VfsNode *);
	NX_Error (*create)(NX_VfsNode *, const char *, NX_U32);
	NX_Error (*remove)(NX_VfsNode *, NX_VfsNode *, const char *);
	NX_Error (*rename)(NX_VfsNode *, const char *, NX_VfsNode *, NX_VfsNode *, const char *);
	NX_Error (*mkdir)(NX_VfsNode *, const char *, NX_U32);
	NX_Error (*rmdir)(NX_VfsNode *, NX_VfsNode *, const char *);
	NX_Error (*chmod)(NX_VfsNode *, NX_U32);
} NX_VfsFileSystem;

NX_VfsFileSystem * NX_VfsSearchFileSystem(const char * name);
NX_Error NX_VfsRegisterFileSystem(NX_VfsFileSystem * fs);
NX_Error NX_VfsUnregisterFileSystem(NX_VfsFileSystem * fs);
NX_VfsMount * NX_VfsGetMount(int index);
int NX_VfsGetMountCount(void);

/* user interface */
NX_Error NX_VfsMountFileSystem(const char * dev, const char * dir, const char * fsname, NX_U32 flags);
NX_Error NX_VfsUnmountFileSystem(const char * path);
NX_Error NX_VfsSync(void);
int NX_VfsOpen(const char * path, NX_U32 flags, NX_U32 mode, NX_Error *outErr);
NX_Error NX_VfsClose(int fd);
NX_U64 NX_VfsRead(int fd, void * buf, NX_U64 len, NX_Error *outErr);
NX_U64 NX_VfsWrite(int fd, void * buf, NX_U64 len, NX_Error *outErr);
NX_I64 NX_VfsFileSeek(int fd, NX_I64 off, int whence, NX_Error *outErr);
NX_Error NX_VfsFileSync(int fd);
NX_Error NX_VfsFileChmod(int fd, NX_U32 mode);
NX_Error NX_VfsFileStat(int fd, NX_VfsStatInfo * st);
int NX_VfsOpenDir(const char * name, NX_Error *outErr);
NX_Error NX_VfsCloseDir(int fd);
NX_Error NX_VfsReadDir(int fd, NX_VfsDirent * dir);
NX_Error NX_VfsRewindDir(int fd);
NX_Error NX_VfsMakeDir(const char * path, NX_U32 mode);
NX_Error NX_VfsRemoveDir(const char * path);
NX_Error NX_VfsRename(const char * src, const char * dst);
NX_Error NX_VfsUnlink(const char * path);
NX_Error NX_VfsAccess(const char * path, NX_U32 mode);
NX_Error NX_VfsChmod(const char * path, NX_U32 mode);
NX_Error NX_VfsStat(const char * path, NX_VfsStatInfo * st);

#endif /* __FS_VFS__ */
