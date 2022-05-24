#include "common.h"

files* addFile(files* elem, char* name) {
  files *tmp, *p;
  tmp = (files*)malloc(sizeof(files));
  if (!tmp) exit(0);
  p = elem->next;
  elem->next = tmp;
  tmp->name = name;
  tmp->stream = NULL;
  tmp->next = p;
  return tmp;
}

files* removeFile(files* elem, files* root) {
  files* tmp;
  tmp = root;
  if (tmp == elem) {
    tmp = tmp->next;
    if (root->stream) fclose(root->stream);
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
      if (elem->stream) fclose(elem->stream);
      free(elem);
    } else {
      tmp = NULL;
    }
  }
  return tmp;
}

void destroyFiles(files* root) {
  files* tmp;
  tmp = root;
  while (tmp->next != NULL) {
    tmp = removeFile(tmp->next, root);
  }
  free(root);
}

void errPrint(int err) {
  if (err == 1) {
    printf("Cannot open some files.\n");
  } else if (err == 2) {
    printf("Cannot open a regex file.\n");
  } else if (err == 3) {
    printf("Cannot compile regular expression.\n");
  } else if (err == 4) {
    printf("Illegal option.\n");
  }
}
