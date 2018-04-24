//////////////////////////////////////
//		README              //
//           Name: Fang Zhu         //
//	  Student ID: 0699 6263     //
////////////////////////////////////// 
//File included:                    // 
//mapper.c, combiner.c, reducer.c   //
//this README                       //
//input.txt                         //
//Makefile(mapper,reducer,combiner) //
//////////////////////////////////////

Homework #1
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
2. main function:
	[1]. At the beginning, I intialize the line to be zero and construct input and output 1 dimension array with 26 length both, because each input tuple occupies 26 char. x is defined as char for storing the action char.
	[2]. Utilizing FILE and fopen function to register two pointers to "input.txt" file(read) and "mapper_output.txt"(write);
	[3]. In the while loop, the tuple is read as input array tuple by tuple. In the for loop inside, I transmit the data in the input buffer to the output buffer. Then utilizing the adjust function defined before to transmit the rewrite the output buffer. Finally using fprintf to output to the file.
	[4]. Finally, close the two input and output flow.
=========================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================
II. reducer.c
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
1.main function:
	[1]. Initializing buffers at first: input 1-d array buffer used for transmit the data from the file tuple by tuple; Save, the 2-d array is used to store the data from input line by line. compare and standard, the two 1-d buffer is used to seek for the tuples with overlapping information for the first two parameters.
	[2]. In the while loop, I read from the "mapper_output.txt" file and store the data into the save array buffer.Then close the read flow after the loop.
	[3]. In the for loop, I skip the data with 'X' ahead.(I will explain afterwards) The first for loop inside is utilized to store the data from the save buffer in this circle as a standard. The second for loop is used to find whether there are tuples with closed information (the first 2 parameters keep the same) and compute the score to add up to the tuple. Finally sign the tuple with repeated information with the 'X' ahead. This why the while loop out side need to skip the tuple with 'X'.
	[4]. In the second while loop, after initializing the pointer fp to write, we printf the tuple and write the tuple into file in individual line, because of the requirement I mentioned before.
	[5]. Finally close the output flow.
=========================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================	
III. combiner.c
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
1.main function:
	[1]. Firstly, I define two file table descriptor and array buffer buf.
	[2]. In the first if, construct and judge whether pipe is constructed successfully.
	[3]. Use fork to build the first child pid_1.
	[4]. After judging whether the child 1 is constructed successfully, close the read end, fd[0], in the child process at first. (judge whether it is closed successfully also)Then duplicate fd[1] to stdout and close fd[1]. Use system() and execl() to compile and execute the mapper.c program. Finally, if the exec is not executed, print error.
	[5]. Parent process wait the child 1 finished and print error, if exists.
	[6]. Use fork to build the first child pid_2.
	[7]. After judging whether the child 2 is constructed successfully, close the write end, fd[1], in the child process at first. (judge whether it is closed successfully also)Then duplicate fd[0] to stdin and close fd[1]. Use system() and execl() to compile and execute the reducer.c program. Finally, if the exec is not executed, print error.
	[8]. Parent process wait the child 2 finished and print error, if exists.
	[9]. Close the two file descriptor if they are not being closed.
	[10]. Exit.
==========================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================================
IV. Conclusion:
	[1]. The mapper program output to the screen and write the new tuple into the file called "mapper_output.txt";
	[2]. The reducer program output to the screen and write the new tuple into the file called "reduce_output.txt";
	[3]. The combiner program combine the two program above and output the two files above.
	[4]. There are three fold, the mapper, reducer and combiner in the zip document. Each includes a makefile.The makefile will execute by input "make" on the command. Afterwards output file mapper,reducer, combiner, mapper.o, reducer.o and combiner.o will be created in each fold, which can help to get the outcomings. "make clean" will clean the *.o document in each fold.

