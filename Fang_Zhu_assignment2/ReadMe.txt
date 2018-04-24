//////////////////////////////////////
//		README              //
//           Name: Fang Zhu         //
//	  Student ID: 0699 6263     //
////////////////////////////////////// 
//File included:                    // 
//pthread.c                         //
//this README                       //
//input.txt                         //
//Makefile(pthread)                 //
//////////////////////////////////////

Homework #2
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
2. mapper(pthread):
	[1]. Mapper thread plays the role of producer.
	[2]. Firstly, read the file name pointer from the parameter, build a new file for mapper to write into and initialize all the buffer and variables.
	[3]. In the while(1) loop, build a new loop and judge fgets() equals to null, if not means error or EOF may happen. If fget() get information from file, read them into the input buffer with length 26(global variables). The mutex is locked and unlocked on the two sides of the fgets() loop.
	[4]. Inside the loop, first get the IDs which represent the name of the tuple. Then store the tuple into the right buffer. If there exists tuples with the same ID number, input the tuple to the buffer. On the other hand, if no, however exist empty buffer, then store the tuple and sign the buffer with ID.
	[5]. Afterwards, if utilize the for loop to find if any buffer is full wake up the empty condition and only the reducer responding to the full buffer can motivate, because the token is given by the sign.
	[6]. Finally, if the fgets while loop over, meaning EOF, then set the token equals to 1 and end equals to 1 then wake up empty condition, for making reducers terminating in order from the signed by 0.
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
3. reducer(pthread):
	[1]. Reducer thread plays the role of consumer.
	[2]. Firstly, initialing the variables and get the number of sign given by the main thread from the parameter.
	[3]. While loop outsides at first for reading and writing tuples into file over and over again. Mtx lock is set at the two sides inside the loop.
	[4]. Then we judge whether the sign and token is the same, because only the reducer with full buffer can read and write.
	[5]. Afterwards I judge whether buffer is empty (the same as full) by the pointer[n](n equals to the token number). If empty, wait for mapper thread and clear the pointer of this buffer to the head. In special case, the EOF condition, judging by end of file, if the buffer is already clear, then the thread need to break out if the token and sign are the same also.
	[6]. What's more, all the tuple will be stored into a buffer which is belong to each reducer and the buffer will be utilized as a whole at the EOF in order to avoid the possible of the same subject among different global reducer buffer.
	[7]. In the reducer the self buffer is processed and output to the file as usual.
	[8]. Finally, every reducer will input done[token] = 1 when the reducer is going to terminate and then add the token with 1, in order to let the next thread to process.
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
4. main function:
	[1]. At the beginning, A dynamic 2-d array is built to save different buffer, and the size of the slot and the size of the buffer is input from the arguments.
	[2]. Initialize and create threads, the file name is transmitted by the parameter of the mapper, and in the for loop of number of buffer the index is transmitted into the reducer by parameters as the sign of each reducer.
	[3]. Finally join the threads,terminate the lock, conditions and free the dynamic space.
=========================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================

II. Conclusion:
	[1]. Important: In order to continue writing the reducers data into the file, please remove the "reducer_output.txt" before each run, because the file will not be overwritten.
	[2]. First type "make" to get "thread" and "thread.o".
	[3]. "make clean" can clean the thread.o
	[4]. "**space" is the pointer pointing at the buffers for communication between the reducers and the mapper.
	[5]. "*pointer" is the pointer pointing at the buffer to save the number of tuple exists in the buffer.
	[6]. "*pos" is the pointer pointing at the buffer to save the indice of the end of the buffer of the last tuple in the buffer. 
	[7]. Because I read the file by the transmit the name of the file into thread, so the format of input is: ./thread <number of slot> <number of buffer> input.txt. (e.g. "./thread 4 7 input.txt")
