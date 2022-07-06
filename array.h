#ifndef _ARRAY_H_
#define _ARRAY_H_


typedef int ElemType;
typedef struct ArrayInfo {
    int loc;
    int count;
}ArrayInfo;

extern status cmd_Array(FILE* fp, int* pav, char* commend, ArrayInfo* tempArrayInfo);


#endif
