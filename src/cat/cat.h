#ifndef SRC_CAT_CAT_H_
#define SRC_CAT_CAT_H_

#include "../common/common.h"

typedef struct catSup {
  unsigned char pr;
  unsigned char next;
  int num;
} catSup;

typedef struct flags {
  int v;
  int E;
  int T;
  int b;
  int e;
  int n;
  int s;
  int t;
  int err;
} flags;

files* init();
void flagsParsing(int argc, char* argv[], flags* flags, files* file);
int gnuElif(char* argv, flags* fl);
void catElif(char* argv, flags* fl);
void filesProcessing(files** fileList, flags* fl);
void output(files* file, flags flags);
int catFlagB(unsigned char c, catSup catSup, flags fl);
void catFlagE(unsigned char c);
int catFlagN(catSup catSup, flags fl);
int catFlagS(unsigned char c, catSup* catSup);
void catFlagT(unsigned char c);
void catFlagV(unsigned char c, flags fl);

#endif  // SRC_CAT_CAT_H_