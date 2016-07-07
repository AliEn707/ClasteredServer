#ifndef WORKLIST_HEADER
#define WORKLIST_HEADER

typedef
struct worklist{
	void * data;
	struct worklist * next;
} worklist;

worklist * worklistAdd(worklist * root, void* data);

worklist * worklistDel(worklist * root, void* data);

void worklistErase(worklist* root, void (f)(void*));

// remove object if f()!=0
void worklistForEachRemove(worklist* root, void*(f)(void* w, void* arg), void* arg);

//return if f()!=0
void* worklistForEachReturn(worklist* root, void*(f)(void* w, void* arg), void* arg);

#endif