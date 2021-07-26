#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int main()
{
	int p[2];
	// int fd;
	int pid;

	pipe(p);
	dprintf(1, "PIPE WRITE:%d\n", p[1]);
	dprintf(1, "PIPE READ:%d\n", p[0]);

	// fd = open("test", O_CREAT | O_RDWR, 0644);
	pid = fork();
	
	if (pid == -1)
		dprintf(1, "ERROR" );
	else if (pid == 0) {
		close(p[0]); // READ SIDE
		dup2(p[1], STDOUT_FILENO);
		write(p[1], "salut\n", strlen("salut\n"));
		close(p[1]);
		exit(0);
	}
	else {
		close(p[1]); // WRITE SIDE
		waitpid(pid, NULL, 0);
		char buffer[6];
		read(p[0], buffer, 6);
		buffer[5] = '\0';
		close(p[0]);
		dprintf(1, "%s\n", buffer);
	}
}
