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

#ifndef __OgreApplication_h__
#define __OgreApplication_h__

#include <pthread.h>
#include "IApplication.h"
#include "Ogre.h"
#include "OgreGraphicObjects/Panel2DOverlayElement.h"
#include "OgreGraphicObjects/Panel2DButtonOverlayElement.h"

using namespace Ogre;

namespace Solipsis {

/** This class represents an Ogre application composed of several running instances.
 */
class OgreApplication : public IApplication
{
private: 
    Root* mRoot;
    pthread_mutex_t mMutex;
    Panel2DOverlayElementFactory *mPanel2DFactory;
    Panel2DButtonOverlayElementFactory *mPanel2DButtonFactory;

protected: 
    OgreApplication::OgreApplication();
    virtual ~OgreApplication(); 

public: 
    Root* getRoot() { return mRoot; }

    void lock();
    void unlock();

    virtual bool initialize(bool configManagedByOgre = false, String windowTitle = "");
    virtual bool finalize();

    /** Show our display configuration dialog instead of standard Ogre dialog. */
    bool showConfigDialog();
    /** Reset display configuration, commonly Ogre.cfg. */
    void resetDisplayConfig(); 

    virtual void createResourceListener(); 
    virtual void initResources(); 
};

} // namespace Solipsis

#endif // #ifndef __OgreApplication_h__