## 系统设计文档

### 1. 设计思路
  我们需要设计一个100M大小的操作系统，设计中必不可少地需要获取我们的操作系统内存块的地址。但是由于内存的实际地址受所使用的Windows操作系统保护，使用存储了前后内存块地址的链表来管理的方法很难实现。于是我们退而求其次，选择将100M进行分块，每0.5M分成一块，一共分成200块，**每个内存块用其在200块中的位置来代表地址**，这样我们就可以通过间接调用C语言`fread`，`fwrite`，`fseek`函数来对每个内存块读写。
  
  每个内存块的头部都存储了一个结构体，包含其自身索引、下一个空余节点的索引、上一个空余节点的索引、自身是否被占用、自身的字节数。
``` c
typedef struct WORD {
  int llink;
  int uplink;
  int tag;//标记域,0表示为空闲块；1表示为占用块
  int size;//记录内存块的存储大小
  int rlink;
}node;
```
  在这样的结构下，我们很轻松地就能完成内存的分配与回收，以及显示内存块的状态--只需要程序先找到一个空余节点。

### 2. 代码结构
  ![image](https://user-images.githubusercontent.com/74846219/177628261-2a37386b-358b-4a1f-ad92-cd573dacbc21.png)
  
  - 我们把操作系统的函数、数据结构的函数以及主函数分开，通过头文件将它们链接起来。
  - 每个数据结构只暴露一个处理用户输入的函数，最后用一个`CMD`函数将它们汇总。
  
### 3. `os.h`及`code.c`中的函数原型
1. code.c
  ``` c
  // 用户接口
  status CMD(FILE* fp, int* pav, char* commend, int* flag, ListInfo* tempphead, ArrayInfo* tempArrayInfo, StackInfo* tempStackInfo, TreeInfo* tempRoot)
  ```
2. os.h
  ``` c
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
  ```

