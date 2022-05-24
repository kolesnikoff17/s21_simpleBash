#ifndef SRC_COMMON_COMMON_H_
#define SRC_COMMON_COMMON_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct files {
  char* name;
  FILE* stream;
  struct files* next;
} files;

files* addFile(files* elem, char* name);
files* removeFile(files* elem, files* root);
void destroyFiles(files* root);
void errPrint(int err);

#endif  // SRC_COMMON_COMMON_H_