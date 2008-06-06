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

#include "OgreApplication.h"
#include "NaviManager.h"

#define RESSOURCE_FILE_NAME "resources.cfg"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
OgreApplication::OgreApplication() :
    mRoot(0),
    mMutex(PTHREAD_MUTEX_INITIALIZER)
{
}

//-------------------------------------------------------------------------------------
OgreApplication::~OgreApplication()
{
}

//-------------------------------------------------------------------------------------
void OgreApplication::lock()
{
    pthread_mutex_lock(&mMutex);
}

//-------------------------------------------------------------------------------------
void OgreApplication::unlock()
{
    pthread_mutex_unlock(&mMutex);
}

//-------------------------------------------------------------------------------------
bool OgreApplication::initialize(bool configManagedByOgre, String windowTitle)
{
    mRoot = new Root();
    if (mRoot == 0)
        return false;

    // Set default query flags to 0
    MovableObject::setDefaultQueryFlags(0);

    if (configManagedByOgre)
    {
        // Show the configuration dialog and initialize the system
        // You can skip this and use root.restoreConfig() to load configuration
        // settings if you were sure there are valid ones saved in ogre.cfg
        if (mRoot->restoreConfig() || mRoot->showConfigDialog())
            // If returned true, user clicked OK so initialize
            ;
        else
            return false;
    }
    else
    {
	    Ogre::RenderSystem *currentRenderSystem = NULL;
	    Ogre::RenderSystemList *rl = mRoot->getAvailableRenderers();
	    Ogre::String str;
	    for (Ogre::RenderSystemList::iterator it = rl->begin(); it != rl->end(); ++it)
        {
		    currentRenderSystem = (*it);
		    str = currentRenderSystem->getName().c_str();
		    if ((int)str.find("3D9") > 0)
			    break;
        }
	    if (currentRenderSystem == NULL)
            return false;
	    // preserve the floating point precision
	    currentRenderSystem->setConfigOption("Floating-point mode", "Consistent");
	    try 
	    {
		    mRoot->setRenderSystem(currentRenderSystem);
        }
        catch (Ogre::Exception& e)
        {
            return false;
        }
    }
    mRoot->initialise(configManagedByOgre, windowTitle);

    addResourceLocations();

    // Create any resource listeners (for loading screens)
    createResourceListener();

    return true;
}

//-------------------------------------------------------------------------------------
bool OgreApplication::finalize()
{
    delete mRoot;

    return true;
}

//-------------------------------------------------------------------------------------
void OgreApplication::createResourceListener()
{

}

//-------------------------------------------------------------------------------------
void OgreApplication::addResourceLocations()
{
    // Load resource paths from config file
    ConfigFile cf;
    cf.load("resources.cfg");

    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
}

//-------------------------------------------------------------------------------------
void OgreApplication::initResources()
{
    ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

//-------------------------------------------------------------------------------------
