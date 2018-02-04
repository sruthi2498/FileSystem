#include "all_include.h"
#include "write_to_log.h"
void LogWrite(char * string){
	fp=fopen("log.txt","a+");
	fwrite(string,1,strlen(string),fp);
	fclose(fp);
}


void ResetLogFile(){
	fp=fopen("log.txt","w");
	fclose(fp);
}