#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

bool is_builtin(const char *command, const char *builtins[], size_t count) {
  for (size_t i = 0; i < count; i++) {
    if (strcmp(command, builtins[i]) == 0) {
      return true;
    }
  }
  return false;
}

static bool find_in_path(const char *command, char *resolved_path, size_t resolved_path_size) {
  const char *path_env = getenv("PATH");
  if (path_env == NULL || *path_env == '\0') {
    return false;
  }

  char path_copy[4096];
  strncpy(path_copy, path_env, sizeof(path_copy) - 1);
  path_copy[sizeof(path_copy) - 1] = '\0';

  char *dir = strtok(path_copy, ":");
  while (dir != NULL) {
    snprintf(resolved_path, resolved_path_size, "%s/%s", dir, command);
    if (access(resolved_path, X_OK) == 0) {
      return true;
    }
    dir = strtok(NULL, ":");
  }

  return false;
}

int main(int argc, char *argv[]) {
  const char *builtins[] = {"exit", "echo", "type"};
  char command[1024];
  setbuf(stdout, NULL);
  while (1) {

    printf("$ ");
    fgets(command, sizeof(command), stdin);
    command[strcspn(command, "\n")] = '\0';

    if (strcmp(command, "exit") == 0) {
      break;
    } 
    
    else if (strncmp(command, "echo ", 5) == 0) {
      printf("%s\n", command + 5);
    } 
    
    else if (strncmp(command, "type ", 5) == 0) {
      const char *arg = command + 5;
      if (is_builtin(arg, builtins, sizeof(builtins) / sizeof(builtins[0]))) {
        printf("%s is a shell builtin\n", arg);
      } else {
        char resolved_path[1024];
        if (find_in_path(arg, resolved_path, sizeof(resolved_path))) {
          printf("%s is %s\n", arg, resolved_path);
        } else {
          printf("%s: not found\n", arg);
        }
      }
    } 
    
    else {
      printf("%s: command not found\n", command);
    }
  }

  return 0;
}
