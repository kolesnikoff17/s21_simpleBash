#include "cat.h"

int main(int argc, char* argv[]) {
  files* fileList = init();
  flags fl = {0};
  flagsParsing(argc, argv, &fl, fileList);
  filesProcessing(&fileList, &fl);
  output(fileList, fl);
  destroyFiles(fileList);
  return 0;
}

files* init() {
  files* ptr;
  ptr = malloc(sizeof(files));
  if (!ptr) exit(0);
  ptr->name = NULL;
  ptr->stream = NULL;
  ptr->next = NULL;
  return ptr;
}

void flagsParsing(int argc, char* argv[], flags* fl, files* file) {
  files* curr = file;
  int i = 1;
  for (; i < argc && *argv[i] == '-'; i++) {
    if (gnuElif(argv[i], fl)) continue;
    if (!fl->err) catElif(argv[i], fl);
  }
  for (; i < argc; i++) {
    if (!fl->err) curr = addFile(curr, argv[i]);
  }
}

int gnuElif(char* argv, flags* fl) {
  int stop = 0;
  for (argv++; *argv; argv++) {
    if (!strcmp(argv, "-number-nonblank")) {
      fl->b = 1;
      stop++;
    } else if (!strcmp(argv, "-squeeze-blank")) {
      fl->s = 1;
      stop++;
    } else if (!strcmp(argv, "-number")) {
      fl->n = 1;
      stop++;
    }
  }
  return stop;
}

void catElif(char* argv, flags* fl) {
  for (argv++; *argv; argv++) {
    if (fl->err) break;
    if (*argv == 'e')
      fl->e = 1;
    else if (*argv == 'b')
      fl->b = 1;
    else if (*argv == 'E')
      fl->E = 1;
    else if (*argv == 'T')
      fl->T = 1;
    else if (*argv == 'n')
      fl->n = 1;
    else if (*argv == 's')
      fl->s = 1;
    else if (*argv == 't')
      fl->t = 1;
    else if (*argv == 'v')
      fl->v = 1;
    else
      fl->err = 4;
  }
}

void filesProcessing(files** fileList, flags* fl) {
  files* file = *fileList;
  for (files* curr = file->next; curr; curr = curr->next) {
    FILE* stream = fopen(curr->name, "r");
    if (!stream) {
      fl->err = 1;
      curr = removeFile(curr, file);
    } else
      curr->stream = stream;
  }
  if (file->next) file = removeFile(file, file);
  *fileList = file;
}

void output(files* file, flags fl) {
  if (fl.err == 4)
    errPrint(fl.err);
  else {
    if (fl.err) errPrint(fl.err);
    for (files* curr = file; curr; curr = curr->next) {
      catSup cs = {'\n', '\0', 1};
      unsigned char c;
      int noPrint = 0;
      for (fread(&c, 1, 1, curr->stream); !feof(curr->stream);
           fread(&c, 1, 1, curr->stream)) {
        fread(&cs.next, 1, 1, curr->stream);
        if (!feof(curr->stream)) fseek(curr->stream, -1, 1);
        if (fl.s) noPrint = catFlagS(c, &cs);
        if (!noPrint) {
          if (fl.b)
            cs.num = catFlagB(c, cs, fl);
          else if (fl.n)
            cs.num = catFlagN(cs, fl);
          if (fl.E) catFlagE(c);
          if (fl.T) catFlagT(c);
          if (fl.t || fl.e || fl.v) {
            catFlagV(c, fl);
          } else
            fputc(c, stdout);
        }
        // if (fl.s && !noPrint) c = '\n';
        cs.pr = c;
      }
    }
  }
}

int catFlagB(unsigned char c, catSup cs, flags fl) {
  if ((cs.pr == '\n' || (cs.pr == 138 && (fl.t || fl.e || fl.v))) &&
      c != '\n') {
    // if (cs.pr == '\n' && c != '\n') {
    printf("%6d\t", cs.num);
    cs.num++;
  }
  return cs.num;
}

void catFlagE(unsigned char c) {
  if (c == '\n') {
    printf("$");
  }
}

int catFlagN(catSup cs, flags fl) {
  if (cs.pr == '\n' || (cs.pr == 138 && (fl.t || fl.e || fl.v))) {
    // if (cs.pr == '\n') {
    printf("%6d\t", cs.num);
    cs.num++;
  }
  return cs.num;
}

int catFlagS(unsigned char c, catSup* cs) {
  int res = 0;
  if (c == '\n' && cs->pr == '\n' && cs->next == '\n') res++;
  return res;
}

void catFlagT(unsigned char c) {
  if (c == '\t') {
    printf("^I");
  }
}

void catFlagV(unsigned char c, flags fl) {
  if (c < 32) {
    if (c == '\n') {
      if (fl.e) printf("$");
      printf("\n");
    } else if (c == '\t' && !fl.t)
      printf("\t");
    else
      printf("^%c", ((int)c + 64));
  } else if (c == 127)
    printf("^?");
  // } else if (c >= 32 && c < 127) {
  //   printf("%c", c);
  else if ((c >= 128) && (c < (128 + 32)))
    printf("M-^%c", ((int)c - 64));
  else
    printf("%c", c);
  // } else if (c >= (128 + 32)) {
  //   printf("M-%c", ((int)c - 128));
  // } else {
  //   printf("M-?");
}
