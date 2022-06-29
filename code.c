#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum status {false,true} status;
# define OS_FILENAME "os.dat"
# define OS_BITSIZE 1024*1024*100
# define PIECE_BITSIZE 1024*512

//////////////////////////////// 数据结构 //////////////////////////
typedef struct WORD{
    int llink;
    int uplink;
    int tag;//标记域,0表示为空闲块；1表示为占用块
    int size;//记录内存块的存储大小
    int rlink;
}node;


//////////////////////////////// 函数原型 //////////////////////////
//创建操作系统和内存节点链表
void CreateOS();

// 解析用户输入
status CMD(FILE*, int* firstFree, char*);

// 内存分配算法
int AllocBoundTag(int* firstFree, int, FILE*);

// 内存回收算法
int Recover(int* firstFree, int, FILE*);

// 根据序号找节点
node* getNode(FILE*, int);

// 找到第一个空余节点
int findfirstFreeNode(FILE*);


/////////////////////////////// 函数实现 //////////////////////////
void CreateOS(){
    FILE* fp = fopen(OS_FILENAME,"wb+");
    if(fp==NULL){
        printf("文件创建失败\n");
        return;
    }
    fseek(fp,OS_BITSIZE+1,SEEK_SET);
    fputc(32,fp);
    rewind(fp);
    // 添加链表
    int i;
    node temphead;
    for(i=0;i<OS_BITSIZE/PIECE_BITSIZE;i++){
        temphead.llink = (i-1)%(OS_BITSIZE/PIECE_BITSIZE);
        temphead.uplink = i;
        temphead.rlink = (i+1)%(OS_BITSIZE/PIECE_BITSIZE);
        temphead.size = PIECE_BITSIZE;
        temphead.tag = 0;
        fwrite(&temphead,sizeof(node),1,fp);
        fseek(fp,PIECE_BITSIZE-sizeof(node),SEEK_CUR);
    }
    rewind(fp);
    fclose(fp);
}

status CMD(FILE* fp, int *pav, char* commend){
    //命令格式：函数命令（alloc recover）+空格+数字+数字，例如 alloc 8 或者 recover 8 9
    char Alloc[] = "alloc";
    char Recov[] = "recover";
    char Exit[] = "exit";
    char cmd[20] = { 0 };//commend 函数命令部分
    int i = 0;
    //解析commend命令中的函数命令
    while (commend[i] != ' '){
        cmd[i] = commend[i];
        i++;
    }
    for(;commend[i]==' ';i++);
    //判断commend命令中的函数命令
    if (strcmp(cmd, Exit) == 0)
        return false;
    else if (strcmp(cmd, Alloc) == 0){
        int n = 0;
        while (commend[i] <= 39 && commend[i] >= 30){
            n = 10 * n + commend[i] - 30;
            i++;
        }
        int ret = AllocBoundTag(pav, n, fp);
        if(ret!=-100) printf("成功调用函数：AllocBoundTag(pav,%d)\n",n);
        return true;
    }
    else if (strcmp(cmd, Recov) == 0){
        int n = 0;
        while (commend[i] <= 39 && commend[i] >= 30){
            n = 10 * n + commend[i] - 30;
            i++;
        }
        int ret = Recover(pav, n, fp);
        if(ret!=-200) printf("成功调用函数：Recover(*pav,%d)\n",n);
        return true;
    }
    else {
        printf("命令错误，请重新输入：");
        return true;
    }       
}

node* getNode(FILE* fp,int n){
    node *nd = NULL;
    fseek(fp,n*PIECE_BITSIZE,SEEK_SET);
    fread(nd,sizeof(node),1,fp);
    return nd;
}


/////////////////////   1   //////////////////
int AllocBoundTag(int* pav,int n, FILE* fp){
    int p,f;
    node *nd = getNode(fp,*pav);
    //如果在遍历过程，当前空闲块的在存储容量比用户申请空间 n 值小，在该空闲块有右孩子的情况下直接跳过
    for (p=(*pav); nd&&nd->size<n&&nd->rlink!=(*pav); p=nd->rlink, nd = getNode(fp,p))
    //跳出循环，首先排除p为空和p指向的空闲块容量小于 n 的情况
    if (!nd ||nd->size<n) {
        return -100;
    }else{
        //调整pav指针的位置，为下次分配做准备
        (*pav)=nd->rlink;
        if ((*pav)==p) {
            *pav = -1;
        }
        else{
            //全部分配用户，即从可利用空间表中删除 p 空闲块
            getNode(fp,*pav)->llink = nd->llink;
            getNode(fp,nd->llink)->rlink = *pav;
        }
        nd->tag = 1;
        return p;
    }
}


///////////////////////   2   /////////////////////////
int Recover(int* pav, int loc, FILE* fp){
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


//////////////////////////   3   /////////////////////////
int findFirstFreeNode(FILE* fp){

}


/* -------------------- main ------------------------*/ 
int main(){
    FILE *fp;
    if ( (fp = fopen(OS_FILENAME, "rb")) == NULL ) {
        CreateOS();
    }
    fp = fopen(OS_FILENAME,"ab+");
    rewind(fp);
    int pav = findFirstFreeNode(fp);

    printf("-------\n");
    printf("欢迎使用zjj与lpl创建的操作系统!\n");
    char * commend;
    status flag = false;
    while(flag){
        gets(commend);
        flag = CMD(fp, &pav, commend);
    }
    printf("成功退出操作系统");
    fclose(fp);
    return 0;
}



