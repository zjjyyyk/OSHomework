#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "os.h"
#include "tree.h"

void writetreenode(FILE* fp,int loc, BinTreeNode tempnode[1]) {
	//将结点node信息写入文件中的指定位置loc
	rewind(fp);
	fseek(fp,PIECE_BITSIZE*loc+sizeof(node),SEEK_SET);
	fwrite(tempnode,sizeof(BinTreeNode),1,fp);
	fflush(fp);
	printf("结点信息成功写入文件中指定位置loc=%d\n",loc);
}

void readtreenode(FILE* fp, int loc, BinTreeNode tempnode[1]) {
	//将文件制定位置p的信息读入节点tempnode中
	rewind(fp);
	fseek(fp,loc*PIECE_BITSIZE+sizeof(node),SEEK_SET);
	fread(tempnode,sizeof(BinTreeNode),1,fp);
	printf("成功读取文件指定位置loc=%d的信息至结点中\n",loc);
}

//创建
int CreateBinTree( int* pav, FILE* fp) {
	printf("请输入链表的数据，以#结束：");
	ElemType x;
	scanf("%c",&x);
	int root=-50;
	BinTreeNode tempNode[1];
	if(x=='#') {
		printf("成功创建空二叉树\n");
		return;
	} else {
		int loc=AllocBoundTag(pav,sizeof(BinTreeNode),fp);
		tempNode->data=x;
		tempNode->leftChild=-80;
		tempNode->rightChild=-80;
		root=loc;
		writetreenode(fp,loc,tempNode);
		scanf("%c",&x);
	}
	while (x!='#') {
		int loc=AllocBoundTag(pav,sizeof(BinTreeNode),fp);
		tempNode->data=x;
		tempNode->leftChild=-80;
		tempNode->rightChild=-80;
		int T=root;
		int Tparent=root;
		int tag=0;//指示新结点在上一结点的哪一侧，0表示左孩子，1表示右孩子
		BinTreeNode TNode[1];
		while(T!=-80) {
			readtreenode(fp,T,TNode);
			if(x<=TNode->data) {
				Tparent=T;
				T=TNode->leftChild;
				tag=0;
			} else {
				Tparent=T;
				T=TNode->rightChild;
				tag=1;
			}
		}
		readtreenode(fp,Tparent,TNode);
		if(tag==0)
			TNode->leftChild=loc;
		else
			TNode->rightChild=loc;
		writetreenode(fp,Tparent,TNode);
		scanf("%c",&x);
	}
	printf("成功创建二叉树，二叉树的根结点为root=%d, 二叉树的中序遍历为：\n",phead);
	InOrder(int root)；
	return root;
}

//先序遍历
void PreOrder(int root, FILE* fp) {
	BinTreeNode tempNode[1];
	if(root!=-80) {
		readtreenode(fp,root,tempNode);
		printf("%c->",tempNode->data);
		int Tl=tempNode->leftChild;
		int Tr=tempNode->rightChild;
		PreOrder(Tl, fp);
		PreOrder(Tr, fp);
	}
}

//中序遍历
void InOrder(int root, FILE* fp) {
	BinTreeNode tempNode[1];
	if(root!=-80) {
		readtreenode(fp,root,tempNode);
		int Tl=tempNode->leftChild;
		int Tr=tempNode->rightChild;
		InOrder(Tl,fp);
		printf("%c->",tempNode->data);
		InOrder(Tr,fp);
	}
}

//后序遍历
void PostOrder(int root, FILE* fp) {
	BinTreeNode tempNode[1];
	if(root!=-80) {
		readtreenode(fp,root,tempNode);
		int Tl=tempNode->leftChild;
		int Tr=tempNode->rightChild;
		PostOrder(Tl, fp);
		PostOrder(Tr, fp);
		printf("%c->",tempNode->data);
	}
}

//查找key值所在的第一个结点
int BinTreeSearch(int root, FILE* fp) {
	//如果root为空树
	if(root==-80) {
		printf("该二叉树为空树，无法查找！\n");
		return -80;
	}
	//如果root不为空树
	BinTreeNode tempNode[1];
	int T=root;
	while(T!=-80) {
		readtreenode(fp,root,tempNode);
		if(tempNode->data==x) {
			printf("找到了key=%d所在的第一个结点，该结点位于文件中的第%d块\n",key,T);
			return T;
		}
		if(x<=tempNode->data) {
			T=tempNode->leftChild;
		} else {
			T=tempNode->rightChild;
		}
	}
	//如果找不到key
	printf("找不到key所在的结点\n");
	return -800; //-800表示在二叉树中找不到key

}

//在二叉树中插入值为key的结点
void InsertBTNode(int* root, ElemType x, int* pav, FILE* fp) {
	BinTreeNode tempNode[1];
	tempNode->data=x;
	tempNode->leftChild=-80;
	tempNode->rightChild=-80;
	int loc=AllocBoundTag(pav,sizeof(BinTreeNode),fp);
	writetreenode(fp,loc,tempNdoe);
	if(*root==-80) {
		*root=loc;
		return;
	}
	int tag=0;//指示新结点在上一结点的哪一侧，0表示左孩子，1表示右孩子
	int T=*root;
	int Tparent=*root;
	while(T!=-80) {
		readtreenode(fp,T,tempNode);
		,if(x<=tempNode->data) {
			tag=0;
			Tparent=T;
			T=tempNode->leftChild;
		} else {
			tag=1;
			Tparent=T;
			T=tempNode->rightChild;
		}
	}
	if(tag==-0) {
		readtreenode(fp,Tparent,tempNode);
		tempNode->leftChild=loc;
		writetreenode(fp,Tparent,tempNdoe);
	} else {
		readtreenode(fp,Tparent,tempNode);
		tempNode->rightChild=loc;
		writetreenode(fp,Tparent,tempNdoe);
	}
	printf("成功插入key=%c\n",key);
}

//清空二叉树
//外层调用函数---在使用时，调用此函数即可
void BinTreeClear(int* root, int* pav, FILE* fp) {
	BinTreeClear_(root, pav, fp);
	*root=-80;
	printf("成功清空二叉树\n");
}
//内层调用函数
void BinTreeClear_(int* root, int* pav, FILE* fp) {
	BinTreeNode tempNode[1];
	int T=*root;
	if(T!=-80) {
		readtreenode(fp,T,tempNode);
		int Tl=tempNode->leftChild;
		int Tr=tempNode->rightChild;
		BinTreeClear(&Tl, pav, fp);
		BinTreeClear(&Tr, pav, fp);
		Recover(pav,T,fp);
	}
}


