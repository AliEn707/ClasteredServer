#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h> 

#include "log.h"

/*
╔══════════════════════════════════════════════════════════════╗
║ 	good log printing 			                       ║
║ created by Dennis Yarikov						                       ║
║ jun 2016									                       ║
╚══════════════════════════════════════════════════════════════╝
*/

//can be used as printf
void printLog(const char* format, ...) {
	log_config* config = mainLogConfig();
	char str[800];
	char tstr[20]="";
	time_t t=time(0);
	strftime(tstr, sizeof(tstr), "%F %T", localtime(&t));
	va_list argptr;
	va_start(argptr, format);
		vsprintf(str, format, argptr);
	va_end(argptr);	
	
	if (config->debug)
		fprintf(stdout, "%s: %s", tstr, str);
	if (config->file[0]!=0){
		FILE *f=fopen(config->file, "a");
		if (f){
			fprintf(f, "%s: %s", tstr, str);
			fclose(f);
		}
	}
}