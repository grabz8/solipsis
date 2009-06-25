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
#include <FileListTransfer.h>
#include <FileList.h>
#include <FileListTransferCBInterface.h>
#include <IncrementalReadInterface.h>

#include <XmlSceneLodContent.h>
#include <XmlSceneContent.h>

namespace Solipsis {

class RakNetConnection;

/** This class defines the callback interface of the cache manager.
*/
class CacheManagerCallback
{
public:
    /** Called when a download is in progress
    @param filename The filename of the received file
    @param progress The download progress of this file (0..1 download ratio, -1.0 if transfer aborted)
    @return should return the global progress of the entity.
    **/
    virtual float onDownloadProgress(const std::string& filename, float progress) = 0;
    /** Called when an upload is in progress
    @param filename The filename of the received file
    @param progress The upload progress of this file (0..1 upload ratio, -1.0 if transfer aborted)
    @return should return the global progress of the entity.
    **/
    virtual float onUploadProgress(const std::string& filename, float progress) = 0;
};

/** This class manages 1 files cache.
*/
class CacheManager : public FileListProgress, public FileListTransferCBInterface, public IncrementalReadInterface
{
public:
    /// CacheManager File Entry state
    typedef float EntryState;
    static const EntryState ESTransferAborted; //(-2.0f)
    static const EntryState ESTransferToRequest; //(-1.0f)
    static const EntryState ESTransferComplete; //(1.0f)

    /// Transfer description
    typedef struct {
        SystemAddress mSystem;
        unsigned short mFileListTransferSetID;
        EntryState mState;
    } TransferDesc;
    /// Pending transfer list
    typedef std::list<TransferDesc> PendingTransferList;

    /// Cache entry
    typedef struct
    {
        FileVersion mVersion;
        long mFileSize;
        CacheManagerCallback* mCallback;
        TransferDesc mDownload;
        PendingTransferList mPendingUploadList;
    } CacheManagerFileEntry;

    /// Cache map of <filename, CacheManagerFileEntry>
    typedef std::map<std::string, CacheManagerFileEntry> CacheMap;

protected:
    /// Connection
    RakNetConnection *mConnection;
    /// FileListTransfer plugin
    FileListTransfer mFileListTransfer;
    /// Cache path
    std::string mCachePath;
    /// Map of files in cache
    CacheMap mCache;

    /// Cache filename
    static const std::string ms_CacheFilename;
    /// Upload transfer chunk size (default value to 64Kb, RakNet use 256Kb)
    static unsigned int CacheManager::ms_SendChunkSize;
    /// Download/Upload progress step between each callback
    static float ms_ProgressStepCallback;

public:
    /** Constructor.
    @param connection The RakNet connection (server or client)
    */
    CacheManager(RakNetConnection* connection);
    /** Destructor. */
    virtual ~CacheManager();

    /** Get Download/Upload progress step between each callback */
    static float getProgressStepCallback() { return ms_ProgressStepCallback; }
    /** Set Download/Upload progress step between each callback */
    static void setProgressStepCallback(float progressStepCallback) { ms_ProgressStepCallback = progressStepCallback; }

    /** Initialize cache by loading last saved state
    @param cachePath The cache path
    */
    void initialize(const std::string& cachePath);
    /** Finalize cache management by saving current state. */
    void finalize();

    /** See FileListProgress. */
	virtual void OnFilePush(const char *fileName, unsigned int fileLengthBytes, unsigned int offset, unsigned int bytesBeingSent, bool done, SystemAddress targetSystem);

    /** See FileListTransferCBInterface. */
    virtual bool OnFile(OnFileStruct *onFileStruct);
    /** See FileListTransferCBInterface. */
    virtual void OnFileProgress(OnFileStruct *onFileStruct,unsigned int partCount,unsigned int partTotal,unsigned int partLength, char *firstDataChunk);

    /** See IncrementalReadInterface. */
	virtual unsigned int GetFilePart(char *filename, unsigned int startReadBytes, unsigned int numBytesToRead, void *preallocatedDestination, FileListNodeContext context);

    /** Add 1 file in cache.
    @param filename The filename of the cached file
    @param version The version of the cached file
    @param saveCacheFile If the cache file must be saved
    @param callback The callback instance (download/upload transfer progress)
    */
    void addFile(const std::string& filename, const FileVersion& version, bool saveCacheFile = false, CacheManagerCallback* callback = 0);
    /** Remove 1 file to manage
    @param filename The filename of the removed file
    */
    void removeFile(const std::string& filename);

    /** Request 1 file in cache (already in cache or send 1 request to sender if not).
    @param sender The address of the system having the file to request to
    @param filename The filename of the requested file
    @param version The version of the requested file
    @param callback The callback instance (download/upload transfer progress)
    */
    void requestFile(const SystemAddress& sender, const std::string& filename, const FileVersion& version, CacheManagerCallback* callback = 0);
    /** Cancel reception of 1 file.
    @param filename The filename of the sent file
    @param removeFile True if file must be removed from cache and deleted
    */
    void cancelFile(const std::string& filename, bool removeFile = false);
    /** Send 1 file to a recipient system which has requested it.
    @param recipient The address of the recipient system
    @param fileListTransferSetID The transfer identifier
    @param filename The filename of the sent file
    @param version The version of the sent file
    */
    void sendFile(const SystemAddress& recipient, unsigned short fileListTransferSetID, std::string& filename, const FileVersion& version);

    /** Whether manager have still files to send
    @returns True if there is pending upload
    */
    bool havePendingDownload();
    /** Whether manager have still files to send
    @returns True if there is pending upload
    */
    bool havePendingUpload();

    /** Remove 1 connection from cache management.
    @param system The address of the system to remove
    */
    void removeConnection(const SystemAddress& system);

    /** Update the cache manager.
    */
    void update();

protected:
    /** Retrieve the pathname of 1 file into the cache directory.
    @param filename The file name
    @param pathname The pathname of the file into the cache directory
    */
    void getCachePathname(const std::string& filename, std::string& pathname);

    /** Load the cache XML file */
    void load();
    /** Save the cache XML file */
    void save();

private:
    /** Log all cache entries */
    void logAll();
};

} // namespace Solipsis

#endif // #ifndef __CacheManager_h__