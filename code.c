#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum status { false, true } status;
# define OS_FILENAME "os.dat"
# define OS_BITSIZE (1024*1024*100)
# define PIECE_BITSIZE (1024*512)

//////////////////////////////// 数据结构 //////////////////////////
typedef struct WORD {
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

// 遍历文件，显示状态
void displayNodeTag(FILE*);


/////////////////////////////// 函数实现 //////////////////////////
void CreateOS() {
    FILE* fp = fopen(OS_FILENAME, "wb+");
    if (fp == NULL) {
        printf("文件创建失败\n");
        return;
    }
    fseek(fp, OS_BITSIZE + 1, SEEK_SET);
    fputc(32, fp);
    rewind(fp);
    // 添加链表
    int i;
    node temphead;
    for (i = 0;i < OS_BITSIZE / PIECE_BITSIZE;i++) {
        temphead.llink = (i - 1) % (OS_BITSIZE / PIECE_BITSIZE);
        temphead.uplink = i;
        temphead.rlink = (i + 1) % (OS_BITSIZE / PIECE_BITSIZE);
        temphead.size = PIECE_BITSIZE;
        temphead.tag = 0;
        fwrite(&temphead, sizeof(node), 1, fp);
        fseek(fp, PIECE_BITSIZE - sizeof(node), SEEK_CUR);
    }
    rewind(fp);
    fclose(fp);
}

status CMD(FILE* fp, int* pav, char* commend) {
    //命令格式：函数命令（alloc recover）+空格+数字+数字，例如 alloc 8 或者 recover 8 9
    char Alloc[] = "alloc";
    char Recov[] = "recover";
    char Exit[] = "exit";
    char Display[] = "display";
    char cmd[20] = { 0 };//commend 函数命令部分
    int i = 0;
    //解析commend命令中的函数命令
    while (commend[i] != ' ') {
        cmd[i] = commend[i];
        i++;
    }
    for (;commend[i] == ' ';i++);
    //判断commend命令中的函数命令
    if (strcmp(cmd, Exit) == 0){
        printf("EXIT OUT\n");
        return false;
    }
    else if (strcmp(cmd, Alloc) == 0) {
        int n = 0;
        while (commend[i] <= 57 && commend[i] >= 48) {
            n = 10 * n + commend[i] - 48;
            i++;
        }
        int ret = AllocBoundTag(pav, n, fp);
        if (ret != -100) printf("成功调用函数：AllocBoundTag(pav,%d)\n", n);
        return true;
    }
    else if (strcmp(cmd, Recov) == 0) {
        int n = 0;
        while (commend[i] <= 57 && commend[i] >= 48) {
            printf("find num\n");
            n = 10 * n + commend[i] - 48;
            i++;
        }
        int ret = Recover(pav, n, fp);
        if (ret != -200) printf("成功调用函数：Recover(*pav,%d)\n", n);
        return true;
    }
    else if (strcmp(cmd, Display) == 0){
        displayNodeTag(fp);
    }
    else {
        printf("Wrong commend, please input again:\n");
        return true;
    }
}

node* getNode(FILE* fp, int n) {
    node* nd = NULL;
    fseek(fp, n * PIECE_BITSIZE, SEEK_SET);
    fread(nd, sizeof(node), 1, fp);
    rewind(fp);
    return nd;
}


int AllocBoundTag(int* pav, int n, FILE* fp) {
    int p, f;
    node* nd = getNode(fp, *pav);
    //如果在遍历过程，当前空闲块的在存储容量比用户申请空间 n 值小，在该空闲块有右孩子的情况下直接跳过
    for (p = (*pav); nd && nd->size < n && nd->rlink != (*pav); p = nd->rlink, nd = getNode(fp, p))
    //跳出循环，首先排除p为空和p指向的空闲块容量小于 n 的情况
    if (!nd || nd->size < n) {
        rewind(fp);
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
        rewind(fp);
        return p;
    }
}


int Recover(int* pav, int loc, FILE* fp) {
    int p = loc;
    node* nd = getNode(fp,p);
    nd->tag = 0;
    //如果pav指针不存在，证明可利用空间表为空，此时设置p为头指针，并重新建立双向循环链表
    if (*pav == -1) {
        *pav = nd->llink = nd->rlink = p;
    }
    else {
        //否则，在p空闲块插入到pav指向的空闲块的左侧
        int q = getNode(fp,*pav)->llink;
        nd->rlink = *pav;
        nd->llink = q;
        getNode(fp,q)->rlink = getNode(fp,*pav)->llink = p;
        *pav = p;
    }
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
    int i;
    node temp;
    rewind(fp);
    printf("%d\n",OS_BITSIZE/PIECE_BITSIZE);
    for(i=0;i<OS_BITSIZE/PIECE_BITSIZE;i++){
        fread(&temp,sizeof(node),1,fp);
        printf("i:%d, tag:%d\n",i,temp.tag);
        fseek(fp,PIECE_BITSIZE-sizeof(node),SEEK_CUR);
    }
    printf("display over.\n");
    rewind(fp);
}


/* -------------------- main ------------------------*/
int main() {
    FILE* fp;
    if ((fp = fopen(OS_FILENAME, "rb")) == NULL) {
        CreateOS();
    }
    fp = fopen(OS_FILENAME, "ab+");
    rewind(fp);
    int pav = findFirstFreeNode(fp);
    printf("pav:%d\n",pav);

    printf("-------\n");
    printf("欢迎使用zjj与lpl创建的操作系统!\n");
    char commend[20];
    status flag = true;
    while (flag) {
        scanf("%*[^\n]"); scanf("%*c");
        gets(commend);
        flag = CMD(fp, &pav, commend);
    }
    printf("成功退出操作系统");
    fclose(fp);
    return 0;
}
