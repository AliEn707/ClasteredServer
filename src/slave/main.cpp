#include <cstdio>
#include <cstdlib>
#include <vector>
extern "C"{
#include <time.h>
#include <signal.h>
#include <unistd.h>

#ifndef __CYGWIN__
#include <execinfo.h>
#endif
}
#include "processors/message.h"
#include "lib/socket.h"
#include "lib/listener.h"
#include "lib/sync.h"
#include "lib/bytes_order.h"
#include "world.h"

using namespace clasteredServer;
using namespace clasteredServerSlave;

pthread_t startThread();//in thread.cpp

static void default_sigaction(int signal, siginfo_t *si, void *arg){
	printf("Stopping\n");
	world::main_loop=0;
}

static void segfault_sigaction(int sig){
	printf("Cought segfault, exiting\n");
#ifndef __CYGWIN__
	void *array[20];
	size_t size;
	
	// get void*'s for all entries on the stack
	size = backtrace(array, 20);

	// print out all the frames to stderr
	fprintf(stderr, "Error: signal %d:\n", sig);
	backtrace_symbols_fd(array, size, STDERR_FILENO);
#endif
	world::main_loop=(world::main_loop+1)&1;
	world::clear();
	exit(1);
}

int main(int argc, char* argv[]){
	int TPS=24;
	clasteredServer::sync syncer;
	struct sigaction sa;
	//pthread_t pid;
	int port=12345;
	
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = default_sigaction;
	sa.sa_flags   = SA_SIGINFO;
	//sigaction(SIGSEGV, &sa, NULL);	
	sigaction(SIGINT, &sa, NULL);	
	sigaction(SIGTERM, &sa, NULL);		
	signal(SIGSEGV, segfault_sigaction);
	
	if (argc>1)
		sscanf(argv[1], "%d", &port);
	
	processors::init();
	srand(time(0));
	//init map
	world::init();
	{
		//initialize listener
		listener l(port);
		world::sock=l.accept();
		//pid=
		startThread();
	}
	//wait for ready 
	while(!withLock(world::m, world::main_loop)){
		usleep(10000);
	}
	while(withLock(world::m, world::main_loop)){
		syncer.timePassed();
		//clear flags
		world::m.lock();
			for(std::map<int, npc*>::iterator it = world::npcs.begin(), end = world::npcs.end();it != end; ++it){
				npc* n=it->second;
				if (n){
					if (withLock(n->m, n->clear())){
						world::npcs[n->id]=0;
						delete n;
					}
				}
			}
		world::m.unlock();
		//now move
		world::m.lock();
			for(std::map<int, npc*>::iterator it = world::npcs.begin(), end = world::npcs.end();it != end; ++it){
				npc* n=it->second;
//				printf("n %d\n",n);
				if (n){
					n->m.lock();
//						printf("%d on %d from %d\n", n->id, world::id, n->gridOwner());
						if (world::id==n->gridOwner())
							n->move();
					n->m.unlock();
				}
			}
		world::m.unlock();
		//send data to players
		world::m.lock();
			for(std::map<int, player*>::iterator it = world::players.begin(), end = world::players.end();it != end; ++it){
				if (it->second)
					it->second->sendUpdates();
			}		
		world::m.unlock();
		//check areas
		world::m.lock();
			for(std::map<int, npc*>::iterator it = world::npcs.begin(), end = world::npcs.end();it != end; ++it){
				npc *n=it->second;
				if (n){
					int oid=n->gridOwner();
/*					printf("%d on (%g %g) %d :",n->id, n->position.x, n->position.y,oid);
					std::vector<int> shares=world::grid->getShares(n->position.x, n->position.y);
					for(unsigned i=0;i<shares.size();i++){
						printf("%d ", shares[i]);
					}
					printf(":\n");
*/	//				printf("%d on %d==%d\n", n->id, world::id, oid);
					if (world::id==oid){
						//i am owner
						n->m.lock();
							if (n->updated()){
								std::vector<int> shares=n->gridShares();
								n->pack(0,1);
								for(unsigned i=0;i<shares.size();i++){
									n->p.dest.id=shares[i];
									world::sock->send(&n->p);
								}
							}
						n->m.unlock();
					}else{
						//i am not owner		
						player *p;
	//					printf("i'm not owner\n");
						if ((p=world::players[n->owner_id])!=0){
							n->m.lock();
								n->pack(1,1);
								n->p.dest.id=oid;
								world::sock->send(&n->p);
							n->m.unlock();
							p->move();
						}
					}
				}
			}	
		world::m.unlock();
		syncer.syncTPS(TPS);
	}
	sleep(1);
	//cleanup
	//pthread_join(pid,0);
	world::clear();
	sleep(1);
	return 1;
}
