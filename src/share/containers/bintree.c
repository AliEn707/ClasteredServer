#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "bintree.h"
#include "worklist.h"
#include "../system/log.h"


//#define MAX_KEY (1<<((8L*sizeof(bintree_key))-2)) // if key more for each not works

/*
╔══════════════════════════════════════════════════════════════╗
║ 		functions for binary tree with data 			                       ║
║ created by Dennis Yarikov						                       ║
║ sep 2014									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

static inline bintree_key reverseKey(register bintree_key x){
	register bintree_key o=0;
	while (x){
		o=(o<<1)+(x&1);
		x>>=1;
	}
	return o;
}

bintree_key bintreeAdd(bintree* root, bintree_key key, void* data){
	bintree* tmp=root;
	int next; 
	bintree_key id=key;
	if (data==0)
		return 0;
//	printDebug("add key %d\n",key);
	while(key>0){
		next=key&1;
		if(tmp->next[next]==0){
			if((tmp->next[next]=malloc(sizeof(bintree)))==0){
				perror("malloc bintreeAdd");
				return 0;
			}
			memset(tmp->next[next],0,sizeof(bintree));
		}
		tmp=tmp->next[next];
		key>>=1;
	}
	if (tmp->data!=0)
		return 0;
	tmp->data=data;
	return id;
}

void * bintreeGet(bintree* root, bintree_key key){
	bintree* tmp=root;
	int next;
	while(key>0){
		next=key&1;
		if(tmp->next[next]==0)
			return 0;
		tmp=tmp->next[next];
		key>>=1;
	}
	return tmp->data;
}

//get or add object size of "size"
void * bintreeFetch(bintree* root,bintree_key key,int size){
	void * a=bintreeGet(root,key);
	if (a==0){
		a=malloc(size);
		bintreeAdd(root,key,a);
	}
	return a;
}

int _bintreeDel(bintree* root, bintree_key key, void (f)(void*v)){//TODO: remove recursion
	int get;
	int next;
	if (root==0)
		return 0;
//	printf("key %lld\n", key);
//	bits(key);
	if (key==0){
		if (f!=0)// && root->data)
			f(root->data);
//		free(root->data); //may be need
		root->data=0;
		if (root->next[0]==0 && root->next[1]==0){
			free(root);
//			printf("free\n");
			return 1;
		}
		return 0;
	}
	next=key&1;
//	printf("take next as %d nextkey %lld\n", next, key>>1) ;
	get=_bintreeDel(root->next[next],key>>1,f);
//	free(root->next[next]);
	if (get!=0){
		root->next[next]=0;
		if (root->next[(next+1)&1]==0 && !root->data){
//			printf("\tremove subnode %d %d\n",next,(next+1)&1);
			free(root);
			return 1;
		}
	}
	return 0;
}

int bintreeDel(bintree* root, bintree_key key, void (f)(void*v)){
	int get;
	int next=key&1;
	get=_bintreeDel(root->next[next],key>>1,f);
	if (get!=0)
		root->next[next]=0;
	return get;
}

/*
int bintreeDel(bintree* root, int key){
	bintree* tmp=root;
	int next;
	while(key>1){
		next=(int)(key&1);
		if(tmp->next[next]==0)
			return 0;
		tmp=tmp->next[next];
		key>>=1;
	}
	free(tmp->next[next]);
	tmp->next[next]=0;
	return 1;
}
*/
void bintreeErase(bintree * root,void (f)(void*v)){
	if (root==0)
		return;
//	printDebug("y\n");
	bintreeErase(root->next[0],f);
	bintreeErase(root->next[1],f);
	if (f!=0 && root->data)//on real root we have data == 0
		f(root->data);
	root->data=0;
	free(root->next[0]);
	root->next[0]=0;
	free(root->next[1]);
	root->next[1]=0;
}

static void* _bintreeForEach(bintree * root, void* (f)(bintree_key k, void *v, void *arg), void* arg, bintree_key key){
	if (root==0)
		return 0;
	int i;
	void* o;
//	printf("\t %d %d %d\n", key,reverseKey(key)>>1,reverseKey(key));
	if (root->data!=0)
		if ((o=f(reverseKey(key)>>1,root->data, arg))!=0)
			return o;
	for(i=0;i<2;i++)
		if ((o=_bintreeForEach(root->next[i],f,arg,(key<<1)+i))!=0)
			return o;
	return 0;
}

void* bintreeForEach(bintree * root, void*(f)(bintree_key k, void *v, void *arg), void *arg){
	if (root==0)
		return 0;
	return _bintreeForEach(root,f,arg,1);
}

int bintreeSize(bintree * root){
	int i=0;
	void* add(bintree_key k, void*v, void* arg){
		i++;
		return 0;
	}
	bintreeForEach(root,add, &i);
	return i;
}

//get all values as array
void** bintreeToArray(bintree * root){
	void** o;
	int i=bintreeSize(root);
	void* add(bintree_key k, void*v, void* arg){
		o[i]=v;
		i++;
		return 0;
	}
	if ((o=malloc(sizeof(*o)*i))==0)
		return 0;
	i=0;
	bintreeForEach(root, add, 0);
	return o;
}

//get clone of bintree
bintree* bintreeClone(bintree * root){
	bintree* o;
	void* add(bintree_key k, void*v, void* arg){
		bintreeAdd(o,k,v);
		return 0;
	}
	if ((o=malloc(sizeof(*o)))==0)
		return 0;
	memset(o,0,sizeof(*o));
	bintreeForEach(root,add,0);
	return o;
}

/*
void bits(bintree_key key){
	printf("bits for %lld  ",key);
	while(key){
		printf("%lld", key&1);
		key>>=1;
	}
	printf("\n");
}

int removeE(bintree_key k, void* v, void* a){
	int o;
	printf("remove key %lld value %lld\n",k,(long long)v);
	o=bintreeDel(a,k,0);
	printf("removed: %lld - %d\n", k, o);
	return 0;
}

int show(bintree_key k, void* v, void* a){
	printf("key %lld value %lld\n",k,(long long)v);
	return 0;
}

#define MAX_KEY 0

int main(){
	bintree r;
	bintree * z;
	int i;
	srand(time(0));
	memset(&r,0,sizeof(r));
	
	printf("%d -> %lld\n",11,reverseKey(11));
	bits(11);
	bits(reverseKey(11));
	
	bintreeAdd(&r,1,(void*)1);
	bintreeAdd(&r,3,(void*)100);
	bintreeAdd(&r,6,(void*)143);
	bintreeAdd(&r,7,(void*)132);
	printf("on 3: %d\n", (int)(long)bintreeGet(&r,3));
	printf("check each:\n");
	bintreeForEach(&r,show,0);
	printf("size %d\n", (int)bintreeSize(&r));
	free(bintreeToArray(&r));

	bintreeDel(&r,7,0);
	printf("size %d\n", (int)bintreeSize(&r));
//	bintreeDel(&r,3);
	printf("on 3: %d\n", (int)(long)bintreeGet(&r,3));
	printf("on 7: %d\n", (int)(long)bintreeGet(&r,7));
	bintreeErase(&r,0);
	
	printf("test of filling MAX_KEY/50\n");
	float q=0;
	int p=100;
	int keys=MAX_KEY/50;
	for(i=MAX_KEY-keys;i<MAX_KEY;i++){
		bintreeAdd(&r,i,(void*)(long)i);
		if (i%((int)(keys/p))==0){
			printf("%g%%.",q);
			fflush(stdout);
			q+=100.0/p;
		}
	}
	printf("\n");
	printf("size: %d\n", (int)bintreeSize(&r));
	printf("test of removing on each\n");
	bintreeForEach(&r,removeE,&r);
	printf("size: %d\n", (int)bintreeSize(&r));
	printf("now erase\n");
	bintreeErase(&r,0);
}

*/
