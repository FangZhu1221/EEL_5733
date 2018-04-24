
#include <linux/ioctl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <semaphore.h>

#define DEVICE "/dev/a5"
#define ramdisk_size (size_t) (16 * PAGE_SIZE)

#define CDRV_IOC_MAGIC 'Z'
#define E2_IOCMODE1 _IOWR(CDRV_IOC_MAGIC, 1, int)
#define E2_IOCMODE2 _IOWR(CDRV_IOC_MAGIC, 2, int)

#define MODE1 1 // define mode here. There can be 2 modes
#define MODE2 2

//static int MODE;
int fd1,fd2;

void* OP_thread1(void* param)
{
	fd1 = open(DEVICE, O_RDWR);//open
	if(fd1 == -1) 
	{
		printf("File %s either does not exist or has been locked by another process\n", DEVICE);
		exit(-1);
	}
	pthread_exit(NULL);
}

void* OP_thread2(void* param)
{
	fd2 = open(DEVICE, O_RDWR);//open
	if(fd2 == -1) 
	{
		printf("File %s either does not exist or has been locked by another process\n", DEVICE);
		exit(-1);
	}
	pthread_exit(NULL);
}

void* WR_thread(void* param)
{
	char write_buf[100];
	printf("Enter Data to write: ");
	scanf(" %[^\n]", write_buf);
	write(fd1, write_buf, sizeof(write_buf));
	pthread_exit(NULL);
}

void* RD_thread(void* param)
{
	char read_buf[10];
	if (read(fd1, read_buf, sizeof(read_buf)) > 0) 
	{
		printf("\n device: %s\n", read_buf);
	} 
	else
	{
		fprintf(stderr, "Reading failed\n");
	}
	pthread_exit(NULL);
}

void* IOC_thread1(void* param)
{
	unsigned int cmd = (unsigned int) param;

	printf("\n Change mode ");
	if(cmd == E2_IOCMODE1)
		printf("1\n");
	else if(cmd == E2_IOCMODE2)
		printf("2\n");
	else
		printf("error\n");

	int rc = ioctl(fd1, cmd, 0);
	pthread_exit(NULL);
}

void* IOC_thread2(void* param)
{
	unsigned int cmd = (unsigned int) param;

	printf("\n Change mode ");
	if(cmd == E2_IOCMODE1)
		printf("1\n");
	else if(cmd == E2_IOCMODE2)
		printf("2\n");
	else
		printf("error\n");

	int rc = ioctl(fd2, cmd, 0);
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{

    printf("\n Start! \n");
    pthread_t IOC1,IOC2,IOC3,OP1,OP2,WR,RD,RL;

  	pthread_create(&IOC1, NULL, IOC_thread1, (void *)(E2_IOCMODE2));
	pthread_create(&OP1, NULL, OP_thread1, NULL);
  	pthread_create(&IOC2, NULL, IOC_thread2, (void *)(E2_IOCMODE1));
    pthread_create(&OP2, NULL, OP_thread1, NULL);
    pthread_create(&IOC3, NULL, IOC_thread1, (void *)(E2_IOCMODE2));

	pthread_join(IOC1, NULL);
	pthread_join(OP1, NULL);
	pthread_join(IOC2, NULL);
	pthread_join(OP2, NULL);
	pthread_join(IOC3, NULL);

	close(fd1);
	close(fd2);
	pthread_exit(NULL);
	return 0;
}
