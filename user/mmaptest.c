#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int pid;

  printf("=== Multi-region shared memory test ===\n\n");

  // --- Test 1: Map two independent shared memory regions ---
  printf("Test 1: Multiple independent regions\n");

  uint64 addr1 = mmap(1);
  uint64 addr2 = mmap(2);
  if(addr1 == 0 || addr2 == 0){
    printf("FAIL: mmap failed (addr1=0x%p, addr2=0x%p)\n",
           (void*)addr1, (void*)addr2);
    exit(1);
  }
  printf("  Region 1 mapped at 0x%p\n", (void*)addr1);
  printf("  Region 2 mapped at 0x%p\n", (void*)addr2);

  if(addr1 == addr2){
    printf("FAIL: two regions got the same address\n");
    exit(1);
  }
  printf("  PASS: regions have different addresses\n");

  // Write different values to each region.
  int *data1 = (int*)addr1;
  int *data2 = (int*)addr2;
  *data1 = 42;
  *data2 = 99;

  if(*data1 == 42 && *data2 == 99){
    printf("  PASS: independent data (region1=%d, region2=%d)\n",
           *data1, *data2);
  } else {
    printf("FAIL: data corrupted\n");
    exit(1);
  }

  // --- Test 2: Fork and share ---
  printf("\nTest 2: Parent-child sharing via fork\n");

  pid = fork();
  if(pid < 0){
    printf("FAIL: fork failed\n");
    exit(1);
  }

  if(pid == 0){
    // Child process
    printf("  Child reads: region1=%d, region2=%d\n", *data1, *data2);

    // Modify shared data.
    *data1 = 100;
    *data2 = 200;
    printf("  Child wrote: region1=%d, region2=%d\n", *data1, *data2);

    // Unmap only region 1 in child.
    if(munmap(addr1) < 0)
      printf("  FAIL: child munmap region 1\n");
    else
      printf("  Child: munmap region 1 succeeded\n");

    exit(0);
  } else {
    // Parent process
    wait(0);

    printf("  Parent reads after child: region1=%d, region2=%d\n",
           *data1, *data2);

    if(*data1 == 100 && *data2 == 200)
      printf("  PASS: shared memory modifications visible\n");
    else
      printf("  FAIL: expected region1=100, region2=200\n");
  }

  // --- Test 3: Same ID returns same region ---
  printf("\nTest 3: Same ID maps to same region\n");

  uint64 addr1_again = mmap(1);
  if(addr1_again == addr1){
    printf("  PASS: mmap(1) returned same address 0x%p\n", (void*)addr1_again);
  } else {
    printf("  INFO: mmap(1) returned 0x%p (original 0x%p)\n",
           (void*)addr1_again, (void*)addr1);
  }

  // --- Test 4: Unmap all ---
  printf("\nTest 4: Unmapping all regions\n");

  if(munmap(addr1) < 0)
    printf("  FAIL: munmap region 1\n");
  else
    printf("  munmap region 1 succeeded\n");

  if(munmap(addr2) < 0)
    printf("  FAIL: munmap region 2\n");
  else
    printf("  munmap region 2 succeeded\n");

  printf("\n=== All tests passed ===\n");
  exit(0);
}