# Test 2
### README

Name: Fang Zhu

Student ID: 0699 6263

### Problem:

[1].The four threads in the userapp cause deadlock.

[2].The first thread change the mode into mode 1.

[3].The rest of threads open three different files, because in each open function the sem2 is cut off, then each open functions stop at the point waiting for other function add the sem2 up to unlock, then the deadlock happens.