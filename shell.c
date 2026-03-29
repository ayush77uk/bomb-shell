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
  if(*s == '\0') return s;

  end = s + (strlen(s)-1);

  while(end>s && (*end==' ' || *end=='\t')) end--;
  *(end+1) = '\0';

  return s;
}

//split the input into tokens to separate commands and parameters
int split_command(char* input, char* args[], int max_args){

  int count = 0;
  char* token = strtok(input, " \t");

  while(token!=NULL && count<max_args-1){
    args[count] = token;
    count++;
    token = strtok(NULL, " \t");
  }

  args[count] = NULL;
  return count;
}

// Create process function
void run_command(char *command, HANDLE hOutput) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = hOutput;
    si.hStdError = hOutput; // optional: redirect errors too    

    if (!CreateProcess(
            NULL,
            command,
            NULL,
            NULL,
            TRUE,
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
            TRUE,
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
    
    char *redirect = strchr(command, '>');

    if(redirect){
      *redirect = '\0';
      char *cmd = trim(command);
      char *filename = trim(redirect+1);

      SECURITY_ATTRIBUTES sa;
      sa.nLength = sizeof(sa);
      sa.lpSecurityDescriptor = NULL;
      sa.bInheritHandle = TRUE;

      HANDLE hFile = CreateFile(
            filename,
            GENERIC_WRITE,
            0,
            &sa,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );
      if (hFile == INVALID_HANDLE_VALUE) {
      printf("Failed to open file.\n");
      continue;
      }

      run_command(cmd, hFile);
      CloseHandle(hFile);
    }

    if(!redirect){
      run_command(command, GetStdHandle(STD_OUTPUT_HANDLE));
    }

    // char* arg[64];
    // int arg_count = split_command(command, arg, 64);
    
    // for(int i=0; i<arg_count; i++){
    //   printf("arg[%d] = %s\n", i, arg[i]);
    // }
    
    // run_command(command);    
  }

  return 0;
}