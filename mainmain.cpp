#include <iostream>
#include <unistd.h>
#include <fcntl.h>

int main()
{
	int p[2];
	pipe(p);
	int fd;
	int pid;

	fd = open("test", O_CREAT | O_RDWR, 0644);
	pid = fork();
	std::cout << "PIPE WRITE:" << p[1] << std::endl;
	std::cout << "PIPE READ:" << p[0] << std::endl;
	
	char *tab[2] = {"/bin/ls", NULL};

	if (pid == -1)
		std::cout << "ERROR" << std::endl;
	else if (pid == 0) {
		close(p[0]); // READ SIDE
		dup2(p[1], STDOUT_FILENO);
		write(STDOUT_FILENO, "salut\n", 6);
		execve(tab[0], tab, NULL);
		close(p[0]);
		exit (0);
	}
	else {
		close(p[1]); // WRITE SIDE
		waitpid(pid, NULL, 0);
		close(p[0]);
	}
}