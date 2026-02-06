#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/getproc.h"
#include "user/user.h"

char *states[] = {
  "unused", "used", "sleep",
  "run", "running", "zombie"
};

int
main(void)
{
  struct procinfo pinfo[64];
  int n = getprocs(pinfo);

  if (n < 0) {
    printf("ps: getprocs failed\n");
    exit(1);
  }

  printf("PID\tSTATE\tSIZE\tNAME\n");
  for (int i = 0; i < n; i++) {
    printf("%d\t%s\t%ld\t%s\n",
      pinfo[i].pid,
      states[pinfo[i].state],
      pinfo[i].sz,
      pinfo[i].name);
  }

  exit(0);
}
