# CSC415-Assignment-2b

**Assignment 2b – Buffered I/O**

Welcome to your next homework assignment.  

Let me remind you of the expectations.  Code should be neat, well documented.  Variables should have meaningful names and be in a consistent format (I do not care if you use camelCase or under_scores in variables but be consistent.  In addition, each file should have a standard header as defined below.

```
/**************************************************************
* Class:  CSC-415-0# Fall 2021
* Name:
* Student ID:
* GitHub UserID:
* Project: Assignment 2B – Buffered I/O read
*
* File: <name of this file>
*
* Description:
*
**************************************************************/
```

This is an INDIVIDUAL assignment.  You can (and should) work in groups to research how to do the assignment, but each person must code their own version and make their own submission.

This assignment is to get you to handle buffered IO where you do the buffering.  

You will see in this assignment that you may question why you are doing operations the way specified here in the assignment.  This is because we are writing a buffering I/O.  We will see later in the course how important this is.

You are to create a set of routines in a file called `b_io.c`.  The prototypes for these functions are in a file called `b_io.h` (provided).  The Makefile is supplied and you should set your FIRSTNAME and LASTNAME.

You will be just writing three functions in `b_io.c`

```
	b_io_fd b_open (char * filename, int flags);
	int b_read (b_io_fd fd, char * buffer, int count);
	void b_close (b_io_fd fd);
```

Your functions here will only use the supplied lowlevel APIs.  i.e. LBAread, and GetFileInfo.

The `b_open` should return a integer file descriptor (a number that you can track the file).  You may want to also allocate the 512 byte buffer you will need for read operations here.  Make sure though you know how to track the buffer for each individual file. Return a negative number if there is an error.  You will call GetFileInfo to find the filesize and location of the desired file.  See the structure fileInfo.  GetFileInfo returns a pointer to fileInfo (this pointer does NOT need to be freed).

The `b_read` takes a file descriptor, a buffer and the number of bytes desired.  The Operation of your `b_read` function must only read 512 bytes chunks at a time from LBAread into your own buffer, you will then copy the appropriate bytes from your buffer to the caller’s buffer.  This means you may not even need to do a read of the actual file if your buffer already has the data needed.  Or, it may mean that you have some bytes in the buffer, but not enough and have to transfer what you have, read the next 512 bytes, then copy the remaining needed bytes to the caller’s buffer.  
The return value is the number of bytes you have transferred to the caller’s buffer.  When it is positive but less than the request, it means you have reached the end of file.
Hint:  You may also need to track between calls where in the buffer you left off, and which block of the file you are at.

You also need to be able to handle if the read request is greater than 512, meaning that you may have to directly fill the caller's buffer from a 512 byte read (no need to buffer) then buffer just any amount needed to complete the caller's read request.

The `b_close` should free any resources you were using.

You can write additional helper routines as needed.

Limits:  You can assume no more than 20 files open at a time. (i.e. you need to ensure that multiple files can be open at one time, so the buffer you have for a file can not be global, but must be associated with that open file.

The main program (2b-main.o is provided) should use the command line arguments to specify data file and the desired target file. 
The main program uses `b_open`, reads exactly 80 characters at a time from the file using `b_read`, prints those 80 characters to the screen (ending in a newline character), and loop until it has read the entire file, then `b_close` the file and exit.

You are provided the file DATA as the file you are accessing and that contains the files IHaveADream.txt, CommonSense.txt, DecOfInd.txt

You should submit your source code file(s) and Makefile, along with a short writeup in PDF format that includes a description of what you did and the compilation and execution output from your program in GitHub, and the PDF also in iLearn.

The ONLY files you will modify is the `Makefile` and `b_io.c`


Rubric
| Grade Item                    | Grade Points                                  |
|:------------------------------|----------------------------------------------:|
| Standard Header               |   2                                           |
| Proper open and malloc        |  10                                           |
| Proper tracking for read      |  15                                           |
| Proper buffering              |  20                                           |
| Proper use of close and free  |   5                                           |
| Correct Output                |   5                                           |
| Code Comments                 |   5                                           |
| Writeup                       |   8 (Description, Compilation, Sample Output) |
