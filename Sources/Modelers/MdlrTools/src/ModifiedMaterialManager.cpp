/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author ARTEFACTO

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

#include ".\ModifiedMaterialManager.h"

#include "ModifiedMaterial.h"

namespace Solipsis {

ModifiedMaterialManager::MMMTextureManager* ModifiedMaterialManager::ms_MMMTextureManager = 0;

ModifiedMaterialManager::ModifiedMaterialManager(Object3D *object3D)
{
    mObject3D = object3D;
	mModifiedMaterial = NULL;
    mCurrentSelectedTextureIterator = mCurrentTextureIterator = mTextures.begin();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
ModifiedMaterialManager::~ModifiedMaterialManager(void)
{
    clearTextures();
	if (mModifiedMaterial)
		delete mModifiedMaterial;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::setMMMTextureManager(MMMTextureManager* textureManager)
{
    ms_MMMTextureManager = textureManager;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
ModifiedMaterialManager::MMMTextureManager* ModifiedMaterialManager::getMMMTextureManager()
{
    return ms_MMMTextureManager;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
Object3D* ModifiedMaterialManager::getObject3D()
{
    return mObject3D;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::initialise(const MaterialPtr& material)
{
	mModifiedMaterial = new ModifiedMaterial(material);
	if (mModifiedMaterial->hasATexture())
	{
		String textureName = mModifiedMaterial->getTextureName();
		TexturePtr texture = TextureManager::getSingleton().getByName(textureName);
		addTexture(texture);
        // First texture added ==> Initialise iterators 
        mCurrentSelectedTextureIterator = mCurrentTextureIterator = mTextures.begin();
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
ModifiedMaterial* ModifiedMaterialManager::getModifiedMaterial()
{
	return mModifiedMaterial;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
bool ModifiedMaterialManager::isColourModifiable()
{
	return mModifiedMaterial->isUsingAddedColour();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::setColourModifiable(bool isColourModifiable)
{
	mModifiedMaterial->useAddedColour(isColourModifiable);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const ColourValue& ModifiedMaterialManager::getColour()
{
	return mModifiedMaterial->getAddedColour();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::setColour(const ColourValue& colour)
{
	mModifiedMaterial->setAddedColour(colour);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::resetColour()
{
	setColour(ColourValue(0.5,0.5,0.5,1));
}



//--------------------------------------------------------------------------------------------------------------------------------------------------
CullingMode ModifiedMaterialManager::getCullingMode()
{
    return mModifiedMaterial->getCullingMode();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::setCullingMode(CullingMode mode)
{
    mModifiedMaterial->setCullingMode( mode );
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
bool ModifiedMaterialManager::isTextureModifiable()
{
	return (mTextures.size() > 1);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
TexturePtr ModifiedMaterialManager::getTexture(const String& name)
{
	TextureVector::iterator textureIterator = mTextures.begin();
    while (textureIterator != mTextures.end())
    {
        if ((*textureIterator)->getName() == name)
            // Found it
            return (*textureIterator);
        textureIterator++;
    }
    LogManager::getSingleton().logMessage("ModifiedMaterialManager::getTexture(String): Texture not found in the list => " + name); 
    return TexturePtr();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
TexturePtr ModifiedMaterialManager::getTexture(const int n)
{
	if( (n>-1)&&(n < (int)mTextures.size()) )
	{
		TextureVector::iterator textureIterator = mTextures.begin();
		for(int i=0; i<n ; i++)
			textureIterator++ ;		
		
		return (*textureIterator) ;
	}
	else
	{
		TexturePtr nullTexture;
        nullTexture.setNull();
		return nullTexture;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
TexturePtr ModifiedMaterialManager::getCurrentTexture()
{
	return *mCurrentTextureIterator;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
TexturePtr ModifiedMaterialManager::getCurrentAppliedTexture()
{
    if (mTextures.empty())
    {
        TexturePtr tRet;
        tRet.setNull();
        return tRet;
    }
	return *mCurrentSelectedTextureIterator;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
TextureVectorIterator ModifiedMaterialManager::getTextureIterator()
{
	return TextureVectorIterator(mTextures.begin(),mTextures.end());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::setPreviousTexture()
{
	if (mCurrentTextureIterator == mTextures.begin()) 
        mCurrentTextureIterator = mTextures.end();
	mCurrentTextureIterator--;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::setPreviousTextureAsCurrent()
{
	setPreviousTexture();
    mModifiedMaterial->setTexture((*mCurrentTextureIterator)->getName());
    // Set the currently Applied texture iterator
    mCurrentSelectedTextureIterator = mCurrentTextureIterator;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::setNextTexture()
{
	mCurrentTextureIterator++;
	if (mCurrentTextureIterator == mTextures.end()) 
        mCurrentTextureIterator = mTextures.begin();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::setNextTextureAsCurrent()
{
	setNextTexture();
	mModifiedMaterial->setTexture((*mCurrentTextureIterator)->getName());
    // Set the currently Applied texture iterator
    mCurrentSelectedTextureIterator = mCurrentTextureIterator;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::setCurrentTexture(TexturePtr texture)
{
    TextureVector::iterator textureIterator = mTextures.begin();
    while (textureIterator !=  mTextures.end())
    {
        if ((*textureIterator) == texture)
        { // Found it
            mCurrentTextureIterator = mCurrentSelectedTextureIterator = textureIterator;
            mModifiedMaterial->setTexture((*mCurrentTextureIterator)->getName());
            return;
        }
        textureIterator ++; 
    }
    LogManager::getSingleton().logMessage("ModifiedMaterialManager::setCurrentTexture(TexturePtr): Texture not found in the list => " + texture->getName()); 
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::setCurrentTexture(const String& textureName)
{
	setCurrentTexture(TextureManager::getSingleton().getByName(textureName));
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::setDefaultTextureAsCurrent()
{
	mCurrentTextureIterator = mDefaultTextureIterator;
	mModifiedMaterial->setTexture((*mCurrentTextureIterator)->getName());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::addTexture(TexturePtr texture, const TextureExtParamsMap& textureExtParamsMap)
{
	mTextures.push_back(texture);
    if (!textureExtParamsMap.empty())
        mTextureNameExtParamsMap[texture->getName()] = textureExtParamsMap;

}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::resetModifications()
{
	setDefaultTextureAsCurrent();
	resetColour();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
int ModifiedMaterialManager::getNbTexture()
{
	return mTextures.size();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::clearTextures()
{
    if (!ms_MMMTextureManager)
    {
        LogManager::getSingleton().logMessage("ModifiedMaterialManager::clearTextures: No MMMTextureManager ==> Cannot delete textures" );
    }
    else
    {
        TextureVector::iterator itText = mTextures.begin();
        TextureNameExtParamsMap::iterator it;
        while (itText != mTextures.end())
        {        
#if 1 // GILLES
            Solipsis::TextureExtParamsMap *textureExtParamsMap;
            textureExtParamsMap = getTextureExtParamsMap((*itText));
            if (textureExtParamsMap)
            {
                // Stop the old sound if any
                if (textureExtParamsMap->find("plugin") != textureExtParamsMap->end())
                {
                    std::string plugin = (textureExtParamsMap->find("plugin"))->second ;
                    if ((plugin == "swf") /*|| (plugin == "swf") || (plugin == "www" )*/)
                        ms_MMMTextureManager->pauseEffect( this, (*itText)->getName(), *textureExtParamsMap );
                }
            }
#endif
            it = mTextureNameExtParamsMap.find((*itText)->getName());
            if (it != mTextureNameExtParamsMap.end())
                ms_MMMTextureManager->releaseTexture(this, (*itText)->getName(), it->second);
            else
                ms_MMMTextureManager->releaseTexture(this, (*itText)->getName(), TextureExtParamsMap());
            // Next element
            itText = mTextures.erase( itText);
        }
        // Reset iterators
        mCurrentSelectedTextureIterator = mCurrentTextureIterator = mTextures.begin();
    }   
    mTextureNameExtParamsMap.clear();
}


//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::deleteTexture(TexturePtr pTexture)
{
    if (pTexture.isNull())
    {
        LogManager::getSingleton().logMessage("ModifiedMaterialManager::deleteTexture() : NULL Texture pointer" );
        return;
    }
	if( pTexture->getName() != "default_texture.jpg" )	//if it is not the default texture ...
    {
        if ((*mCurrentSelectedTextureIterator)->getName() == pTexture->getName())
           // The texture to delete is the current one ==> We must change the current selected texture
           setPreviousTextureAsCurrent();
        
        // Destroy texture
        if (ms_MMMTextureManager != 0)
        {
#if 1 // GILLES
            Solipsis::TextureExtParamsMap *textureExtParamsMap;
            textureExtParamsMap = getTextureExtParamsMap(pTexture);
            if (textureExtParamsMap)
            {
                // Stop the old sound if any
                if (textureExtParamsMap->find("plugin") != textureExtParamsMap->end())
                {
                    std::string plugin = (textureExtParamsMap->find("plugin"))->second ;
                    if ((plugin == "swf") /*|| (plugin == "swf") || (plugin == "www" )*/)
                        ms_MMMTextureManager->pauseEffect( this, pTexture->getName(), *textureExtParamsMap );
                }
            }
#endif
            TextureNameExtParamsMap::iterator it = mTextureNameExtParamsMap.find(pTexture->getName());
            if (it != mTextureNameExtParamsMap.end())
            {
                ms_MMMTextureManager->releaseTexture(this, pTexture->getName(), it->second);
                it = mTextureNameExtParamsMap.erase(it); // Delete texture ext params accessor
            }
            else
                ms_MMMTextureManager->releaseTexture(this, pTexture->getName(), TextureExtParamsMap());
        }
        mTextures.remove( pTexture );
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
bool ModifiedMaterialManager::isPresentInList(TexturePtr pTexture) 
{
	TextureVector::iterator itr ;
	for (itr=mTextures.begin() ; itr != mTextures.end() ; itr++)
	{
		if( (*itr) == pTexture)
		{ 
			return true ;
		}
	}
	return false ;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
TextureExtParamsMap* ModifiedMaterialManager::getTextureExtParamsMap(TexturePtr pTexture)
{
    return getTextureExtParamsMap(pTexture->getName());
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
TextureExtParamsMap* ModifiedMaterialManager::getTextureExtParamsMap(const Ogre::String name)
{
    TextureNameExtParamsMap::iterator it = mTextureNameExtParamsMap.find(name);
    if (it == mTextureNameExtParamsMap.end())
        return 0;
    return &(it->second);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::setTextureScroll(float pU, float pV)
{
	mModifiedMaterial->setTextureScroll( pU, pV ) ;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
Ogre::Vector2 ModifiedMaterialManager::getTextureScroll()
{
	return (mModifiedMaterial->getTextureScroll() ) ;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::setTextureScale(float pU, float pV)
{
	mModifiedMaterial->setTextureScale( pU, pV) ;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
Ogre::Vector2 ModifiedMaterialManager::getTextureScale()
{
	return (mModifiedMaterial->getTextureScale() ) ;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::setTextureRotate(Ogre::Radian pAngle)
{
	mModifiedMaterial->setTextureRotate( pAngle) ;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
Ogre::Radian ModifiedMaterialManager::getTextureRotate()
{
	return mModifiedMaterial->getTextureRotate();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::setAlpha(float pValue)
{
	mModifiedMaterial->setAlpha(pValue);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float ModifiedMaterialManager::getAlpha()
{
	return mModifiedMaterial->getAlpha() ;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::setSceneBlendType(Ogre::SceneBlendType pSceneBlendType)
{
	mModifiedMaterial->setSceneBlendType(pSceneBlendType);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
Ogre::SceneBlendType ModifiedMaterialManager::getSceneBlendType()
{
	return mModifiedMaterial->getSceneBlendType() ;
}

}// namespace