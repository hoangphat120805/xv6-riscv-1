#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid;
  uint64 shared_addr;

  printf("Testing mmap shared memory...\n");

  shared_addr = mmap();
  if(shared_addr == 0){
    printf("mmap failed\n");
    exit(1);
  }

  printf("Mapped shared memory at %p\n", (void*)shared_addr);

  int *shared_data = (int*)shared_addr;
  *shared_data = 42;
  printf("Parent wrote: %d\n", *shared_data);

  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }

  if(pid == 0){
    printf("Child read: %d\n", *shared_data);
    *shared_data = 100;
    printf("Child wrote: %d\n", *shared_data);
    exit(0);
  }

  wait(0);
  printf("Parent read after child: %d\n", *shared_data);
  exit(0);
}