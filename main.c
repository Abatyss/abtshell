#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct {
  size_t capacity;
  size_t size;
  char **argsArray;
} ProgArgs;

ProgArgs createProgArgs() {
  ProgArgs args = {10, 0, (char **)malloc(sizeof(char *) * 10)};
  return args;
}

void resetProgArgs(ProgArgs *args) { args->size = 0; }

int resizeProgArgs(ProgArgs *args) {
  char **tmp =
      (char **)realloc(args->argsArray, sizeof(char *) * args->capacity * 2);
  if (tmp == NULL) {
    return -1;
  }
  args->capacity *= 2;
  args->argsArray = tmp;
  return 0;
}

int addArg(ProgArgs *args, char *arg) {
  if (args->size + 2 > args->capacity) {
    if (resizeProgArgs(args) == -1) {
      return -1;
    }
  }
  args->argsArray[args->size++] = strdup(arg);
  args->argsArray[args->size] = NULL;
  return 0;
}

int main(int argc, char *argv[]) {
  while (1) {
    char *buffer;
    size_t buffsize = 0;
    ssize_t characters;
    // Prompt user
    printf("abtsh$");
    // Process input
    getline(&buffer, &buffsize, stdin);
    ProgArgs args = createProgArgs();
    if (characters != -1) {
      size_t len = strlen(buffer);
      if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
      }
      char *token = strtok(buffer, " ");
      while (token != NULL) {
        addArg(&args, token);
        token = strtok(NULL, " ");
      }
      // Run program
      int rc = fork();
      if (rc < 0) {
        fprintf(stderr, "abtsh: Something went horribly wrong!");
      } else if (rc == 0) {
        execvp(args.argsArray[0], args.argsArray);
      } else {
        wait(NULL);
      }
    }
  }

  return EXIT_SUCCESS;
}
