/**************************************************************
* Class:  CSC-415-02 Fall 2021
* Name: Arslan Alimov
* Student ID: 916612104
* GitHub UserID:
* Project: Assignment 2B – Buffered I/O
*
* File: b_io.c
*
* Description: 
*				we are basically creating a buffer that takes bytes and reads it off the file 
*				and gives it to our user 
*				it is similar to a server / user example - the user asks the server for some specifict bytes, and the server gives it to him
* 				till it has enough space in the buffer, else it will give a user 0 or whatever is leftout in the buffer.
*
**************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "b_io.h"
#include "fsLowSmall.h"

#define MAXFCBS 20

typedef struct b_fcb
{
	fileInfo *fi; //holds the low level systems file info
				  // Add any other needed variables here to track the individual open file
	int blockOffset;
	int bufferOffset;
	char *Buffer;
} b_fcb;

/*
My struct - I decided to add three more 'variables'
blockOffset - which basically takes care of block index
bufferOffset takes care of each buffer index
*/

b_fcb fcbArray[MAXFCBS];

int startup = 0; //Indicates that this has not been initialized

//Method to initialize our file system
void b_init()
{
	//init fcbArray to all free
	for (int i = 0; i < MAXFCBS; i++)
	{
		fcbArray[i].fi = NULL; //indicates a free fcbArray
	}

	startup = 1;
}

//Method to get a free FCB element
b_io_fd b_getFCB()
{
	for (int i = 0; i < MAXFCBS; i++)
	{
		if (fcbArray[i].fi == NULL)
		{
			return i; //Not thread safe (but do not worry about this)
		}
	}
	return (-1); //all in use
}

// You will create your own file descriptor which is just an integer index into an array
// that you maintain for each open file.

b_io_fd b_open(char *filename, int flags)
{
	if (startup == 0)
		b_init(); //Initialize our system

	//*** TODO ***:  Write open function to return your file descriptor
	//                 You may want to allocate the buffer here as well
	//                 But make sure every file has its own buffer


	b_io_fd FD = b_getFCB();
	/*
	Here we are getting free FCB element
	we also check for element to be not -1 because if its -1 means all elements are in use
	*/
	if (FD != -1)
	{
		fcbArray[FD].fi = GetFileInfo(filename);
	}


	/*
	we return FD - our element
	*/
	//printf("%s",fileData.fi->fileName);
	//printf("\n%d\n",fileData.fi->location);
	return FD;
}

int b_read(b_io_fd fd, char *buffer, int count)
{
	//*** TODO ***:  Write buffered read function to return the data and # bytes read
	//               You must use the LBAread and you must buffer the data
	//				 in 512 byte chunks.

	if (startup == 0)
		b_init(); //Initialize our system
	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
	{
		return (-1); //invalid file descriptor
	}



	/*
	b_fcb* file = &fcbArray[fd];
	b_fcb* file = fcbArray + fd;
	I decided to create a pointer to my datastruct,because we will be using it quiet often in this project and typing 
	fcbArray[fd] not super convenient to me.
	 IGNORE -made a mistake - b_fcb file = fcbArray[fd] was making a shallow copy of fcbArray[fd] for  future
	*/
	b_fcb *file = &fcbArray[fd];


	/*
	In this portion we check if our FD was not an open file therefore we return -1

	*/
	if (!file->fi)
		return -1; 

	/*
	We check for a buffer if there is no buffer then we initialize it 
	Mallocate 512 - because we don't wanna waste too much memory.

	*/
	if (!file->Buffer)
	{
		/*
		Thats first time we are using LBAread, to read the file into our datastruct buffer.
		lbaCount is how many chunks to copy into the provided buffer
		lbaPosition is which chunk to start the copy from.
		So in order to copy the indicated chunk, you'd call LBARead( fcbArray[fd].yourBuffer, 1, fcbArray[fd].fi->location) - read 1 chunk, starting from chunk 1.
		and location is give to us by our file.fi->location
		*/
		file->Buffer = (char *)malloc(B_CHUNK_SIZE * sizeof(char));
		LBAread(file->Buffer, 1, file->fi->location);

		// file->blockOffset = 0
		// file->bufferOffset = 0
	}

	/*
	I am creating a read cursor which keeps track of my file and how much of a file I read. 
	It checks if we are at the end of the file yet?
	*/
	int readCursor = file->blockOffset * B_CHUNK_SIZE + file->bufferOffset;

	/*
	this condition  occurs if the previous `b_read` reads exactly  the entire remainder of the file!
	which means that we just return 0
	*/

	if (file->fi->fileSize == readCursor)
		return 0;

	/*
		Check if this call will reach EOF
		I also decided to create my return value which is written(basically our bytes passed into our arguments)
		it is convenient to have one return value that I can keep track of rather than having different values each time.
	*/
	if (count > (file->fi->fileSize - readCursor))
		count = (file->fi->fileSize - readCursor);
	int written = count; 

	/*
	This code checks if we get bytes anything more than 0 works 
	*/
	while (count > 0)
	{
		//we calculate how much of bytes left in our Buffer we take the buffer offset(index,position) and - the chunk size that we were given
		// in this case it is 512- whatever bytes we were given.
		int leftInBuffer = B_CHUNK_SIZE - file->bufferOffset;

		//if whatever bytes we were given is less than or equals to whatever left in buffer, we copy to memory without doing anything
		// and we set count to 0 because our while works if there is something greater than 0
		// we also do file->bufferoffset=file->bufferoffset+count
		if (count <= leftInBuffer)
		{
			memcpy(buffer, file->Buffer + file->bufferOffset, count);
			file->bufferOffset += count;
			count = 0;
		}
		else
		{ 	
			// if count is something that is greater than whatever left in buffer
			// we can not give more memory to our use than whatever is left in the buffer,because it will cause segfault 
			//buffer=buffer+leftinBuffer
			// we set our bufferoffset to 0 and we increment our blockoffset by 1 file->blockOffset= file->blockOffset +1
			memcpy(buffer, file->Buffer + file->bufferOffset, leftInBuffer);
			buffer += leftInBuffer;
			file->bufferOffset = 0;
			file->blockOffset += 1;
			//we call for lbaread again from a buffer, where we still give it that 1 chunk of file
			// this time our  location changes , we add whatever we have in blockoffset to our file location which moves to the next chunk that we are reading
			LBAread(file->Buffer, 1, file->fi->location + file->blockOffset);
			//we are decreasing count to whatever is left in our buffer because we can't provide more than whatever is left in our buffer.
			count -= leftInBuffer;
			
		}
	}

	return written;
}
/*
this is just releases the file descriptor
for every malloc there should be free 
we check if buffer exists andthen freeing it and setting the memory to 0
*/

void b_close(b_io_fd fd)
{
	//*** TODO ***:  Release any resources
	b_fcb *file = &fcbArray[fd];
	if (file->Buffer != NULL)
	{
		free(file->Buffer);
		memset(file, 0, sizeof(b_fcb));
		//file->Buffer=NULL;
	}

	/*
	file.Buffer = NULL;
	file.fi = NULL;
	file.blockOffset = 0;
	file.bufferOffset = 0;
	*/
}

/*
 I am not sure why we need b_init() since The array will be automatically zeroed-out at its creation.
*/

