#ifdef STORAGE_TEXT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../share/containers/worklist.h"
#include "../../share/system/log.h"
#include "../storage.h"

/*
╔══════════════════════════════════════════════════════════════╗
║ 		text files storage implementation 			                       ║
║ created by Dennis Yarikov						                       ║
║ jul 2016									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

#define SLAVES_FILE_DEFAULT "storage/slaves.txt"
#define USERS_FILE_DEFAULT "storage/users.txt"
typedef
struct{
	char users[100];
	char slaves[100];
} files_config;

static files_config files;

#define getUserInfo(f, u) fscanf(f, "%d %s %s", &(u)->id, (u)->name, (u)->passwd)

int storageInit(){
	storage_config* conf=mainStorageConfig();
	FILE* f;
	if (conf->file[0]!=0 && ((f=fopen(conf->file, "rt"))!=0)){
		while(feof(f)==0){
			char buf[100];
			fscanf(f,"%s",buf);
			if (strcmp(buf,"users_file")==0){
				fscanf(f, "%s", files.users);
			}else
			if (strcmp(buf,"slaves_file")==0){
				fscanf(f, "%s", files.slaves);
			}
		}
		fclose(f);
		return 0;
	}
	//storageConfigParse(&files);
	//TODO: change
	printf("can open storage config file %s, using defaults\n", conf->file);
	sprintf(files.slaves,"%s",SLAVES_FILE_DEFAULT);
	sprintf(files.users,"%s",USERS_FILE_DEFAULT);
	return 1;
}

int storageClear(){
	return 0;
}

int storageSlaves(int(*$)(slave_info*,void*arg),void*arg){
	FILE *f=fopen(files.slaves,"rt");
	slave_info s;
	int i=0;
	if (f){
		while(feof(f)==0){
			i++;
			memset(&s,0,sizeof(s));
			if (fscanf(f,"%s %hd\n", s.host, &s.port)!=2){
				printf("error in %s on line %d\n", files.slaves, i);
				continue;
			}
			$(&s, arg);
		}
		fclose(f);
		return 0;
	}
	perror("fopen");
	return 1;
}


int storageUsers(int(*$)(user_info*,void*arg),void*arg){
	FILE *f=fopen(files.users,"rt");
	user_info u;
	if (f){
		while(feof(f)==0){
			memset(&u,0,sizeof(u));
			if (getUserInfo(f, &u)==2){
				$(&u, arg);
			}
		}
		fclose(f);
		return 0;
	}
	perror("fopen");
	return 1;
}

int storageUserById(int id, user_info* u){
	FILE *f=fopen(files.users,"rt");
	if (f){
		while(feof(f)==0){
			getUserInfo(f, u);
			if (u->id==id){
				printf("user with id %d found\n", id);
				break;
			}
		}
		fclose(f);
		if (u->id==id)
			return 0;
		printf("user with id %d not found\n", id);
		memset(u,0,sizeof(*u));
		return -1;
	}
	perror("fopen");
	return 1;
}

int storageUserByName(char* name, user_info* u){
	short $=0;
	FILE *f=fopen(files.users,"rt");
	if (f){
		while(feof(f)==0){
			getUserInfo(f, u);
			if (strcmp(u->name,name)==0){
				printf("user with name %s found\n", name);
				$=1;
				break;
			}
		}
		fclose(f);
		if ($)
			return 0;
		printf("user with name %s not found\n", name);
		memset(u,0,sizeof(*u));
		return -1;
	}
	perror("fopen");
	return 1;
}

#endif