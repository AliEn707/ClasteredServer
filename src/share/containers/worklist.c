#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "worklist.h"
#include "../system/log.h"


/*
╔══════════════════════════════════════════════════════════════╗
║ 		functions for l1 list with data                                                                                 ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/


worklist * worklistAdd(worklist * root, void* data){
	worklist * tmp;
	if (data==0)
		return 0;
	for(tmp=root;tmp->next!=0;tmp=tmp->next);
	if ((tmp->next=malloc(sizeof(worklist)))==0){
		perror("malloc worklistAdd");
		return 0;
	}
	memset(tmp->next,0,sizeof(worklist));
	tmp->next->data=data;
	return tmp->next;
}

worklist * worklistDel(worklist * root, void* data){
	worklist* tmp, * tmp1;
	for(tmp=root;tmp->next!=0;tmp=tmp->next)
		if (tmp->next->data==data)
			break;
	if (tmp->next==0)
		return 0;
	tmp1=tmp->next;
	tmp->next=tmp->next->next;
	free(tmp1);
	return tmp;
}

void worklistErase(worklist* root, void (f)(void*)){
	if (root==0 || root->next==0)
		return;
	worklistErase(root->next, f);
	if (f)// && root->next->data)
		f(root->next->data);
	free(root->next);
	root->next=0;
}

void worklistForEachRemove(worklist* root, void*(f)(void* w, void* arg), void* arg){
	worklist * tmp=root;
	worklist * _tmp=tmp;
	for(tmp=tmp->next;tmp!=0;tmp=tmp->next){
		if (f(tmp->data, arg)!=0)
			tmp=worklistDel(_tmp,tmp->data);
		_tmp=tmp;//TODO:check for errors
	}
}

void* worklistForEachReturn(worklist* root, void*(f)(void* w, void* arg), void* arg){
	worklist * tmp=root;
	void *o=0;
	for(tmp=tmp->next;tmp!=0;tmp=tmp->next){
		if ((o=f(tmp->data, arg))!=0)
			return o;
	}
	return o;
}

/*
int main(){
	worklist l;
	memset(&l,0,sizeof(l));
	worklistAdd(&l,5);
	worklistAdd(&l,7);
	worklistDel(&l,7);
	worklistErase(&l);
}
*/