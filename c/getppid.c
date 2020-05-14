#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
	pid_t ppid = getppid();
	char line[30];
	while( gets(line) )
		printf("%s\n", line);

	printf("PPID\n");
	printf("%d\n", ppid);
}
