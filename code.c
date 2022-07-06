#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "os.h"
#include "list.h"
#include "array.h"
#include "stack.h"
#include "tree.h"


status CMD(FILE* fp, int* pav, char* commend, int* flag, ListInfo* tempphead, ArrayInfo* tempArrayInfo, StackInfo* tempStackInfo, TreeInfo* tempRoot) {
    //命令格式：函数命令（alloc recover�?+空格+数字+数字，例�? alloc 8 或�? recover 8 9
    char Alloc[] = "alloc";
    char Recov[] = "recover";
    char Exit[] = "exit";
    char Display[] = "display";
    char Pav[] = "pav";
    char Check[] = "check";

    char List[] = "list";
    char Array[] = "array";
    char Stack[] = "stack";
    char Tree[] = "tree";

    char cmd[128] = { 0 };//commend 函数命令部分
    int i = 0;
    //解析commend命令�?的函数命�?
    for (; isblank(commend[i]);i++);
    int tempflag = 1;
    while (commend[i] != ' ' && tempflag) {
        if(commend[i]==0) tempflag = 0;
        cmd[i] = commend[i];
        i++;
    }
    //判断commend命令�?的函数命�?
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
    else if (strcmp(cmd,List) == 0){
        for (; isblank(commend[i]);i++);
        if(*flag != 4){
            *tempphead = -40;
            *flag = 4;
            printf("Now flag = %d\n",*flag);
        }
        char ListCommend[128] = {0};
        strncpy(ListCommend,commend+i,strlen(commend)-i);
        return cmd_List(fp,pav,ListCommend,tempphead);
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
    else if (strcmp(cmd,Stack) == 0){
        for (; isblank(commend[i]);i++);
        if(*flag != 7){
            tempStackInfo->loc = -70;
            tempStackInfo->top = -1;
            *flag = 7;
            printf("Now flag = %d\n",*flag);
        }
        char StackCommend[128] = {0};
        strncpy(StackCommend,commend+i,strlen(commend)-i);
        return cmd_Stack(fp,pav,StackCommend,tempStackInfo);
    }
    else if (strcmp(cmd,Tree) == 0){
        for (; isblank(commend[i]);i++);
        if(*flag != 8){
            *tempRoot = -80;
            *flag = 8;
            printf("Now flag = %d\n",*flag);
        }
        char TreeCommend[128] = {0};
        strncpy(TreeCommend,commend+i,strlen(commend)-i);
        return cmd_Tree(fp,pav,TreeCommend,tempRoot);
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
    ListInfo* tempphead = (ListInfo*)malloc(sizeof(ListInfo)); *tempphead = -40;
    ArrayInfo* tempArrayInfo = (ArrayInfo*)malloc(sizeof(ArrayInfo)); tempArrayInfo->loc = -50; tempArrayInfo->count = 0;
    StackInfo* tempStackInfo = (StackInfo*)malloc(sizeof(StackInfo)); tempStackInfo->loc = -70; tempStackInfo->top = -1;
    TreeInfo* tempRoot = (TreeInfo*)malloc(sizeof(TreeInfo)); *tempRoot = -80;
    while (notExit) {
        gets(commend);
        notExit = CMD(fp, &pav, commend, &flag, tempphead, tempArrayInfo, tempStackInfo, tempRoot);
    }
    printf("Safely exited");
    fclose(fp);
    return 0;
}
