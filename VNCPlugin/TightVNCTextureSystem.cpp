#include "TightVNCTextureSystem.h"
#include "VNCPlugin.h"

#include <OgreExternalTextureSourceManager.h>
#include <OgreLogManager.h>
#include <OgreMaterialManager.h>
#include <OgrePass.h>
#include <OgreRoot.h>
#include <OgreTechnique.h>
#include <OgreTextureManager.h>
#include <OgreStringConverter.h>

using namespace Solipsis;

TightVNCTextureSystem::TightVNCTextureSystem(VNCPlugin *plugin)
    : mPlugin(plugin)
{
    mDictionaryName = "vnc";
    mParamDictName = "vnc";
    Ogre::ExternalTextureSourceManager::getSingleton().setExternalTextureSource("vnc", this);
}

void TightVNCTextureSystem::connectionCreated(int id, const Ogre::String& textureName)
{
    IDMaterialMap::iterator i = mMaterials.find(id);
    if (i != mMaterials.end())
    {        
        MaterialList& materials = i->second;
        for (MaterialList::iterator j = materials.begin(); j != materials.end(); ++j)
        {
            Ogre::MaterialPtr matPtr = *j;
            if (!matPtr.isNull())
            {
                Ogre::LogManager::getSingleton().logMessage(
                    "TightVNCTextureSystem - Assigning VNC texture '" + textureName +
                    "' to material");

                // A texture has been created for the connection, assign it to the material

                Ogre::Technique* tech = matPtr->getTechnique(0);
                Ogre::Pass* pass = tech->getPass(0);
                pass->removeAllTextureUnitStates();
                Ogre::TextureUnitState* state = pass->createTextureUnitState();
                // GREG BEGIN
                state->setTextureFiltering(Ogre::FO_ANISOTROPIC, Ogre::FO_ANISOTROPIC, Ogre::FO_NONE);
                state->setTextureAnisotropy(4);
                // GREG END
                state->setTextureName(textureName);
            }
        }
    }
}

void TightVNCTextureSystem::connectionClosed(int id)
{
    // Remove pending materials for connection
    IDMaterialMap::iterator i = mMaterials.find(id);
    if (i != mMaterials.end())
        mMaterials.erase(id);
}

void TightVNCTextureSystem::clearConnections()
{
    mMaterials.clear();
}

bool TightVNCTextureSystem::initialise()
{
    addBaseParams();

    // Add commands to set VNC connection parameters
    Ogre::ParamDictionary* dict = getParamDictionary();
    dict->addParameter(Ogre::ParameterDef("address", "VNC server address (vnc://host:port)", Ogre::PT_STRING), &mCmdAddr);
// GREG BEGIN
    dict->addParameter(Ogre::ParameterDef("password", "VNC server password (vncpwd:pwd)", Ogre::PT_STRING), &mCmdPwd);
// GREG END

    return true;
}

void TightVNCTextureSystem::shutDown()
{
    clearConnections();
    cleanupDictionary();
}

void TightVNCTextureSystem::setAddress(const Ogre::String& addr)
{
    Ogre::LogManager::getSingleton().logMessage("TightVNCTextureSystem::setAddress (" + addr + ")");
    mAddress = addr;
}

// GREG BEGIN
void TightVNCTextureSystem::setPwd(const Ogre::String& pwd)
{
    Ogre::LogManager::getSingleton().logMessage("TightVNCTextureSystem::setPwd (" + pwd + ")");
    mPwd = pwd;
}
// GREG END

void TightVNCTextureSystem::createDefinedTexture(const Ogre::String& materialName, const Ogre::String& group)
{
    // XXX We should respect resource group name here, e.g. group should be passed to TightVNCUpdater somehow

    Ogre::LogManager::getSingleton().logMessage("TightVNCTextureSystem::createDefinedTexture (" + materialName + ")");

    // Get material by name
    // XXX Should we create a new material if an existing isn't found?
    Ogre::MaterialPtr material = (Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().getByName(materialName);
    if (material.isNull())
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "TightVNC material '" + materialName +
            "' not found!");
        return;
    }

    // Parse VNC material parameters
    // GREG comment: normally parameters are already set we do not have to call getParameter because
    // setParameter set our variables for us
    Ogre::String addr = getParameter("address");
    addr = addr.substr(addr.find_first_of("://") + 3);
    if (addr.empty() || (addr.find(':') == std::string::npos))
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL,
            "TightVNC - Error: missing or invalid parameter 'address'");
        return;
    }
	Ogre::String host = addr.substr(0, addr.find_first_of(':'));
	Ogre::String port = addr.substr(addr.find_first_of(':') + 1);

    int portNum = Ogre::StringConverter::parseInt(port);

// GREG BEGIN
    Ogre::String pwdParam = getParameter("password");
	Ogre::String pwd = pwdParam.substr(pwdParam.find_first_of(':') + 1);
// GREG END

    // Lookup existing connection
    int id = mPlugin->lookupConnection(host, portNum);

    if (id == -1)
    {      
        // If no existing connection was found create a new one

// GREG BEGIN
//        id = mPlugin->newConnection(host, portNum);
        id = mPlugin->newConnection(host, portNum, pwd, mFramesPerSecond);
// GREG END
        if (id == -1)
        {
            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL,
                "TightVNC - Error: Connection to '" + host + ":" + port + "' failed!");
            return;
        }
    }

    // Add material to pending materials list so that it's texture will be updated once
    // a VNC texture is available for the connection
    IDMaterialMap::iterator i = mMaterials.find(id);
    if (i == mMaterials.end())
    {
        MaterialList list;
        list.push_back(material);
        mMaterials.insert(std::make_pair(id, list));
    }
    else
    {
        i->second.push_back(material);
        Ogre::TexturePtr tex = mPlugin->getTextureForConnection(id);
        if (!tex.isNull())
        {
            Ogre::LogManager::getSingleton().logMessage(
                "TightVNCTextureSystem - Assigning VNC texture '" + tex->getName() +
                "' to material");

            Ogre::Technique* tech = material->getTechnique(0);
            Ogre::Pass* pass = tech->getPass(0);
            pass->removeAllTextureUnitStates();
            Ogre::TextureUnitState* state = pass->createTextureUnitState();
            // GREG BEGIN
            state->setTextureFiltering(Ogre::FO_ANISOTROPIC, Ogre::FO_ANISOTROPIC, Ogre::FO_NONE);
            state->setTextureAnisotropy(4);
            // GREG END
            state->setTextureName(tex->getName());
        }
    }
}

void TightVNCTextureSystem::destroyAdvancedTexture(const Ogre::String& material, const Ogre::String& group)
{
    Ogre::LogManager::getSingleton().logMessage("TightVNCTextureSystem::destroyAdvancedTexture (" + material + ")");

    for (IDMaterialMap::iterator i = mMaterials.begin(); i != mMaterials.end(); ++i)
    {
        MaterialList& materials = i->second;
        for (MaterialList::iterator j = materials.begin(); j != materials.end(); ++j)
        {
            Ogre::MaterialPtr matPtr = *j;
            if (!matPtr.isNull() && matPtr->getName() == material)
            {
                materials.erase(j);
                if (materials.empty())
                    mPlugin->closeConnection(i->first);
                return;
            }
        }
    }
}

// GREG BEGIN
void TightVNCTextureSystem::handleEvt(const Ogre::String& material, const Event& evt)
{
    for (IDMaterialMap::iterator materialListIt = mMaterials.begin(); materialListIt != mMaterials.end(); ++materialListIt)
    {
        MaterialList& materials = materialListIt->second;
        for (MaterialList::iterator materialIt = materials.begin(); materialIt != materials.end(); ++materialIt)
        {
            Ogre::MaterialPtr matPtr = *materialIt;
            if (!matPtr.isNull() && matPtr->getName() == material)
                return mPlugin->handleEvt(materialListIt->first, evt);
        }
    }
}
// GREG END

bool TightVNCTextureSystem::requiresAuthorization() const
{
    return true;
}
