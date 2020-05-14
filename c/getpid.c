#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	pid_t pid = getpid();
	char line[30];
	while( gets(line) )
		printf("%s\n", line);

	printf("PID\n");
	printf("%d\n", pid);
}
