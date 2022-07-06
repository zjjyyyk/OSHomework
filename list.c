#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "os.h"
#include "list.h"


void writelistnode(FILE* fp,int loc, SListNode tempnode[1]) {
	//将结点node信息写入文件中的指定位置loc
	fseek(fp,PIECE_BITSIZE*loc+sizeof(node),SEEK_SET);
	fwrite(tempnode,sizeof(SListNode),1,fp);
	fflush(fp);
	rewind(fp);
}

void readlistnode(FILE* fp, int loc, SListNode tempnode[1]) {
	//将文件制定位置p的信息读入节点tempnode中
	rewind(fp);
	fseek(fp,loc*PIECE_BITSIZE+sizeof(node),SEEK_SET);
	fread(tempnode,sizeof(SListNode),1,fp);
	rewind(fp);
}

void SListPrint(int phead,FILE* fp) {
	//显示链表内容
	int cur = phead;
	SListNode tempNode[1];
	printf("List data: ");
	while (cur != -40) {
		readlistnode(fp,cur,tempNode);
		printf("%d->", tempNode->data);
		cur = tempNode->next;
	}
	printf("\n");
}

void SListPushBack(int* phead, SLTDataType x, FILE* fp, int* pav) {
	//尾插
	//创建新节点
	SListNode tempNode[1];
	tempNode->data=x;
	tempNode->next=-40;
	int loc=AllocBoundTag(pav,sizeof(SListNode),fp);
	writelistnode(fp, loc, tempNode);

	//节点为空时直接链接
	if (*phead == -40) {
		*phead = loc;
	} else {
		//不为空时：找尾->链接
		int p=*phead;
		readlistnode(fp,p,tempNode);
		while(tempNode->next!=-40) {
			p= tempNode->next;
			readlistnode(fp,p,tempNode);
		}
		printf("找到尾结点tail=%d\n",p);
		//链接
		tempNode->next=loc;
		writelistnode(fp,p,tempNode);
		printf("成功在链表尾部插入数据x=%d\n",x);
	}
}

void SListPushFront(int* phead, SLTDataType x, FILE* fp, int* pav) {
	//头插
	//建立新结点

	SListNode tempNode[1];
	tempNode->data=x;
	tempNode->next=-40;
	int loc=AllocBoundTag(pav,sizeof(SListNode),fp);
	writelistnode(fp,loc,tempNode);
	if (*phead == -40) {
		*phead = loc;
	} else {
		readlistnode(fp,loc,tempNode);
		tempNode->next=*phead;
		*phead=loc;
		writelistnode(fp,loc,tempNode);
	}
}

void SListPopBack(int* phead, int* pav, FILE* fp) {
	//尾删

	//如果节点为空，那么不需要删除
	if (*phead == -40) {
		return;
	} else {
		int p=*phead;
		SListNode tempNode[1];
		readlistnode(fp,p,tempNode);
		//如果头结点为最后一个结点，则回收头结点所指向的空间，然后将头结点置为空（-40），
		if(tempNode->next==-40) {
			*phead=-40;
			Recover(pav,p,fp);
		} else {
			//如果头结点非空，则需找到最后一个结点所在的区块，回收最后一个结点所在的区域，将倒数第二个结点的指针置空（-40）
			int ppre=*phead;
			while(tempNode->next!=-40) {
				ppre=p;
				p=tempNode->next;
				readlistnode(fp,p,tempNode);
			}
			Recover(pav,p,fp);
			readlistnode(fp,ppre,tempNode);
			tempNode->next=-40;
			writelistnode(fp,ppre,tempNode);
		}
	}
}

void SListPopFront(int* phead,int* pav, FILE* fp) {
	//头删
	//如果链表为空链表，则不需要删除
	if (*phead == -40) {
		printf("该链表为空链表，无法进行删除操作\n");
		return;
	} else {
		SListNode tempNode[1];
		readlistnode(fp, *phead, tempNode);
		Recover(pav,*phead,fp);
		*phead=tempNode->next;
		printf("删除头结点成功，返回新的头结点序号\n");
	}
}

int SListFind(int* phead, SLTDataType x, FILE* fp) {
	//查找结点
	int p=*phead;
	SListNode tempNode[1];
	while(p!=-40) {
		readlistnode(fp,p,tempNode);
		if(tempNode->data==x)
			return p;
		else
			p=tempNode->next;
	}
	return -40;
}

void SListInsert(int* phead, int pos, SLTDataType x,int* pav,FILE* fp) {
	//在pos结点前插入一个新的结点
	SListNode tempNode[1];
	tempNode->data=x;
	tempNode->next=-40;
	int loc=AllocBoundTag(pav,sizeof(SListNode),fp);
	writelistnode(fp,loc,tempNode);
	if(*phead==-40) {
		//头结点为空,找不到pos结点，无法进行插入操作
		printf("头结点为空，找不到pos结点，无法进行插入操作\n");
		return;
	} else {
		int p=*phead;
		SListNode pNode[1];
		readlistnode(fp,p,pNode);
		if(*phead==pos) {
			SListPushFront(phead, x, fp, pav);
			printf("成功将数据x=%d插入结点pos之前，且新插入的结点为头结点\n",x);
		} else {
			while(pNode->next!=pos&&pNode->next!=-40) {
				p=pNode->next;
				readlistnode(fp,p,pNode);
			}
			if(pNode->next==-40) {
				printf("链表中找不到储存在pos=%d中的结点\n",pos);
				return;
			}
			readlistnode(fp,loc,tempNode);
			pNode->next=loc;
			tempNode->next=pos;
			writelistnode(fp,loc,tempNode);
			writelistnode(fp,p,pNode);
			printf("成功将数据x=%d插入结点pos之前\n",x);
		}
	}
}

void SListTrueInsert(int* phead, int index, SLTDataType x,int* pav,FILE* fp) {
	int p = *phead;
	SListNode tempNode[1];
	if(p!=-40) readlistnode(fp,p,tempNode);
	int i;
	for(i=0;i<index;i++){
		p = tempNode->next;
		if(p==-40){
			printf("No such index.\n");
			return;
		}
		readlistnode(fp,p,tempNode);
	}
	SListInsert(phead,p,x,pav,fp);
}

void SListErase(int* phead, int pos, int* pav, FILE* fp) {
	//删除pos结点前面的结点
	//节点为空即不用删除
	if (*phead == -40) {
		printf("头结点为空，无法进行删除操作\n");
		return;
	} else if(*phead==pos) {
		printf("pos=%d为头结点，无法进行删除操作\n");
		return;
	} else {
		SListNode pNode[1];
		int p=*phead;
		int ppre=*phead;
		readlistnode(fp,p,pNode);
		if(pNode->next==pos) {
			SListPopFront( phead, pav, fp);
			printf("成功删除pos=%d之前的结点，且\n",pos);
			return;
		}
		while(pNode->next!=pos&&pNode->next!=-40) {
			ppre=p;
			p=pNode->next;
			readlistnode(fp,p,pNode);
		}
		if(pNode->next==-40) {
			printf("链表中找不到储存在pos=%d中的结点\n",pos);
			return;
		}
		SListNode ppreNode[1];
		readlistnode(fp,ppre,ppreNode);
		ppreNode->next=pos;
		Recover(pav,p,fp);
		writelistnode(fp,ppre,ppreNode);
		printf("成功删除pos=%d之前的结点\n",pos);
	}
}

void SListTrueErase(int* phead, int index, int* pav, FILE* fp) {
	int p = *phead;
	SListNode tempNode[1];
	readlistnode(fp,p,tempNode);
	int i;
	for(i=0;i<index;i++){
		p = tempNode->next;
		if(p==-40){
			printf("No such index.\n");
			return;
		}
		readlistnode(fp,p,tempNode);
	}
	p = tempNode->next;
	SListErase(phead,p,pav,fp);
}


void SListInsertAfter(int* phead, int pos, SLTDataType x, int* pav, FILE* fp) {
	//在pos结点后面插入一个结点
	//头结点为空，找不到pos结点，无法进行插入操作
	if (*phead == -40) {
		printf("头结点为空，找不到pos结点，无法进行插入操作\n");
		return;
	} else {
		SListNode newNode[1];
		int loc=AllocBoundTag(pav,sizeof(SListNode),fp);
		newNode->data=x;
		SListNode posNode[1];
		readlistnode(fp,pos,posNode);
		newNode->next = posNode->next;
		posNode->next=loc;
		writelistnode(fp,loc,newNode);
		writelistnode(fp,pos,posNode);
		printf("成功在pos节点后插入新的结点\n");
	}
}

void SListEraseAfter(int* phead, int pos, int* pav, FILE* fp) {
	// 删除pos结点后面的一个结点
	//如果pos结点为最后一个结点，则无法进行删除操作
	SListNode posNode[1];
	readlistnode(fp,pos,posNode);
	if(posNode->next==-40) {
		printf("pos为最后一个结点，无法进行删除操作\n");
		return;
	} else {
		int next=posNode->next;
		SListNode nextNode[1];
		readlistnode(fp,next,nextNode);
		posNode->next=nextNode->next;
		Recover(pav,next,fp);
		writelistnode(fp,pos,posNode);
		printf("成功删除pos后面的一个结点\n");
	}
}

void SListDestory(int* phead, int* pav, FILE* fp) {
	//释放链表
	int cur = *phead;
	SListNode tempNode[1];
	while (cur!=-40) {
		readlistnode(fp,cur,tempNode);
		Recover(pav,cur,fp);
		cur=tempNode->next;
	}
	*phead=-40;
	printf("成功释放链表\n");
}

int InitList(int* pav, FILE* fp, SLTDataType* elems, int count){
	//初始化链表 
	int i=0;
	int phead=-40;
	while (i<count){
		SListPushBack(&phead, elems[i], fp, pav);
		i++;
	}
	return phead; 
}


status cmd_List(FILE* fp, int* pav, char* commend, ListInfo* tempphead){
    printf("ListCommend:%s END, pav : %d\n",commend,*pav);
    char init[] = "init";  
    char insert[] = "insert";  
    char erase[] = "erase"; 
	char find[] = "find"; 
    char display[] = "display";
	char destroy[] = "destroy"; 
    char cmd[128] = { 0 };
    int i = 0;

    int tempflag = 1;
    while (commend[i] != ' ' && tempflag) {
        if(commend[i]==0) tempflag = 0;
        cmd[i] = commend[i];
        i++;
    }
    if(strcmp(cmd,init)==0){
		for (; isblank(commend[i]);i++);
        int count = 0;
        SLTDataType data[100] = {0};
        while( isdigit(commend[i]) || isblank(commend[i])){
            for (; isblank(commend[i]);i++);
            int n = 0;
            while (isdigit(commend[i])) {
                n = 10 * n + commend[i] - 48;
                i++;
            }
            data[count] = n;
            count++;
        }
        *tempphead =  InitList(pav,fp,data,count);
        printf("Successfully initList, ListInfo: [ phead = %d ]\n",*tempphead);      
        return true;
    }
    else if(strcmp(cmd,insert)==0){
        for (; isblank(commend[i]);i++);
        int index = 0;
        while (isdigit(commend[i])) {
            index = 10 * index + commend[i] - 48;
            i++;
        }
        for (; isblank(commend[i]);i++);
        SLTDataType elem = 0;
        while (isdigit(commend[i])) {
            elem = 10 * elem + commend[i] - 48;
            i++;
        }
        SListTrueInsert(tempphead,index,elem,pav,fp);
        printf("Successfully insertList [ index = %d, elem = %d ], ListInfo: [ phead = %d ]\n",index,elem,*tempphead);
        return true;
    }
    else if(strcmp(cmd,erase)==0){
        for (; isblank(commend[i]);i++);
        int index = 0;
        while (isdigit(commend[i])) {
            index = 10 * index + commend[i] - 48;
            i++;
        }
        SListTrueErase(tempphead,index,pav,fp);
        printf("Successfully eraseList [ index = %d ], ListInfo: [ phead = %d ]\n",index,*tempphead);
        return true;
    }
	else if(strcmp(cmd,find)==0){
		for (; isblank(commend[i]);i++);
        SLTDataType elem = 0;
        while (isdigit(commend[i])) {
            elem = 10 * elem + commend[i] - 48;
            i++;
        }
        int loc = SListFind(tempphead,elem,fp);
        printf("Successfully findList [ elem = %d ], result: [ loc = %d ], ListInfo: [ phead = %d ]\n",elem,loc,*tempphead);
        return true;
    }
    else if(strcmp(cmd,display)==0){
        SListPrint(*tempphead,fp);
        return true;
    }
	else if(strcmp(cmd,destroy)==0){
        SListDestory(tempphead,pav,fp);
        return true;
    }
    else{
        printf("Stack dosen't have this commend.\n");
        return true;
    }
}
