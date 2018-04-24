# Test 4
### README

Name: Fang Zhu

Student ID: 0699 6263

### Problem:

[1].The five threads in the userapp cause deadlock.

[2].The first thread change the mode into mode 2, then open fd1, then open change the mode into model 1, futhermore open fd2. 

[3].The last thread change the mode into mode 2. Because I add delay before wait() in the ioctl, then the wake() function occurs before the wait, then nothing can wake the wait up, then the deadlock occurs.