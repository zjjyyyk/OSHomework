#include <stdio.h>
#include <stdlib.h>

typedef enum status {false,true} status;
# define BYTE 8

//////////////////////////////// 数据结构 //////////////////////////
typedef struct WORD{

}WORD,*Space;


//////////////////////////////// 函数原型 //////////////////////////
//创建操作系统
FILE CreateOS();

// 在操作系统中创建内存节点链表
Space* InitSpaceLink(FILE);

// 解析用户输入
status CMD(char*);

// 内存分配算法
Space AllocBoundTag(Space*, int n);

// 内存回收算法
Space Recover(Space*, Space);

// 功能函数
Space FootLoc(Space);


int main(){


    return 0;
}