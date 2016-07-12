#include <stdio.h>
#include <string.h>

#include "../share/base64.h"
#include "../share/md5.h"


int main(int argc, char *argv[]){
	char md5[20], token[100];
	if (argc!=2){
		printf("You must enter password as argument\n");
		return 1;
	}
	MD5_Create((void*)argv[1], strlen(argv[1]), md5); 
	base64_encode((void*)md5, (void*)token, 16, 0);
	printf("%s\n", token);
	return 0;
}