//
// Created by Administrator on 2022/1/18.
//
#include <type.h>
#include <mkrtos/fs.h>
#include <mkrtos/sp.h>
#include <mkrtos/bk.h>
#include <fcntl.h>
#include <string.h>
#include <mkrtos/stat.h>
#include "mkrtos/debug.h"

int sp_file_read(struct inode * inode, struct file * filp, char * buf, int count){
    uint32_t fileUsedBKNum;
    uint32_t rLen = 0;
    int32_t r_inx = 0;
    struct super_block *sb;
    struct sp_inode *sp_ino;
    //����һ��ʱ���Ĵ�С
    uint32_t bkRemainSize;
    //ƫ���ڿ��п�ʼ��λ��
    uint32_t bInx;
    //ƫ�ƿ�ʼ��λ��
    uint32_t ofstBkInx;
    if (count == 0) {
        return 0;
    }
    r_inx=filp->f_ofs;
    if (r_inx >= inode->i_file_size) {
        return -1;
    }
    sb = inode->i_sb;
    sp_ino = (struct sp_inode *)(inode->i_fs_priv_info);

    ofstBkInx= ROUND_DOWN(r_inx, sb->s_bk_size);
    bInx= r_inx % sb->s_bk_size;
    bkRemainSize= sb->s_bk_size - bInx;
    fileUsedBKNum = FILE_USED_BK_NUM(sb, inode);

    for (uint32_t i = ofstBkInx; i < fileUsedBKNum && rLen<count && (rLen+r_inx)< inode->i_file_size; i++) {
        uint32_t remainSize = count - rLen;
        uint32_t rSize = 0;
        uint32_t readFileRamainSize;
        readFileRamainSize = inode->i_file_size - (rLen + r_inx);
        rSize = MIN(remainSize, sb->s_bk_size);
        rSize = MIN(rSize, readFileRamainSize);
        if (i == ofstBkInx) {
            rSize = MIN(rSize, bkRemainSize);
        }
        else {
            bInx = 0;
            bkRemainSize = sb->s_bk_size;
        }
        if (i < A_BK_NUM(sp_ino)) {
            struct bk_cache *tmp;
            tmp= bk_read(sb->s_dev_no,sp_ino->p_ino[i],0,0);
            memcpy(buf+rLen,tmp->cache+bInx,rSize);
            rLen += rSize;
            filp->f_ofs+=rSize;
            bk_release(tmp);
        }
        else if (i < A_BK_NUM(sp_ino) + B_BK_NUM(sb, sp_ino)) {

            uint32_t overNum = i - A_BK_NUM(sp_ino);
            uint32_t bkNo = overNum / BK_INC_X_NUM(sb);
            uint32_t bkInx = overNum % BK_INC_X_NUM(sb);
            uint32_t readBkInx;
            struct bk_cache *tmp;
            struct bk_cache *tmp1;

            tmp= bk_read(sb->s_dev_no,sp_ino->pp_ino[bkNo],0,0);
            memcpy((uint8_t*)(&readBkInx),tmp->cache+bkInx*sizeof(uint32_t),sizeof(uint32_t));
            bk_release(tmp);

            tmp1= bk_read(sb->s_dev_no,readBkInx,0,0);
            memcpy(buf + rLen,tmp1->cache+bInx,rSize);
            rLen += rSize;
            filp->f_ofs+=rSize;
            bk_release(tmp1);


        }
        else if (i < A_BK_NUM(sp_ino) + B_BK_NUM(sb, sp_ino) + C_BK_NUM(sb, sp_ino)) {
            struct bk_cache *tmp;
            int a,b;
            //������С
            uint32_t overANum = i - A_BK_NUM(sp_ino)-B_BK_NUM(sb, sp_ino);
            //�ڳ������е�ƫ��
            uint32_t pFileBksInx = overANum/(BK_INC_X_NUM(sb)*BK_INC_X_NUM(sb));
            //�������ƫ�ƣ����ڳ������ִ�С������
            uint32_t ppFileBksInx = (overANum%(BK_INC_X_NUM(sb)*BK_INC_X_NUM(sb)))/BK_INC_X_NUM(sb);
            uint32_t pppFileBksInx= overANum%BK_INC_X_NUM(sb);

            tmp= bk_read(sb->s_dev_no,sp_ino->ppp_ino[pFileBksInx],0,0);
            memcpy((uint8_t*)(&a),tmp->cache+(ppFileBksInx<<2),sizeof(uint32_t));
            bk_release(tmp);

            tmp= bk_read(sb->s_dev_no,a,0,0);
            memcpy(&b,tmp->cache+(pppFileBksInx<<2),sizeof(uint32_t));
            bk_release(tmp);

            tmp= bk_read(sb->s_dev_no,b,0,0);
            memcpy(buf + rLen,tmp->cache+bInx,rSize);
            rLen += rSize;
            filp->f_ofs+=rSize;
            bk_release(tmp);
        }
        else {
            return -1;
        }
    }

    return rLen;
}
/**
 * ��ȡָ��ƫ�����ڵĿ��
 * @param inode
 * @param offset
 * @param fpBkNum
 * @return
 */
int32_t get_ofs_bk_no(struct inode* inode, uint32_t offset,uint32_t* fpBkNum) {
    struct super_block *sb;
    struct sp_inode *sp_ino;
    if(inode==NULL){
        return -1;
    }

    sb=inode->i_sb;
    sp_ino=(struct sp_inode *)(inode->i_fs_priv_info);
    if(offset > inode->i_file_size){
        return -1;
    }
    uint32_t usedBkNum = ROUND_UP(offset, sb->s_bk_size)-1;
    if (usedBkNum < A_BK_NUM(sp_ino)) {
        *fpBkNum = sp_ino->p_ino[usedBkNum ];
        return 0;
    }
    else if (usedBkNum < A_BK_NUM(sp_ino) + B_BK_NUM(sb, sp_ino)) {
        //����A���ֵĴ�С
        uint32_t overANum = usedBkNum - A_BK_NUM(sp_ino);
        //������С
        uint32_t pFileBksInx = overANum / BK_INC_X_NUM(sb);
        uint32_t pFileBksi = overANum % BK_INC_X_NUM(sb);
        uint32_t bkNum;

        struct bk_cache* tmp;
        tmp=bk_read(sb->s_dev_no,sp_ino->pp_ino[pFileBksInx],0,0);
        memcpy((uint8_t*)(&bkNum), tmp->cache+pFileBksi*sizeof(uint32_t),sizeof(uint32_t));
        bk_release(tmp);
        *fpBkNum = bkNum;
        return 0;
    }
    else if (usedBkNum < A_BK_NUM(sp_ino) + B_BK_NUM(sb, sp_ino) + C_BK_NUM(sb, sp_ino)) {
        struct bk_cache *tmp;
        //������С
        uint32_t overANum = usedBkNum - A_BK_NUM(sp_ino)-B_BK_NUM(sb, sp_ino);
        //�ڳ������е�ƫ��
        uint32_t pFileBksInx = overANum/(BK_INC_X_NUM(sb)*BK_INC_X_NUM(sb));
        //�������ƫ�ƣ����ڳ������ִ�С������
        uint32_t ppFileBksInx = (overANum%(BK_INC_X_NUM(sb)*BK_INC_X_NUM(sb)))/BK_INC_X_NUM(sb);
        uint32_t pppFileBksInx= overANum%BK_INC_X_NUM(sb);

        if(!sp_ino->ppp_ino[pFileBksInx]){
            return -ENOSPC;
        }
        int a;
        tmp=bk_read(sb->s_dev_no,sp_ino->ppp_ino[pFileBksInx],0,0);
        memset(&a,tmp->cache+(ppFileBksInx<<2),sizeof(a));
        bk_release(tmp);
        if(!a){
            return -ENOSPC;
        }
        int b;
        tmp=bk_read(sb->s_dev_no,a,0,0);
        memset(&b,tmp->cache+(pppFileBksInx<<2),sizeof(b));
        bk_release(tmp);
        if(!b){
            return -ENOSPC;
        }
        *fpBkNum = b;
        return 0;
    }
    else {
       return -1;
    }
    return -1;
}
int32_t inode_alloc_new_bk(struct inode* inode, uint32_t* newBkNum){
    struct super_block* sb;
    struct sp_inode *sp_ino;
    sb=inode->i_sb;
    sp_ino=(struct sp_inode *)(inode->i_fs_priv_info);
    uint32_t usedNewBkNum;
    if(inode->i_file_size==0){
        usedNewBkNum =0;
    }else{
        usedNewBkNum= FILE_USED_BK_NUM(sb, inode);
    }

    if (usedNewBkNum < A_BK_NUM(sp_ino)) {

            if(alloc_bk(sb,newBkNum)<0){
                return -1;
            }
            sp_ino->p_ino[usedNewBkNum] = *newBkNum;
            inode->i_dirt=1;

    }
    else if (usedNewBkNum < A_BK_NUM(sp_ino) + B_BK_NUM(sb, sp_ino)) {
        //����A���ֵĴ�С
        uint32_t overANum = usedNewBkNum - A_BK_NUM(sp_ino);
        //������С
        uint32_t pFileBksInx = overANum / BK_INC_X_NUM(sb);
        uint32_t pFileBksi = overANum % BK_INC_X_NUM(sb);
        struct bk_cache* tmp;

        uint32_t a;
        if (sp_ino->pp_ino[pFileBksInx] == 0) {
            if(alloc_bk(sb,&a)<0){
                return -1;
            }
            //ȫ������Ϊ0
            tmp=bk_read(sb->s_dev_no,a,1,1);
            memset(tmp->cache,0,sb->s_bk_size);
            bk_release(tmp);
            sp_ino->pp_ino[pFileBksInx] = a;
            inode->i_dirt=1;
        }
        uint32_t b;
        if(alloc_bk(sb,&b)<0){
            return -1;
        }
        tmp=bk_read(sb->s_dev_no,sp_ino->pp_ino[pFileBksInx],1,0);
        memcpy(tmp->cache+pFileBksi * 4, (uint8_t*)&b,sizeof(b));
        bk_release(tmp);
        *newBkNum = b;
    }
    else if (usedNewBkNum < A_BK_NUM(sp_ino) + B_BK_NUM(sb, sp_ino) + C_BK_NUM(sb, sp_ino)) {
        //������С
        uint32_t overANum = usedNewBkNum - A_BK_NUM(sp_ino)-B_BK_NUM(sb, sp_ino);
        //�ڳ������е�ƫ��
        uint32_t pFileBksInx = overANum/(BK_INC_X_NUM(sb)*BK_INC_X_NUM(sb));
        //�������ƫ�ƣ����ڳ������ִ�С������
        uint32_t ppFileBksInx = (overANum%(BK_INC_X_NUM(sb)*BK_INC_X_NUM(sb)))/BK_INC_X_NUM(sb);
        uint32_t pppFileBksInx= overANum%BK_INC_X_NUM(sb);
        uint32_t a;
        struct bk_cache* tmp;
        if(!sp_ino->ppp_ino[pFileBksInx]){
            if(alloc_bk(sb,&a)<0){
                return -1;
            }
            //ȫ������Ϊ0
            tmp=bk_read(sb->s_dev_no,a,1,1);
            memset(tmp->cache,0,sb->s_bk_size);
            bk_release(tmp);
            sp_ino->ppp_ino[pFileBksInx] = a;
            inode->i_dirt=1;
        }
        //�õ�������ֵ
        uint32_t b;
        tmp=bk_read(sb->s_dev_no,sp_ino->ppp_ino[pFileBksInx],1,0);
        memcpy(&b, tmp->cache+ppFileBksInx * 4,sizeof(b));
        if(!b) {
            if (alloc_bk(sb, &b) < 0) {
                sp_ino->ppp_ino[pFileBksInx] = 0;
                free_bk(sb, a);
                bk_release(tmp);
                return -1;
            }
            memcpy( tmp->cache+ppFileBksInx * 4,&b,sizeof(b));
            bk_release(tmp);
            //ȫ������Ϊ0
            tmp=bk_read(sb->s_dev_no,b,1,1);
            memset(tmp->cache,0,sb->s_bk_size);
            inode->i_dirt=1;
        }else{
            bk_release(tmp);
            tmp=bk_read(sb->s_dev_no,b,1,0);
        }
//        bk_release(tmp);
        uint32_t c;
        if(alloc_bk(sb,&c)<0){
            sp_ino->ppp_ino[pFileBksInx]=0;

            free_bk(sb,a);
            free_bk(sb,b);
            return -1;
        }
//        tmp=bk_read(sb->s_dev_no,b,1);
        memcpy(tmp->cache+pppFileBksInx * 4, (uint8_t*)&c,sizeof(c));
        bk_release(tmp);
        *newBkNum=c;
        return 0;
    }
    else {
       return -1;
    }

    return 0;
}
int sp_file_write(struct inode * inode, struct file * filp, char * buf, int count){
    uint32_t usedBkNum;
    uint32_t upSize;
    uint32_t wSize = 0;
    uint8_t flag = 0;
    uint32_t wLen = 0;
    //д���ƫ��λ��
    uint32_t wOffset = 0;
    struct super_block *sb;
    struct bk_cache* tmp=NULL;
    if(!S_ISREG(inode->i_type_mode)){
        return -EISDIR;
    }

    if (count == 0) {
        return 0;
    }
    if(filp->f_flags&O_APPEND){
        filp->f_ofs=inode->i_file_size;
    }
    wOffset = filp->f_ofs;
    //д���ƫ�Ƴ������ļ���С
    if(wOffset > inode->i_file_size){
        return -1;
    }

    sb=inode->i_sb;
    //�ļ�ռ�ö��ٿ�
    usedBkNum = ROUND_UP(wOffset, sb->s_bk_size);

    //ʹ�ÿ��ܹ����ڴ�
    upSize = usedBkNum * sb->s_bk_size;
    //д������
    while (wLen < count) {

        if (flag == 0) {//��д���һ��
            if (upSize > wOffset && upSize != wOffset) {//������ȣ����˵���պ�д����һ��
                //д�����һ��ʣ��Ŀռ�
                uint32_t last_bk_no;
                if (get_ofs_bk_no(inode,wOffset, &last_bk_no) < 0) {
                    return -1;
                }
                wSize = count > (sb->s_bk_size - (wOffset % sb->s_bk_size))
                        ? (sb->s_bk_size - (wOffset % sb->s_bk_size)) : count;

                tmp=bk_read(sb->s_dev_no,last_bk_no,1,0);
                memcpy(tmp->cache+wOffset % sb->s_bk_size,buf+wLen,wSize);

                wLen += wSize;
                wOffset += wSize;

                if(wOffset > inode->i_file_size){
                    //�����ļ���С
                    inode->i_file_size = wOffset;
                    filp->f_ofs=wOffset;
                    inode->i_dirt = 1;
                }
                bk_release(tmp);
            }
            flag = 1;
        }
        else {
            //ʣ��Ĵ�С
            uint32_t remainSize;
            //������µĿ�
            uint32_t needWBk;
            if(wOffset >= inode->i_file_size){
                //���д��ƫ�ƴ��ڻ��ߵ����ļ���С������һ���µ�
                if (inode_alloc_new_bk(inode, &needWBk) < 0) {
                    return -ENOSPC;
                }
            }else{
                //�����ȡ��ǰƫ�ƵĿ��
                if (get_ofs_bk_no(inode,wOffset, &needWBk) < 0) {
                    return -1;
                }
            }
            remainSize = count - wLen;
            //���㻹��Ҫд�����
            wSize = remainSize > sb->s_bk_size ?  sb->s_bk_size : remainSize;
            tmp=bk_read(sb->s_dev_no,needWBk,1,wSize==sb->s_bk_size);
            memcpy(tmp->cache,buf+wLen,wSize);
            wLen += wSize;
            wOffset += wSize;
            if(wOffset > inode->i_file_size){
                //�����ļ���С
                inode->i_file_size = wOffset;
                filp->f_ofs=wOffset;
                inode->i_dirt = 1;
            }
            bk_release(tmp);
        }
    }
//    DEBUG("fs",INFO,"ʣ�����:%d.",get_free_bk(sb));
//    DEBUG("fs",INFO,"ʣ��inode:%d.",get_free_inode(sb));
    trace("file size %d\r\n",inode->i_file_size);
    return wLen;
}
