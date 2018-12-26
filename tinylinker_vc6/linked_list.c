#include <stdio.h>
#include <malloc.h>
#include <linked_list.h>
 
/*
int SL_Creat(SList *p_list,int size)
参数
p_list：指向一个链表指针，此处传入表头地址
size：要创建的链表分配的数据元素空间个数，不包含头节点
返回值
若成功返回1，否则返回0。
功能
该函数的功能是创建一个大小为size的链表并把链表的头指针赋给p_list所指的链表指针。
*/
int SL_Create(SList *p_list,int size)
{
	PNode p=NULL;
	int i;
 
	*p_list = (SList)malloc(sizeof(Node));
	if(*p_list==NULL)
		return -1;
	(*p_list)->next = NULL;
	for(i=size;i>0;i--)
	{
		p = (PNode)malloc(sizeof(Node));
		if(p==NULL)
			return -1;
		p->item = 0;
		p->next = (*p_list)->next;
		(*p_list)->next = p;
	}
	return 1;
}
/*
int SL_Insert(SList list,int pos,Item item)
参数
list：要插入数据的单链表
int：指定要插入元素的位置，从1开始计数
item:要插入的数据项
返回值
若成功返回1，否则返回-1。
功能
该函数的功能是在链表list的pos位置插入新元素，其值为item。
*/
int SL_Insert(SList list,int pos,Item item)
{
	PNode p,q;
	int i;
 
	p = list;
	i = 0;
	while(p!=NULL && i<pos-1)//将指针p移动到要插入元素位置之前
	{
		p = p->next;
		i++;//i记录p指向的是第几个位置
	}
	if(p==NULL || i > pos-1)
		return -1;
	q = (Node *)malloc(sizeof(Node));//未插入节点分配内存
	if(q!=NULL)//若内存分配成功，将节点插入指定位置
	{
		q->item = item;
		q->next = p->next;
		p->next = q;
		return 1;
	}
	else
	{
		return -1;
	}
}
/*
int SL_GetItem(SList list,int pos,Item *p_item)
参数
list：要获取数据项所在的单链表
int：指定要获取元素在单链表中的位置
p_item:指向要接收数据项的变量
返回值
若成功返回1，否则返回-1。
功能
该函数的功能是获取在链表list的pos位置的元素的数据项，其值赋给p_item所指的变量。
*/
int SL_GetItem(SList list,int pos,Item *p_item)
{
	PNode p;
	int i;	
 
	p = list;
	i = 0;
	while(p!=NULL && i<pos)//将指针p移动到要返回的元素位置
	{
		p = p->next;
		i++;//i记录p指向的是第几个位置
	}
	if((p==NULL)||(i>pos))
	{
		return -1;
	}
	*p_item = p->item;
	return 1;
}
/*
int SL_Delete(SList list,int pos,Item * p_item)
参数
list：要删除元素所在的单链表
int：指定要删除元素在单链表中的位置
p_item:指向接收删除元素的数据项的变量
返回值
若成功返回1，否则返回-1。
功能
该函数的功能是删除在链表list的pos位置的元素，其值赋给p_item所指的变量。
*/
int SL_Delete(SList list,int pos,Item * p_item)
{
	PNode p,q;
	int i;
	p = list;
	i = 0;
	while(p!=NULL && i<pos-1)//将指针p移动到要插入元素位置之前
	{
		p = p->next;
		i++;//i记录p指向的是第几个位置
	}
	if(p->next==NULL || i > pos-1)
		return -1;
	q = p->next;
	p->next = q->next;
	if(p_item != NULL)
		*p_item = q->item;
	free(q);
	return 1;
}
/*
int SL_SetItem(SList list,int pos,Item item)
参数
list：要设置元素所在的单链表
int：指定要设置元素在单链表中的位置
p_item:要设置元素的数据项的值
返回值
若成功返回1，否则返回-1。
功能
该函数的功能是将链表list的pos位置的元素的数据项设置为item。
*/
int SL_SetItem(SList list,int pos,Item item)
{
	PNode p=NULL;
	int i;
	p = list;
	i = 0;
	while(p!=NULL && i<pos)//将指针p移动到要插入元素位置之前
	{
		p = p->next;
		i++;//i记录p指向的是第几个位置
	}
	if(p==NULL || i > pos)
		return -1;
	p->item = item;
	return 1;
 
}
/*
int SL_Find(SList list,int *pos,Item item)
参数
list：要查找元素所在的单链表
int：指向要存储的查得的元素的位置的变量
p_item:要查找元素的数据项的值
返回值
若成功返回1，否则返回-1。
功能
该函数的功能是在链表list中查找数据项为item的元素，将其位置值赋给pos所指的变量。
*/
int SL_Find(SList list,int *pos,Item item)
{
	PNode p;
	int i;
	p = list;
	i = 0;
	while(p!=NULL && p->item!=item)//将指针p移动到要插入元素位置之前
	{
		p = p->next;
		i++;//i记录p指向的是第几个位置
		if(p->item == item)
		{
			*pos = i; //返回查询到的位置
			return 1;
		}
	}
	return -1;	
}
/*
int SL_Empty(SList list)
参数
list：要判断的单链表
返回值
若为空则返回1，否则返回 0。
功能
该函数的功能是判断链表list是否为空表。
*/
int SL_Empty(SList list)
{
	PNode p;
	p = list;
	if(p->next == NULL)
		return 1;
	return 0;
}
/*
int SL_Size(SList list)
参数
list：要查找的单链表
返回值
返回包含节点的个数。
功能
该函数的功能是返回链表list中节点的个数，包含头节点。
*/
int SL_Size(SList list)
{
	PNode p;
	int i;
 
	p = list;
	i = 0;
	while(p!=NULL)
	{
		p = p->next;
		i++;
		
	}
	return i;
}
/*
int SL_Clear(SList *p_list)
参数
p_list：指向要清除的单链表
返回值
成功返回值为1。
功能
该函数的功能是清除链表的所有节点，包含头节点。
*/
int SL_Clear(SList *p_list)
{
	PNode p,q;
	int i;
 
	p = *p_list;
	i = 0;
	while(p!=NULL)
	{
		q = p->next;//借助于q存储p的链域，否则释放p后无法引用
		free(p);
		p = q;
	}
	*p_list = NULL;//将所指的链表指针设为NULL
	
	return 1;
}
