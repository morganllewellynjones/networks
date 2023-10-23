#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

readFile(int fd) {
	size_t fileSize = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	char* fileBuffer = malloc(fileSize + 1);
	read(fd, fileBuffer, fileSize);

}

readStream(int fd) {

}

cacheFile(int fd) {
}
