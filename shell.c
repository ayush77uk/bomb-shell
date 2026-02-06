#include <stdio.h>
#include <string.h>
#include <windows.h>

#define MAX_INPUT 1024

// Create process function
void run_command(char *command) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcess(
            NULL,
            command,
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi)) {
        printf("Failed to execute command.\n");
        return;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}


// MAIN FUNCTION-----------------------------
int main(){
  char input_buff[MAX_INPUT];

  while(1){
    printf("bomb-shell> ");
    fflush(stdout);

    if(fgets(input_buff, MAX_INPUT, stdin) == NULL){
      break;
    }

    input_buff[strcspn(input_buff, "\n")] = '\0';

    if(strcmp(input_buff, "exit")==0){
      printf("Exiting the shell...\n");
      break;
    }

    if(strncmp(input_buff, "cd ", 3)==0){
      char *path = input_buff + 3;
      if(SetCurrentDirectory(path)){
        printf("Current directory changed to: %s\n", path);
        continue;
      }
      else{
        printf("Failed to change directory !\n");
        continue;
      }
    }
    
    run_command(input_buff);    
  }

  return 0;
}