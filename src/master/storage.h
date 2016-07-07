#ifndef STORAGE_HEADER
#define STORAGE_HEADER


typedef
struct {
	char host[40];
	short port;
} slave_info;

typedef
struct {
	int id;
	char name[40];
} user_info;

typedef
struct {
	char file[100];
} storage_config;


storage_config* mainStorageConfig();

int storageInit();
int storageClear();

int storageSlaves(int(*f)(slave_info*,void*arg),void*arg);

int storageUsers(int(*f)(user_info*,void*arg),void*arg);
int storageUserById(int id, user_info* u);
int storageUserByName(char *name, user_info* u);

#endif