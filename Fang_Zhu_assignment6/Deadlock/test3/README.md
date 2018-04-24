# Test 3
### README

Name: Fang Zhu

Student ID: 0699 6263

### Problem:

[1].The five threads in the userapp cause deadlock.

[2].In the first IOC1 thread, change the two mode with two file descriptors to mode 1.

[3].In the next two mode open two file.

[4].In the last two thread will let two fds into mode 2, but the two threads both want to change mode, then the count number is over 2 causing the deadlock.
