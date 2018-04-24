//////////////////////////////////////
//		README              //
//           Name: Fang Zhu         //
//	  Student ID: 0699 6263     //
////////////////////////////////////// 
//File included:                    // 
//test.c                            //
//this README                       //
//input.txt                         //
//Makefile(test)                    //
//////////////////////////////////////

Homework #3
========================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================
I.mapper.c
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
1. void adjust(char A, char In[]):
	[1]. In this function, the first parameter char A is used to judge in the switch afterwards to match the right score
	[2]. The second parameter is represented to transmit the buffer to rewrite the input. 
	[3]. I define five pointers point to five strings. These five string is used to add at the end of each tuple.
	[4]. The first for loop is utilized to skip the ',' and the action char in the middle until end of tuple. 
	[5]. The second for loop is responsible to transmitt the pointer n to the position before ')'.
	[6]. The last for loop is used to prolong the buffer and fill up with the string defined defined in the third step.(choose the string according to the switch in the forth step.)
	[7]. According to the requirements, each tuple should stay in one line seperately. Thus, '\n' and '\0' are supposed to fill at the end of the buffer.(each line)
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
1. void output(int check,char save[50][50]):
	[1]. When all of the input file is finished, I utilize this function to fill the data into the file.
	[2]. Firstly, we check in the save buffer whether any tuple starts with "X" in order to avoid outputing the reduced tuple.
	[3]. Secondly, store and compare the first 7 elements in each tuple to find whether there is any tuple needs to be reduced.
	[4]. In the comparison, compute the new score by adding all the score together.
	[5]. Output the save file into file and stdout.
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
2. mapper(pthread):
	[1]. Mapper thread plays the role of producer.
	[2]. Firstly, read the file name pointer from the parameter, build a new file for mapper to write into and initialize all the buffer and variables. Sem_wait(&empty) and Sem_wait(&mutex).
	[3]. Build a loop and judge fgets() equals to null, if not means error or EOF may happen. If fget() get information from file, read them into the input buffer with length 26(global variables). The mutex is locked and unlocked on the two sides of the fgets() loop.
	[4]. Inside the loop, first get the IDs which represent the name of the tuple. Then store the tuple into the right buffer. If there exists tuples with the same ID number, input the tuple to the buffer. On the other hand, if no, however exist empty buffer, then store the tuple and sign the buffer with ID.
	[5]. Afterwards, if utilize the for loop to find if any buffer is full post the full[indice of the full buffer] and the mutex, else if not empty, post the empty and mutex to continue run.
	[6]. Finally, if the fgets while loop over, meaning EOF, then first sleep for the last reducer finishing its job, post full[0] and set end equals to 1 to terminate all of other threads. 
	[7]. Mapper is done. 
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
3. reducer(pthread):
	[1]. Reducer thread plays the role of consumer.
	[2]. Firstly, initialing the variables and get the number of sign given by the main thread from the parameter. Sem_wait(&full[indice of this reducer]) and Sem_wait(&mutex) in the loop.
	[3]. Then in the while loop, I read and write tuples into file over and over again.
	[4]. Finally if the reducer does not finish picking out the tuples, then post full[indice of the buffer] and post the mutex to continue circle. If the buffer is empty but not end of file, then I post empty and mutex to continue mapper, else if the buffer is empty and end of file, utilize output function before and post the full[next indice of buffer] and mutex and break the loop, or just break the loop.
	[5]. Reducer is done.
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
4. main function:
	[1]. At the beginning, A dynamic 2-d array is built to save different buffer, and the size of the slot and the size of the buffer is input from the arguments.
	[2]. Initialize and create threads, in the for loop of number of buffer the index is transmitted into the reducer by parameters as the sign of each reducer.
	[3]. Initialize the semapore function.
	[4]. Finally join the threads and free the dynamic space.
=========================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================

II. Conclusion:
	[1]. Important: In order to continue writing the reducers data into the file, please remove the "reducer_output.txt" before each run, because the file will not be overwritten.
	[2]. First type "make" to get "test" and "test.o".
	[3]. "test clean" can clean the "test.o".
	[4]. "**space" is the pointer pointing at the buffers for communication between the reducers and the mapper.
	[5]. "*pointer" is the pointer pointing at the buffer to save the number of tuple exists in the buffer.
	[6]. "*pos" is the pointer pointing at the buffer to save the indice of the end of the buffer of the last tuple in the buffer. 
	[7]. Because I read the file by the stdin, so the format of input is: ./test <number of slot> <number of buffer> <input.txt. (e.g. "./test 4 7 <input.txt")
