/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

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

#include "CacheManager.h"
#include "RakNetConnection.h"
#include <BitStream.h>
#include <FileList.h>
#include <FileOperations.h>
#include <CTLog.h>
#include <CTIO.h>

using namespace RakNet;
using namespace CommonTools;

namespace Solipsis {

const CacheManager::EntryState CacheManager::ESTransferAborted = -2.0f;
const CacheManager::EntryState CacheManager::ESTransferToRequest = -1.0f;
const CacheManager::EntryState CacheManager::ESTransferComplete = 1.0f;

const std::string CacheManager::ms_CacheFilename = "cache.xml";
unsigned int CacheManager::ms_SendChunkSize = 65536;
float CacheManager::ms_ProgressStepCallback = 0.1f;

//-------------------------------------------------------------------------------------
CacheManager::CacheManager(RakNetConnection* connection) :
    mConnection(connection),
    mCachePath("") 
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::CacheManager()");
}

//-------------------------------------------------------------------------------------
CacheManager::~CacheManager()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::~CacheManager()");
}

//-------------------------------------------------------------------------------------
void CacheManager::initialize(const std::string& cachePath)
{
    mCachePath = cachePath;

    // load the cache file
    load();

	// write the new cache file.
	// if there where errors (file not found, they will be removed)
	save();

    // Attach the FileListTransfer plugin
    mConnection->getRakPeer()->AttachPlugin(&mFileListTransfer);

    // set FileListTransfer callback to be warn about pushed files
    mFileListTransfer.SetCallback(this);
}

//-------------------------------------------------------------------------------------
void CacheManager::finalize()
{
    // unset FileListTransfer callback
    mFileListTransfer.SetCallback(0);

    // save the cache file
    save();
}

//-------------------------------------------------------------------------------------
void CacheManager::OnFilePush(const char *fileName, unsigned int fileLengthBytes, unsigned int offset, unsigned int bytesBeingSent, bool done, SystemAddress targetSystem)
{
    CacheMap::iterator entryIt = mCache.find(fileName);
    if (entryIt == mCache.end())
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "CacheManager::OnFilePush() File %s not found in cache table !", fileName);
        return;
    }
    CacheManagerFileEntry &entry = entryIt->second;
    PendingTransferList& pendingUploadList = entry.mPendingUploadList;
    for (PendingTransferList::iterator pendingUploadIt = pendingUploadList.begin(); pendingUploadIt != pendingUploadList.end(); ++pendingUploadIt)
        if (pendingUploadIt->mSystem == targetSystem)
        {
            float progress = (float)(offset + bytesBeingSent)/(float)fileLengthBytes;
            if (pendingUploadIt->mState >= 0.0f)
            {
                if ((pendingUploadIt->mState == 0.0f) || (progress >= std::min(1.0f, pendingUploadIt->mState + ms_ProgressStepCallback)))
                {
                    pendingUploadIt->mState = progress;
                    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::OnFilePush() File %s %.2f %% pushed to %s", fileName, pendingUploadIt->mState*100, targetSystem.ToString());
                    // Callback ?
                    if (entry.mCallback != 0)
                        entry.mCallback->onUploadProgress(entryIt->first, pendingUploadIt->mState);
                }
            }
            if (done)
                pendingUploadIt->mState = ESTransferComplete;
            break;
        }
}

//-------------------------------------------------------------------------------------
bool CacheManager::OnFile(OnFileStruct *onFileStruct)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::OnFile() File %s", onFileStruct->fileName);

    std::string filename = onFileStruct->fileName;
    std::string pathname;
    getCachePathname(filename, pathname);
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::OnFile() Writing file content into pathname %s", pathname.c_str());
    if (!IO::writeFileContent(pathname, (char*)onFileStruct->fileData, (unsigned int)onFileStruct->finalDataLength))
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "CacheManager::OnFile() writeFileContent(%s) failed !", pathname.c_str());
        return true;
    }
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::OnFile() Finished writing file content into pathname %s", pathname.c_str());

    CacheMap::iterator entryIt = mCache.find(filename);
    if (entryIt == mCache.end())
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "CacheManager::OnFile() File %s not found in cache table !", filename.c_str());
        return true;
    }
    CacheManagerFileEntry &entry = entryIt->second;

    entry.mFileSize = IO::getFileSize(pathname);
    entry.mDownload.mState = ESTransferComplete;

    // Call callback
    if (entry.mCallback != 0)
        entry.mCallback->onDownloadProgress(entryIt->first, ESTransferComplete);

    // flush/update the cache file
    save();

    return true;
}

//-------------------------------------------------------------------------------------
void CacheManager::OnFileProgress(OnFileStruct *onFileStruct,unsigned int partCount,unsigned int partTotal,unsigned int partLength, char *firstDataChunk)
{
    std::string filename = onFileStruct->fileName;
    CacheMap::iterator entryIt = mCache.find(filename);
    if (entryIt == mCache.end())
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "CacheManager::OnFileProgress() File %s not found in cache table !", filename.c_str());
        return;
    }

    CacheManagerFileEntry &entry = entryIt->second;
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::OnFileProgress() File %s %d, %d, %d", onFileStruct->fileName, partCount, partTotal, partLength);
    float progress = (float)partCount/(float)partTotal;
    if (progress >= std::min(1.0f, entry.mDownload.mState + ms_ProgressStepCallback))
    {
        entry.mDownload.mState = progress;
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::OnFileProgress() File %s %.2f %% received from %s", onFileStruct->fileName, entry.mDownload.mState*100, entry.mDownload.mSystem.ToString());
        // Callback ?
        if ((entry.mCallback != 0) && (entry.mDownload.mState < ESTransferComplete))
            entry.mCallback->onDownloadProgress(entryIt->first, entry.mDownload.mState);
    }
}
 
//-------------------------------------------------------------------------------------
unsigned int CacheManager::GetFilePart(char *filename, 
                                       unsigned int startReadBytes, unsigned int numBytesToRead, 
                                       void *preallocatedDestination, 
                                       FileListNodeContext context)
{
/*    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::GetFilePart() File %s (%d, %d, 0x%08x, (%d, %d))",
        filename, startReadBytes, numBytesToRead, 
        preallocatedDestination, context.op, context.fileId);*/

#if ((RAKNET_VERSION_MAJOR < 3) || \
     (RAKNET_VERSION_MAJOR == 3 && RAKNET_VERSION_MINOR < 5) || \
     (RAKNET_VERSION_MAJOR == 3 && RAKNET_VERSION_MINOR == 5 && RAKNET_VERSION_PATCH < 1))
    // on RakNet 3.401
    std::string baseFilename = filename;
    std::string pathname;
    getCachePathname(baseFilename, pathname);
#else
    // on RakNet 3.51
    std::string pathname = filename;
    std::string baseFilename = IO::getFileName(pathname);
#endif

    CacheMap::iterator entryIt = mCache.find(baseFilename);
    if (entryIt == mCache.end())
    {
        LOGHANDLER_LOGF(LogHandler::VL_ERROR, "CacheManager::GetFilePart() File %s not found in cache table !", baseFilename);
        return numBytesToRead;
    }
    long size = entryIt->second.mFileSize;

/*    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::GetFilePart() File %s %.2f %% (%d, %d, 0x%08x, (%d, %d))",
        baseFilename.c_str(), (float)(std::min(size, long(startReadBytes + numBytesToRead)))*100/(float)size , startReadBytes, numBytesToRead, 
        preallocatedDestination, context.op, context.fileId);*/
//    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::GetFilePart() File %s %.2f %%", baseFilename.c_str(), (float)(std::min(size, long(startReadBytes + numBytesToRead)))*100/(float)size);

    return IncrementalReadInterface::GetFilePart((char *)pathname.c_str(), startReadBytes, numBytesToRead, preallocatedDestination, context);
}

//-------------------------------------------------------------------------------------
void CacheManager::addFile(const std::string& filename, const FileVersion& version, CacheManagerCallback* callback)
{
    std::string completeFileName;
    getCachePathname(filename, completeFileName);
	// check if the file is present
	if (IO::isFileExists(completeFileName)) 
	{
		CacheMap::iterator entryIt = mCache.find(filename);
		if (entryIt == mCache.end())
		{
			CacheManagerFileEntry entry;
			mCache[filename] = entry;
			entryIt = mCache.find(filename);
		}
        CacheManagerFileEntry &entry = entryIt->second;
		entry.mVersion = version;
        entry.mFileSize = IO::getFileSize(completeFileName);
        entry.mCallback = callback;
        entry.mDownload.mSystem = UNASSIGNED_SYSTEM_ADDRESS;
        entry.mDownload.mFileListTransferSetID = 0;
        entry.mDownload.mState = ESTransferComplete;

        // flush/update the cache file
        save();
	}
}

//-------------------------------------------------------------------------------------
void CacheManager::removeFile(const std::string& filename)
{
    cancelFile(filename, true);
}

//-------------------------------------------------------------------------------------
void CacheManager::requestFile(const SystemAddress& sender,
                               const std::string& filename,
                               const FileVersion& version,
                               CacheManagerCallback* callback)
{
    CacheMap::iterator entryIt = mCache.find(filename);
    if (entryIt == mCache.end())
    {
        CacheManagerFileEntry entry;
        entry.mDownload.mState = ESTransferToRequest;
        mCache[filename] = entry;
        entryIt = mCache.find(filename);
    }
    else if (entryIt->second.mVersion != version)
    {
        entryIt->second.mDownload.mState = ESTransferToRequest;
    }
    else if (entryIt->second.mDownload.mState == ESTransferComplete)
	{
        CacheManagerFileEntry &entry = entryIt->second;
        std::string completeFileName;
        getCachePathname(filename, completeFileName);
		// to prevent internal errors, check that the file is really here
		if (IO::isFileExists(completeFileName))
            callback->onDownloadProgress(entryIt->first, ESTransferComplete);
		else
			entry.mDownload.mState = ESTransferToRequest;
	}
    if (entryIt->second.mDownload.mState == ESTransferToRequest)
    {
        CacheManagerFileEntry &entry = entryIt->second;
        entry.mVersion = version;
        entry.mCallback = callback;
        entry.mDownload.mState = 0.0f;
        BitStream bitStream;
        bitStream.Write((MessageID)RakNetConnection::ID_CM_REQUESTING_FILETRANSFER);
        entry.mDownload.mSystem = sender;
        entry.mDownload.mFileListTransferSetID = mFileListTransfer.SetupReceive(this, false, sender);
        bitStream.Write(entry.mDownload.mFileListTransferSetID);
        RakNetConnection::SerializeString(&bitStream, filename);
        bitStream.Write(entry.mVersion);
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::requestFile() Requesting file %s, version %d, fileListTransferSetID %d", filename.c_str(), version, entry.mDownload.mFileListTransferSetID);
        // Send the request to the server
        mConnection->getRakPeer()->Send(&bitStream, LOW_PRIORITY, RELIABLE_ORDERED, 0, sender, false);
    }
}

//-------------------------------------------------------------------------------------
void CacheManager::cancelFile(const std::string& filename, bool removeFile)
{
    CacheMap::iterator entryIt = mCache.find(filename);
    if (entryIt == mCache.end())
    {
        LOGHANDLER_LOGF(LogHandler::VL_WARNING, "CacheManager::cancelFile() File %s not found in cache table !", filename.c_str());
        return;
    }
    CacheManagerFileEntry &entry = entryIt->second;

    // Download
    if ((entry.mDownload.mState >= 0.0f) && (entry.mDownload.mState != ESTransferComplete))
    {
        removeFile = true;
        if (mFileListTransfer.IsHandlerActive(entry.mDownload.mFileListTransferSetID))
            mFileListTransfer.CancelReceive(entry.mDownload.mFileListTransferSetID);
        if (entry.mCallback != 0)
            entry.mCallback->onDownloadProgress(entryIt->first, ESTransferAborted);
    }
    // Uploads (notice that they cannot be really cancelled with RakNet 3.x)
    PendingTransferList& pendingUploadList = entryIt->second.mPendingUploadList;
    for (PendingTransferList::iterator pendingUploadIt = pendingUploadList.begin(); pendingUploadIt != pendingUploadList.end(); ++pendingUploadIt)
    {
        if (entry.mCallback != 0)
            entry.mCallback->onUploadProgress(entryIt->first, ESTransferAborted);
    }

    entry.mCallback = 0;
    if (removeFile)
    {
        mCache.erase(entryIt);
        std::string completeFileName;
        getCachePathname(filename, completeFileName);
        IO::deleteFile(completeFileName);
    }
}

//-------------------------------------------------------------------------------------
void CacheManager::sendFile(const SystemAddress& recipient, unsigned short fileListTransferSetID, std::string& filename, const FileVersion& version)
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::sendFile() Sending file %s, version %d to recipient %s, fileListTransferSetID %d", filename.c_str(), version, recipient.ToString(), fileListTransferSetID);

    CacheMap::iterator entryIt = mCache.find(filename);
    if (entryIt == mCache.end())
    {
        LOGHANDLER_LOGF(LogHandler::VL_WARNING, "CacheManager::sendFile() File %s not found in cache table !", filename.c_str());
        return;
    }
    CacheManagerFileEntry &entry = entryIt->second;

    // Add it to the pending upload list, it will be sent now or as soon as it will be downloaded
    TransferDesc pendingUpload;
    pendingUpload.mSystem = recipient;
    pendingUpload.mFileListTransferSetID = fileListTransferSetID;
    if (entry.mDownload.mState == ESTransferComplete)
        pendingUpload.mState = 0.0f;
    else
        pendingUpload.mState = ESTransferToRequest;
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::sendFile() Adding upload file %s, version %d to recipient %s, fileListTransferSetID %d", filename.c_str(), entry.mVersion, recipient.ToString(), fileListTransferSetID);
    entry.mPendingUploadList.push_back(pendingUpload);
    if (entry.mDownload.mState == ESTransferComplete)
    {
        // Send it now !
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::sendFile() Sending now file %s, version %d to recipient %s, fileListTransferSetID %d", filename.c_str(), version, recipient.ToString(), fileListTransferSetID);
        FileList fileList;
        std::string pathname;
        getCachePathname(filename, pathname);
        long filesize = IO::getFileSize(pathname);
        if (filesize == -1)
            LOGHANDLER_LOGF(LogHandler::VL_ERROR, "CacheManager::sendFile() Unable to get size of file %s !", filename.c_str());
#if ((RAKNET_VERSION_MAJOR < 3) || \
     (RAKNET_VERSION_MAJOR == 3 && RAKNET_VERSION_MINOR < 5) || \
     (RAKNET_VERSION_MAJOR == 3 && RAKNET_VERSION_MINOR == 5 && RAKNET_VERSION_PATCH < 1))
        // on RakNet 3.401
        fileList.AddFile(filename.c_str(), 0, (unsigned int)filesize, (unsigned int)filesize, FileListNodeContext(0, 0), true);
#else
        // on RakNet 3.51
        fileList.AddFile(filename.c_str(), pathname.c_str(), 0, (unsigned int)filesize, (unsigned int)filesize, FileListNodeContext(0, 0), true);
#endif
        mFileListTransfer.Send(&fileList, mConnection->getRakPeer(), pendingUpload.mSystem, pendingUpload.mFileListTransferSetID, LOW_PRIORITY, 0, false, this, ms_SendChunkSize);
    }
    else
    {
        // It will be sent as soon as it will be downloaded
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::sendFile() Add in pendingUpload file %s, version %d to recipient %s, fileListTransferSetID %d", filename.c_str(), version, recipient.ToString(), fileListTransferSetID);
    }
}

//-------------------------------------------------------------------------------------
bool CacheManager::havePendingDownload()
{
    unsigned int totalPendingDownloads = 0;
    for (CacheMap::iterator entryIt = mCache.begin(); entryIt != mCache.end(); ++entryIt)
    {
        CacheManagerFileEntry &entry = entryIt->second;
        if (entry.mDownload.mState != ESTransferComplete)
        {
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::havePendingDownload() %d: %s version %d system %s state %.2f",
                totalPendingDownloads,
                entryIt->first.c_str(),
                entry.mVersion,
                entry.mDownload.mSystem.ToString(),
                entry.mDownload.mState);
            totalPendingDownloads++;
        }
    }

    return (totalPendingDownloads != 0);
}

//-------------------------------------------------------------------------------------
bool CacheManager::havePendingUpload()
{
    unsigned int totalPendingUploads = 0;
    for (CacheMap::iterator entryIt = mCache.begin(); entryIt != mCache.end(); ++entryIt)
    {
        PendingTransferList& pendingUploadList = entryIt->second.mPendingUploadList;
        for (PendingTransferList::iterator pendingUploadIt = pendingUploadList.begin(); pendingUploadIt != pendingUploadList.end(); ++pendingUploadIt)
        {
            if (pendingUploadIt->mState == ESTransferComplete)
                continue;
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::havePendingUpload() %d: %s version %d system %s state %.2f",
                totalPendingUploads,
                entryIt->first.c_str(),
                entryIt->second.mVersion,
                pendingUploadIt->mSystem.ToString(),
                pendingUploadIt->mState);
            totalPendingUploads++;
        }
    }
    return (totalPendingUploads != 0);
}

//-------------------------------------------------------------------------------------
void CacheManager::removeConnection(const SystemAddress& system)
{
    // Cancel downloading files
    std::list<std::string> filesToCancel;
    for (CacheMap::iterator entryIt = mCache.begin(); entryIt != mCache.end(); ++entryIt)
    {
        CacheManagerFileEntry &entry = entryIt->second;
        if ((entry.mDownload.mState != ESTransferComplete) && (entry.mDownload.mSystem == system))
            filesToCancel.push_back(entryIt->first);
    }
    // Cancel downloading files
    for (std::list<std::string>::const_iterator it = filesToCancel.begin(); it != filesToCancel.end(); ++it)
        cancelFile(*it, true);

    // Cancel uploading files
    for (CacheMap::iterator entryIt = mCache.begin(); entryIt != mCache.end(); ++entryIt)
    {
        CacheManagerFileEntry &entry = entryIt->second;
        PendingTransferList& pendingUploadList = entry.mPendingUploadList;
        PendingTransferList::iterator pendingUploadIt = pendingUploadList.begin();
        while (pendingUploadIt != pendingUploadList.end())
        {
            if (pendingUploadIt->mSystem != system)
            {
                pendingUploadIt++;
                continue;
            }
            // Uploads (notice that they cannot be really cancelled with RakNet 3.x)
            if (entry.mCallback != 0)
                entry.mCallback->onUploadProgress(entryIt->first, ESTransferAborted);
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::removeConnection() Removing upload file %s, version %d to recipient %s, fileListTransferSetID %d", entryIt->first.c_str(), entry.mVersion, pendingUploadIt->mSystem.ToString(), pendingUploadIt->mFileListTransferSetID);
            pendingUploadIt = pendingUploadList.erase(pendingUploadIt);
        }
    }

    // Plugin will remove receiver into OnClosedConnection
    //mFileListTransfer.RemoveReceiver(system);
}

//-------------------------------------------------------------------------------------
void CacheManager::update()
{
    for (CacheMap::iterator entryIt = mCache.begin(); entryIt != mCache.end(); ++entryIt)
    {
        CacheManagerFileEntry &entry = entryIt->second;

        std::string filename = entryIt->first;
        std::string pathname;
        getCachePathname(entryIt->first, pathname);

        PendingTransferList& pendingUploadList = entry.mPendingUploadList;
        PendingTransferList::iterator pendingUploadIt = pendingUploadList.begin();
        while (pendingUploadIt != pendingUploadList.end())
        {
            // Removing uploaded entry
            if (pendingUploadIt->mState == ESTransferComplete)
            {
                LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::update() Removing uploaded file %s, version %d to recipient %s, fileListTransferSetID %d", filename.c_str(), entry.mVersion, pendingUploadIt->mSystem.ToString(), pendingUploadIt->mFileListTransferSetID);
                pendingUploadIt = pendingUploadList.erase(pendingUploadIt);
                continue;
            }
            // Send pending upload
            if ((pendingUploadIt->mState == ESTransferToRequest) &&
                (entry.mDownload.mState == ESTransferComplete))
            {
                long filesize = IO::getFileSize(pathname);
                if (filesize == -1)
                    LOGHANDLER_LOGF(LogHandler::VL_ERROR, "CacheManager::update() Unable to get size of file %s !", filename.c_str());
                FileList fileList;
#if ((RAKNET_VERSION_MAJOR < 3) || \
 (RAKNET_VERSION_MAJOR == 3 && RAKNET_VERSION_MINOR < 5) || \
 (RAKNET_VERSION_MAJOR == 3 && RAKNET_VERSION_MINOR == 5 && RAKNET_VERSION_PATCH < 1))
                // on RakNet 3.401
                fileList.AddFile(filename.c_str(), 0, (unsigned int)filesize, (unsigned int)filesize, FileListNodeContext(0, 0), true);
#else
                // on RakNet 3.51
                fileList.AddFile(filename.c_str(), pathname.c_str(), 0, (unsigned int)filesize, (unsigned int)filesize, FileListNodeContext(0, 0), true);
#endif
                LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::update() Sending now file %s, version %d to recipient %s, fileListTransferSetID %d", filename.c_str(), entry.mVersion, pendingUploadIt->mSystem.ToString(), pendingUploadIt->mFileListTransferSetID);
                pendingUploadIt->mState = 0.0f;
                mFileListTransfer.Send(&fileList, mConnection->getRakPeer(), pendingUploadIt->mSystem, pendingUploadIt->mFileListTransferSetID, LOW_PRIORITY, 0, false, this, ms_SendChunkSize);
            }
            pendingUploadIt++;
        }
    }
}

//-------------------------------------------------------------------------------------
void CacheManager::getCachePathname(const std::string& filename, std::string& pathname)
{
    if (!IO::isDirectoryExists(mCachePath))
        IO::createDirectory(mCachePath);
    pathname = mCachePath + IO::getPathSeparator() + filename;
}

//-------------------------------------------------------------------------------------
void CacheManager::load()
{
    std::string filename;
    getCachePathname(ms_CacheFilename, filename);

    LOGHANDLER_LOGF(LogHandler::VL_INFO, "CacheManager::load() Loading cache from %s", filename.c_str());

    try
    {
        TiXmlDocument xmlFileDoc(filename.c_str());
        if (!xmlFileDoc.LoadFile())
            throw std::string("Parsing error");

        TiXmlElement* cacheElt = xmlFileDoc.FirstChildElement("cache");
        if (cacheElt == 0)
            throw std::string("Parsing error");

        TiXmlElement* filesElt;
        if ((filesElt = cacheElt->FirstChildElement("files")) != 0)
        {
            const char* attr = 0;
            for (TiXmlElement* fileElt = filesElt->FirstChildElement("file"); fileElt != 0; fileElt = fileElt->NextSiblingElement("file"))
            {
                LodContentFileStruct lodContentFileStruct;
                if (!XmlHelpers::fromXmlEltLodContentFileStruct(fileElt, lodContentFileStruct))
                    throw std::string("Parsing error");

                addFile(lodContentFileStruct.mFilename, lodContentFileStruct.mVersion);
            }
        }
    }
    catch (std::string exceptionStr)
    {
        LOGHANDLER_LOGF(LogHandler::VL_WARNING, "CacheManager::initialize() Unable to read/parse cache from %s, Resetting it.", filename);
        mCache.clear();
    }
}

//-------------------------------------------------------------------------------------
void CacheManager::save()
{
    std::string pathname;
    getCachePathname(ms_CacheFilename, pathname);

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::save() Saving cache into %s", pathname.c_str());

    TiXmlDocument xmlFileDoc(pathname.c_str());
    // root node
    TiXmlElement* cacheElt = new TiXmlElement("cache");
    // Add entries
    TiXmlElement* filesElt = new TiXmlElement("files");
    cacheElt->LinkEndChild(filesElt); 
    for (CacheMap::iterator it = mCache.begin(); it != mCache.end(); ++it)
    {
        if (it->second.mDownload.mState != ESTransferComplete) continue;
        LodContentFileStruct lodContentFileStruct;
        lodContentFileStruct.mFilename = it->first;
        lodContentFileStruct.mVersion = it->second.mVersion;
        TiXmlElement* fileElt = XmlHelpers::toXmlEltLodContentFileStruct("file", lodContentFileStruct);
        filesElt->LinkEndChild(fileElt);
    }
    xmlFileDoc.LinkEndChild(cacheElt); 
    xmlFileDoc.SaveFile();
}

//-------------------------------------------------------------------------------------
void CacheManager::logAll()
{
    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "CacheManager::logAll()");
    for (CacheMap::iterator entryIt = mCache.begin(); entryIt != mCache.end(); ++entryIt)
    {
        CacheManagerFileEntry &entry = entryIt->second;
        PendingTransferList& pendingUploadList = entry.mPendingUploadList;
        LOGHANDLER_LOGF(LogHandler::VL_DEBUG, " file %s, version %d, size %ld, download(%s, %d, %.2f)", entryIt->first.c_str(), entry.mVersion, entry.mFileSize, entry.mDownload.mSystem.ToString(), entry.mDownload.mFileListTransferSetID, entry.mDownload.mState);
        std::string uploads;
        for (PendingTransferList::const_iterator pendingUploadIt = pendingUploadList.begin(); pendingUploadIt != pendingUploadList.end(); ++pendingUploadIt)
            LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "  upload(%s, %d, %.2f)", pendingUploadIt->mSystem.ToString(), pendingUploadIt->mFileListTransferSetID, pendingUploadIt->mState);
    }
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
