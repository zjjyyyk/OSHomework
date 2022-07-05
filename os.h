#ifndef _OS_H_
#define _OS_H_

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
typedef enum status{false,true} status; 

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

#endif