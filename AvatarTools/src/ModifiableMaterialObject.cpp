#include "ModifiableMaterialObject.h"

using namespace Solipsis;

//--------------------------------------------------------------------------------------------------------------------------------------------------
ModifiableMaterialObject::ModifiableMaterialObject()
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
	return mModifiedMaterial->getDiffus();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::setColourDiffuse(const ColourValue& colour)
{
	mModifiedMaterial->setDiffus(colour);
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
const float ModifiableMaterialObject::getTransparency()
{
	return mModifiedMaterial->getAlpha();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::setTransparency(const float opacity)
{
	mModifiedMaterial->setAlpha(opacity);
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
	setColour(ColourValue(0.5,0.5,0.5,1));
}



//--------------------------------------------------------------------------------------------------------------------------------------------------
bool ModifiableMaterialObject::isTextureModifiable()
{
	return (mTextures.size() > 1);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
TexturePtr ModifiableMaterialObject::getTexture(const String& name)
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
TexturePtr ModifiableMaterialObject::getCurrentTexture()
{
	return *mCurrentTextureIterator;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
TextureVectorIterator ModifiableMaterialObject::getTextureIterator()
{
	return TextureVectorIterator(mTextures.begin(),mTextures.end());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::setPreviousTextureAsCurrent()
{
	if (mCurrentTextureIterator == mTextures.begin()) mCurrentTextureIterator = mTextures.end();
	mCurrentTextureIterator--;
	mModifiedMaterial->setTexture((*mCurrentTextureIterator)->getName());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::setNextTextureAsCurrent()
{
	mCurrentTextureIterator++;
	if (mCurrentTextureIterator == mTextures.end()) mCurrentTextureIterator = mTextures.begin();
	mModifiedMaterial->setTexture((*mCurrentTextureIterator)->getName());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::setCurrentTexture(TexturePtr texture)
{
	TextureVector::iterator textureIterator = mTextures.begin();
	TextureVector::iterator endVector = mTextures.end();
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
	mCurrentTextureIterator = mDefaultTextureIterator;
	mModifiedMaterial->setTexture((*mCurrentTextureIterator)->getName());

}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::addTexture(TexturePtr texture)
{
	mTextures.push_back(texture);
	mDefaultTextureIterator = mTextures.begin();
	mCurrentTextureIterator = mDefaultTextureIterator;
}



//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiableMaterialObject::resetModifications()
{
	setDefaultTextureAsCurrent();
	resetColour();
}
