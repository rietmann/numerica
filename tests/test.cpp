#include <stdio.h>
#include <stdlib.h>

void factorial(int** x) {
  *x = new int[10];
  printf("assigning stuff\n");
  (*x)[0] = 42;
  (*x)[1] = 43;
  printf("made x\n");
}

int main(int argc, char **argv) {
  int* x;
  factorial(&x);
  printf("x[0]=%d\n",x[0]); 
}
