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
        struct WORD *uplink;//指向结点�?�?
    };
    int tag;//标�?�域,0表示为空闲块�?1表示为占用块
    int size;//记录内存块的存储大小
    struct WORD *rlink;//指向直接后继
}WORD,head,foot,*Space;


//////////////////////////////// 函数原型 //////////////////////////
//创建操作系统和内存节点链�?
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
    printf("�����ڣ�fp����\n");
    // TODO: 添加链表
    head* HEAD;
    head* temphead = (head*)malloc(sizeof(head));
    temphead->llink = temphead;
    temphead->rlink = temphead;
    temphead->tag = 0;
    temphead->size = PIECE_BITSIZE/UNIT_BITSIZE;
    HEAD = temphead;
    foot* tempfoot = (foot*)malloc(sizeof(foot));
    tempfoot->uplink = temphead;
    tempfoot->rlink = NULL;
    tempfoot->tag = 0;
    tempfoot->size = PIECE_BITSIZE/UNIT_BITSIZE;
    for(int i=0;i<PIECES_NUM;i++){
        printf("%d\n",i);
        head* nextHead = (head*)malloc(sizeof(head));
        nextHead->llink = temphead;
        nextHead->rlink = HEAD;
        nextHead->tag = 0;
        nextHead->size = PIECE_BITSIZE/UNIT_BITSIZE;
        temphead->rlink = nextHead;
        foot* nextFoot = (foot*)malloc(sizeof(foot));
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
    //命令格式：函数命令（alloc recover�?+空格+数字+数字，例�? alloc 8 或�? recover 8 9
    char Alloc[] = "alloc";
    char Recov[] = "recover";
    char ExiT[] = "exit";
    char cmd[20] = { 0 };//commend 函数命令部分
    int i = 0;
    //解析commend命令�?的函数命�?
    while (commend[i] != ' '){
        cmd[i] = commend[i];
        i++;
    }
    i++;
    //判断commend命令�?的函数命�?
    if (strcmp(cmd, ExiT) == 0)
        return false;
    else if (strcmp(cmd, Alloc) == 0){
        //解析commend命令�?的�??一�?参数n
        int n = 0;
        //将数字字符转化为整型
        while (commend[i] <= 39 && commend[i] >= 30){
            n = 10 * n + commend[i] - 30;
            i++;
        }
        AllocBoundTag(pav, n);
    }
    else if (strcmp(cmd, Recov) == 0){
        //解析commend命令�?的�??一�?参数n
        int n = 0;
        while (commend[i] <= 39 && commend[i] >= 30){
            //将数字字符转化为整型
            n = 10 * n + commend[i] - 30;
            i++;
        }
        Recover(*pav, n);
    }
    else {
        printf("命令错�??，�?�重新输入：");
    }       
    return true;
}

Space AllocBoundTag(Space *pav,int n){
    Space p,f;
    int e=10;//设定常亮 e 的�?
    //如果在遍历过程，当前空闲块的在存储�?�量比用户申请空�? n 值小，在该空闲块有右孩子的情况下直接跳过
    for (p=(*pav); p&&p->size<n&&p->rlink!=(*pav); p=p->rlink);
    //跳出�?�?，�?�先排除p为空和p指向的空闲块容量小于 n 的情�?
    if (!p ||p->size<n) {
        return NULL;
    }else{
        //指针f指向p空闲块的foot�?
        f=FootLoc(p);
        //调整pav指针的位�?，为下�?�分配做准�??
        (*pav)=p->rlink;
        //如果该空闲块的存储大小比 n 大，�? n+e 小，负责�?一种情况，�? p 指向的空闲块全部分配给用�?
        if (p->size-n <= e) {
            if ((*pav)==p) {
                pav=NULL;
            }
            else{
                //全部分配用户，即从可利用空间表中删除 p 空闲�?
                (*pav)->llink=p->llink;
                p->llink->rlink=(*pav);
            }
            //同时调整head域和foot域中的tag�?
            p->tag=f->tag=1;
        }
        //否则，从p空闲块中拿出 大小�? n 的连�?空间分配给用户，同时更新p剩余存储块中的信�?�?
        else{
            //更改分配块foot域的信息
            f->tag=1;
            p->size-=n;
            //f指针指向剩余空闲�? p 的底�?
            f=FootLoc(p);
            f->tag=0;   f->uplink=p;
            p=f+1;//p指向的是分配给用户的块的head域，也就�?该块的�?�地址
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
        //f指针指向p空闲块的foot�?
        Space f=FootLoc(p);
        f->uplink=p;
        f->tag=0;
        //如果pav指针不存�?，证明可利用空间表为空，此时设置p为头指针，并重新建立双向�?�?链表
        if (!pav) {
            pav=p->llink=p->rlink=p;
        }else{
            //否则，在p空闲块插入到pav指向的空闲块的左�?
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
        Space s=(p-1)->uplink;//p-1 为当前块的左侧块的foot域，foot域中的uplink指向的就�?左侧块的首地址，s指针代表的是当前块的左侧存储�?
        s->size+=n;//设置左侧存储块的存储容量
        Space f=p+n-1;//f指针指向的是空闲�? p 的foot�?
        f->uplink=s;//这是foot域的uplink指针重新指向合并后的存储空间的�?�地址
        f->tag=0;//设置foot域的tag标�?�为空闲�?
    }
    else if ((p+p->size)->tag==0 && (p-1)->uplink->tag==1){
        Space t=p+p->size;//t指针指向右侧空闲块的首地址
        p->tag=0;//初�?�化head域的tag值为0
        //找到t右侧空闲块的前驱结点和后继结点，用当前释放的空闲块替换右侧空闲块
        Space q=t->llink;
        p->llink=q; q->rlink=p;
        Space q1=t->rlink;
        p->rlink=q1; q1->llink=p;
        //更新释放块的size的�?
        p->size+=t->size;
        //更改合并后的foot域的uplink指针的指�?
        Space f=FootLoc(t);
        f->uplink=p;
    }
    else if ((p+p->size)->tag==0 && (p-1)->uplink->tag==0){
        int n=p->size;
        Space s=(p-1)->uplink;//找到释放内存块物理位�?相邻的低地址的空闲块
        Space t=p+p->size;//找到物理位置相邻的高地址处的空闲�?
        s->size+=n+t->size;//更新左侧空闲块的size的�?
        //从可利用空间表中摘除右侧空闲�?
        Space q=t->llink;
        Space q1=t->rlink;
        q->rlink=q1;
        q1->llink=q;
        //更新合并后的空闲块的uplink指针的指�?
        Space f=FootLoc(t);
        f->uplink=s;
    }
}


/* -------------------- main ------------------------*/ 
int main(){
    FILE *fp;
    Space* head;
    if ( (fp = fopen(OS_FILENAME, "r")) == NULL ) {
        printf("�ļ������ڣ����ڴ���~~~\n");
        fp = CreateOS();
        printf("�������\n");
    }
    fclose(fp);
    fp = fopen(OS_FILENAME,"wb+");
    fread((*head),sizeof(WORD),1,fp);
    printf("��ӭʹ�ò���ϵͳ!������ָ��:\n");
    char * commend;
    status flag = true;
    while(flag){
        gets(commend);
        flag = CMD(head, commend);
    }
    printf("成功退出操作系�?");
    printf("%d",sizeof(WORD));
    return 0;
}
