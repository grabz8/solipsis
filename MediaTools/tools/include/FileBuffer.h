/*
		COPYRIGHTS (c) ARTEFACTO 
		
			!! Licence GPL !!	
*/

#ifndef __FileBuffer_h__
#define __FileBuffer_h__

//regular mem handler
#include <OgreMemoryMacros.h>
#include <Ogre.h>


using namespace Ogre;

/// brief This is the class representing a FileBuffer. A FileBuffer is a smart pointer which contain a void* data of a file (and his size). It works with a references counter, when the ,last FileBuffer pointing on a void* is deleted, the void* is deleted to.
/// file FileBuffer.h
/// author François FOURNEL
/// date 2007.06.25


class FileBuffer
{
public:
	///brief Constructor, will add 1 to the number of references of the void*.
	///param fileData void* of the file.
	///param fileSize Size of the file in bytes.
	FileBuffer(void* fileData, size_t fileSize) : data(fileData), size(fileSize)
	{
		//If the buffer is NULL, the FileBuffer is empty no references count needed
		if (data == NULL) return;

		if (mReferencesCounts.find(data) == mReferencesCounts.end()) mReferencesCounts[data] = 0;
		mReferencesCounts[data]++;
	}

	///brief Copy constructor, will add 1 to the reference of the void* of pointed by the two FileBuffer (source and destination).
	///param fileBuffer FileBuffer source used to build the new FileBuffer this.
	FileBuffer(const FileBuffer& fileBuffer) : data(fileBuffer.data), size(fileBuffer.size)
	{
		//If the buffer is NULL, the FileBuffer is empty no references count needed
		if (data == NULL) return;

		mReferencesCounts[data]++;
	}

	///Destructor, if the class is the last reference to the void* data, the void* is deleted.
	~FileBuffer()
	{
		//If the buffer is NULL, the FileBuffer is empty no references count needed
		if (data == NULL) return;

		mReferencesCounts[data]--;

		//Is this class the last reference to the data ?
		if (mReferencesCounts[data] == 0)
		{
			//we delete the data buffer.
			delete data;
			//we could let the reference count of the data buffer to 0 in the map, but it's better to delete it, in order to free memory.
			mReferencesCounts.erase(data);
		}
	}



	///brief Method which return the file buffer formated to text. In fact, it just add the '\0' character at the end of the file and return a String.
	///return The file buffer formated to text
	String getBufferFormatedToText()
	{
		char* textBuffer = new char[size+1];
		memcpy(textBuffer,data,size);
		textBuffer[size] = '\0';

		String res(textBuffer);

		delete textBuffer;

		return res;
	}
		



 
	void* data;										///brief void* buffer containing our file (must be deleted only by the class).
	size_t size;									///brief size of the buffer in bytes.

private:
	static std::map<void*,int> mReferencesCounts;	///brief Number or references of each void* registered in a FileBuffer.
};


#endif
