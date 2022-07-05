#ifndef _ARRAY_H_
#define _ARRAY_H_


typedef int ElemType;
typedef struct ArrayInfo {
    int loc;
    int count;
}ArrayInfo;

// ArrayInfo initArray(FILE* fp, int* pav, ElemType* elems, int count);

// void displayArray(FILE* fp, ArrayInfo info);

// ArrayInfo addArray(FILE* fp, ArrayInfo info, ElemType elem, int index);

// ArrayInfo removeArray(FILE* fp, ArrayInfo info, int index);

// int findArray(FILE* fp, ArrayInfo info, ElemType elem);

// ArrayInfo setArray(FILE* fp, ArrayInfo info, int elem, int index);

extern status cmd_Array(FILE* fp, int* pav, char* commend, ArrayInfo* tempArrayInfo);


#endif
