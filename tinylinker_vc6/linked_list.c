#include <stdio.h>
#include <malloc.h>
#include <linked_list.h>
 
/*
int SL_Creat(SList *p_list,int size)
����
p_list��ָ��һ������ָ�룬�˴������ͷ��ַ
size��Ҫ������������������Ԫ�ؿռ������������ͷ�ڵ�
����ֵ
���ɹ�����1�����򷵻�0��
����
�ú����Ĺ����Ǵ���һ����СΪsize�������������ͷָ�븳��p_list��ָ������ָ�롣
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
����
list��Ҫ�������ݵĵ�����
int��ָ��Ҫ����Ԫ�ص�λ�ã���1��ʼ����
item:Ҫ�����������
����ֵ
���ɹ�����1�����򷵻�-1��
����
�ú����Ĺ�����������list��posλ�ò�����Ԫ�أ���ֵΪitem��
*/
int SL_Insert(SList list,int pos,Item item)
{
	PNode p,q;
	int i;
 
	p = list;
	i = 0;
	while(p!=NULL && i<pos-1)//��ָ��p�ƶ���Ҫ����Ԫ��λ��֮ǰ
	{
		p = p->next;
		i++;//i��¼pָ����ǵڼ���λ��
	}
	if(p==NULL || i > pos-1)
		return -1;
	q = (Node *)malloc(sizeof(Node));//δ����ڵ�����ڴ�
	if(q!=NULL)//���ڴ����ɹ������ڵ����ָ��λ��
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
����
list��Ҫ��ȡ���������ڵĵ�����
int��ָ��Ҫ��ȡԪ���ڵ������е�λ��
p_item:ָ��Ҫ����������ı���
����ֵ
���ɹ�����1�����򷵻�-1��
����
�ú����Ĺ����ǻ�ȡ������list��posλ�õ�Ԫ�ص��������ֵ����p_item��ָ�ı�����
*/
int SL_GetItem(SList list,int pos,Item *p_item)
{
	PNode p;
	int i;	
 
	p = list;
	i = 0;
	while(p!=NULL && i<pos)//��ָ��p�ƶ���Ҫ���ص�Ԫ��λ��
	{
		p = p->next;
		i++;//i��¼pָ����ǵڼ���λ��
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
����
list��Ҫɾ��Ԫ�����ڵĵ�����
int��ָ��Ҫɾ��Ԫ���ڵ������е�λ��
p_item:ָ�����ɾ��Ԫ�ص�������ı���
����ֵ
���ɹ�����1�����򷵻�-1��
����
�ú����Ĺ�����ɾ��������list��posλ�õ�Ԫ�أ���ֵ����p_item��ָ�ı�����
*/
int SL_Delete(SList list,int pos,Item * p_item)
{
	PNode p,q;
	int i;
	p = list;
	i = 0;
	while(p!=NULL && i<pos-1)//��ָ��p�ƶ���Ҫ����Ԫ��λ��֮ǰ
	{
		p = p->next;
		i++;//i��¼pָ����ǵڼ���λ��
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
����
list��Ҫ����Ԫ�����ڵĵ�����
int��ָ��Ҫ����Ԫ���ڵ������е�λ��
p_item:Ҫ����Ԫ�ص��������ֵ
����ֵ
���ɹ�����1�����򷵻�-1��
����
�ú����Ĺ����ǽ�����list��posλ�õ�Ԫ�ص�����������Ϊitem��
*/
int SL_SetItem(SList list,int pos,Item item)
{
	PNode p=NULL;
	int i;
	p = list;
	i = 0;
	while(p!=NULL && i<pos)//��ָ��p�ƶ���Ҫ����Ԫ��λ��֮ǰ
	{
		p = p->next;
		i++;//i��¼pָ����ǵڼ���λ��
	}
	if(p==NULL || i > pos)
		return -1;
	p->item = item;
	return 1;
 
}
/*
int SL_Find(SList list,int *pos,Item item)
����
list��Ҫ����Ԫ�����ڵĵ�����
int��ָ��Ҫ�洢�Ĳ�õ�Ԫ�ص�λ�õı���
p_item:Ҫ����Ԫ�ص��������ֵ
����ֵ
���ɹ�����1�����򷵻�-1��
����
�ú����Ĺ�����������list�в���������Ϊitem��Ԫ�أ�����λ��ֵ����pos��ָ�ı�����
*/
int SL_Find(SList list,int *pos,Item item)
{
	PNode p;
	int i;
	p = list;
	i = 0;
	while(p!=NULL && p->item!=item)//��ָ��p�ƶ���Ҫ����Ԫ��λ��֮ǰ
	{
		p = p->next;
		i++;//i��¼pָ����ǵڼ���λ��
		if(p->item == item)
		{
			*pos = i; //���ز�ѯ����λ��
			return 1;
		}
	}
	return -1;	
}
/*
int SL_Empty(SList list)
����
list��Ҫ�жϵĵ�����
����ֵ
��Ϊ���򷵻�1�����򷵻� 0��
����
�ú����Ĺ������ж�����list�Ƿ�Ϊ�ձ�
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
����
list��Ҫ���ҵĵ�����
����ֵ
���ذ����ڵ�ĸ�����
����
�ú����Ĺ����Ƿ�������list�нڵ�ĸ���������ͷ�ڵ㡣
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
����
p_list��ָ��Ҫ����ĵ�����
����ֵ
�ɹ�����ֵΪ1��
����
�ú����Ĺ����������������нڵ㣬����ͷ�ڵ㡣
*/
int SL_Clear(SList *p_list)
{
	PNode p,q;
	int i;
 
	p = *p_list;
	i = 0;
	while(p!=NULL)
	{
		q = p->next;//������q�洢p�����򣬷����ͷ�p���޷�����
		free(p);
		p = q;
	}
	*p_list = NULL;//����ָ������ָ����ΪNULL
	
	return 1;
}
