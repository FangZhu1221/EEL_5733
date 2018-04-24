# Test 1
### README

Name: Fang Zhu

Student ID: 0699 6263

### Problem:

[1].The four threads in the userapp cause deadlock.

[2].The first thread change the mode into mode 2, the count2 equals to 1.

[3].The second and the third thread open the file twice the count2 is added to 2.

[4].The last thread change mode into 1 and count2 is added to 3. In the switch region of ioctl, the count2 cannot be wake up for the value of count over 2, then the deadlock occurs.
