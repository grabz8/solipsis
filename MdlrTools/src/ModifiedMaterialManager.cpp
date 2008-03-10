#include ".\ModifiedMaterialManager.h"

#include "ModifiedMaterial.h"

namespace Solipsis {

ModifiedMaterialManager::ModifiedMaterialManager(void)
{
	mModifiedMaterial = NULL;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
ModifiedMaterialManager::~ModifiedMaterialManager(void)
{
	if (mModifiedMaterial)
		delete mModifiedMaterial;
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
bool ModifiedMaterialManager::isTextureModifiable()
{
	return (mTextures.size() > 1);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
TexturePtr ModifiedMaterialManager::getTexture(const String& name)
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
TexturePtr ModifiedMaterialManager::getTexture(const int n)
{
	if( (n>-1)&&(n < mTextures.size()) )
	{
		TextureVector::iterator textureIterator = mTextures.begin();
		for(int i=0; i<n ; i++)
			textureIterator++ ;		
		
		return (*textureIterator) ;
	}
	else
	{
		static TexturePtr nullTexture;
		return nullTexture;
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
TexturePtr ModifiedMaterialManager::getCurrentTexture()
{
	return *mCurrentTextureIterator;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
TextureVectorIterator ModifiedMaterialManager::getTextureIterator()
{
	return TextureVectorIterator(mTextures.begin(),mTextures.end());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::setPreviousTexture()
{
	if (mCurrentTextureIterator == mTextures.begin()) mCurrentTextureIterator = mTextures.end();
	mCurrentTextureIterator--;
	//mModifiedMaterial->setTexture((*mCurrentTextureIterator)->getName());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::setPreviousTextureAsCurrent()
{
	if (mCurrentTextureIterator == mTextures.begin()) mCurrentTextureIterator = mTextures.end();
	mCurrentTextureIterator--;
	mModifiedMaterial->setTexture((*mCurrentTextureIterator)->getName());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::setNextTexture()
{
	mCurrentTextureIterator++;
	if (mCurrentTextureIterator == mTextures.end()) mCurrentTextureIterator = mTextures.begin();
	//  mModifiedMaterial->setTexture((*mCurrentTextureIterator)->getName());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::setNextTextureAsCurrent()
{
	mCurrentTextureIterator++;
	if (mCurrentTextureIterator == mTextures.end()) mCurrentTextureIterator = mTextures.begin();
	mModifiedMaterial->setTexture((*mCurrentTextureIterator)->getName());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::setCurrentTexture(TexturePtr texture)
{
	TextureVector::iterator textureIterator = mTextures.begin();
	TextureVector::iterator endVector = mTextures.end();
	while ((textureIterator != endVector) && ((*textureIterator)->getName() != texture->getName())) textureIterator++;
	assert( (textureIterator != endVector) && "Texture not found !");

    mCurrentTextureIterator = textureIterator;
	mModifiedMaterial->setTexture((*mCurrentTextureIterator)->getName());
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
void ModifiedMaterialManager::addTexture(TexturePtr texture)
{
	mTextures.push_back(texture);

	//mDefaultTextureIterator = mTextures.end();
	//mDefaultTextureIterator--;
	//mCurrentTextureIterator = mDefaultTextureIterator;
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
void ModifiedMaterialManager::deleteLastTexture()
{
	if (mTextures.size() > 0)	//if there are 2 textures or more
	{
		TextureVector::iterator itr ;
		itr = mTextures.end() ;
		if( mDefaultTextureIterator == itr)
            mDefaultTextureIterator = mTextures.begin() ;
		if( mCurrentTextureIterator == itr)
			setCurrentTexture( (*(mTextures.begin())) ) ;
				
		mTextures.pop_back();
		
	}

}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterialManager::deleteTexture(TexturePtr pTexture)
{
	if( pTexture->getName() != "default_texture.jpg" )	//if it is not the default texture ...
		/*
		if (mTextures.size() > 0)	//if there are 2 textures or more ...
		{	
			if( (*mCurrentTextureIterator) == pTexture)
				setCurrentTexture( "default_texture.jpg" ) ;

			if( (*mDefaultTextureIterator) == pTexture)
				this->setDefaultTextureAsCurrent();
				
			setCurrentTexture( getCurrentTexture() ) ;
			mTextures.remove( pTexture );
		}
		*/
	{
		setPreviousTextureAsCurrent();
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
	mModifiedMaterial->setAlpha( pValue ) ;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float ModifiedMaterialManager::getAlpha()
{
	return mModifiedMaterial->getAlpha() ;
}

}// namespace