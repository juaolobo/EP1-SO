#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_PARAM 100
#define MAX_SIZE 100

void type_prompt() {
   char *cwd = NULL;
   char *user = NULL;

   cwd = getcwd(cwd, PATH_MAX+1);
   user = getenv("USER");

   printf("{%s@%s} ", user, cwd);
   free(cwd);
}

void read_command(char *command, char *parameters[]) {
   char *buf = readline("");
   int i = 0;
   int param = 1;

   if (buf == NULL){
      printf("\n");
      exit(0);
   }

   add_history(buf);

   for (int k = 0; k < MAX_PARAM; k++)
      parameters[k] = NULL;

   parameters[0] = malloc(sizeof(char)*MAX_SIZE);

   while (buf[i] != ' ') {
      command[i]  = buf[i];
      parameters[0][i] = buf[i];
      i++;
   }

   command[i] = '\0';
   parameters[0][i] = '\0';

   for (int j = i + 1; j < strlen(buf) + 1; j++) {
      if (buf[j] == ' ' || j == strlen(buf)) {
         int t, cur_char = 0;
         parameters[param] = malloc(sizeof(char)*MAX_SIZE);

         for (t = i + 1; t < j; t++)
            parameters[param][cur_char++] = buf[t];
         
         parameters[param][t] = '\0';
         param++;
         i = j;
      }
   }
}

int main (int argc, char **argv) {
   char command[MAX_SIZE];
   char *parameters[MAX_PARAM];
   pid_t childpid;
   
   while (1) {
      type_prompt();
      read_command(command, parameters);

      if ((childpid = fork()) != 0) {
         waitpid(childpid, NULL, 0);
      }

      else {
         if (strcmp(command, "mkdir") == 0) {
            if (mkdir(parameters[1], S_IFDIR) == -1)
               printf("O diretório não pôde ser criado.\n");
         }
         
         if (strcmp(command, "kill") == 0 && strcmp(parameters[1], "-9") == 0) {
            if (kill(atoi(parameters[2]), SIGKILL) == -1)
               printf("O processo não pôde ser terminado.\n");
         }

         if (strcmp(command, "ln") == 0 && strcmp(parameters[1], "-s") == 0) {
            if (symlink(parameters[2], parameters[3]))
               printf("Não foi possível criar o link desejado\n");
         }

         else 
            execve(command, parameters, NULL);
      }

   }

   printf("\n");

   for (int k = 0; k < MAX_PARAM && parameters[k] != NULL; k++)
      free(parameters[k]);

   return 0;
}
