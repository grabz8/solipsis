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

#include "SWFTextureSource.h"
#include "SWFPlugin.h"

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

SWFTextureSource::CmdUrl SWFTextureSource::msCmdUrl;
SWFTextureSource::CmdWidth SWFTextureSource::msCmdWidth;
SWFTextureSource::CmdHeight SWFTextureSource::msCmdHeight;
SWFTextureSource::CmdSwfParams SWFTextureSource::msCmdSwfParams;

//-------------------------------------------------------------------------------------
SWFTextureSource::SWFTextureSource(SWFPlugin *plugin)
    : mPlugin(plugin)
{
    mDictionaryName = "swf";
    mParamDictName = "swf";
    mPlugInName = "SWF texture source plugin";
    mUrl = "";
    mWidth = DEFAULT_WIDTH;
    mHeight = DEFAULT_HEIGHT;
    Ogre::ExternalTextureSourceManager::getSingleton().setExternalTextureSource("swf", this);
}

//-------------------------------------------------------------------------------------
void SWFTextureSource::instanceDestroyed(int id)
{
    MaterialListMap::iterator i = mMaterials.find(id);
    if (i != mMaterials.end())
        mMaterials.erase(id);
    mPlugin->deleteInstance(id);
}

//-------------------------------------------------------------------------------------
void SWFTextureSource::clearInstances()
{
    for(MaterialListMap::iterator materialListIt=mMaterials.begin();materialListIt!=mMaterials.end();materialListIt=mMaterials.begin())
    {
        mPlugin->destroyInstance(materialListIt->first, true);
        mMaterials.erase(materialListIt);
    }
}

//-------------------------------------------------------------------------------------
Ogre::String SWFTextureSource::CmdUrl::doGet(const void* target) const
{
	return static_cast<const SWFTextureSource*>(target)->getUrl();
}

//-------------------------------------------------------------------------------------
void SWFTextureSource::CmdUrl::doSet(void* target, const Ogre::String& val)
{
	static_cast<SWFTextureSource*>(target)->setUrl(val);
}

//-------------------------------------------------------------------------------------
Ogre::String SWFTextureSource::CmdWidth::doGet(const void* target) const
{
	return Ogre::StringConverter::toString(static_cast<const SWFTextureSource*>(target)->getWidth());
}

//-------------------------------------------------------------------------------------
void SWFTextureSource::CmdWidth::doSet(void* target, const Ogre::String& val)
{
	static_cast<SWFTextureSource*>(target)->setWidth(Ogre::StringConverter::parseInt(val));
}

//-------------------------------------------------------------------------------------
Ogre::String SWFTextureSource::CmdHeight::doGet(const void* target) const
{
	return Ogre::StringConverter::toString(static_cast<const SWFTextureSource*>(target)->getHeight());
}

//-------------------------------------------------------------------------------------
void SWFTextureSource::CmdHeight::doSet(void* target, const Ogre::String& val)
{
	static_cast<SWFTextureSource*>(target)->setHeight(Ogre::StringConverter::parseInt(val));
}

//-------------------------------------------------------------------------------------
Ogre::String SWFTextureSource::CmdSwfParams::doGet(const void* target) const
{
	return static_cast<const SWFTextureSource*>(target)->getSwfParams();
}

//-------------------------------------------------------------------------------------
void SWFTextureSource::CmdSwfParams::doSet(void* target, const Ogre::String& val)
{
	static_cast<SWFTextureSource*>(target)->setSwfParams(val);
}

//-------------------------------------------------------------------------------------
bool SWFTextureSource::initialise()
{
    addBaseParams();

    // Add commands to set SWF parameters
    Ogre::ParamDictionary* dict = getParamDictionary();
    dict->addParameter(Ogre::ParameterDef("url", "SWF Media Resource Link", Ogre::PT_STRING), &msCmdUrl);
    dict->addParameter(Ogre::ParameterDef("width", "Video width", Ogre::PT_INT), &msCmdWidth);
    dict->addParameter(Ogre::ParameterDef("height", "Video height", Ogre::PT_INT), &msCmdHeight);
    dict->addParameter(Ogre::ParameterDef("height", "Video height", Ogre::PT_INT), &msCmdHeight);
    // TODO frames_per_second
    dict->addParameter(Ogre::ParameterDef("swf_params", "Additional SWF parameters", Ogre::PT_STRING), &msCmdSwfParams);

    return true;
}

//-------------------------------------------------------------------------------------
void SWFTextureSource::shutDown()
{
    clearInstances();
    cleanupDictionary();
}

//-------------------------------------------------------------------------------------
void SWFTextureSource::createDefinedTexture(const Ogre::String& materialName, const Ogre::String& group)
{
    Ogre::LogManager::getSingleton().logMessage("SWFTextureSource::createDefinedTexture (" + materialName + ")");

    // Get material by name
    // XXX Should we create a new material if an existing isn't found?
    Ogre::MaterialPtr material = (Ogre::MaterialPtr)Ogre::MaterialManager::getSingleton().getByName(materialName);
    if (material.isNull())
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL, "SWF material '" + materialName +
            "' not found!");
        return;
    }

    if (mUrl.empty())
    {
        Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL,
            "SWFTextureSource - Error: missing or invalid parameter 'mrl'");
        return;
    }

    // Lookup existing instance on this mrl
    int id = mPlugin->lookupInstance(mUrl);
    if (id == -1)
    {
        // If no existing instance was found create a new one
        id = mPlugin->newInstance(mUrl, mWidth, mHeight, mFramesPerSecond, mSwfParams);
        if (id == -1)
        {
            Ogre::LogManager::getSingleton().logMessage(Ogre::LML_CRITICAL,
                "SWFTextureSource - Error: SWF instance creation for '" + mUrl + "' failed!");
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
            "SWFTextureSource - Assigning SWF texture '" + tex->getName() +
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
void SWFTextureSource::destroyAdvancedTexture(const Ogre::String& material, const Ogre::String& group)
{
    Ogre::LogManager::getSingleton().logMessage("SWFTextureSource::destroyAdvancedTexture (" + material + ")");

    for (MaterialListMap::iterator materialListIt = mMaterials.begin(); materialListIt != mMaterials.end(); ++materialListIt)
    {
        MaterialList& materials = materialListIt->second;
        for (MaterialList::iterator materialIt = materials.begin(); materialIt != materials.end(); ++materialIt)
        {
            Ogre::MaterialPtr matPtr = *materialIt;
            if (!matPtr.isNull() && matPtr->getName() == material)
            {
                // remove this material from list of materials using this SWF texture
                materials.erase(materialIt);
                // no more material using this SWF texture ?
                if (materials.empty())
                    // destroy this SWF texture
                    mPlugin->destroyInstance(materialListIt->first, false);
                return;
            }
        }
    }
}

//-------------------------------------------------------------------------------------
void SWFTextureSource::handleEvt(const Ogre::String& material, const Event& evt)
{
    for (MaterialListMap::iterator materialListIt = mMaterials.begin(); materialListIt != mMaterials.end(); ++materialListIt)
    {
        MaterialList& materials = materialListIt->second;
        for (MaterialList::iterator materialIt = materials.begin(); materialIt != materials.end(); ++materialIt)
        {
            Ogre::MaterialPtr matPtr = *materialIt;
            if (!matPtr.isNull() && matPtr->getName() == material)
                mPlugin->handleEvt(materialListIt->first, evt);
        }
    }
}

//-------------------------------------------------------------------------------------
Ogre::String SWFTextureSource::handleEvt(const Ogre::String& material, const Ogre::String& evt)
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
    return "";
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
