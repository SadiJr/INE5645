#include<stdio.h>
#define FALSE 0
#define TRUE 1

void treatInput(char *inputMessage, char *errorMessage, int *parameter) {
  int expectedInput = FALSE;
  do {
    printf(inputMessage);
    if (scanf("%d", parameter) == 1) {
      expectedInput = TRUE;
    } else {
      printf(errorMessage);
      fgetc(stdin);
      continue;
    }
  } while (!expectedInput);
}