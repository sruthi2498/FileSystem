#include "all_include.h"
#include "write_to_log.h"

void LogWrite(char * string){
	fp=fopen("log.txt","a+");
	time_t current_time;
    char* c_time_string;
	 /* Obtain current time. */
    current_time = time(NULL);
	/* Convert to local time format. */
    c_time_string = ctime(&current_time);
    c_time_string[strlen(c_time_string)-1]='\0';
    strcat( c_time_string,": ");
	fwrite(c_time_string,1, strlen(c_time_string),fp);
	fwrite(string,1,strlen(string),fp);
	fclose(fp);
}


void ResetLogFile(){
	fp=fopen("log.txt","w");
	fclose(fp);
}

// int main(){
// 	ResetLogFile();
// 	LogWrite("abc\n");
// 	LogWrite("def\n");

// }