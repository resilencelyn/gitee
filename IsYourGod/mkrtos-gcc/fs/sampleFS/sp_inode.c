
#include <mkrtos/fs.h>
#include <mkrtos/bk.h>
#include <mkrtos/mem.h>
#include <mkrtos/sp.h>
#include <string.h>
#include "mkrtos/debug.h"

struct inode * sp_alloc_inode(struct inode * p_inode){
    struct sp_inode* sp_ino;
    p_inode->i_fs_priv_info= OSMalloc(sizeof(struct sp_inode));
    if(p_inode->i_fs_priv_info==NULL){
        return NULL;
    }
    sp_ino=p_inode->i_fs_priv_info;
    p_inode->i_ops=&sp_dir_inode_operations;
    memset(sp_ino,0,sizeof(struct sp_inode));

    return p_inode;
}

int32_t sp_free_inode(struct inode *p_inode){
//    if(p_inode->i_hlink){
//        return -1;
//    }
    if(atomic_read(&p_inode->i_used_count)){
        return -1;
    }
    OSFree(p_inode->i_fs_priv_info);
    return 0;
}

/**
 * ����һ���ĵ�inode
 * @param p_inode
 * @return
 */
struct inode* sp_new_inode(struct inode* p_inode){
    ino_t res_ino;
    struct inode* new_inode;

    if(alloc_inode_no(p_inode->i_sb,&res_ino)<0){
        return NULL;
    }
    new_inode=get_empty_inode();
    new_inode->i_no=res_ino;
    new_inode->i_hlink=1;
    new_inode->i_file_size=0;
    new_inode->i_sb=p_inode->i_sb;
    new_inode->i_dirt=0;
    new_inode->i_ops=NULL;
    atomic_set(&(new_inode->i_used_count),1);
    atomic_set(&(new_inode->i_lock),0);
    if(sp_alloc_inode((new_inode))==NULL){
        free_inode_no(p_inode->i_sb,res_ino);
        puti(new_inode);
        return NULL;
    }
    return new_inode;
}

/**
 * ��ȡһ��inode
 * @param inode
 * @return
 */
int32_t sp_read_inode(struct inode * inode){
    uint32_t ino;
    struct super_block *sb=inode->i_sb;
    struct sp_super_block * sp_sb=sb->s_sb_priv_info;
    //��Ҫ��ȡ�Ŀ��
    uint32_t ndReadBkInx;
    //inode�ڿ���ƫ��
    uint32_t ndReadBkInInx;
    ino=inode->i_no;
    //���Inode�Ƿ�ʹ��
    if (check_inode_no(sb, ino) != 1) {
        return -1;
    }

    uint8_t w_ch[128]={0};

    ndReadBkInx = (ino * sb->s_inode_size) / sb->s_bk_size + sp_sb->iNodeDataBkStInx;
    ndReadBkInInx = (ino) % (sb->s_bk_size / sb->s_inode_size);
    if (rbk(sb->s_dev_no,
            ndReadBkInx,
            w_ch,
            ndReadBkInInx * sb->s_inode_size,
            sb->s_inode_size) <0
            ) {
       while(1);
    }

    memcpy(inode->i_fs_priv_info,w_ch,sizeof(struct sp_inode));

    struct inode* temp=(struct inode*)(w_ch+sizeof(struct sp_inode));

    //�ļ�������Ȩ��
    inode->i_type_mode = temp->i_type_mode;
    //�ļ���С
    inode->i_file_size = temp->i_file_size;
    //�Լ���Inode����
    inode-> i_no= temp->i_no;
    //Ӳ������
    inode-> i_hlink=temp->i_hlink;
    inode->i_rdev_no=temp->i_rdev_no;
    switch(FILE_TYPE(inode->i_type_mode)){
        case 0:
            //��ͨ�ļ�
            inode->i_ops=&sp_file_inode_operations;
            break;
        case 1:
            //Ŀ¼�ļ�
            inode->i_ops=&sp_dir_inode_operations;
            break;
        case 2://�ַ��豸
            inode->i_ops=&chrdev_inode_operations;
            break;
        case 3://���豸
            inode->i_ops=&blkdev_inode_operations;
            break;
    }

    //�Ƿ��޸Ĺ�
    inode-> i_dirt=0;
    return 0;
}

int sp_notify_change(int flags, struct inode * i_node){

    return -ENOSYS;
}
/**
 * ��inodeд�ش���
 * @param i_node
 * @return none
 */
void sp_write_inode (struct inode * i_node){
    struct super_block *sb=i_node->i_sb;
    struct sp_super_block * sp_sb=sb->s_sb_priv_info;
    if(!i_node->i_dirt){
        return ;
    }
    //дInode
    uint32_t bk_inx = ROUND_DOWN(i_node->i_no, INODE_NUM_IN_BK(sb));

    uint8_t w_ch[128]={0};

    memcpy(w_ch,i_node->i_fs_priv_info,sizeof(struct sp_inode));

    struct inode* temp=(struct inode*)(w_ch+sizeof(struct sp_inode));
    //�ļ�������Ȩ��
    temp->i_type_mode = i_node->i_type_mode;
    //�ļ���С
    temp->i_file_size = i_node->i_file_size;
    //�Լ���Inode����
    temp-> i_no= i_node->i_no;
    //Ӳ������
    temp-> i_hlink=i_node->i_hlink;
    //�豸��
    temp->i_rdev_no=i_node->i_rdev_no;

    //��д
    if (wbk(sb->s_dev_no
            , sp_sb->iNodeDataBkStInx + bk_inx
            , w_ch
            , sb->s_inode_size * (i_node->i_no % INODE_NUM_IN_BK(sb))
            , sb->s_inode_size) <0) {
        return ;
    }
    i_node->i_dirt=0;
}
//ɾ�����inode���ͷ���ռ�õĿռ䣬
void sp_put_inode(struct inode * i_node){
    if(i_node->i_hlink>0){
        return ;
    }
    //�h���ļ�ռ�õ�bk
    sp_truncate(i_node,0);
    //�ͷ�ռ��inode
    free_inode_no(i_node->i_sb,i_node->i_no);
    DEBUG("fs",INFO,"ʣ�����:%d.",get_free_bk(i_node->i_sb));
    DEBUG("fs",INFO,"ʣ��inode:%d.",get_free_inode(i_node->i_sb));
}
//�ͷų�����
void sp_put_super(struct super_block * sb){
    //
}
/**
 * ��ȡָ���豸��sb
 * @param dev_no
 * @return
 */
struct super_block* sp_read_sb(struct super_block* sb){

    uint8_t *data;
    struct super_block* sb_temp;

    again:

    data = OSMalloc(sizeof(struct super_block)+sizeof(struct sp_super_block));
    if(data==NULL){
        goto again;
    }
    sb_temp = (uint8_t*)data;
    if (rbk(sb->s_dev_no,
            1,
            (uint8_t*)data,
            0,
            sizeof(struct super_block)) < 0) {
        OSFree(data);
        return NULL;
    }

    //��
    if(sb_temp->s_magic_num!=SP_MAGIC_NUM){
        OSFree(data);
        return NULL;
    }

    if (rbk(sb->s_dev_no,
            1,
            (uint8_t*)(data) + sizeof(struct super_block),
            sizeof(struct super_block),
            sizeof(struct sp_super_block)) < 0) {
        OSFree(data);
        return NULL;
    }

    //ħ��
    sb->s_magic_num=sb_temp->s_magic_num;
    //���С
    sb->s_bk_size=sb_temp->s_bk_size;
    //���豸���豸��
    sb->s_dev_no=sb_temp->s_dev_no;
    //inodeд�뵽���̵Ĵ�С
    sb->s_inode_size=sb_temp->s_inode_size;
    //super����������������ʱ���գ�Ӧ������
    sb->s_ops=&sp_s_ops;
    //�ļ�ϵͳ��ţ�ͨ���ñ�Ų��ҵ���Ӧ���ļ�ϵͳ
    sb->s_fs_no=0;
    //�Ƿ��޸Ĺ�
    sb->s_dirt=0;
    //�ļ�ϵͳӵ�е�˽����Ϣ
    memcpy(sb->s_sb_priv_info,(uint8_t*)(data)+sizeof(struct super_block),sizeof(struct sp_super_block));

    sb->root_inode=geti(sb,0);
    sb->root_inode->i_ops=&sp_dir_inode_operations;

    OSFree(data);
    return sb;
}
void sp_write_super (struct super_block * sb){
    struct bk_cache* bk_tmp;
    if(!sb->s_dirt){
        return ;
    }
    bk_tmp=bk_read(sb->s_dev_no,1,1,1);
    memcpy(bk_tmp->cache,sb,sizeof(struct super_block));
//    wbk(sb->s_dev_no,1,sb,0,sizeof(struct super_block));
    memcpy(bk_tmp->cache+sizeof(struct super_block),sb->s_sb_priv_info,sizeof(struct sp_super_block));
//    wbk(sb->s_dev_no,1,sb->s_sb_priv_info,sizeof(struct super_block),sizeof(struct sp_super_block));
    sb->s_dirt=0;
    bk_release(bk_tmp);
}
int sp_sync_inode(struct inode * inode)
{
    lock_inode(inode);
    sp_write_inode(inode);
    unlock_inode(inode);
    return 0;
}

void sp_statfs (struct super_block * sb, struct statfs * s_fs){

}
int sp_remount_fs(struct super_block * sb, int * a, char * b){

    return -ENOSYS;
}

struct super_operations sp_s_ops={
    //����ָ���ļ�ϵͳ��inodeռ�õ��ڴ�
    .alloc_inode=sp_alloc_inode,
    //�ͷ�ָ���ļ�ϵͳ��inodeռ�õ��ڴ�
    .free_inode=sp_free_inode,
    .read_inode=sp_read_inode,
    .notify_change=NULL,
    .write_inode=sp_write_inode,
    .put_inode=sp_put_inode,
    .put_super=NULL,
    .write_super=sp_write_super,
    .statfs=NULL,
    .remount_fs=NULL
};