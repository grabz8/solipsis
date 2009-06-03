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

#include "ModifiedMaterial.h"

namespace Solipsis {

//--------------------------------------------------------------------------------------------------------------------------------------------------
ModifiedMaterial::ModifiedMaterial(const MaterialPtr& material) :
mMaterial(material), mUseAddedColour(false), mAddedColour(ColourValue(0.5,0.5,0.5,1))
{
	if (mMaterial->getNumTechniques() > 0)
	{
		mTechnique = mMaterial->getTechnique(0);
	}else{
		mTechnique = mMaterial->createTechnique();
	}

	if (mTechnique->getNumPasses() > 0)
	{
		mPass = mTechnique->getPass(0);
	}else{
		mPass = mTechnique->createPass();
	}

	if (mPass->getNumTextureUnitStates() > 0)
	{
		mTextureUnitState = mPass->getTextureUnitState(0);
	}else{
		mTextureUnitState = mPass->createTextureUnitState();
	}

#if 1 // GILLES
    //mPass->setAmbient( .5, .5, .5 );

    // mipmap level
    //mTextureUnitState->setTextureMipmapBias(0.);
    // set the filter anisotropic
    //mTextureUnitState->setTextureFiltering(Ogre::FO_ANISOTROPIC, Ogre::FO_ANISOTROPIC, Ogre::FO_NONE);
    //mTextureUnitState->setTextureAnisotropy(4);
    mTextureUnitState->setTextureFiltering(Ogre::TFO_ANISOTROPIC);
    mTextureUnitState->setTextureAnisotropy(16);
    // set the filter ...
    //mTextureUnitState->setTextureFiltering(Ogre::FO_NONE);
    // set the filter ...
    //mTextureUnitState->setTextureFiltering(Ogre::TFO_TRILINEAR);
#endif
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
ModifiedMaterial::ModifiedMaterial(SubEntity* subEntity) :
mUseAddedColour(false), mAddedColour(ColourValue(0.5,0.5,0.5,1))
{
	assert(subEntity != NULL);
	mMaterial = subEntity->getMaterial();

	refreshTechPassTextUnit();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
ModifiedMaterial::~ModifiedMaterial() 
{ 
    // TODO : Clean propre des matériaux !!!

    // Clean Pass
    //mPass->removeAllTextureUnitStates();
    //mTextureUnitState = NULL;
    //mAddedColourTextureUnitState = NULL;
    //// Clean Pass
    //mTechnique->removeAllPasses();
    //mPass = NULL;
    //// Clean technique
    //mMaterial->removeAllTechniques();
    //mTechnique = NULL;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterial::refreshTechPassTextUnit()
{
	if (mMaterial->getNumTechniques() > 0)
	{
		mTechnique = mMaterial->getTechnique(0);
	}else{
		mTechnique = mMaterial->createTechnique();
	}

	if (mTechnique->getNumPasses() > 0)
	{
		mPass = mTechnique->getPass(0);
	}else{
		mPass = mTechnique->createPass();
	}

	if (mPass->getNumTextureUnitStates() > 0)
	{
		mTextureUnitState = mPass->getTextureUnitState(0);
	}else{
		mTextureUnitState = mPass->createTextureUnitState();
	}
#if 1 // GILLES
    // mipmap level
    //mTextureUnitState->setTextureMipmapBias(0.);
    // set the filter anisotropic
    //mTextureUnitState->setTextureFiltering(Ogre::FO_ANISOTROPIC, Ogre::FO_ANISOTROPIC, Ogre::FO_NONE);
    //mTextureUnitState->setTextureAnisotropy(4);
    mTextureUnitState->setTextureFiltering(Ogre::TFO_ANISOTROPIC);
    mTextureUnitState->setTextureAnisotropy(16);
    // set the filter ...
    //mTextureUnitState->setTextureFiltering(Ogre::FO_NONE);
    // set the filter ...
    //mTextureUnitState->setTextureFiltering(Ogre::TFO_TRILINEAR);
#endif
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
const MaterialPtr& ModifiedMaterial::getOwner()
{
	return mMaterial;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
const ColourValue& ModifiedMaterial::getDiffuse()
{
	return mPass->getDiffuse();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
const ColourValue& ModifiedMaterial::getSpecular()
{
	return mPass->getSpecular();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
const float ModifiedMaterial::getShininess()
{
	return mPass->getShininess();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
const ColourValue& ModifiedMaterial::getAmbient()
{
	return mPass->getAmbient();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float ModifiedMaterial::getRedAmbient()
{
	return mPass->getAmbient().r;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float ModifiedMaterial::getGreenAmbient()
{
	return mPass->getAmbient().g;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float ModifiedMaterial::getBlueAmbient()
{
	return mPass->getAmbient().b;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
bool ModifiedMaterial::isUsingAddedColour()
{
	return mUseAddedColour;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const ColourValue& ModifiedMaterial::getAddedColour()
{
	return mAddedColour;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float ModifiedMaterial::getRedAddedColour()
{
	return mAddedColour.r;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float ModifiedMaterial::getGreenAddedColour()
{
	return mAddedColour.g;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float ModifiedMaterial::getBlueAddedColour()
{
	return mAddedColour.b;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterial::setAmbient(const ColourValue& colour)
{
	mPass->setAmbient(colour);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterial::setRedAmbient(float red)
{
	ColourValue colour = mPass->getAmbient();
	colour.r = red;
	mPass->setAmbient(colour);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterial::setGreenAmbient(float green)
{
	ColourValue colour = mPass->getAmbient();
	colour.g = green;
	mPass->setAmbient(colour);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterial::setBlueAmbient(float blue)
{
	ColourValue colour = mPass->getAmbient();
	colour.b = blue;
	mPass->setAmbient(colour);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterial::setDiffuse(const ColourValue& colour)
{
	mPass->setDiffuse(colour);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterial::setSpecular(const ColourValue& colour)
{
	mPass->setSpecular(colour);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterial::setShininess(const float colour)
{
	mPass->setShininess(colour) ;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
CullingMode ModifiedMaterial::getCullingMode()
{
    return mPass->getCullingMode();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterial::setCullingMode(CullingMode mode)
{
    mPass->setCullingMode( mode );
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterial::useAddedColour(bool b)
{
	if (!mUseAddedColour && b) //We must create a texture unit state since before this time no added colour were used
	{
#if 0 // GILLES
        mIndexAddedColourTextureUnitState = mPass->getNumTextureUnitStates();
		
        // if no textures : it's already an added colour TextureUnitState so don't crate another one
        if (mIndexAddedColourTextureUnitState > 0 &&
            mPass->getTextureUnitState(mIndexAddedColourTextureUnitState-1)->getNumFrames() < 1)
        {
            mAddedColourTextureUnitState = mPass->getTextureUnitState(mIndexAddedColourTextureUnitState-1);
            mAddedColour = mAddedColourTextureUnitState->getColourBlendMode().colourArg1;
        }
        else
        {
            mAddedColourTextureUnitState = mPass->createTextureUnitState();
		    mAddedColourTextureUnitState->setColourOperationEx(LBX_ADD_SIGNED, LBS_MANUAL, LBS_CURRENT, mAddedColour);
        }
#else
		mIndexAddedColourTextureUnitState = mPass->getNumTextureUnitStates();
		mAddedColourTextureUnitState = mPass->createTextureUnitState();
		mAddedColourTextureUnitState->setColourOperationEx(LBX_ADD_SIGNED, LBS_MANUAL, LBS_CURRENT, mAddedColour);
#endif
	}
	if (!b && mUseAddedColour) //We must delete the texture unit state since before this time it was present and now we don't want it anymore
	{
		mPass->removeTextureUnitState(mIndexAddedColourTextureUnitState);
	}

	mUseAddedColour = b;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterial::setAddedColour(const ColourValue& colour)
{
	if (!mUseAddedColour) return;

	mAddedColour = colour;
	mAddedColourTextureUnitState->setColourOperationEx(LBX_ADD_SIGNED, LBS_MANUAL, LBS_CURRENT, mAddedColour);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterial::setRedAddedColour(float red)
{
	if (!mUseAddedColour) return;

	mAddedColour.r = red;
	mAddedColourTextureUnitState->setColourOperationEx(LBX_ADD_SIGNED, LBS_MANUAL, LBS_CURRENT, mAddedColour);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterial::setGreenAddedColour(float green)
{
	if (!mUseAddedColour) return;

	mAddedColour.g = green;
	mAddedColourTextureUnitState->setColourOperationEx(LBX_ADD_SIGNED, LBS_MANUAL, LBS_CURRENT, mAddedColour);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterial::setBlueAddedColour(float blue)
{
	if (!mUseAddedColour) return;

	mAddedColour.b = blue;
	mAddedColourTextureUnitState->setColourOperationEx(LBX_ADD_SIGNED, LBS_MANUAL, LBS_CURRENT, mAddedColour);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
bool ModifiedMaterial::hasATexture()
{
	return (mTextureUnitState->getTextureName() != "");
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const String& ModifiedMaterial::getTextureName()
{
	return mTextureUnitState->getTextureName();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterial::setTexture(const String& name)
{
    refreshTechPassTextUnit(); // Avoid a crash while reloading a web texture. 
#if 1 // GILLES
    std::string ext( name.substr(name.size()-3, 3) );
    if( ext == "png" ||
        ext == "tga" )
    {
        mPass->setSceneBlending( SBT_TRANSPARENT_ALPHA );
        mPass->setDepthWriteEnabled( false );
    }
#endif
	mTextureUnitState->setTextureName(name);
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterial::setTextureScroll(float pU, float pV)
{
	mTextureUnitState->setTextureScroll( pU, pV ) ;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
Ogre::Vector2 ModifiedMaterial::getTextureScroll()
{
	Ogre::Vector2 result (0, 0);
	result.x = mTextureUnitState->getTextureUScroll ();
	result.y = mTextureUnitState->getTextureVScroll ();
	return result ;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterial::setTextureScale(float pU, float pV)
{
	mTextureUnitState->setTextureScale( pU, pV) ;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
Ogre::Vector2 ModifiedMaterial::getTextureScale()
{
	Ogre::Vector2 result (0, 0);
	result.x = mTextureUnitState->getTextureUScale ();
	result.y = mTextureUnitState->getTextureVScale ();
	return result ;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterial::setTextureRotate(Ogre::Radian pAngle)
{
	mTextureUnitState->setTextureRotate( pAngle) ;
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
Ogre::Radian ModifiedMaterial::getTextureRotate()
{
	return mTextureUnitState->getTextureRotate();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterial::setAlpha(float pValue)
{
	ColourValue colour = mPass->getDiffuse();
	colour.a = pValue;
	mPass->setDiffuse(colour);
    if (colour.a < 1.0f)
    {
        mPass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
        mPass->setDepthWriteEnabled(false);
    }
    else
    {
        mPass->setSceneBlending(Ogre::SBT_REPLACE);
        mPass->setDepthWriteEnabled(true);
    }
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
float ModifiedMaterial::getAlpha()
{
	return mPass->getDiffuse().a ;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void ModifiedMaterial::setSceneBlendType(SceneBlendType pSceneBlendType)
{
    mPass->setSceneBlending(pSceneBlendType);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
SceneBlendType ModifiedMaterial::getSceneBlendType()
{
    if ((mPass->getSourceBlendFactor() == SBF_SOURCE_ALPHA) && (mPass->getDestBlendFactor() == SBF_ONE_MINUS_SOURCE_ALPHA))
        return SBT_TRANSPARENT_ALPHA;
    return SBT_REPLACE;
}

} // namespace