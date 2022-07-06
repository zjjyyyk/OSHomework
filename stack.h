#ifndef _STACK_H_
#define _STACK_H_

typedef int ElemType;
typedef struct StackInfo {
    int loc;
    int top;
}StackInfo;

extern status cmd_Stack(FILE* fp, int* pav, char* commend, StackInfo* tempStackInfo);


#endif