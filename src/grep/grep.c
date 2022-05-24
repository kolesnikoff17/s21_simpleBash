#include "grep.h"

int main(int argc, char** argv) {
  data* store = init();
  flags fl = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  store->fl = &fl;
  flagsParsing(argc, argv, store);
  templatesProcessing(store);
  // printf("1\n");
  if (store->file->next) filesProcessing(store);
  // printf("1\n");
  output(store);
  // printf("1\n");
  destroyFiles(store->file);
  destroyTemplates(store->t);
  free(store);
  return 0;
}

void flagsParsing(int argc, char* argv[], data* store) {
  templates* curr = store->t;
  files* currFile = store->file;
  for (int i = 1; i < argc; i++) {
    if (!store->fl->err) {
      switch (flagsInfo(argv[i], store, &curr)) {
        case 3:
          curr = addTemplate(curr, argv[++i], 1, 0);
          break;
        case 4:
          curr = addTemplate(curr, argv[++i], 0, 0);
          break;
      }
    }
  }
  for (int i = 1; i < argc; i++) {
    if (*argv[i] == '-') continue;
    if (checkIfParsed(argv[i], store->t)) continue;
    if (!store->t->next)
      curr = addTemplate(curr, argv[i], 0, 0);
    else
      currFile = addFile(currFile, argv[i]);
  }
  if (store->file->next && store->file->next->next) store->fl->multi = 1;
}

int checkIfParsed(const char* argv, templates* templ) {
  int res = 0;
  while (templ) {
    if (templ->templ == argv) res = 1;
    templ = templ->next;
  }
  return res;
}

int flagsInfo(char* argv, data* store, templates** currTempl) {
  int res = 0;
  if (*argv == '-') {
    for (argv++; *argv; argv++) {
      if (res || store->fl->err) break;
      if (*argv == 'i') {
        store->fl->i = 1;
      } else if (*argv == 'v') {
        store->fl->v = 1;
      } else if (*argv == 'c') {
        store->fl->c = 1;
      } else if (*argv == 'l') {
        store->fl->l = 1;
      } else if (*argv == 'n') {
        store->fl->n = 1;
      } else if (*argv == 'h') {
        store->fl->h = 1;
      } else if (*argv == 's') {
        store->fl->s = 1;
      } else if (*argv == 'o') {
        store->fl->o = 1;
      } else if (*argv == 'f' && *(argv + 1)) {
        res = 1;
        *currTempl = addTemplate(*currTempl, ++argv, 1, 0);
      } else if (*argv == 'e' && *(argv + 1)) {
        res = 2;
        *currTempl = addTemplate(*currTempl, ++argv, 0, 0);
      } else if (*argv == 'f' && !*(argv + 1)) {
        res = 3;
      } else if (*argv == 'e' && !*(argv + 1)) {
        res = 4;
      } else {
        store->fl->err = 4;
      }
    }
  }
  return res;
}

void templatesProcessing(data* store) {
  for (templates* curr = store->t; curr; curr = curr->next) {
    if (curr->isFile) {
      FILE* stream = fopen(curr->templ, "r");
      curr = removeTemplate(curr, store->t);
      if (!stream)
        store->fl->err = 2;
      else {
        while (!feof(stream)) {
          int c = newLineSearcher(stream) + 2;
          char* str = malloc(c);
          if (!str) exit(0);
          fgets(str, c, stream);
          if (*str == '\n')
            *str = '.';
          else
            str[c - 2] = '\0';
          // printf("%s\n", str);
          curr = addTemplate(curr, str, 0, 1);
        }
        fclose(stream);
      }
    }
  }
  if (store->t->next) store->t = removeTemplate(store->t, store->t);
  // for (templates* curr = store->t; curr; curr = curr->next) {
  //   if (!strcmp(curr->templ, ".") && curr->next) {
  //     destroyTemplates(curr->next);
  //     curr->next = NULL;
  //   }
  // }
}

int newLineSearcher(FILE* stream) {
  int count = 0;
  long pos = ftell(stream);
  for (int a = fgetc(stream); a != '\n' && a != -1; a = fgetc(stream)) {
    count++;
  }
  fseek(stream, pos, 0);
  return count;
}

void filesProcessing(data* store) {
  for (files* curr = store->file->next; curr; curr = curr->next) {
    FILE* stream = fopen(curr->name, "r");
    if (!stream) {
      store->fl->err = 1;
      curr = removeFile(curr, store->file);
    } else
      curr->stream = stream;
  }
  if (store->file->next) store->file = removeFile(store->file, store->file);
}

void output(data* store) {
  if (store->fl->err == 4)
    errPrint(store->fl->err);
  else {
    if (store->fl->err && !store->fl->s) errPrint(store->fl->err);
    for (files* curr = store->file; curr && curr->stream; curr = curr->next) {
      int n = 1;
      int count = 0;
      while (!feof(curr->stream)) {
        if (store->fl->l && count) break;
        int s = newLineSearcher(curr->stream) + 2;
        char* line = malloc(s);
        if (!line) exit(0);
        fgets(line, s, curr->stream);
        count += regexPrint(store, curr, line, n);
        n++;
        free(line);
      }
      printAtEOF(store->fl, curr, count);
    }
  }
}

void printAtEOF(flags* fl, files* curr, int c) {
  if (fl->c && fl->multi && !fl->h)
    printf("%s:%d\n", curr->name, c);
  else if (fl->c)
    printf("%d\n", c);
  if (fl->l && c) printf("%s\n", curr->name);
}

int regexPrint(data* store, files* file, char* line, int n) {
  int res = 0;
  int add = 0;
  char* matched = malloc(1);
  if (!matched) exit(0);
  *matched = '\n';
  int stop = 0;
  for (templates* curr = store->t; curr && !stop; curr = curr->next) {
    regmatch_t* buff = malloc(sizeof(regmatch_t));
    if (!buff) exit(0);
    regex_t exp;
    // int fl = REG_EXTENDED | REG_NEWLINE;
    int fl = 0;
    if (store->fl->i) fl = fl | REG_ICASE;
    if (!store->fl->o) fl = fl | REG_NOSUB;
    if (!regcomp(&exp, curr->templ, fl)) {
      int err = regexec(&exp, line, 1, buff, 0);
      if (!err && !store->fl->v) {
        res = 1;
        if (!store->fl->c && !store->fl->l) {
          if (store->fl->multi && !store->fl->h && !add)
            printf("%s:", file->name);
          if (store->fl->n && !add) printf("%d:", n);
          if (!store->fl->o) {
            stop++;
            fputs(line, stdout);
            if (line[strlen(line) - 1] != '\n') fputc('\n', stdout);
          } else {
            int widthMatched = 0;
            for (; *curr->templ != '.' && add && !err;
                 err = regexec(&exp, matched + widthMatched, 1, buff,
                               REG_NOTBOL)) {
              // matched += buff->rm_so;
              int width = buff->rm_eo - buff->rm_so;
              printf("%.*s\n", width, matched + buff->rm_so + widthMatched);
              widthMatched += buff->rm_eo;
            }
            add = 1;
            for (; !err; err = regexec(&exp, line, 1, buff, 0)) {
              line += buff->rm_so;
              int width = buff->rm_eo - buff->rm_so;
              matched = realloc(matched, strlen(matched) + width + 2);
              if (!matched) exit(0);
              strncat(matched, line, width);
              strcat(matched, "\n");
              printf("%.*s\n", width, line);
              line += width;
            }
          }
        }
      } else if ((store->fl->v && !err)) {
        stop++;
      }
    } else {
      // errPrint(3);
    }
    regfree(&exp);
    if (buff) free(buff);
  }
  free(matched);
  if (store->fl->v && !stop && *line) {
    res = 1;
    if (!store->fl->c && !store->fl->l) {
      if (store->fl->multi && !store->fl->h && !add) printf("%s:", file->name);
      if (store->fl->n && !add) printf("%d:", n);
      stop++;
      fputs(line, stdout);
      // printf("%s\t%c\n", line, *line);
      if (line[strlen(line) - 1] != '\n') fputc('\n', stdout);
    }
  }
  return res;
}

data* init() {
  data* dataPtr;
  dataPtr = malloc(sizeof(data));
  if (!dataPtr) exit(0);
  dataPtr->t = malloc(sizeof(templates));
  if (!dataPtr->t) exit(0);
  dataPtr->file = malloc(sizeof(files));
  if (!dataPtr->file) exit(0);
  dataPtr->t->templ = NULL;
  dataPtr->t->isFile = 0;
  dataPtr->t->fromFile = 0;
  dataPtr->t->next = NULL;
  dataPtr->file->name = NULL;
  dataPtr->file->stream = NULL;
  dataPtr->file->next = NULL;
  return dataPtr;
}

templates* addTemplate(templates* elem, char* name, int file, int from) {
  templates *tmp, *p;
  tmp = (templates*)malloc(sizeof(templates));
  if (!tmp) exit(0);
  p = elem->next;
  elem->next = tmp;
  tmp->templ = name;
  tmp->isFile = file;
  tmp->fromFile = from;
  tmp->next = p;
  return tmp;
}

templates* removeTemplate(templates* elem, templates* root) {
  templates* tmp;
  tmp = root;
  if (tmp == elem) {
    tmp = tmp->next;
    if (root->fromFile) free(root->templ);
    free(root);
  } else {
    int err = 0;
    while (tmp->next != elem) {
      tmp = tmp->next;
      if (tmp == NULL) {
        err = 1;
        break;
      }
    }
    if (!err) {
      tmp->next = elem->next;
      if (elem->fromFile) free(elem->templ);
      free(elem);
    } else {
      tmp = NULL;
    }
  }
  return tmp;
}

void destroyTemplates(templates* root) {
  templates* tmp;
  tmp = root;
  while (tmp->next != NULL) {
    tmp = removeTemplate(tmp->next, root);
  }
  free(root);
}
