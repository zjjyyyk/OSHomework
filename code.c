#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum status {false,true} status;
# define OS_FILENAME "os.dat"
# define OS_BITSIZE 1024*1024*100
# define UNIT_BITSIZE 16
# define PIECE_BITSIZE 1024*4
# define PIECES_NUM 1024*25

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
FILE* CreateOS();

// 解析用户输入
status CMD(Space*, char*);

// 内存分配算法
Space AllocBoundTag(Space*, int);

// 内存回收算法
Space Recover(Space, int);

// 功能函数
Space FootLoc(head* p){
    return p + p->size - 1;
}

/////////////////////////////// 函数实现 //////////////////////////
FILE* CreateOS(){
    FILE* fp = fopen(OS_FILENAME,"wb+");
    if(fp==NULL){
        printf("文件创建失败\n");
        return NULL;
    }
    // TODO: 添加链表
    head* HEAD;
    head* temphead;
    temphead->llink = temphead;
    temphead->rlink = temphead;
    temphead->tag = 0;
    temphead->size = PIECE_BITSIZE/UNIT_BITSIZE;
    HEAD = temphead;
    foot* tempfoot;
    tempfoot->uplink = temphead;
    tempfoot->rlink = NULL;
    tempfoot->tag = 0;
    tempfoot->size = PIECE_BITSIZE/UNIT_BITSIZE;
    for(int i=0;i<PIECES_NUM;i++){
        printf("%d\n",i);
        head* nextHead;
        nextHead->llink = temphead;
        nextHead->rlink = HEAD;
        nextHead->tag = 0;
        nextHead->size = PIECE_BITSIZE/UNIT_BITSIZE;
        temphead->rlink = nextHead;
        foot* nextFoot;
        nextFoot->uplink = nextHead;
        nextFoot->rlink = NULL;
        nextFoot->tag = 0;
        nextHead->size = PIECE_BITSIZE/UNIT_BITSIZE;
        fwrite(temphead,UNIT_BITSIZE,1,fp);
        fseek(fp,PIECE_BITSIZE-UNIT_BITSIZE,SEEK_CUR);
        fwrite(tempfoot,UNIT_BITSIZE,1,fp);
        temphead = nextHead;
        tempfoot = nextFoot;
    }
    return fp;
}

status CMD(Space* pav, char* commend){
    //命令格式：函数命令（alloc recover）+空格+数字+数字，例如 alloc 8 或者 recover 8 9
    char Alloc[] = "alloc";
    char Recov[] = "recover";
    char ExiT[] = "exit";
    char cmd[20] = { 0 };//commend 函数命令部分
    int i = 0;
    //解析commend命令中的函数命令
    while (commend[i] != ' '){
        cmd[i] = commend[i];
        i++;
    }
    i++;
    //判断commend命令中的函数命令
    if (strcmp(cmd, ExiT) == 0)
        return false;
    else if (strcmp(cmd, Alloc) == 0){
        //解析commend命令中的第一个参数n
        int n = 0;
        //将数字字符转化为整型
        while (commend[i] <= 39 && commend[i] >= 30){
            n = 10 * n + commend[i] - 30;
            i++;
        }
        AllocBoundTag(pav, n);
    }
    else if (strcmp(cmd, Recov) == 0){
        //解析commend命令中的第一个参数n
        int n = 0;
        while (commend[i] <= 39 && commend[i] >= 30){
            //将数字字符转化为整型
            n = 10 * n + commend[i] - 30;
            i++;
        }
        Recover(*pav, n);
    }
    else {
        printf("命令错误，请重新输入：");
    }       
    return true;
}

Space AllocBoundTag(Space *pav,int n){
    Space p,f;
    int e=10;//设定常亮 e 的值
    //如果在遍历过程，当前空闲块的在存储容量比用户申请空间 n 值小，在该空闲块有右孩子的情况下直接跳过
    for (p=(*pav); p&&p->size<n&&p->rlink!=(*pav); p=p->rlink);
    //跳出循环，首先排除p为空和p指向的空闲块容量小于 n 的情况
    if (!p ||p->size<n) {
        return NULL;
    }else{
        //指针f指向p空闲块的foot域
        f=FootLoc(p);
        //调整pav指针的位置，为下次分配做准备
        (*pav)=p->rlink;
        //如果该空闲块的存储大小比 n 大，比 n+e 小，负责第一种情况，将 p 指向的空闲块全部分配给用户
        if (p->size-n <= e) {
            if ((*pav)==p) {
                pav=NULL;
            }
            else{
                //全部分配用户，即从可利用空间表中删除 p 空闲块
                (*pav)->llink=p->llink;
                p->llink->rlink=(*pav);
            }
            //同时调整head域和foot域中的tag值
            p->tag=f->tag=1;
        }
        //否则，从p空闲块中拿出 大小为 n 的连续空间分配给用户，同时更新p剩余存储块中的信息。
        else{
            //更改分配块foot域的信息
            f->tag=1;
            p->size-=n;
            //f指针指向剩余空闲块 p 的底部
            f=FootLoc(p);
            f->tag=0;   f->uplink=p;
            p=f+1;//p指向的是分配给用户的块的head域，也就是该块的首地址
            p->tag=1;   p->size=n;
        }
        return p;
    }
}

Space Recover(Space pav, int loc){
    Space p = pav;
    //设定p指针指向的为用户释放的空闲块
    for(int i=0;i<loc;i++){
        p = p->rlink;
    }
    p->tag=0;
    if((p+p->size)->tag==1 && (p-1)->uplink->tag==1){
        //f指针指向p空闲块的foot域
        Space f=FootLoc(p);
        f->uplink=p;
        f->tag=0;
        //如果pav指针不存在，证明可利用空间表为空，此时设置p为头指针，并重新建立双向循环链表
        if (!pav) {
            pav=p->llink=p->rlink=p;
        }else{
            //否则，在p空闲块插入到pav指向的空闲块的左侧
            Space q=pav->llink;
            p->rlink=pav;
            p->llink=q;
            q->rlink=pav->llink=p;
            pav=p;
        }
    }
    else if ((p+p->size)->tag==1 && (p-1)->uplink->tag==0){
        //常量 n 表示当前空闲块的存储大小
        int n=p->size;
        Space s=(p-1)->uplink;//p-1 为当前块的左侧块的foot域，foot域中的uplink指向的就是左侧块的首地址，s指针代表的是当前块的左侧存储块
        s->size+=n;//设置左侧存储块的存储容量
        Space f=p+n-1;//f指针指向的是空闲块 p 的foot域
        f->uplink=s;//这是foot域的uplink指针重新指向合并后的存储空间的首地址
        f->tag=0;//设置foot域的tag标记为空闲块
    }
    else if ((p+p->size)->tag==0 && (p-1)->uplink->tag==1){
        Space t=p+p->size;//t指针指向右侧空闲块的首地址
        p->tag=0;//初始化head域的tag值为0
        //找到t右侧空闲块的前驱结点和后继结点，用当前释放的空闲块替换右侧空闲块
        Space q=t->llink;
        p->llink=q; q->rlink=p;
        Space q1=t->rlink;
        p->rlink=q1; q1->llink=p;
        //更新释放块的size的值
        p->size+=t->size;
        //更改合并后的foot域的uplink指针的指向
        Space f=FootLoc(t);
        f->uplink=p;
    }
    else if ((p+p->size)->tag==0 && (p-1)->uplink->tag==0){
        int n=p->size;
        Space s=(p-1)->uplink;//找到释放内存块物理位置相邻的低地址的空闲块
        Space t=p+p->size;//找到物理位置相邻的高地址处的空闲块
        s->size+=n+t->size;//更新左侧空闲块的size的值
        //从可利用空间表中摘除右侧空闲块
        Space q=t->llink;
        Space q1=t->rlink;
        q->rlink=q1;
        q1->llink=q;
        //更新合并后的空闲块的uplink指针的指向
        Space f=FootLoc(t);
        f->uplink=s;
    }
}


/* -------------------- main ------------------------*/ 
int main(){
    FILE *fp;
    Space* head;
    if ( (fp = fopen(OS_FILENAME, "r")) == NULL ) {
        if( (fp = CreateOS()) == NULL){
            return -1;   
        }
    }
    fclose(fp);
    fp = fopen(OS_FILENAME,"wb+");
    fread((*head),sizeof(WORD),1,fp);
    printf("欢迎使用zjj与lpl创建的操作系统!\n");
    char * commend;
    status flag = true;
    while(flag){
        gets(commend);
        flag = CMD(head, commend);
    }
    printf("成功退出操作系统");
    printf("%d",sizeof(WORD));
    return 0;
}
