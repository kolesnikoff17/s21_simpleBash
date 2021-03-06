#ifndef SRC_GREP_GREP_H_
#define SRC_GREP_GREP_H_
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 40000

typedef struct templates {
  char* templ;
  int isFile;
  int fromFile;
  struct templates* next;
} templates;

typedef struct {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
  int multi;
  int err;
  int count;
} flags;

typedef struct files {
  char* name;
  FILE* stream;
  struct files* next;
} files;

typedef struct {
  flags* fl;
  templates* t;
  files* file;
} data;

files* addFile(files* elem, char* name);
files* removeFile(files* elem, files* root);
void destroyFiles(files* root);
void errPrint(int err);

void flagsParsing(int argc, char* argv[], data* store);
int checkIfParsed(const char* argv, templates* templ);
int flagsInfo(char* argv, data* store, templates** currTempl);
void templatesProcessing(data* store);
int newLineSearcher(FILE* stream);
void filesProcessing(data* store);
void output(data* store);
void printAtEOF(flags* fl, files* curr, int c);
int regexPrint(data* store, files* file, char* line, int n);
void flagOPrint(data* store, char* line, templates* curr, int prev, int head,
                int flag);
void vanillaPrint(data* store, files* file, char* line, int n, templates* curr);
data* init();
templates* addTemplate(templates* elem, char* name, int file, int from);
templates* removeTemplate(templates* elem, templates* root);
void destroyTemplates(templates* root);

#endif  // SRC_GREP_GREP_H_
