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

#ifndef __SWFPlugin_h__
#define __SWFPlugin_h__

#include <pthread.h>

#include "OgrePlugin.h"
#include <OgreSharedPtr.h>

#include <hikari.h>

#include <ExternalTextureSourceEx.h>
#include <Event.h>

using namespace Hikari;

namespace Solipsis {

class SWFTextureSource;
class SWFInstance;

/** This class represents the VLC plugin.
*/
class SWFPlugin : public Ogre::Plugin
{
public:
    SWFPlugin();

    /// @copydoc Plugin::getName
    const Ogre::String& getName() const;

    /// @copydoc Plugin::install
    void install();

    /// @copydoc Plugin::initialise
    void initialise();

    /// @copydoc Plugin::shutdown
    void shutdown();

    /// @copydoc Plugin::uninstall
    void uninstall();

    /** Creates a new SWF instance
    @param[in] url The media resource link
    @param[in] width The video width
    @param[in] height The video height
    @param[in] fps The update rate in frames per second
    @param[in] swfParams The additional SWF parameters
    @return Connection id
    */
    int newInstance(const Ogre::String& mrl, int width, int height, int fps, const Ogre::String& swfParams);

    /** Destroy a SWF instance (notifying it should no more listen frames)
    @param[in] id SWF Instance id
    */
    void destroyInstance(int id, bool force = false);

    /** delete a SWF instance
    @param[in] id SWF Instance id
    */
    void deleteInstance(int id);

    /** Retrieves an existing SWF instance by media resource link
    @param[in] url media resource link
    @return Instance id
    */
    int lookupInstance(const Ogre::String& url);

    /** Returns the SWF texture for the given instance
    @param[in] id SWF instance id
    @return SWF texture or a NULL TexturePtr if the texture is not yet available
    */
    Ogre::TexturePtr getTextureForInstance(const int id);

    /** Handle 1 event by an existing SWF instance
    @param[in] id SWF Instance id
    @param[in] evt Event to handle
    @return Event resulting string
    */
    Ogre::String handleEvt(const int id, const Event& evt);

    /** Handle 1 event by an existing SWF instance
    @param[in] id SWF Instance id
    @param[in] evt Event to handle
    @return Event resulting string
    */
    Ogre::String handleEvt(int id, const Ogre::String& evt);

private:
    /// SWF texture source
    SWFTextureSource* mSWFTextureSource;

    typedef Ogre::SharedPtr<SWFInstance> SWFInstancePtr;
    typedef std::map<int, SWFInstancePtr> SWFInstanceMap;
    /// SWF instances map by id
    SWFInstanceMap mSWFInstanceMap;
    /// SWF instances map mutex
    pthread_mutex_t mSWFInstanceMapMutex;

    /// Unique SWF instance identifier counter
    int mSWFInstanceMapCounter;

    /// Hikari Manager
    HikariManager* mHikariMgr;
};

} // namespace Solipsis

#endif // #ifndef __SWFPlugin_h__