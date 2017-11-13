#include <stdio.h>

static char buffer[2048];

int main(int argc, char** argv) {

  puts("Lispy Version 0.0.0.0.1");
  puts("Press Ctrl+c to Exit\n");

  while(1) {
    fputs("lispy>", stdout);

    fgets(buffer, 2048, stdin);

    printf("No you're a %s", buffer);
  }

  return 0;
}
