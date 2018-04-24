# Assignment 6
### README

Name: Fang Zhu

Student ID: 0699 6263

### Section 1: Deadlock

[1].The four possible deadlock program is under the folder "Deadlock", please take a look.

[2]. Make the assignment6.c/assignment6_mod1.c to get files.

```
make
```

[3]. Load the assignment6.ko/assignment6_mod1.ko.

```
sudo insmod assignment6.ko/assignment6_mod1.ko
```

[3]. Check whether the driver has been loaded.

```
lsmod
```

[4]. If the driver has been loaded, make the userapp.c file.

```
sudo ./userapp
```

[6]. Follow the instructions to do write, read and lseek function according to the driver.

[5]. Remove the driver.

```
sudo rmmod assignment6.ko/assignment6_mod1.ko
```
[6]. Three kinds of remove. "make clean" can remove every file except the userapp, "make clean-userapp" can remove the userapp and "make-all" can remove everything.

### Section 2: Race condition

[1].Code review:

	Region 1 (r1)
	{
		pthread_mutex_lock(&lock);
		ioctl(fd1,MODE2);
		pthread_mutex_unlock(&lock);	
		pthread_exit(NULL);
	} 

	Region 2 (r2)
	{
		pthread_mutex_lock(&lock);
		fd1 = open();
		pthread_mutex_unlock(&lock);	
		pthread_exit(NULL);
	} 

	Region 3 (r3)
	{
		pthread_mutex_lock(&lock);
		write(fd1);
		pthread_mutex_unlock(&lock);	
		pthread_exit(NULL);
	} 

	Region 4 (r4)
	{
		pthread_mutex_lock(&lock);
		read(fd1);
		pthread_mutex_unlock(&lock);	
		pthread_exit(NULL);
	} 

1).The right sequence should be r1->r2->r3->r4, and we can write the data into fd1 and read from it.

2).If r1 is not the first one, the mode of the user program is the default mode 1 for the region above r1.

3).If r1 is the first one, but the r2 thread comes after any or all of the threads (r3 and r4) then error may happen, because fd1 is not right.

4).If the sequence is r1->r2->r4->r3, then we first get the data from the fd1 without any data, then write into the fd1.
