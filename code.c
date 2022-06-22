#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 100

typedef enum status { false, true } status;
# define OS_FILENAME "os.dat"

//////////////////////////////// 数据结构 //////////////////////////
typedef struct WORD {
    union {
        struct WORD* llink;//指向直接前驱
        struct WORD* uplink;//指向结点本身
    };
    int tag;//标记域,0表示为空闲块；1表示为占用块
    int size;//记录内存块的存储大小
    struct WORD* rlink;//指向直接后继
}WORD, head, foot, * Space;


//////////////////////////////// 函数原型 //////////////////////////
//创建操作系统和内存节点链表
Space CreateOS();

// 解析用户输入
status CMD(Space, char*, int );

// 内存分配算法
Space AllocBoundTag(Space, int);

// 内存回收算法
Space Recover(Space, Space);

// 功能函数
Space FootLoc(Space);

/////////////////////////////// 函数实现 //////////////////////////
Space CreateOS() {
    FILE* fp = fopen(OS_FILENAME, "wb+");
    if (fp == NULL) {
        printf("文件创建失败\n");
        return NULL;
    }
    fseek(fp, 100 * 1024 * 1024 - 1, SEEK_SET);
    fputc(621, fp);
    rewind(fp);
    Space head = (Space)malloc(sizeof(WORD));
    head->uplink = fp + 1024 * 64;  // 最多存储1024*128个内存碎片,需要1024*128*16Byte,即1024*64个&fp单位
    head->tag = 0;
    head->size = 1024 * 1024 * 99;  // 去掉1024*128个存储链表节点我们的操作系统一共有99M内存可以挥霍
    head->llink = head->rlink = NULL;
    fwrite(head, sizeof(WORD), 1, fp);
    fclose(fp);
    return head;
}

status CMD(Space* pav, char* commend)
{
    //命令格式：函数命令（alloc recover）+空格+数字+数字，例如 alloc 8 或者 recover 8 9
    char Alloc[] = "alloc";
    char Recov[] = "recover";
    char ExiT[] = "exit";
    char cmd[20] = { 0 };//commend 函数命令部分
    int i = 0;
    //解析commend命令中的函数命令
    while (commend[i] != ' ')
    {
        cmd[i] = commend[i];
        i++;
    }
    i++;
    //判断commend命令中的函数命令
    if (strcmp(cmd, ExiT) == 0)
        return false;
    else if (strcmp(cmd, Alloc) == 0)
    {
        //解析commend命令中的第一个参数n
        int n = 0;
        //将数字字符转化为整型
        while (commend[i] <= 39 && commend[i] >= 30)
        {
            n = 10 * n + commend[i];
            i++;
        }
        AllocBoundTag(pav, n);
    }
    else if (strcmp(cmd, Recov) == 0)
    {
        //解析commend命令中的第一个参数n
        int n = 0;
        while (commend[i] <= 39 && commend[i] >= 30)
        {
            //将数字字符转化为整型
            n = 10 * n + commend[i] - 30;
            i++;
        }
        //i++;
        ////解析commend命令中的第二个参数m
        //int m = 0;
        //while (commend[i] <= 39 && commend[i] >= 30)
        //{
        //    //将数字字符转化为整型
        //    m = 10 * m + commend[i] - 30;
        //    i++;
        //}
        Recover(pav, n);
    }
    else
        printf("命令错误，请重新输入：");
    return true;
}
/* -------------------- main ------------------------*/
int main() {
    FILE* fp;
    Space head;
    if ((fp = fopen(OS_FILENAME, "r")) == NULL) {
        head = CreateOS();
        fp = fopen(OS_FILENAME, 'wb+');
    }
    else {
        fp = fopen(OS_FILENAME, 'wb+');
        fread(head, sizeof(Space), 1, fp);
    }
    char* commend;
    status flag = true;
    while (flag) {
        gets(commend);
        flag = CMD(head, commend, N);
    }
    printf("成功退出操作系统");
    return 0;
}
