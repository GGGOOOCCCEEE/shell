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

static int tokenize(char *input, char *argv[], int max_args) {
  int argc = 0;
  char *token = strtok(input, " \t");

  while (token != NULL && argc < max_args - 1) {
    argv[argc++] = token;
    token = strtok(NULL, " \t");
  }

  argv[argc] = NULL;
  return argc;
}


int main(int argc, char *argv[]) {
  const char *builtins[] = {"exit", "echo", "type"};
  char command[1024];
  setbuf(stdout, NULL);
  while (1) {

    printf("$ ");
    if (fgets(command, sizeof(command), stdin) == NULL) {
      break;
    }
    command[strcspn(command, "\n")] = '\0';

    char *args[128];
    int arg_count = tokenize(command, args, 128);
    if (arg_count == 0) {
      continue;
    }

    if (strcmp(args[0], "exit") == 0) {
      break;
    } 
    
    else if (strcmp(args[0], "echo") == 0) {
      for (int i = 1; i < arg_count; i++) {
        if (i > 1) {
          printf(" ");
        }
        printf("%s", args[i]);
      }
      printf("\n");
    } 
    
    else if (strcmp(args[0], "type") == 0) {
      if (arg_count < 2) {
        printf("\n");
      } else if (is_builtin(args[1], builtins, sizeof(builtins) / sizeof(builtins[0]))) {
        printf("%s is a shell builtin\n", args[1]);
      } else {
        char resolved_path[1024];
        if (find_in_path(args[1], resolved_path, sizeof(resolved_path))) {
          printf("%s is %s\n", args[1], resolved_path);
        } else {
          printf("%s: not found\n", args[1]);
        }
      }
    } 

    else {
      printf("%s: command not found\n", args[0]);
    }
  }

  return 0;
}
