#ifndef T_SEM_HEADER
#define T_SEM_HEADER

#include <pthread.h>
#include <stdlib.h>

#ifndef t_sem
struct sembuf {
	short sem_num;
	short sem_op;
	short flags;
};
#endif

struct t_sem_t_ {
	pthread_mutex_t mutex;
	short *val;
};

//typedef int t_sem_t;
typedef struct t_sem_t_* t_sem_t;
//#define t_sem_t int
/*
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
*/

#ifndef IPC_PRIVATE

#define IPC_PRIVATE 0
#define IPC_RMID 4096
#define IPC_CREAT  0x0200

#endif
#define t_semop semop
#define t_semctl semctl
#define t_semget semget



#undef t_semop
#undef t_semctl
#undef t_semget

int t_semop(t_sem_t semid, struct sembuf *sops, unsigned nsops);
#define t_semSet(sem, num, op) {struct sembuf sops={num, op, 0};t_semop(sem, &sops, 0);} 

t_sem_t t_semget(key_t key, int nsems, int semflg);
#define t_semGet(n) t_semget(0,n,0)

int t_semctl(t_sem_t semid, int semnum, int cmd);
#define t_semRemove(sem) t_semctl(sem,0,IPC_RMID)


#endif