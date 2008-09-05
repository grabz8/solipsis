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

/**
	\file 
		ChooseColorWindow.h
	\brief
		Solipsis class for manage amm materials of one object
	\author
		ARTEFACTO -  François FOURNEL & Patrice DESFONDS
*/


#ifndef __ModifiedMaterialManager_h_
#define __ModifiedMaterialManager_h_

#include <Ogre.h>
using namespace Ogre;
namespace Solipsis {

class ModifiedMaterial;
class Object3D;

//Texture iterator :
typedef std::map<String,TexturePtr> TextureMap;
typedef MapIterator<TextureMap> TextureMapIterator;

typedef std::list<TexturePtr> TextureVector;
typedef VectorIterator<TextureVector> TextureVectorIterator;

/// brief The texture extended parameters map
typedef std::map<String,String> TextureExtParamsMap;

typedef std::map<String,TextureExtParamsMap> TextureNameExtParamsMap;

class ModifiedMaterialManager
{
public:
    class MMMTextureManager
    {
    public:
	    /// brief callback to load a texture
	    /// param object3D The object3D loading the texture
	    /// param name The name of the texture
	    /// param textureExtParamsMap The extended parameters of the texture
	    /// return the texture
        virtual TexturePtr loadTexture(Object3D* object, const String& name, const TextureExtParamsMap& textureExtParamsMap) = 0;
	    /// brief callback to release a texture
	    /// param modifiedMaterialManager The material manager
	    /// param name The name of the texture
	    /// param textureExtParamsMap The extended parameters of the texture
        virtual void releaseTexture(ModifiedMaterialManager* modifiedMaterialManager, const String& name, const TextureExtParamsMap& textureExtParamsMap) = 0;
    };

public:
	///brief Constructor
	ModifiedMaterialManager(void);
	///brief Destructor
	~ModifiedMaterialManager(void);

	///brief Static method to set the texture manager.
	///param textureManager Texture manager.
    static void setMMMTextureManager(MMMTextureManager* textureManager);

	///brief Static method to get the texture manager.
	///return The texture manager.
    static MMMTextureManager* getMMMTextureManager();

	///brief Method which initialises the class according to a material.
	///param material Material of the object which is going to be modified.
	void initialise(const MaterialPtr& material);

	///brief Method which return the ModifiedMaterial of the object.
	///return The ModifiedMaterial of the object.
	ModifiedMaterial* getModifiedMaterial();

	///brief Method which return whether or not the colour of the object is modifiable.
	///return true is colour of the object is modifiable, false else.
	bool isColourModifiable();

	///brief Method which set wether or not the colour of the object is modifiable.
	///param isColourModifiable true if the colour of the object is modifiable, false else.
	void setColourModifiable(bool isColourModifiable);

	///brief Method which return the colour of the object.
	///return colour of the object.
	const ColourValue& getColour();

	///brief Method which set the colour of the object.
	///param colour Colour to apply to the object.
	void setColour(const ColourValue& colour);

	///brief Method which reset the colour of the object, i. e. the colour is set to RGBA = 0.5 0.5 0.5 1
	void resetColour();



	///brief Method which return whether or not the texture of the BodyPartModel is modifiable.
	///return true is texture of the object is modifiable (i.e. if the number of textures of its collection is higher to 1), false else.
	bool isTextureModifiable();

	///brief Method which return a texture with a given name which belongs to the map of textures of the object.
	TexturePtr getTexture(const String& name);

	///brief Method which return a texture with a given number
	TexturePtr getTexture(const int n);

	///brief Method which return the current texture applied on the object.
	///return the current texture applied on the object.
	TexturePtr getCurrentTexture();

	///brief Method which return an Ogre iterator on the textures of the object.
	///return An Ogre iterator on the textures of the object.
	TextureVectorIterator getTextureIterator();

	///brief Method which navigate through the collection of textures of the object and set the previous texture.
	void setPreviousTexture();	

	///brief Method which navigate through the collection of textures of the object and set the previous texture as the current texture applied on the object.
	void setPreviousTextureAsCurrent();	

	///brief Method which navigate through the collection of textures of the object and set the next texture.
	void setNextTexture();

	///brief Method which navigate through the collection of textures of the object and set the next texture as the current texture applied on the object.
	void setNextTextureAsCurrent();

	///brief Method which set the given texture as the current texture of the object. The texture must belong to the textures map of the object.
	///param texture Texture to set as current texture.
	void setCurrentTexture(TexturePtr texture);

	///brief Method which set the given texture as the current texture of the object. The texture must belong to the textures map of the object.
	///param textureName Texture Name to set as current texture.
	void setCurrentTexture(const String& textureName);

	///brief Method which set the default texture as current.
	void setDefaultTextureAsCurrent();

	///brief Method which add a texture to the object, this texture will be choosable by the user who will be able to apply it on the object.
	///param texture Ogre texture to add
	///param textureExtParamsMap Texture extended parameters of added texture.
	void addTexture(TexturePtr texture, const TextureExtParamsMap& textureExtParamsMap = TextureExtParamsMap());

	///brief Method which delete the last texture to the object textures list
	void deleteLastTexture();

	///brief Method which delete a texture to the object textures list. If it is the current texture, changes to deflaut texture.
	///param texture Ogre texture to delete
	void deleteTexture(TexturePtr pTexture) ;

	///brief Method which return the number of textures to the object
	///return number of texture
	int getNbTexture();

	///biref Test a texture is already in the list of texture of this object.
	///param pTexture texture to test
	///return TRUE if the texture is present, and FALSE if it isn't present in the list of texture
	bool isPresentInList(TexturePtr pTexture) ;

	///brief Method which reset the modifications of the object, i. e. which reset the colour and set the default texture.
	void resetModifications();


    ///brief Method which return the texture extended parameters.
	///param pTexture texture
	///return the texture extended parameters.
	TextureExtParamsMap* getTextureExtParamsMap(TexturePtr pTexture);

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
	ModifiedMaterial* mModifiedMaterial;				///brief ModifiedMaterial associated to the object's material.
	TextureVector mTextures;							///brief Collection of possibles textures of the object.
	TextureNameExtParamsMap mTextureNameExtParamsMap;	///brief Map of extended parameters of textures.
    static MMMTextureManager* ms_MMMTextureManager;		///brief Texture manager singleton.
	TextureVector::iterator mDefaultTextureIterator;	///brief Iterator pointing on the default texture of the object (the first added in fact).
	TextureVector::iterator mCurrentTextureIterator;	///brief Iterator pointing on the texture which is currently applied on the object.

};

} //namespace

#endif