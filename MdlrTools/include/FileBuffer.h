/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author ARTEFACTO - François FOURNEL

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/*
		COPYRIGHTS (c) ARTEFACTO 
		
			!! Licence GPL !!	
*/

#ifndef __FileBuffer_h__
#define __FileBuffer_h__

#include <OgrePrerequisites.h>

//regular mem handler
#if (OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR < 6)
		#include <OgreMemoryMacros.h>
#endif
#include <Ogre.h>


using namespace Ogre;

/// brief This is the class representing a FileBuffer. A FileBuffer is a smart pointer which contain a void* data of a file (and his size). It works with a references counter, when the ,last FileBuffer pointing on a void* is deleted, the void* is deleted to.
/// file FileBuffer.h
/// author François FOURNEL
/// date 2007.06.25

namespace Solipsis {

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

} //namespace

#endif
