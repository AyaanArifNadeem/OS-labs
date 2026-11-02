#include "kernel/types.h"
#include "kernel/memlayout.h"
#include "kernel/riscv.h"
#include "user/user.h"

// test copy-on-write fork
void
simpleft(void)
{
  int pid;
  char *p;

  p = sbrk(PGSIZE * 3);
  if(p == (char*)-1){
    printf("simpleft: sbrk failed\n");
    exit(1);
  }
  p[0] = 99;

  pid = fork();
  if(pid < 0){
    printf("simpleft: fork failed\n");
    exit(1);
  }
  if(pid == 0){
    if(p[0] != 99)
      printf("simpleft child: child not seeing parent data\n");
    p[0] = 100;
    if(p[0] != 100)
      printf("simpleft child: child write failed\n");
    exit(0);
  }

  wait(0);
  if(p[0] != 99)
    printf("simpleft parent: parent data changed by child\n");

  printf("simple: ok\n");
}

void
three(void)
{
  int pid;
  char *p = sbrk(PGSIZE * 3);
  p[0] = 1;
  p[PGSIZE] = 2;
  p[PGSIZE * 2] = 3;

  pid = fork();
  if(pid < 0){
    printf("three: fork failed\n");
    exit(1);
  }
  if(pid == 0){
    p[0] = 10;
    p[PGSIZE] = 20;
    p[PGSIZE * 2] = 30;
    exit(0);
  }
  wait(0);
  if(p[0] != 1 || p[PGSIZE] != 2 || p[PGSIZE * 2] != 3)
    printf("three: parent data corrupted\n");

  printf("three: ok\n");
}

void
file(void)
{
  int pid;
  char *p = sbrk(PGSIZE);
  int fd = open("cowfile", 0);
  if(fd < 0){
    fd = open("cowfile", 0-2); // O_CREATE
    close(fd);
    fd = open("cowfile", 2);
    write(fd, "hello", 5);
    close(fd);
    fd = open("cowfile", 0);
  }

  pid = fork();
  if(pid < 0){
    printf("file: fork failed\n");
    exit(1);
  }
  if(pid == 0){
    read(fd, p, 5);
    exit(0);
  }
  wait(0);
  close(fd);
  printf("file: ok\n");
}

int
main(int argc, char *argv[])
{
  simpleft();
  three();
  file();
  printf("ALL COW TESTS PASSED\n");
  exit(0);
}
