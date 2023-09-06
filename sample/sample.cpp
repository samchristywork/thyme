#include <stdio.h>

int main() {
  int acc = 0;

  for (int i = 0; i < 1000000000; i++) {
    if (i % 10000000 == 0) {
      printf("i = %d\n", i);
    }

    acc += i;
  }

  printf("acc = %d\n", acc);
}
