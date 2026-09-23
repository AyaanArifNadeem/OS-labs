#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"
#include "kernel/riscv.h"

int is_valid_char(char c) {
  return (c >= 'a' && c <= 'z') || 
         (c >= 'A' && c <= 'Z') || 
         (c >= '0' && c <= '9');
}

int main(int argc, char *argv[]) {
  // Allocate 32 pages using sbrk instead of malloc
  int sz = 32 * 4096;
  char *mem = sbrk(sz);
  
  // Safety check to prevent page faults
  if(mem == (char*)-1) {
    printf("sbrk failed\n");
    exit(1);
  }

  // Scan the memory byte by byte
  for(int i = 0; i < sz; i++) {
    if(is_valid_char(mem[i])) {
      int len = 0;
      
      while(i + len < sz && is_valid_char(mem[i + len])) {
        len++;
      }
      
      if(len >= 4) {
        char temp = mem[i + len];
        mem[i + len] = '\0'; 
        printf("%s\n", &mem[i]);
        mem[i + len] = temp; 
      }
      i += len; 
    }
  }
  
  exit(0);
}
