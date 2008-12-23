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

#include "SWFPlugin.h"

#include "SWFTextureSource.h"
#include "SWFInstance.h"

#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreStringConverter.h>

using namespace Ogre;

namespace Solipsis {

const String sPluginName = "SWF Texture Source";

//-------------------------------------------------------------------------------------
SWFPlugin::SWFPlugin() :
    mSWFTextureSource(0),
    mSWFInstanceMapMutex(PTHREAD_MUTEX_INITIALIZER),
    mSWFInstanceMapCounter(0),
    mHikariMgr(0)
{
}

//-------------------------------------------------------------------------------------
const String& SWFPlugin::getName() const
{
    return sPluginName;
}

//-------------------------------------------------------------------------------------
void SWFPlugin::install()
{
}

//-------------------------------------------------------------------------------------
void SWFPlugin::initialise()
{
    mSWFTextureSource = new SWFTextureSource(this);
    mHikariMgr = new HikariManager();
    mHikariMgr->setKeyboardHookEnabled(true);
}

//-------------------------------------------------------------------------------------
void SWFPlugin::shutdown()
{
    if (mSWFTextureSource != 0)
    {
        mSWFTextureSource->clearInstances();
        delete mSWFTextureSource;
        mSWFTextureSource = 0;
    }

    if (mHikariMgr != 0)
    {
        mHikariMgr->defocusAll();
        mHikariMgr->destroyAllControls();
        delete mHikariMgr;
        mHikariMgr = 0;
    }
}

//-------------------------------------------------------------------------------------
void SWFPlugin::uninstall()
{
}

//-------------------------------------------------------------------------------------
int SWFPlugin::newInstance(const String& url, int width, int height, int fps, const String& swfParams)
{
    LogManager::getSingleton().logMessage("SWFPlugin - Creating new instance mrl='" + url + "', " + StringConverter::toString(width) + "x" + StringConverter::toString(height) + "x" + StringConverter::toString(fps) + ", swfParams=" + swfParams);

    pthread_mutex_lock(&mSWFInstanceMapMutex);

    // Set mrl and map it by id
    int id = mSWFInstanceMapCounter++;
    //Hikari::FlashControl* flash = mHikariMgr->createFlashMaterial(url + StringConverter::toString(id), width, height);
    Hikari::FlashControl* flash = mHikariMgr->createFlashMaterial(url, width, height);
    flash->load(url);
    SWFInstancePtr instance(new SWFInstance(id, 
        mSWFTextureSource, 
        flash,
        url, width, height, fps, swfParams));
    mSWFInstanceMap.insert(std::make_pair(id, instance));

    pthread_mutex_unlock(&mSWFInstanceMapMutex);

    return id;
}

//-------------------------------------------------------------------------------------
void SWFPlugin::destroyInstance(int id, bool force)
{
    LogManager::getSingleton().logMessage("SWFPlugin::destroyInstance" + force ? " forced" : "");

    pthread_mutex_lock(&mSWFInstanceMapMutex);
    SWFInstanceMap::iterator i = mSWFInstanceMap.find(id);
    if (i != mSWFInstanceMap.end())
        i->second->destroy(force);
    if (force)
        mSWFInstanceMap.erase(i);
    pthread_mutex_unlock(&mSWFInstanceMapMutex);
}

//-------------------------------------------------------------------------------------
void SWFPlugin::deleteInstance(int id)
{
    LogManager::getSingleton().logMessage("SWFPlugin::deleteInstance");

    pthread_mutex_lock(&mSWFInstanceMapMutex);
    SWFInstanceMap::iterator i = mSWFInstanceMap.find(id);
    if (i != mSWFInstanceMap.end())
        mSWFInstanceMap.erase(i);
    pthread_mutex_unlock(&mSWFInstanceMapMutex);
}

//-------------------------------------------------------------------------------------
int SWFPlugin::lookupInstance(const String& url)
{
    LogManager::getSingleton().logMessage("SWFPlugin::lookupInstance");

    pthread_mutex_lock(&mSWFInstanceMapMutex);
    for (SWFInstanceMap::const_iterator i = mSWFInstanceMap.begin(); i != mSWFInstanceMap.end(); ++i)
    {
        if (i->second->getUrl() == url)
        {
            pthread_mutex_unlock(&mSWFInstanceMapMutex);
            return i->first;
        }
    }
    pthread_mutex_unlock(&mSWFInstanceMapMutex);

    return -1;
}

//-------------------------------------------------------------------------------------
TexturePtr SWFPlugin::getTextureForInstance(const int id)
{
    LogManager::getSingleton().logMessage("SWFPlugin::getTextureForInstance");

    pthread_mutex_lock(&mSWFInstanceMapMutex);
    SWFInstanceMap::iterator i = mSWFInstanceMap.find(id);
    if (i != mSWFInstanceMap.end())
    {
        pthread_mutex_unlock(&mSWFInstanceMapMutex);
        return i->second->getTexture();
    }

    pthread_mutex_unlock(&mSWFInstanceMapMutex);

    // Not found
    return TexturePtr();
}

//-------------------------------------------------------------------------------------
Ogre::String SWFPlugin::handleEvt(const int id, const Event& evt)
{
    Ogre::String result;

    LogManager::getSingleton().logMessage("SWFPlugin::handleEvt");

    pthread_mutex_lock(&mSWFInstanceMapMutex);
    SWFInstanceMap::iterator i = mSWFInstanceMap.find(id);
    if (i != mSWFInstanceMap.end())
        result = i->second->handleEvt(evt);
    pthread_mutex_unlock(&mSWFInstanceMapMutex);

    return result;
}

//-------------------------------------------------------------------------------------
Ogre::String SWFPlugin::handleEvt(const int id, const Ogre::String& evt)
{
    Ogre::String result;

    LogManager::getSingleton().logMessage("SWFPlugin::handleEvt");

    pthread_mutex_lock(&mSWFInstanceMapMutex);
    SWFInstanceMap::iterator i = mSWFInstanceMap.find(id);
    if (i != mSWFInstanceMap.end())
        result = i->second->handleEvt(evt);
    pthread_mutex_unlock(&mSWFInstanceMapMutex);

    return result;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
