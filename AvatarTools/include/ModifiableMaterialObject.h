#ifndef __ModifiableMaterialObject_h__
#define __ModifiableMaterialObject_h__

#include <OgreNoMemoryMacros.h>
#include <OgreMemoryMacros.h>
#include <Ogre.h>

#include "TextureIterator.h"
#include "ModifiedMaterial.h"


using namespace Ogre;
namespace Solipsis {

/// brief This is the class of a ModifiableMaterialObject. A ModifiableMaterialObject is an object (a body part model, goody model) whose material is modifiable. Especially, his colour (added colour) and his texture can be changed if this is allowed.
/// file BodyPartModel.h
/// author François FOURNEL
/// date 2007.07.17

class ModifiableMaterialObject
{
public:

	///brief Destructor
	~ModifiableMaterialObject();
	///brief Destructor
	ModifiableMaterialObject();

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

	///brief Method which return the current texture applied on the object.
	///return the current texture applied on the object.
	TexturePtr getCurrentTexture();

	///brief Method which return an Ogre iterator on the textures of the object.
	///return An Ogre iterator on the textures of the object.
	TextureVectorIterator getTextureIterator();

	///brief Method which navigate through the collection of textures of the object and set the previous texture as the current texture applied on the object.
	void setPreviousTextureAsCurrent();	

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
	void addTexture(TexturePtr texture);



	///brief Method which reset the modifications of the object, i. e. which reset the colour and set the default texture.
	void resetModifications();


private:
	ModifiedMaterial* mModifiedMaterial;				///brief ModifiedMaterial associated to the object's material.
	TextureVector mTextures;							///brief Collection of possibles textures of the object.
	TextureVector::iterator mDefaultTextureIterator;	///brief Iterator pointing on the default texture of the object (the first added in fact).
	TextureVector::iterator mCurrentTextureIterator;	///brief Iterator pointing on the texture which is currently applied on the object.
};

}

#endif
