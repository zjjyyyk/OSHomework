#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "os.h"

# define OS_FILENAME "os.dat"
# define OS_BITSIZE (1024*1024*100)
# define PIECE_BITSIZE (1024*512)


/////////////////////////////// 函数实现 //////////////////////////
void CreateOS() {
    FILE* fp = fopen(OS_FILENAME, "wb");
    if (fp == NULL) {
        printf("文件创建失败\n");
        return;
    }
    fseek(fp, OS_BITSIZE + 1, SEEK_SET);
    fputc(32, fp);
    rewind(fp);
    // 添加链表
    int i;
    node* temphead = (node*)malloc(sizeof(node));
    for (i = 0;i < OS_BITSIZE / PIECE_BITSIZE;i++) {
        printf("i=%d\n",i);
        temphead->llink = (i - 1) % (OS_BITSIZE / PIECE_BITSIZE);
        temphead->uplink = i;
        temphead->rlink = (i + 1) % (OS_BITSIZE / PIECE_BITSIZE);
        temphead->size = PIECE_BITSIZE;
        temphead->tag = 0;
        fwrite(temphead, sizeof(node), 1, fp);
        fseek(fp, PIECE_BITSIZE - sizeof(node), SEEK_CUR);
    }
    rewind(fp);
    fclose(fp);
}


node* getNode(FILE* fp, int n) {
    node* nd = (node*)malloc(sizeof(node));
    fseek(fp, n * PIECE_BITSIZE, SEEK_SET);
    fread(nd, sizeof(node), 1, fp);
    rewind(fp);
    return nd;
}


void writeNode(FILE* fp, int n, node* nd){
    fseek(fp,n * PIECE_BITSIZE,SEEK_SET);
    int ret = fwrite(nd,sizeof(node),1,fp);
    rewind(fp);
    fflush(fp);
}


int AllocBoundTag(int* pav, int n, FILE* fp) {
    int p, f;
    int _pav = *pav;
    node* nd = getNode(fp, *pav);
    //如果在遍历过程，当前空闲块的在存储容量比用户申请空间 n 值小，在该空闲块有右孩子的情况下直接跳过
    for (p = _pav; nd && nd->size < n && nd->rlink != (*pav); p = nd->rlink, nd = getNode(fp, p));
    //跳出循环，首先排除p为空和p指向的空闲块容量小于 n 的情况
    if (!nd || nd->size < n) {
        rewind(fp);
        if(!nd) printf("No free space to alloc\n");
        if(nd->size < n) printf("Too big to alloc: only %ld, need %ld\n",nd->size, n);
        printf("alloc failed\n");
        return -100;
    }
    else {
        //调整pav指针的位置，为下次分配做准备
        (*pav) = nd->rlink;
        if ((*pav) == p) {
            *pav = -1;
        }
        else {
            //全部分配用户，即从可利用空间表中删除 p 空闲块
            getNode(fp, *pav)->llink = nd->llink;
            getNode(fp, nd->llink)->rlink = *pav;
        }
        nd->tag = 1;
        writeNode(fp,_pav,nd);
        fflush(fp);
        rewind(fp);
        return p;
    }
}


int Recover(int* pav, int loc, FILE* fp) {
    int p = loc;
    node* nd = getNode(fp,p);
    if(nd->tag == 0) return p;
    nd->tag = 0;
    //如果pav指针不存在，证明可利用空间表为空，此时设置p为头指针，并重新建立双向循环链表
    if (*pav == -1) {
        *pav = nd->llink = nd->rlink = p;
    }
    else {
        //否则，在p空闲块插入到pav指向的空闲块的左侧
        int _pav = *pav;
        node* nd_pav = getNode(fp,*pav);
        int q = nd_pav->llink;
        node* nd_q = getNode(fp,q);
        nd->rlink = *pav;
        nd->llink = q;
        nd_q->rlink = nd_pav->llink = p;
        *pav = p;
        writeNode(fp,_pav,nd_pav);
        writeNode(fp,q,nd_q);
    }
    writeNode(fp,p,nd);
    rewind(fp);
    return p;
}


int findFirstFreeNode(FILE* fp) {
    int i = 0;
    node temphead;
    rewind(fp);
    for (i = 0;i < OS_BITSIZE/PIECE_BITSIZE;i++) {
        fread(&temphead, sizeof(node), 1, fp);
        if (temphead.tag == 0)
            return i;
        else
            fseek(fp, PIECE_BITSIZE - sizeof(node), SEEK_CUR);
    }
    rewind(fp);
    return -1;
}


void displayNodeTag(FILE* fp){
    fflush(fp);
    int i;
    node* temp = (node*)malloc(sizeof(node));
    rewind(fp);
    for(i=0;i<OS_BITSIZE/PIECE_BITSIZE;i++){
        fread(temp,sizeof(node),1,fp);
        printf("%d ",temp->tag);
        fseek(fp,PIECE_BITSIZE-sizeof(node),SEEK_CUR);
    }
    printf("\ndisplay over.\n");
    rewind(fp);
}

