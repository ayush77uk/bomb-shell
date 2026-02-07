#include <stdio.h>
#include <string.h>
#include <windows.h>

#define MAX_INPUT 1024

//function to check if input is blank
int empty_input(const char* s){
  while(*s){
    if(*s!=' ' && *s!='\t' && *s!='\n'){
      return 0;
    }
    s++;
  }
  return 1;
}

//trim input string
char* trim(char* s){
  char *end;

  while(*s==' ' || *s=='\t') s++;
  if(s==0) return s;

  end = s + (strlen(s)-1);

  while(end>s && (*end==' ' || *end=='\t')) end--;
  *(end+1) = '\0';

  return s;
}

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
      char full_cmd[1024];
      snprintf(full_cmd, sizeof(full_cmd), "cmd.exe /c %s", command);

      if(!CreateProcess(
            NULL,
            full_cmd,
            NULL,
            NULL,
            FALSE,
            0,
            NULL,
            NULL,
            &si,
            &pi)){
      printf("Failed to execute command.\n");
       return;
            }     
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

    if(empty_input(input_buff)){
      continue;
    }

    char *command = trim(input_buff);

    if(strcmp(command, "exit")==0){
      printf("Exiting the shell...\n");
      break;
    }

    if(strncmp(command, "cd ", 3)==0){
      char *path = command + 3;
      if(SetCurrentDirectory(path)){
        printf("Current directory changed to: %s\n", path);
        continue;
      }
      else{
        printf("Failed to change directory !\n");
        continue;
      }
    }
    
    run_command(command);    
  }

  return 0;
}