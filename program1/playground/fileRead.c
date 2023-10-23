#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{

	int fd = open("../index.html", 0);
	off_t fileSize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	char buffer[fileSize];
	read(fd, buffer, fileSize);
	printf("%s\n", buffer);
	return 0;
}
