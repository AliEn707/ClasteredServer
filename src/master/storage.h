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
	char login[40];
	char name[40];
	char passwd[50];
} user_info;

typedef
struct {
	char file[100];
} storage_config;


storage_config* mainStorageConfig();

int storageInit();
int storageClear();

int storageSlaves(int(*f)(slave_info*,void*arg),void*arg);
int storageSlaveSetBroken(char *host, short port);
int storageSlaveSetUnbroken(char *host, short port);

int storageUsers(int(*f)(user_info*,void*arg),void*arg);
int storageUserById(int id, user_info* u);
int storageUserByLogin(char *login, user_info* u);

int storageAttributeGet(int id, const char* k, char* out);
int storageAttributeSet(int id, const char* k, const char* v);

int storageAttributesGet(int id, char** k, char** out, short size);
int storageAttributesForEach(int id, char** ks, short size, void*(f)(char* k,char*v, void*arg), void*arg);
int storageAttributesSet(int id, char** k, char** v, short size);

#endif