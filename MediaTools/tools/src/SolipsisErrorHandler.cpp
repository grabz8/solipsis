
#include <Ogre.h>
#include "SolipsisErrorHandler.h"
#include <sys/stat.h>


void insertInLog(int type,const char *message,const char* message2)
{
	FILE * flog = fopen("SOLlog.log","a+");
	if (flog)
	{
		char* pBuf;
		pBuf = new char[4096];

#ifdef WIN32
		SYSTEMTIME time;
		GetLocalTime(&time);
		if (strcmp(message,"\n"))
			switch (type) {
				case 0  : 
					if (!message2)
						sprintf(pBuf,"%.4u-%.2u-%.2u %.2u:%.2u:%.2u - [INFO] %s \n",time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond,message);
					else
                        sprintf(pBuf,"%.4u-%.2u-%.2u %.2u:%.2u:%.2u - [INFO] %s : %s\n",time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond,message,message2);
					break;
				case 1  : 
					if (!message2)
						sprintf(pBuf,"%.4u-%.2u-%.2u %.2u:%.2u:%.2u - [WARNING] %s \n",time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond,message);
					else
						sprintf(pBuf,"%.4u-%.2u-%.2u %.2u:%.2u:%.2u - [WARNING] %s : %s\n",time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond,message,message2);
					break;
				case 2  : 
					if (!message2)
                        sprintf(pBuf,"%.4u-%.2u-%.2u %.2u:%.2u:%.2u - [ERROR] %s \n",time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond,message);
					else
						sprintf(pBuf,"%.4u-%.2u-%.2u %.2u:%.2u:%.2u - [ERROR] %s : %s\n",time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond,message,message2);
					break;
			}
		else
			sprintf(pBuf,"\n");	
#else
		struct timeval tv;
		struct tm* ptm;
		char time_string[40];
		long milliseconds;

		/* Obtain the time of day, and convert it to a tm struct. */
		gettimeofday (&tv, NULL);
		ptm = localtime (&tv.tv_sec);
		/* Format the date and time, down to a single second. */
		strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
		/* Compute milliseconds from microseconds. */
		milliseconds = tv.tv_usec / 1000;
		/* Print the formatted time, in seconds, followed by a decimal point
		and the milliseconds. */
		if (!message2)
			sprintf ("%s %s : %s ", time_string, message, message2);
		else
			sprintf ("%s %s ", time_string, message);
#endif

		fwrite(pBuf,sizeof(char),strlen(pBuf),flog);
		fclose(flog);

		delete []pBuf;
	}
}


void SOLIPSISERROR(const char *message,const char* message2)
{
	insertInLog(2,message,message2);
}


void SOLIPSISWARNING(const char *message,const char* message2)
{
	insertInLog(1,message,message2);
}

void SOLIPSISINFO(const char *message,const char* message2)
{
	insertInLog(0,message,message2);
}

Ogre::String replaceSubstr(const Ogre::String &src, const char* toFind, const char* subst)
{
	Ogre::String return_value = src; // backup original string
	// Find first occurence of toFind string
	size_t ind = return_value.find(toFind);
	do 
	{
		if (ind == return_value.npos) 
			break; // No more to find
		// Substitute substr
		return_value.replace(ind,strlen(toFind),subst);
		// Find next occurence of substr
		ind = return_value.find(toFind, ind);

	} while (ind != return_value.npos);

	return return_value;
}

bool SOLdeleteFile(const char *filePath)
{
#ifdef WIN32
	if (!DeleteFile(filePath))
		SOLIPSISWARNING("Unable to delete file - Please verify sharing access and user rights :",filePath);
#else
	// if the file exists and there's no write access
	if (access (filePath, F_OK) == 0)
	{
		// try to set the read/write access
		if (chmod (filePath, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IWGRP|S_IXGRP|S_IROTH|S_IWOTH|S_IXOTH) == -1)
		{
			SOLIPSISWARNING("Can't set RW access to file : ",filePath);
			return false;
		}
		int err = unlink (filePath);
		if (err == -1)
		{
			SOLIPSISWARNING("Unable to delete file - Please verify sharing access and user rights :",filePath);
			return false;
		}
	}
	else
	{
		SOLIPSISWARNING("Can't access to file ",filePath);
	}
#endif 
	return true;
}
