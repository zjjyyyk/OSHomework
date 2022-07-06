#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "os.h"
#include "array.h"


ArrayInfo initArray(FILE* fp, int* pav, ElemType* elems, int count){
    int loc = AllocBoundTag(pav, count * sizeof(ElemType), fp);
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
    data[i+1] = elem;
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
    char add[] = "add";  // �?
    char remove[] = "remove";  // �? 
    char find[] = "find";  // �?
    char set[] = "set";  // �?
    char display[] = "display";  // �?
    char cmd[20] = { 0 };//commend 函数命令部分
    int i = 0;
    //解析commend命令�?的函数命�?
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
