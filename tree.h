#ifndef __TREE_H__
#define __TREE_H__

typedef int ElemType;

typedef int TreeInfo;

typedef struct BinTreeNode
{
	ElemType data;  
	int leftChild; 
	int rightChild;
}BinTreeNode;

extern status cmd_Tree(FILE* fp, int* pav, char* commend, TreeInfo* tempRoot);

#endif
