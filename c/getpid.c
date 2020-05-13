#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
  pid_t pid = getpid();
  pid_t ppid = getppid();

  printf("PID  PPID\n");
  printf("%lu %lu\n", pid, ppid);
}
