#include "MyZipArchive.h"

#include <stdio.h>
#include "SolipsisErrorHandler.h"

#include "../../tools/src/minizip/zip.h"
#include "../../tools/src/minizip/unzip.h"
#include "FileBuffer.h"

#include <fstream>
#include <iostream>

//--------------------------------------------------------------------------------------------------------------------------------------------------
MyZipArchive::MyZipArchive(const String& path)
{
	mPath = new Path(path);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
MyZipArchive::~MyZipArchive()
{
	delete mPath;
}



//--------------------------------------------------------------------------------------------------------------------------------------------------
bool MyZipArchive::isArchivePresent()
{
	unzFile archive = unzOpen(mPath->getFormatedPath().c_str());
	if (archive == NULL)
	{
		return false;
	}else{
        unzClose(archive);
		return true;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
bool MyZipArchive::isFilePresent(const String& filePath)
{
	if (!isArchivePresent()) return false;

	Path path(filePath);

	const char * testDebug = mPath->getFormatedPath().c_str() ;
	unzFile archive = unzOpen(testDebug);

	int resultResearch = unzLocateFile(archive,path.getFormatedPath().c_str(),0);
	bool fileFound = (resultResearch == UNZ_OK)? true : false;

	unzClose(archive);

	return fileFound;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
FileBuffer MyZipArchive::readFile(const String& filePath)
{
	if (!isFilePresent(filePath)) return FileBuffer(NULL,0);

	unzFile archive = unzOpen(mPath->getFormatedPath().c_str());

	Path path(filePath);
	unzLocateFile(archive,path.getFormatedPath().c_str(),0);

	//getting the file size.
	unz_file_info fileInfo;
	unzGetCurrentFileInfo(archive,&fileInfo,NULL,0,NULL,0,NULL,0);

	//open, read the file and store it in a buffer.
	unzOpenCurrentFile(archive);
	void* fileData = new char[fileInfo.uncompressed_size];
	unzReadCurrentFile(archive,fileData,fileInfo.uncompressed_size);

	//closing the current file
	unzCloseCurrentFile(archive);


	unzClose(archive);


	return FileBuffer(fileData,fileInfo.uncompressed_size);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
String MyZipArchive::getName(const int pNumFile)
{
	if (pNumFile >= getNbFile()) return (String (""));

	unzFile uFile = unzOpen(mPath->getFormatedPath().c_str());

	int positionInZip = unzGoToFirstFile(uFile);

	for(int i=0; i<pNumFile ; i++)
	{
		positionInZip = unzGoToNextFile(uFile);	//go to next file
	}

	//getting the current file informations
	unz_file_info currentFileInfo;
	char currentFileName[256];
	unzGetCurrentFileInfo(uFile,
		&currentFileInfo,
		currentFileName,
		sizeof(currentFileName),
		NULL,
		0,
		NULL,
		0);
	

	unzClose(uFile);
		
	return String (currentFileName) ;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void MyZipArchive::writeFile(const String& filePath)
{

	FILE* toAdd = fopen(filePath.c_str(),"rb");
	fseek(toAdd,0,SEEK_END);
	size_t toAddFileSize = ftell(toAdd);
	fseek(toAdd,0,SEEK_SET);
	void* toAddFileData = new unsigned char[toAddFileSize];
	fread(toAddFileData,toAddFileSize,sizeof(unsigned char),toAdd);
	writeFile(filePath.c_str(), FileBuffer(toAddFileData, toAddFileSize));

	fclose(toAdd);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void MyZipArchive::writeFile(const String& filePath, const FileBuffer& fileBuffer)
{
	removeFile(filePath);

	Path path(filePath);


	zipFile archive = zipOpen( mPath->getFormatedPath().c_str() ,APPEND_STATUS_ADDINZIP);
	if (!archive) // If the file does not exist => Create it 
		archive = zipOpen(mPath->getFormatedPath().c_str(),0);	

	//Get only name :
    int nameSizeChar = path.getFormatedPath().find_last_of( '\\' );
	std::string fileName (path.getFormatedPath(), nameSizeChar+1, path.getFormatedPath().length() );

	zipOpenNewFileInZip(archive,
		fileName.c_str(),
		NULL,
		NULL,
		0,
		NULL,
		0,
		NULL,
		Z_DEFLATED,
		Z_DEFAULT_COMPRESSION);

	zipWriteInFileInZip(archive,fileBuffer.data,(unsigned int)fileBuffer.size);

	zipCloseFileInZip(archive);

	zipClose(archive,NULL);
}


//--------------------------------------------------------------------------------------------------------------------------------------------------
void MyZipArchive::removeFile(const String& filePath)
{
	if (!isFilePresent(filePath)) return;

	Path path(filePath);

	SOLcopyFile(mPath->getFormatedPath().c_str(),Ogre::String(mPath->getFormatedPath() + ".__backup__").c_str());

	unzFile uFile = unzOpen((mPath->getFormatedPath() + ".__backup__").c_str());

	zipFile zFile = zipOpen((mPath->getFormatedPath()).c_str(),APPEND_STATUS_CREATE);
	if (zFile == NULL) 
	{
		SOLdeleteFile(Ogre::String(mPath->getFormatedPath() + ".__backup__").c_str());
		return;
	}

	int positionInZip = unzGoToFirstFile(uFile);
	while(positionInZip == UNZ_OK)
	{
		//getting the current file informations
		unz_file_info currentFileInfo;
		char currentFileName[256];
		unzGetCurrentFileInfo(uFile,
			&currentFileInfo,
			currentFileName,
			sizeof(currentFileName),
			NULL,
			0,
			NULL,
			0);


		if (strcmp(currentFileName,(const char*)(path.getFormatedPath()) .c_str()) != 0) //We add the file in the new zip only if it isn't the file we want to remove.
		{		
			//opening reading the data of the current file
			unzOpenCurrentFile(uFile);
			void* currentFileData = new unsigned char[currentFileInfo.uncompressed_size];
			unzReadCurrentFile(uFile,currentFileData,currentFileInfo.uncompressed_size);

			//copying te current file in the new zip
			zipOpenNewFileInZip(zFile,
				currentFileName,
				NULL,
				NULL,
				0,
				NULL,
				0,
				NULL,
				Z_DEFLATED,
				Z_DEFAULT_COMPRESSION);
			zipWriteInFileInZip(zFile,currentFileData,currentFileInfo.uncompressed_size);
			zipCloseFileInZip(zFile);


			//closing the current file
			delete currentFileData;
			unzCloseCurrentFile(uFile);
		}


		//go to next file
		positionInZip = unzGoToNextFile(uFile);
	}

	

	zipClose(zFile,NULL);
	unzClose(uFile);

	SOLdeleteFile(Ogre::String(mPath->getFormatedPath() + ".__backup__").c_str());
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
int MyZipArchive::getNbFile() 
{
	if (!isArchivePresent()) return 0;

	int result = 0 ;

	unzFile uFile = unzOpen(mPath->getFormatedPath().c_str());
	int positionInZip = unzGoToFirstFile(uFile);

	while(positionInZip == UNZ_OK)
	{
		result++; 
		positionInZip = unzGoToNextFile(uFile);	//go to next file
	}

	unzClose(uFile);

	return result ;
}

