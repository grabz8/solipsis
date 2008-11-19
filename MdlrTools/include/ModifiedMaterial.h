/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author ARTEFACTO - François FOURNEL & Patrice DESFONDS

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

#ifndef __ModifiedMaterial_h__
#define __ModifiedMaterial_h__

#include <Ogre.h>


/// brief This is the ModifiedMaterial class associated to an Ogre material. It make possible to easily modify colours and texture of a material 
/// file ModifiedMaterial.h
/// author François FOURNEL & Patrice DESFONDS
/// date 2007.07.18


using namespace Ogre;
namespace Solipsis {

class ModifiedMaterial
{
public:
	///brief Constructor, the class will take the first texture unit state of the first pass of the first technique of the material to modify colours and textures. The class will create technique,pass, texture unit state if they doesn't exist
	///param material Ogre material to modify
	ModifiedMaterial(const MaterialPtr& material);

	///brief Constructor, the class will take the first texture unit state of the first pass of the first technique of the material to modify colours and textures. The class will create technique,pass, texture unit state if they doesn't exist
	///param subEntity SubEntity from which the material has to be modified
	ModifiedMaterial(SubEntity* subEntity);

	///brief Refresh technique, pass and texture unit
    void refreshTechPassTextUnit();

	///brief Return the owner material
	///return the owner material
	const MaterialPtr& getOwner();

	///brief Return the shininess  of the material
	///return the shininess  of the material
	const float getShininess();

	///brief Return the specular colour of the material
	///return the specular colour of the material
	const ColourValue& getSpecular();
	
	///brief Return the diffus colour of the material
	///return the diffus colour of the material
	const ColourValue& getDiffus();

	///brief Return the ambient colour of the material
	///return the ambient colour of the material
	const ColourValue& getAmbient();

	///brief Method which return the red composant of the ambient colour of the material
	///return the red composant of the ambient colour of the material
	float getRedAmbient();

	///brief Method which return the green composant of the ambient colour of the material
	///return the green composant of the ambient colour of the material
	float getGreenAmbient();

	///brief Method which return the blue composant of the ambient colour of the material
	///return the blue composant of the ambient colour of the material
	float getBlueAmbient();

	///brief Method which tells wether or not we are using a signed added colour applied on all texture layers of the material
	///return true if we are using an added colour, false otherwise
	bool isUsingAddedColour();

	///brief Return the (signed) added colour of the material
	///return the (signed) added colour of the material
	const ColourValue& getAddedColour();

	///brief Method which return the red composant of the (signed) added colour of the material
	///return the red composant of the (signed) added colour of the material
	float getRedAddedColour();

	///brief Method which return the green composant of the (signed) added colour of the material
	///return the green composant of the (signed) added colour of the material
	float getGreenAddedColour();

	///brief Method which return the blue composant of the (signed) added colour of the material
	///return the blue composant of the (signed) added colour of the material
	float getBlueAddedColour();

	


	///brief Method which set the ambient colour of the material
	///param colour Colour to set as ambient colour of the material
	void setAmbient(const ColourValue& colour);

	///brief Method which set the red composant of the ambient colour of the material
	///param red Red composant of the ambient colour of the material (between 0 and 1)
	void setRedAmbient(float red);

	///brief Method which set the green composant of the ambient colour of the material
	///param green Green composant of the ambient colour of the material (between 0 and 1)
	void setGreenAmbient(float green);

	///brief Method which set the blue composant of the ambient colour of the material
	///param blue Blue composant of the ambient colour of the material (between 0 and 1)
	void setBlueAmbient(float blue);

	///brief Method which set the diffus colour of the material
	///param colour Colour to set as diffus colour of the material
	void setDiffus(const ColourValue& colour);

	///brief Method which set the specular colour of the material
	///param colour Colour to set as specular colour of the material
	void setSpecular(const ColourValue& colour);

	///brief Method which set the shininess of the material
	///param colour Colour to set as shininess of the material
	void setShininess(const float colour);


	///brief Method which specifies if we have to use a signed added colour applied on all texture layers of the material
	///param b If true, an added colour will be applied, if false, no added colour will be used
	void useAddedColour(bool b);

	///brief Method which set the (signed) added colour of the material
	///param colour Colour to set as (signed) added colour of the material
	void setAddedColour(const ColourValue& colour);

	///brief Method which set the red composant of the (signed) added colour of the material
	///param red Red composant of the (signed) added colour of the material (between 0 and 1)
	void setRedAddedColour(float red);

	///brief Method which set the green composant of the (signed) added colour of the material
	///param green Green composant of the (signed) added colour of the material (between 0 and 1)
	void setGreenAddedColour(float green);

	///brief Method which set the blue composant of the (signed) added colour of the material
	///param blue Blue composant of the (signed) added colour of the material (between 0 and 1)
	void setBlueAddedColour(float blue);

    ///brief Method which return the culling mode of the object.
	///return Culling mode of the object.
	const CullingMode& getCullingMode();

	///brief Method which set the culling mode of the object.
	///param mode Culling mode to apply to the object.
	void setCullingMode(const CullingMode& mode);



	///brief Method which tells wether or not the material has a texture.
	///return true if the material has a texture
	bool hasATexture();

	///brief Method which return the texture's name of the material.
	///return the texture'name of the material.
	const String& getTextureName();

	///brief Method which change the texture of the material.
	///param name Name of the texture to apply to the material.
	void setTexture(const String& name); 


	///biref Sets the translation offset of the texture, ie scrolls the texture
	///param pU  The amount the texture should be moved horizontally (u direction). 
	///param pV  The amount the texture should be moved vertically (v direction). 
	void setTextureScroll(float pU, float pV);
	///biref Get the translation offset of the texture, ie scrolls the texture
	///return Vector2 (U, V) 
	Ogre::Vector2 getTextureScroll();

	///biref Sets the scaling factor applied to texture coordinates
	///param pU  The amount the texture should be scalled horizontally (u direction). 
	///param pV  The amount the texture should be scalled vertically (v direction). 
	void setTextureScale(float pU, float pV);
	///biref Get the scale of the texture
	///return Vector2 (U, V) 
	Ogre::Vector2 getTextureScale();

	///biref Sets the anticlockwise rotation factor applied to texture coordinates. (in radian)
	///param pAngle  angle  The angle of rotation (anticlockwise).   
	void setTextureRotate(Ogre::Radian pAngle);
	///biref Get the rotate of the texture (in radian)
	///return the value of rotation
	Ogre::Radian getTextureRotate();

	///biref Sets the alpha value to be applied to this object. 
	///param pValue alpha value (between 0 - 1)
	void setAlpha(float pValue);
	///biref Get the alpha value of the object
	///return the value of alpha
	float getAlpha();
	///brief Sets the scene blending type to be applied to this object and all it children. 
	///param pSceneBlendType scene blending type
    void setSceneBlendType(Ogre::SceneBlendType pSceneBlendType);
	///brief Get the scene blending type of the object
	///return the scene blending type
	Ogre::SceneBlendType getSceneBlendType();



private:
	MaterialPtr mMaterial;								///brief Ogre Material associated to the ModifiedMaterial class
	Technique* mTechnique;								///brief Technique containing the pass to modify
	Pass* mPass;										///brief Pass to modify, containing the colours and the TextureUnitState to modify
	TextureUnitState* mTextureUnitState;				///brief TextureUnitState containing the texture to modify

	bool mUseAddedColour;								///brief Tells whether or not the material has a texture layer filled with a colour which is signed added to the previous layers
	ColourValue mAddedColour;							///brief Colour which can be signed added to the texture layers of the material
	TextureUnitState* mAddedColourTextureUnitState;		///brief TextureUnitState containing the texture to modify
	unsigned short mIndexAddedColourTextureUnitState;	///brief index of mAddedColourTextureUnitState if mPass
};

} //namespace

#endif
