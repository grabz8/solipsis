#include "VLCPlugin.h"

#include "VLCTextureSource.h"
#include "VLCInstance.h"

#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreStringConverter.h>

using namespace Ogre;

namespace Solipsis {

const String sPluginName = "VLC Texture Source";

//-------------------------------------------------------------------------------------
VLCPlugin::VLCPlugin() :
    mVLCTextureSource(0),
    mVLCInstanceMapMutex(PTHREAD_MUTEX_INITIALIZER),
    mVLCInstanceMapCounter(0)
{
}

//-------------------------------------------------------------------------------------
const String& VLCPlugin::getName() const
{
    return sPluginName;
}

//-------------------------------------------------------------------------------------
void VLCPlugin::install()
{
}

//-------------------------------------------------------------------------------------
void VLCPlugin::initialise()
{
    mVLCTextureSource = new VLCTextureSource(this);
}

//-------------------------------------------------------------------------------------
void VLCPlugin::shutdown()
{
    delete mVLCTextureSource;
    mVLCTextureSource = 0;
}

//-------------------------------------------------------------------------------------
void VLCPlugin::uninstall()
{
}

//-------------------------------------------------------------------------------------
int VLCPlugin::newInstance(const String& mrl, int width, int height, int fps, const String& vlcParams)
{
    LogManager::getSingleton().logMessage("VLCPlugin - Creating new instance mrl='" + mrl + "', " + StringConverter::toString(width) + "x" + StringConverter::toString(height) + "x" + StringConverter::toString(fps) + ", vlcParams=" + vlcParams);

    pthread_mutex_lock(&mVLCInstanceMapMutex);

    // Set mrl and map it by id
    int id = mVLCInstanceMapCounter++;
    VLCInstancePtr instance(new VLCInstance(id, mVLCTextureSource, mrl, width, height, fps, vlcParams));
    mVLCInstanceMap.insert(std::make_pair(id, instance));

    pthread_mutex_unlock(&mVLCInstanceMapMutex);

    return id;
}

//-------------------------------------------------------------------------------------
void VLCPlugin::destroyInstance(int id)
{
    LogManager::getSingleton().logMessage("VLCPlugin::destroyInstance");

    pthread_mutex_lock(&mVLCInstanceMapMutex);
    VLCInstanceMap::iterator i = mVLCInstanceMap.find(id);
    if (i != mVLCInstanceMap.end())
        i->second->destroy();
    pthread_mutex_unlock(&mVLCInstanceMapMutex);
}

//-------------------------------------------------------------------------------------
void VLCPlugin::deleteInstance(int id)
{
    LogManager::getSingleton().logMessage("VLCPlugin::deleteInstance");

    pthread_mutex_lock(&mVLCInstanceMapMutex);
    VLCInstanceMap::iterator i = mVLCInstanceMap.find(id);
    if (i != mVLCInstanceMap.end())
        mVLCInstanceMap.erase(i);
    pthread_mutex_unlock(&mVLCInstanceMapMutex);
}

//-------------------------------------------------------------------------------------
int VLCPlugin::lookupInstance(const String& mrl)
{
    LogManager::getSingleton().logMessage("VLCPlugin::lookupInstance");

    pthread_mutex_lock(&mVLCInstanceMapMutex);
    for (VLCInstanceMap::const_iterator i = mVLCInstanceMap.begin(); i != mVLCInstanceMap.end(); ++i)
    {
        if (i->second->getMrl() == mrl)
        {
            pthread_mutex_unlock(&mVLCInstanceMapMutex);
            return i->first;
        }
    }
    pthread_mutex_unlock(&mVLCInstanceMapMutex);

    return -1;
}

//-------------------------------------------------------------------------------------
TexturePtr VLCPlugin::getTextureForInstance(const int id)
{
    LogManager::getSingleton().logMessage("VLCPlugin::getTextureForInstance");

    pthread_mutex_lock(&mVLCInstanceMapMutex);
    VLCInstanceMap::iterator i = mVLCInstanceMap.find(id);
    if (i != mVLCInstanceMap.end())
    {
        pthread_mutex_unlock(&mVLCInstanceMapMutex);
        return i->second->getTexture();
    }

    pthread_mutex_unlock(&mVLCInstanceMapMutex);

    // Not found
    return TexturePtr();
}

//-------------------------------------------------------------------------------------
Ogre::String VLCPlugin::handleEvt(int id, const Ogre::String& evt)
{
    Ogre::String result;

    LogManager::getSingleton().logMessage("VLCPlugin::handleEvt");

    pthread_mutex_lock(&mVLCInstanceMapMutex);
    VLCInstanceMap::iterator i = mVLCInstanceMap.find(id);
    if (i != mVLCInstanceMap.end())
        result = i->second->handleEvt(evt);
    pthread_mutex_unlock(&mVLCInstanceMapMutex);

    return result;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
