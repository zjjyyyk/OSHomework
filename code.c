#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "os.h"

typedef enum status {false,true} status;

status CMD(FILE* fp, int* pav, char* commend) {
    //命令格式：函数命令（alloc recover）+空格+数字+数字，例如 alloc 8 或者 recover 8 9
    char Alloc[] = "alloc";
    char Recov[] = "recover";
    char Exit[] = "exit";
    char Display[] = "display";
    char Pav[] = "pav";
    char cmd[20] = { 0 };//commend 函数命令部分
    int i = 0;
    //解析commend命令中的函数命令
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
    status flag = true;
    while (flag) {
        gets(commend);
        flag = CMD(fp, &pav, commend);
    }
    printf("成功退出操作系统");
    fclose(fp);

    return 0;
}