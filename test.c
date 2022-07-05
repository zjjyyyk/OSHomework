#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

# define OS_FILENAME "os.dat"
# define OS_BITSIZE (1024*1024*100)
# define PIECE_BITSIZE (1024*512)

typedef enum status {false,true} status;

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
extern void CreateOS();

// 内存分配算法
extern int AllocBoundTag(int* firstFree, int, FILE*);

// 内存回收算法
extern int Recover(int* firstFree, int, FILE*);

// 根据序号找节点
extern node* getNode(FILE*, int);

// 将指针数据写入文件
extern void writeNode(FILE*, int, node*);

// 找到第一个空余节点
extern int findfirstFreeNode(FILE*);

// 遍历文件，显示状态
extern void displayNodeTag(FILE*);

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
        temphead->llink = i != 0 ? i - 1 : OS_BITSIZE/PIECE_BITSIZE-1;
        temphead->uplink = i;
        temphead->rlink = i != OS_BITSIZE/PIECE_BITSIZE-1  ? i + 1 : 0;
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
            node* nd2 = getNode(fp, *pav);
            node* nd3 =  getNode(fp, nd->llink);
            //全部分配用户，即从可利用空间表中删除 p 空闲块
            nd2->llink = nd->llink;
            nd3->rlink = *pav;
            writeNode(fp,*pav,nd2);
            writeNode(fp,nd->llink,nd3);
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


typedef int ElemType;
typedef struct ArrayInfo {
    int loc;
    int count;
}ArrayInfo;

ArrayInfo initArray(FILE* fp, int* pav, ElemType* elems, int count){
    printf("Initiating Array......\n");
    printf("pav : %d\n",*pav);
    int loc = AllocBoundTag(pav, count * sizeof(ElemType), fp);
    printf("loc : %d\n",loc);
    fseek(fp,loc * PIECE_BITSIZE + sizeof(node), SEEK_SET);
    fwrite(elems,sizeof(ElemType),count,fp);
    rewind(fp);
    fflush(fp);
    ArrayInfo info;
    info.count = count;
    info.loc = loc;
    return info;
}

void displayArray(FILE* fp, ArrayInfo info){
    if(info.loc == -50){
        printf("tempArrayInfo NOT FOUND!\n");
        return;
    }
    fseek(fp,info.loc * PIECE_BITSIZE + sizeof(node), SEEK_SET);
    ElemType* data = (ElemType*)malloc(sizeof(ElemType) * (info.count));
    fread(data,sizeof(ElemType),info.count,fp);
    int i;
    printf("Array data: ");
    for(i=0;i<info.count;i++){
        printf("%d ",data[i]);
    }
    printf("\n");
    rewind(fp);
}

ArrayInfo addArray(FILE* fp, ArrayInfo info, ElemType elem, int index){
    if(info.loc == -50){
        printf("tempArrayInfo NOT FOUND!\n");
        return info;
    }
    fseek(fp,info.loc * PIECE_BITSIZE + sizeof(node), SEEK_SET);
    ElemType* data = (ElemType*)malloc(sizeof(ElemType) * (info.count+1));
    fread(data,sizeof(ElemType),info.count,fp);
    int i;
    for(i=info.count;i>=index;i--){
        data[i+1] = data[i];
    }
    data[i] = elem;
    fseek(fp,info.loc * PIECE_BITSIZE + sizeof(node), SEEK_SET);
    fwrite(data,sizeof(ElemType),info.count+1,fp);
    rewind(fp);
    fflush(fp);
    info.count++;
    return info;
}

ArrayInfo removeArray(FILE* fp, ArrayInfo info, int index){
    if(info.loc == -50){
        printf("tempArrayInfo NOT FOUND!\n");
        return info;
    }
    fseek(fp,info.loc * PIECE_BITSIZE + sizeof(node), SEEK_SET);
    ElemType* data = (ElemType*)malloc(sizeof(ElemType) * (info.count));
    fread(data,sizeof(ElemType),info.count,fp);
    int i;
    for(i=index;i<info.count;i++){
        data[i] = data[i+1];
    }
    fseek(fp,info.loc * PIECE_BITSIZE + sizeof(node), SEEK_SET);
    fwrite(data,sizeof(ElemType),info.count-1,fp);
    rewind(fp);
    fflush(fp);
    info.count--;
    return info;
}

int findArray(FILE* fp, ArrayInfo info, ElemType elem){
    if(info.loc == -50){
        printf("tempArrayInfo NOT FOUND!\n");
        return -50;
    }
    fseek(fp,info.loc * PIECE_BITSIZE + sizeof(node), SEEK_SET);
    ElemType* data = (ElemType*)malloc(sizeof(ElemType) * (info.count));
    fread(data,sizeof(ElemType),info.count,fp);
    int i;
    for(i=0;i<info.count;i++){
        if(data[i] == elem){
            rewind(fp);
            return i;
        }
    }
    rewind(fp);
    return -1;
}

ArrayInfo setArray(FILE* fp, ArrayInfo info, int elem, int index){
    if(info.loc == -50){
        printf("tempArrayInfo NOT FOUND!\n");
        return info;
    }
    fseek(fp,info.loc * PIECE_BITSIZE + sizeof(node), SEEK_SET);
    ElemType* data = (ElemType*)malloc(sizeof(ElemType) * (info.count));
    fread(data,sizeof(ElemType),info.count,fp);
    data[index] = elem;
    fseek(fp,info.loc * PIECE_BITSIZE + sizeof(node), SEEK_SET);
    fwrite(data,sizeof(ElemType),info.count,fp);
    rewind(fp);
    fflush(fp);
    return info;
}


status cmd_Array(FILE* fp, int* pav, char* commend, ArrayInfo* tempArrayInfo){
    printf("arrayCommend:%s END, pav : %d\n",commend,*pav);
    char init[] = "init";  // 创建
    char add[] = "add";  // 增
    char remove[] = "remove";  // 删 
    char find[] = "find";  // 查
    char set[] = "set";  // 改
    char display[] = "display";  // 看
    char cmd[20] = { 0 };//commend 函数命令部分
    int i = 0;
    //解析commend命令中的函数命令
    int tempflag = 1;
    while (commend[i] != ' ' && tempflag) {
        if(commend[i]==0) tempflag = 0;
        cmd[i] = commend[i];
        i++;
    }
    if(strcmp(cmd,init)==0){
        for (; isblank(commend[i]);i++);
        int count = 0;
        ElemType data[100] = {0};
        while( isdigit(commend[i]) || isblank(commend[i])){
            for (; isblank(commend[i]);i++);
            int n = 0;
            while (isdigit(commend[i])) {
                n = 10 * n + commend[i] - 48;
                i++;
            }
            data[count] = n;
            count++;
        }
        *tempArrayInfo = initArray(fp,pav,data,count);
        printf("Successfully initArray, ArrayInfo: [ loc = %d, count = %d ]\n",tempArrayInfo->loc,tempArrayInfo->count);      
        return true;
    }
    else if(strcmp(cmd,add)==0){
        for (; isblank(commend[i]);i++);
        int index = 0;
        while (isdigit(commend[i])) {
            index = 10 * index + commend[i] - 48;
            i++;
        }
        for (; isblank(commend[i]);i++);
        ElemType elem = 0;
        while (isdigit(commend[i])) {
            elem = 10 * elem + commend[i] - 48;
            i++;
        }
        *tempArrayInfo = addArray(fp,*tempArrayInfo,elem,index);
        printf("Successfully addArray [ index = %d, elem = %d ], ArrayInfo: [ loc = %d, count = %d ]\n",index,elem,tempArrayInfo->loc,tempArrayInfo->count);
        return true;
    }
    else if(strcmp(cmd,remove)==0){
        for (; isblank(commend[i]);i++);
        int index = 0;
        while (isdigit(commend[i])) {
            index = 10 * index + commend[i] - 48;
            i++;
        }
        *tempArrayInfo = removeArray(fp,*tempArrayInfo,index);
        printf("Successfully removeArray [ index = %d ], ArrayInfo: [ loc = %d, count = %d ]\n",index,tempArrayInfo->loc,tempArrayInfo->count);
        return true;
    }
    else if(strcmp(cmd,find)==0){
        ElemType elem = 0;
        while (isdigit(commend[i])) {
            elem = 10 * elem + commend[i] - 48;
            i++;
        }
        int index = findArray(fp,*tempArrayInfo,elem);
        printf("Successfully findArray [ elem = %d ], result: [ index = %d ], ArrayInfo: [ loc = %d, count = %d ]\n",elem,index,tempArrayInfo->loc,tempArrayInfo->count);
        return true;
    }
    else if(strcmp(cmd,set)==0){
        for (; isblank(commend[i]);i++);
        int index = 0;
        while (isdigit(commend[i])) {
            index = 10 * index + commend[i] - 48;
            i++;
        }
        for (; isblank(commend[i]);i++);
        ElemType elem = 0;
        while (isdigit(commend[i])) {
            elem = 10 * elem + commend[i] - 48;
            i++;
        }
        *tempArrayInfo = setArray(fp,*tempArrayInfo,elem,index);
        printf("Successfully setArray [ index = %d, elem = %d ], ArrayInfo: [ loc = %d, count = %d ]\n",index,elem,tempArrayInfo->loc,tempArrayInfo->count);
        return true;
    }
    else if(strcmp(cmd,display)==0){
        displayArray(fp, *tempArrayInfo);
        return true;
    }
    else{
        printf("Array dosen't have this commend.\n");
        return true;
    }
}




status CMD(FILE* fp, int* pav, char* commend, int* flag, ArrayInfo* tempArrayInfo) {
    //命令格式：函数命令（alloc recover）+空格+数字+数字，例如 alloc 8 或者 recover 8 9
    char Alloc[] = "alloc";
    char Recov[] = "recover";
    char Exit[] = "exit";
    char Display[] = "display";
    char Pav[] = "pav";
    char Check[] = "check";

    char Array[] = "array";

    char cmd[128] = { 0 };//commend 函数命令部分
    int i = 0;
    //解析commend命令中的函数命令
    for (; isblank(commend[i]);i++);
    int tempflag = 1;
    while (commend[i] != ' ' && tempflag) {
        if(commend[i]==0) tempflag = 0;
        cmd[i] = commend[i];
        i++;
    }
    //判断commend命令中的函数命令
    if (strcmp(cmd, Exit) == 0){
        printf("EXIT OUT\n");
        return false;
    }
    else if (strcmp(cmd, Alloc) == 0) {
        for (; isblank(commend[i]);i++);
        int n = 0;
        while (isdigit(commend[i])) {
            n = 10 * n + commend[i] - 48;
            i++;
        }
        int ret = AllocBoundTag(pav, n, fp);
        if (ret != -100) printf("Successfully called: AllocBoundTag(pav,%d), ret = %d\n", n,ret);
        return true;
    }
    else if (strcmp(cmd, Recov) == 0) {
        for (; isblank(commend[i]);i++);
        if(commend[i] == 'a' && commend[i+1] == 'l' && commend[i+2] == 'l'){
            int i;
            for(i=0;i<OS_BITSIZE/PIECE_BITSIZE;i++){
                i = Recover(pav,i,fp);
            }
            printf("Recovered all\n");
            return true;
        }
        int n = 0;
        while( isdigit(commend[i]) || isblank(commend[i])){
            for (; isblank(commend[i]);i++);
            n = 0;
            while (isdigit(commend[i])) {
                n = 10 * n + commend[i] - 48;
                i++;
            }
            int ret = Recover(pav, n, fp);
            if (ret != -200) printf("Successfully called: Recover(*pav,%d)\n", n);
        }      
        return true;
    }
    else if (strcmp(cmd, Display) == 0){
        displayNodeTag(fp);
        return true;
    }
    else if (strcmp(cmd, Pav) == 0){
        printf("Now pav = %d\n",*pav);
        return true;
    }
    else if (strcmp(cmd, Check) == 0){
        for (; isblank(commend[i]);i++);
        int n = 0;
        while (isdigit(commend[i])) {
            n = 10 * n + commend[i] - 48;
            i++;
        }
        node* nd = getNode(fp,n);
        if(n>=0&&n<OS_BITSIZE/PIECE_BITSIZE) printf("Node %d: [tag:%d uplink:%d llink:%d rlink:%d]\n",n,nd->tag,nd->uplink,nd->llink,nd->rlink);
        else printf("There is no node %d\n",n);
        return true;
    }
    else if (strcmp(cmd,Array) == 0){
        for (; isblank(commend[i]);i++);
        if(*flag != 5){
            tempArrayInfo->loc = -50;
            tempArrayInfo->count = 0;
            *flag = 5;
            printf("Now flag = %d\n",*flag);
        }
        char arrayCommend[128] = {0};
        strncpy(arrayCommend,commend+i,strlen(commend)-i);
        return cmd_Array(fp,pav,arrayCommend,tempArrayInfo);
    }
    else {
        printf("Wrong commend, please input again:\n");
        return true;
    }
}


/* -------------------- main ------------------------*/
int main() {
    FILE* fp;
    if ((fp = fopen(OS_FILENAME, "rb")) == NULL) {
        CreateOS();
        printf("OS created!\n");
    }
    fp = fopen(OS_FILENAME, "rwb+");
    rewind(fp);
    int pav = findFirstFreeNode(fp);
    printf("pav:%d\n",pav);

    printf("-------\n");
    printf("Welcome to our OS!\n");
    char commend[20] = {0};
    status notExit = true;
    int flag = 1;
    ArrayInfo* tempArrayInfo = (ArrayInfo*)malloc(sizeof(ArrayInfo)); tempArrayInfo->loc = -50; tempArrayInfo->count = 0;
    while (notExit) {
        gets(commend);
        notExit = CMD(fp, &pav, commend, &flag,tempArrayInfo);
    }
    printf("Safely exited");
    fclose(fp);
    return 0;
}