#include <sys/file.h>
#include <fcntl.h>
#include <time.h>
#include "tlpi_hdr.h"

#define BUF_SIZE 1000

char *currTime(const char *format);

int main(int argc, char *argv[])
{
	int fd_a, fd_b;

	fd_a = open("./a", O_RDWR);
	if (fd_a == -1)
		errExit("open");

	fd_b = open("./b", O_RDWR);
	if (fd_b == -1)
		errExit("open");

	printf("PID %ld: requesting file a at %s\n", (long)getpid(),
		   currTime("%T"));
	if (flock(fd_a, LOCK_EX) == -1)
	{
		if (errno == EWOULDBLOCK)
			fatal("PID %ld: already locked file a !", (long)getpid());
		else
			errExit("File a flock (PID=%ld)", (long)getpid());
	}
	sleep(3);

	// printf("PID %ld: requesting file b at %s\n", (long)getpid(),
	// 	   currTime("%T"));
	// if (flock(fd_b, LOCK_EX) == -1)
	// {
	// 	if (errno == EWOULDBLOCK)
	// 		fatal("PID %ld: already locked file b !", (long)getpid());
	// 	else
	// 		errExit("File b flock (PID=%ld)", (long)getpid());
	// }
	printf("write强制性下要无别的进程有b的 flock锁，但write本身不持有锁\n");
	if (write(fd_b, "b", 1) != 1) // 强制性锁让write会阻塞到无别的进程持有写锁的时候
		errExit("write");
	printf("write b成功\n");
	sleep(3);

	// if (flock(fd_b, LOCK_UN) == -1)
	// 	errExit("flock");
	// printf("PID %ld: released file b at %s\n", (long)getpid(),
	// 	   currTime("%T"));

	if (flock(fd_a, LOCK_UN) == -1)
		errExit("flock");
	printf("PID %ld: released file a at %s\n", (long)getpid(),
		   currTime("%T"));

	exit(EXIT_SUCCESS);
}

char *currTime(const char *format)
{
	static char buf[BUF_SIZE]; /* Nonreentrant */
	time_t t;
	size_t s;
	struct tm *tm;

	t = time(NULL);
	tm = localtime(&t);
	if (tm == NULL)
		return NULL;

	s = strftime(buf, BUF_SIZE, (format != NULL) ? format : "%c", tm);

	return (s == 0) ? NULL : buf;
}
