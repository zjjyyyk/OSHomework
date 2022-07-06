#ifndef __LIST_H__
#define __LIST_H__


typedef int SLTDataType;
typedef int ListInfo;

typedef struct SList
{
    SLTDataType data;   
    int next; 			
} SListNode;

extern status cmd_List(FILE* fp, int* pav, char* commend, ListInfo* tempphead);


#endif
