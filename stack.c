#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "os.h"
#include "stack.h"


StackInfo initStack(FILE* fp, int* pav){
    int loc = AllocBoundTag(pav, 50 * sizeof(ElemType), fp);
    StackInfo info;
    info.top = -1;
    info.loc = loc;
    return info;
}

void displayStack(FILE* fp, StackInfo* info){
    if(info->loc == -70){
        printf("tempStackInfo NOT FOUND!\n");
        return;
    }
    fseek(fp,info->loc * PIECE_BITSIZE + sizeof(node), SEEK_SET);
    if(info->top == -1){
        printf("Stack is empty, there's nothing to display\n");
        return;
    }
    ElemType* data = (ElemType*)malloc(sizeof(ElemType) * (info->top+1));
    fread(data,sizeof(ElemType),info->top+1,fp);
    int i;
    printf("Stack data: ");
    for(i=0;i<=info->top;i++){
        printf("%d ",data[i]);
    }
    printf("\n");
    rewind(fp);
}

void pushStack(FILE* fp, StackInfo* info, ElemType elem){
    if(info->loc == -70){
        printf("tempStackInfo NOT FOUND!\n");
        return;
    }
    fseek(fp,info->loc * PIECE_BITSIZE + sizeof(node) + (info->top+1)*sizeof(ElemType), SEEK_SET);
    fwrite(&elem,sizeof(ElemType),1,fp);
    rewind(fp);
    fflush(fp);
    info->top++;
    return;
}

ElemType popStack(FILE* fp, StackInfo* info){
    if(info->loc == -70){
        printf("tempStackInfo NOT FOUND!\n");
        return 8848;
    }
    if(info->top == -1){
        printf("Stack is empty, there's nothing to pop\n");
        return 8848;
    }
    fseek(fp,info->loc * PIECE_BITSIZE + sizeof(node) + (info->top) * sizeof(ElemType), SEEK_SET);
    ElemType* data = (ElemType*)malloc(sizeof(ElemType));
    fread(data,sizeof(ElemType),1,fp);
    rewind(fp);
    info->top--;
    return *data;
}


status cmd_Stack(FILE* fp, int* pav, char* commend, StackInfo* tempStackInfo){
    printf("StackCommend:%s END, pav : %d\n",commend,*pav);
    char init[] = "init";  
    char push[] = "push";  
    char pop[] = "pop";  
    char display[] = "display"; 
    char cmd[20] = { 0 };
    int i = 0;

    int tempflag = 1;
    while (commend[i] != ' ' && tempflag) {
        if(commend[i]==0) tempflag = 0;
        cmd[i] = commend[i];
        i++;
    }
    if(strcmp(cmd,init)==0){
        *tempStackInfo =  initStack(fp,pav);
        printf("Successfully initStack, StackInfo: [ loc = %d, top = %d ]\n",tempStackInfo->loc,tempStackInfo->top);      
        return true;
    }
    else if(strcmp(cmd,push)==0){
        for (; isblank(commend[i]);i++);
        ElemType elem = 0;
        while (isdigit(commend[i])) {
            elem = 10 * elem + commend[i] - 48;
            i++;
        }
        pushStack(fp,tempStackInfo,elem);
        printf("Successfully pushStack [elem = %d ], StackInfo: [ loc = %d, top = %d ]\n",elem,tempStackInfo->loc,tempStackInfo->top);
        return true;
    }
    else if(strcmp(cmd,pop)==0){
        int popret = popStack(fp,tempStackInfo);
        printf("Successfully popStack [ popResult = %d ], StackInfo: [ loc = %d, top = %d ]\n",popret,tempStackInfo->loc,tempStackInfo->top);
        return true;
    }
    else if(strcmp(cmd,display)==0){
        displayStack(fp, tempStackInfo);
        return true;
    }
    else{
        printf("Stack dosen't have this commend.\n");
        return true;
    }
}
