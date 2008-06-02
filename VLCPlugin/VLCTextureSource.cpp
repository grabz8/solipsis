#include "VLCTextureSource.h"
#include "VLCPlugin.h"

#include <OgreExternalTextureSourceManager.h>
#include <OgreLogManager.h>
#include <OgreMaterialManager.h>
#include <OgrePass.h>
#include <OgreRoot.h>
#include <OgreTechnique.h>
#include <OgreTextureManager.h>
#include <OgreStringConverter.h>

namespace Solipsis {

#define DEFAULT_WIDTH 256
#define DEFAULT_HEIGHT 256

VLCTextureSource::CmdMrl VLCTextureSource::msCmdMrl;
VLCTextureSource::CmdWidth VLCTextureSource::msCmdWidth;
VLCTextureSource::CmdHeight VLCTextureSource::msCmdHeight;
VLCTextureSource::CmdVlcParams VLCTextureSource::msCmdVlcParams;

//-------------------------------------------------------------------------------------
VLCTextureSource::VLCTextureSource(VLCPlugin *plugin)
    : mPlugin(plugin)
{
    mDictionaryName = "vlc";
    mParamDictName = "vlc";
    mMrl = "";
    mWidth = DEFAULT_WIDTH;
    mHeight = DEFAULT_HEIGHT;
    Ogre::ExternalTextureSourceManager::getSingleton().setExternalTextureSource("vlc", this);
}

//-------------------------------------------------------------------------------------
void VLCTextureSource::instanceDestroyed(int id)
{
    MaterialListMap::iterator i = mMaterials.find(id);
    if (i != mMaterials.end())
        mMaterials.erase(id);
    mPlugin->deleteInstance(id);
}

//-------------------------------------------------------------------------------------
void VLCTextureSource::clearInstances()
{
    mMaterials.clear();
}

//-------------------------------------------------------------------------------------
Ogre::String VLCTextureSource::CmdMrl::doGet(const void* target) const
{
	return static_cast<const VLCTextureSource*>(target)->getMrl();
}

//-------------------------------------------------------------------------------------
void VLCTextureSource::CmdMrl::doSet(void* target, const Ogre::String& val)
{
	static_cast<VLCTextureSource*>(target)->setMrl(val);
}

//-------------------------------------------------------------------------------------
Ogre::String VLCTextureSource::CmdWidth::doGet(const void* target) const
{
	return Ogre::StringConverter::toString(static_cast<const VLCTextureSource*>(target)->getWidth());
}

//-------------------------------------------------------------------------------------
void VLCTextureSource::CmdWidth::doSet(void* target, const Ogre::String& val)
{
	static_cast<VLCTextureSource*>(target)->setWidth(Ogre::StringConverter::parseInt(val));
}

//-------------------------------------------------------------------------------------
Ogre::String VLCTextureSource::CmdHeight::doGet(const void* target) const
{
	return Ogre::StringConverter::toString(static_cast<const VLCTextureSource*>(target)->getHeight());
}

//-------------------------------------------------------------------------------------
void VLCTextureSource::CmdHeight::doSet(void* target, const Ogre::String& val)
{
	static_cast<VLCTextureSource*>(target)->setHeight(Ogre::StringConverter::parseInt(val));
}

//-------------------------------------------------------------------------------------
Ogre::String VLCTextureSource::CmdVlcParams::doGet(const void* target) const
{
	return static_cast<const VLCTextureSource*>(target)->getVlcParams();
}

//-------------------------------------------------------------------------------------
void VLCTextureSource::CmdVlcParams::doSet(void* target, const Ogre::String& val)
{
	static_cast<VLCTextureSource*>(target)->setVlcParams(val);
}

//-------------------------------------------------------------------------------------
bool VLCTextureSource::initialise()
{
    addBaseParams();

    // Add commands to set VLC parameters
    Ogre::ParamDictionary* dict = getParamDictionary();
    dict->addParameter(Ogre::ParameterDef("mrl", "VLC Media Resource Link", Ogre::PT_STRING), &msCmdMrl);
    dict->addParameter(Ogre::ParameterDef("width", "Video width", Ogre::PT_INT), &msCmdWidth);
    dict->addParameter(Ogre::ParameterDef("height", "Video height", Ogre::PT_INT), &msCmdHeight);
    dict->addParameter(Ogre::ParameterDef("vlc_params", "Additional VLC parameters", Ogre::PT_STRING), &msCmdVlcParams);

    return true;
}

//-------------------------------------------------------------------------------------
void VLCTextureSource::shutDown()
{
    clearInstances();
    cleanupDictionary();
}

//-------------------------------------------------------------------------------------
void VLCTextureSource::createDefinedTexture(const Ogre::String& materialName, const Ogre::String& group)
{
    Ogre::LogManager::getSingleton().logMessage("VLCTextureSource::createDefinedTexture (" + materialName + ")");

    // Get material by name
    // XXX Should we create a new material if an existing isn't found?
    Ogre::MaterialPtr material = (Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().getByName(materialName);
    if (material.isNull())
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "VLC material '" + materialName +
            "' not found!");
        return;
    }

    if (mMrl.empty())
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL,
            "VLCTextureSource - Error: missing or invalid parameter 'mrl'");
        return;
    }

    // Lookup existing instance on this mrl
    int id = mPlugin->lookupInstance(mMrl);
    if (id == -1)
    {
        // If no existing instance was found create a new one
        id = mPlugin->newInstance(mMrl, mWidth, mHeight, mFramesPerSecond, mVlcParams);
        if (id == -1)
        {
            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL,
                "VLCTextureSource - Error: VLC instance creation for '" + mMrl + "' failed!");
            return;
        }
    }

    // Add material to materials list using this VLC texture instance
    MaterialListMap::iterator materialListIt = mMaterials.find(id);
    if (materialListIt == mMaterials.end())
    {
        MaterialList newMaterialList;
        newMaterialList.push_back(material);
        mMaterials.insert(std::make_pair(id, newMaterialList));
    }
    else
    {
        materialListIt->second.push_back(material);
    }
    Ogre::TexturePtr tex = mPlugin->getTextureForInstance(id);
    if (!tex.isNull())
    {
        Ogre::LogManager::getSingleton().logMessage(
            "VLCTextureSource - Assigning VLC texture '" + tex->getName() +
            "' to material");

        Ogre::Technique* tech = material->getTechnique(0);
        Ogre::Pass* pass = tech->getPass(0);
        pass->removeAllTextureUnitStates();
        Ogre::TextureUnitState* state = pass->createTextureUnitState();
        state->setTextureFiltering(Ogre::FO_ANISOTROPIC, Ogre::FO_ANISOTROPIC, Ogre::FO_NONE);
        state->setTextureAnisotropy(4);
        state->setTextureName(tex->getName());
    }
}

//-------------------------------------------------------------------------------------
void VLCTextureSource::destroyAdvancedTexture(const Ogre::String& material, const Ogre::String& group)
{
    Ogre::LogManager::getSingleton().logMessage("VLCTextureSource::destroyAdvancedTexture (" + material + ")");

    for (MaterialListMap::iterator materialListIt = mMaterials.begin(); materialListIt != mMaterials.end(); ++materialListIt)
    {
        MaterialList& materials = materialListIt->second;
        for (MaterialList::iterator materialIt = materials.begin(); materialIt != materials.end(); ++materialIt)
        {
            Ogre::MaterialPtr matPtr = *materialIt;
            if (!matPtr.isNull() && matPtr->getName() == material)
            {
                // remove this material from list of materials using this VLC texture
                materials.erase(materialIt);
                // no more material using this VLC texture ?
                if (materials.empty())
                    // destroy this VLC texture
                    mPlugin->destroyInstance(materialListIt->first);
                return;
            }
        }
    }
}

//-------------------------------------------------------------------------------------
Ogre::String VLCTextureSource::handleEvt(const Ogre::String& material, const Ogre::String& evt)
{
    for (MaterialListMap::iterator materialListIt = mMaterials.begin(); materialListIt != mMaterials.end(); ++materialListIt)
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

//-------------------------------------------------------------------------------------

} // namespace Solipsis
