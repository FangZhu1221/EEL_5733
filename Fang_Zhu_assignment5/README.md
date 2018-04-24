# Homework 5
### README

Name: Fang Zhu

Student ID: 0699 6263

#### File included:   

char_driver.c, this README, userapp.c, Makefile

## I. Process:

[1]. Open the terminal under the folder.

[2]. Make the char_driver.c to get files.

```
make
```

[3]. Load the char_driver.ko with the number of device to add driver moduler.

```
sudo insmod char_driver.ko <NUMBER_OF_DEVICE>
```

[3]. Check whether the driver has been loaded.

```
lsmod
```

[4]. If the driver has been loaded, make the userapp.c file with number of device you want to use.

```
sudo ./userapp <NO_OF_DEVICE>
```

[6]. Follow the instructions to do write, read and lseek function according to the driver.

[5]. Remove the char_driver.ko driver.

```
sudo rmmod char_driver.ko
```
[6]. Three kinds of remove. "make clean" can remove every file except the userapp, "make-userapp" can remove the userapp and "make-all" can remove everything.
