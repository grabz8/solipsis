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

#ifndef __VLCPlugin_h__
#define __VLCPlugin_h__

#include <pthread.h>

#include "OgrePlugin.h"
#include <OgreSharedPtr.h>

namespace Solipsis {

class VLCTextureSource;
class VLCInstance;

/** This class represents the VLC plugin.
*/
class VLCPlugin : public Ogre::Plugin
{
public:
    VLCPlugin();

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

    /** Creates a new VLC instance
    @param[in] mrl The media resource link
    @param[in] width The video width
    @param[in] height The video height
    @param[in] fps The update rate in frames per second
    @param[in] vlcParams The additional VLC parameters
    @return Connection id
    */
    int newInstance(const Ogre::String& mrl, int width, int height, int fps, const Ogre::String& vlcParams);

    /** Destroy a VLC instance (notifying it should no more listen frames)
    @param[in] id VLC Instance id
    */
    void destroyInstance(int id, bool force = false);

    /** delete a VLC instance
    @param[in] id VLC Instance id
    */
    void deleteInstance(int id);

    /** Retrieves an existing VLC instance by media resource link
    @param[in] mrl media resource link
    @return Instance id
    */
    int lookupInstance(const Ogre::String& mrl);

    /** Returns the VLC texture for the given instance
    @param[in] id VLC instance id
    @return VLC texture or a NULL TexturePtr if the texture is not yet available
    */
    Ogre::TexturePtr getTextureForInstance(const int id);

    /** Handle 1 event by an existing VLC instance
    @param[in] id VLC Instance id
    @param[in] evt Event to handle
    @return Event resulting string
    */
    Ogre::String handleEvt(int id, const Ogre::String& evt);

private:
    /// VLC texture source
    VLCTextureSource* mVLCTextureSource;

    typedef Ogre::SharedPtr<VLCInstance> VLCInstancePtr;
    typedef std::map<int, VLCInstancePtr> VLCInstanceMap;
    /// VLC instances map by id
    VLCInstanceMap mVLCInstanceMap;
    /// VLC instances map mutex
    pthread_mutex_t mVLCInstanceMapMutex;

    /// Unique VLC instance identifier counter
    int mVLCInstanceMapCounter;
};

} // namespace Solipsis

#endif // #ifndef __VLCPlugin_h__