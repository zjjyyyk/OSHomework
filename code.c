#include <stdio.h>
#include <stdlib.h>

typedef enum status {false,true} status;
# define OS_FILENAME "os.dat"

//////////////////////////////// 数据结构 //////////////////////////
typedef struct WORD{
    union{
        struct WORD *llink;//指向直接前驱
        struct WORD *uplink;//指向结点本身
    };
    int tag;//标记域,0表示为空闲块；1表示为占用块
    int size;//记录内存块的存储大小
    struct WORD *rlink;//指向直接后继
}WORD,head,foot,*Space;


//////////////////////////////// 函数原型 //////////////////////////
//创建操作系统和内存节点链表
Space CreateOS();

// 解析用户输入
status CMD(Space, char*);

// 内存分配算法
Space AllocBoundTag(Space, int);

// 内存回收算法
Space Recover(Space, Space);

// 功能函数
Space FootLoc(Space);

/////////////////////////////// 函数实现 //////////////////////////
Space CreateOS(){
    FILE* fp = fopen(OS_FILENAME,"wb+");
    if(fp==NULL){
        printf("文件创建失败\n");
        return NULL;
    }
    fseek(fp, 100*1024*1024-1,SEEK_SET);
    fputc(621,fp);
    rewind(fp);
    Space head = (Space)malloc(sizeof(WORD));
    head->uplink = fp + 1024*64;  // 最多存储1024*128个内存碎片,需要1024*128*16Byte,即1024*64个&fp单位
    head->tag = 0;
    head->size = 1024*1024*99;  // 去掉1024*128个存储链表节点我们的操作系统一共有99M内存可以挥霍
    head->llink = head->rlink = NULL;
    fwrite(head,sizeof(WORD),1,fp);
    fclose(fp);
    return head;
}



/* -------------------- main ------------------------*/ 
int main(){
    FILE *fp;
    Space head;
    if ( (fp = fopen(OS_FILENAME, "r")) == NULL ) {
        head = CreateOS();
        fp = fopen(OS_FILENAME,'wb+');
    } else {
        fp = fopen(OS_FILENAME,'wb+');
        fread(head,sizeof(Space),1,fp);
    }
    char * commend;
    status flag = true;
    while(flag){
        gets(commend);
        flag = CMD(head, commend);
    }
    printf("成功退出操作系统");
    return 0;
}
