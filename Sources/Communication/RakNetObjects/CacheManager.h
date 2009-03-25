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

#ifndef __CacheManager_h__
#define __CacheManager_h__

#include <string>
#include <list>
#include <RakPeerInterface.h>
#include <FileListTransferCBInterface.h>
#include <IncrementalReadInterface.h>
#include <XmlDatas.h>

namespace Solipsis {

class RakNetConnection;

/** This class defines the callback interface of the cache manager.
*/
class CacheManagerCallback
{
public:
    /** Called when transfer of 1 file is completed
    @param filename The filename of the received file
    */
    virtual void onTransferComplete(const std::string& filename) = 0;

    /** Called when a transfer is in progress
    @param filename The filename of the received file
    @param fProgress The download progress of this file
    @return should return the global progress of the entity.
    **/
    virtual float onTransferProgress(const std::string& filename, float fProgress) = 0;
 
};

/** This class manages 1 files cache.
*/
class CacheManager : public FileListTransferCBInterface, public IncrementalReadInterface
{
public:
    /// Entry state
    typedef float EntryState;
    static const EntryState ESTransferToRequest; //(-1)
    static const EntryState ESTransferComplete; //(1)

    /// Pending download
    typedef struct {
        unsigned short mFileListTransferSetID;
        CacheManagerCallback* mCallback;
    } PendingDownload;
    /// Pending downloads list
    typedef std::list<PendingDownload> PendingDownloadList;

    /// Pending upload
    typedef struct {
        unsigned short mFileListTransferSetID;
        SystemAddress mRecipient;
    } PendingUpload;
    /// Pending uploads list
    typedef std::list<PendingUpload> PendingUploadList;

    /// Cache entry
    typedef struct
    {
        FileVersion mVersion;
        EntryState mState;
        PendingDownloadList mPendingDownloadList;
        PendingUploadList mPendingUploadList;
    } Entry;
    /// Cache map of <filename, Entry>
    typedef std::map<std::string, Entry> CacheMap;

protected:
    /// Connection
    RakNetConnection *mConnection;
    /// Cache path
    std::string mCachePath;
    /// Map of files in cache
    CacheMap mCache;

    /// Cache filename
    static const std::string ms_CacheFilename;

public:
    /** Constructor.
    @param connection The RakNet connection (server or client)
    */
    CacheManager(RakNetConnection* connection);
    /** Destructor. */
    virtual ~CacheManager();

    /** Initialize cache by loading last saved state
    @param cachePath The cache path
    */
    void initialize(const std::string& cachePath);
    /** Finalize cache management by saving current state. */
    void finalize();

    /** See IncrementalReadInterface. */
	virtual unsigned int GetFilePart( char *filename, unsigned int startReadBytes, unsigned int numBytesToRead, void *preallocatedDestination, FileListNodeContext context);

    /** See FileListTransferCBInterface. */
    virtual bool OnFile(OnFileStruct *onFileStruct);
    /** See FileListTransferCBInterface. */
    virtual void OnFileProgress(OnFileStruct *onFileStruct,unsigned int partCount,unsigned int partTotal,unsigned int partLength, char *firstDataChunk);

    /** Add 1 file in cache.
    @param filename The filename of the cached file
    @param version The version of the cached file
    */
    void addFile(const std::string& filename, const FileVersion& version);
    /** Request 1 file in cache (already in cache or send 1 request to sender if not).
    @param sender The address of the system having the file to request to
    @param filename The filename of the requested file
    @param version The version of the requested file
    @param callback The callback instance (when transfer is complete, ...)
    */
    void requestFile(const SystemAddress& sender, const std::string& filename, const FileVersion& version, CacheManagerCallback* callback);
    /** Remove 1 file to manage
    @param filename The filename of the requested file
    @param callback The callback instance to remove from the pending download list
    */
    void removeFile(const std::string& filename, CacheManagerCallback* callback);

    /** Send 1 file to a recipient system which has requested it.
    @param recipient The address of the recipient system
    @param fileListTransferSetID The transfer identifier
    @param filename The filename of the sent file
    @param version The version of the sent file
    */
    void sendFile(const SystemAddress& recipient, unsigned short fileListTransferSetID, std::string& filename, const FileVersion& version);

protected:
    /** Retrieve the pathname of 1 file into the cache directory.
    @param filename The file name
    @param pathname The pathname of the file into the cache directory
    */
    void getCachePathname(const std::string& filename, std::string& pathname);
};

} // namespace Solipsis

#endif // #ifndef __CacheManager_h__