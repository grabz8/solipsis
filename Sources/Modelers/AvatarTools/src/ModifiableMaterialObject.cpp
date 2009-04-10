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

#include "ModifiableMaterialObject.h"

using namespace Solipsis;

ModifiableMaterialObjectBase::ModifiableMaterialObjectBase() :
mColourModifiable(false)
{
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
bool ModifiableMaterialObjectBase::isColourModifiable()
{
	return mColourModifiable;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObjectBase::setColourModifiable(bool isColourModifiable)
{
	mColourModifiable = isColourModifiable;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
bool ModifiableMaterialObjectBase::isTextureModifiable()
{
	return (mTextures.size() > 1);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
TexturePtr ModifiableMaterialObjectBase::getTexture(const String& name)
{
	TextureVector::iterator textureIterator = mTextures.begin();
	TextureVector::iterator endVector = mTextures.end();
	while ((textureIterator != endVector) && ((*textureIterator)->getName() != name)) textureIterator++;
	if (textureIterator != endVector)
	{
		return *textureIterator;
	}else{
		static TexturePtr nullTexture;
		return nullTexture;
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
TextureVectorIterator ModifiableMaterialObjectBase::getTextureIterator()
{
	return TextureVectorIterator(mTextures.begin(),mTextures.end());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObjectBase::addTexture(TexturePtr texture)
{
	mTextures.push_back(texture);
	mDefaultTextureIterator = mTextures.begin();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObjectBase::removeTexture(TexturePtr texture)
{
	mTextures.remove(texture);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
ModifiableMaterialObject::ModifiableMaterialObject(ModifiableMaterialObjectBase* modifiableMaterialObjectBase) :
mModifiableMaterialObjectBase(modifiableMaterialObjectBase), mGhost(false)
{
	mModifiedMaterial = NULL;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
ModifiableMaterialObject::~ModifiableMaterialObject()
{
	if (mModifiedMaterial)
		delete mModifiedMaterial;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::initialise(const MaterialPtr& material)
{
	mModifiedMaterial = new ModifiedMaterial(material);

	mModifiedMaterial->useAddedColour(mModifiableMaterialObjectBase->isColourModifiable());

	mBackAmbient = getColourAmbient();
	mBackDiffuse = getColourDiffuse();
	mBackSpecular = getColourSpecular();
	mBackShininess = getShininess();
	mBackAlpha = mModifiedMaterial->getAlpha();
    mAlpha = mBackAlpha;

	if (mModifiedMaterial->hasATexture())
	{
		String textureName = mModifiedMaterial->getTextureName();
		TexturePtr texture = TextureManager::getSingleton().getByName(textureName);
		addTexture(texture);
	}
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
ModifiedMaterial* ModifiableMaterialObject::getModifiedMaterial()
{
	return mModifiedMaterial;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
bool ModifiableMaterialObject::isColourModifiable()
{
    return mModifiedMaterial->isUsingAddedColour();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::setColourModifiable(bool isColourModifiable)
{
    mModifiableMaterialObjectBase->setColourModifiable(isColourModifiable);
	mModifiedMaterial->useAddedColour(isColourModifiable);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const ColourValue& ModifiableMaterialObject::getColour()
{
	return mModifiedMaterial->getAddedColour();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::setColour(const ColourValue& colour)
{
	mModifiedMaterial->setAddedColour(colour);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const ColourValue& ModifiableMaterialObject::getColourAmbient()
{
	return mModifiedMaterial->getAmbient();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::setColourAmbient(const ColourValue& colour)
{
	mModifiedMaterial->setAmbient(colour);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const ColourValue& ModifiableMaterialObject::getColourDiffuse()
{
	return mModifiedMaterial->getDiffuse();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::setColourDiffuse(const ColourValue& colour)
{
	mModifiedMaterial->setDiffuse(colour);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const ColourValue& ModifiableMaterialObject::getColourSpecular()
{
	return mModifiedMaterial->getSpecular();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::setColourSpecular(const ColourValue& colour)
{
	mModifiedMaterial->setSpecular(colour);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const float ModifiableMaterialObject::getAlpha()
{
    return mAlpha;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::setAlpha(const float opacity)
{
    mAlpha = opacity;
    mModifiedMaterial->setAlpha(opacity*(mGhost ? 0.5f : 1.0f));
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::setGhost(bool ghost)
{
    mGhost = ghost;
    mModifiedMaterial->setAlpha(mAlpha*(mGhost ? 0.5f : 1.0f));
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const float ModifiableMaterialObject::getShininess()
{
	return mModifiedMaterial->getShininess();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::setShininess(const float shininess)
{
	mModifiedMaterial->setShininess(shininess);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::resetColour()
{
	//setColour(ColourValue(0.5,0.5,0.5,1));

	setColourAmbient( mBackAmbient );
	setColourDiffuse( mBackDiffuse );
	setColourSpecular( mBackSpecular );
	setShininess( mBackShininess );
	setAlpha( mBackAlpha );
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
TexturePtr ModifiableMaterialObject::getCurrentTexture()
{
	return *mCurrentTextureIterator;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::setPreviousTextureAsCurrent()
{
    if (mCurrentTextureIterator == mModifiableMaterialObjectBase->mTextures.begin()) 
        mCurrentTextureIterator = mModifiableMaterialObjectBase->mTextures.end();
	mCurrentTextureIterator--;
	mModifiedMaterial->setTexture((*mCurrentTextureIterator)->getName());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::setNextTextureAsCurrent()
{
	mCurrentTextureIterator++;
	if (mCurrentTextureIterator == mModifiableMaterialObjectBase->mTextures.end()) 
        mCurrentTextureIterator = mModifiableMaterialObjectBase->mTextures.begin();
	mModifiedMaterial->setTexture((*mCurrentTextureIterator)->getName());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::setCurrentTexture(TexturePtr texture)
{
	TextureVector::iterator textureIterator = mModifiableMaterialObjectBase->mTextures.begin();
	TextureVector::iterator endVector = mModifiableMaterialObjectBase->mTextures.end();
	while ((textureIterator != endVector) && ((*textureIterator)->getName() != texture->getName())) textureIterator++;
	assert( (textureIterator != endVector) && "Texture not found !");

    mCurrentTextureIterator = textureIterator;
	mModifiedMaterial->setTexture((*mCurrentTextureIterator)->getName());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::setCurrentTexture(const String& textureName)
{
	setCurrentTexture(TextureManager::getSingleton().getByName(textureName));
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::setDefaultTextureAsCurrent()
{
	mCurrentTextureIterator = mModifiableMaterialObjectBase->mDefaultTextureIterator;
	mModifiedMaterial->setTexture((*mCurrentTextureIterator)->getName());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::addTexture(TexturePtr texture)
{
    mModifiableMaterialObjectBase->addTexture(texture);
	mCurrentTextureIterator = mModifiableMaterialObjectBase->mDefaultTextureIterator;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::removeTexture(TexturePtr texture)
{
	setPreviousTextureAsCurrent();
    mModifiableMaterialObjectBase->removeTexture(texture);
}



//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::resetModifications()
{
	setDefaultTextureAsCurrent();
	resetColour();
}
