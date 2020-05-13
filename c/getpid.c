#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	pid_t pid = getpid();
	pid_t ppid = getppid();
	char line[30];
	while( gets(line) )
		printf("%s\n", line);

	printf("PID  PPID\n");
	printf("%d %d\n", pid, ppid);
}
