#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
  // Call your new system call
  int free_bytes = freemem();
  
  printf("Free memory: %d bytes\n", free_bytes);
  
  exit(0);
}
