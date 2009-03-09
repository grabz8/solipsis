/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author ARTEFACTO - François FOURNEL

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

#ifndef __BodyPart_h__
#define __BodyPart_h__

#include <OgrePrerequisites.h>

#if (OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR < 6)
	#include <OgreNoMemoryMacros.h>
	#include <OgreMemoryMacros.h>
#endif
#include <Ogre.h>

#include "BodyPartModel.h"
#include "CoupleOfPoses.h"

using namespace Ogre;

namespace Solipsis {

class Character;
class CharacterInstance;

typedef std::map<String,CoupleOfPoses*> CouplesOfPosesMap;
typedef MapIterator<CouplesOfPosesMap> CouplesOfPosesMapIterator;

typedef std::map<String,BodyPartModel*> BodyPartModelsMap;
typedef MapIterator<BodyPartModelsMap> BodyPartModelsMapIterator;

class BodyPart;
class BodyPartInstance
{
public:
	///Constructor, instantiate the body part.
	BodyPartInstance(BodyPart* pBodyPart, CharacterInstance* owner);
	///brief Destructor
	~BodyPartInstance();

    BodyPart* getBodyPart() { return mBodyPart; };
    CharacterInstance* getOwner() { return mOwner; }

	///brief Method which tells whether or not the colour of the BodyPart is modifiable. Note that this entirely depends on the current BodyPartModel. It's important fo enabling or disabling colours scrollbars.
	///return true is the BodyPart's colour is modifiable, false otherwise (false too if there isn't default BodyPartModel).
	bool isColourModifiable();

	///brief Method which sets wether or not the colour of the BodyPart is modifiable. In fact, this only affects the current BodyPartModel, so this one must not be NULL.
	///param colourModifiable If true the BodyPart's colour will be modifiable, else not.
	void setColourModifiable(bool isColourModifiable);

	///brief Method which gives the BodyPart's colour. In fact, it is the current BodyPartModel's colour. So this one must not be NULL.
	///return The BodyPart's colour.
	const ColourValue& getColour();

	///brief Method which modify the BodyPart's colour. In fact, it is the current BodyPartModel's colour. So this one must not be NULL.
	///param colour The new BodyPart's colour.
	void setColour(const ColourValue& colour);



	///brief Method which tells whether or not the BodyPart's texture is modifiable (if we can change the texture). Note that this entirely depends on the current BodyPartModel. It's important fo enabling or disabling texture's buttons.
	///return true is the texture is modifiable, false otherwise (false too if there is no current BodyPartModel).
	bool isTextureModifiable();

	///brief Method which return the current texture of the current BodyPartModel. So, this one must not be NULL.
	///return the current texture of the current BodyPartModel.
	TexturePtr getCurrentTexture();

	///brief Method which navigates through the collection of textures of the BodyPart, more precisely the current BodyPartModel (So this one must be not be NULL), and set the previous texture as the current texture applied on the current BodyPartModel.
	void setPreviousTextureAsCurrent();	

	///brief Method which navigates through the collection of textures of the BodyPart, more precisely the current BodyPartModel (So this one must be not be NULL), and set the next texture as the current texture applied on the current BodyPartModel.
	void setNextTextureAsCurrent();

	///brief Method which adds a texture to the BodyPart, more precisely the current BodyPartModel (So this one must be not be NULL), this texture will be choosable by the user who will be able to apply it on the BodyPartModel.
	///param texture Ogre texture to add
	void addTexture(const TexturePtr& texture);



	///brief Method which returns the current BodyPartModel.
	///return the current BodyPartModel.
	BodyPartModel* getCurrentBodyPartModel();

	///brief Method which return the name of the current model of the bodypart. If there is no body part model, return "None"
	///return The name of the current body part model, or "None" if there isn't any body part model
	const String& getCurrentBodyPartModelName();

    BodyPartModelInstance* getCurrentBodyPartModelInstance() { return mCurrentBodyPartModelInstance; }

	///brief Method which set a BodyPartModel of a given name as current BodyPartModel. This BodyPartModel must be already present in the BodyPartModels map.
	void setCurrentBodyPartModel(const String& bodyPartModelName);

	///brief Method which set the previous BodyPartModel in the map as CurrentBodyPartModel.
	void setPreviousBodyPartModelAsCurrent();

	///brief Method which set the next BodyPartModel in the map as CurrentBodyPartModel.
	void setNextBodyPartModelAsCurrent();

	///brief Method which set the default BodyPartModel as CurrentBodyPartModel.
	void setDefaultBodyPartModelAsCurrent();



	///brief Method which reset all the couples of poses of the BodyPart i. e. which set all of their positions to 0.5.
	void resetCouplesOfPoses();

	///brief Method which reset all modifications of the BodyPart i. e. which reset all BodyPartModels, set the default BodyPartModel and reset all couples of poses.
	void resetModifications();

    /// brief Set the ghost attribute
    void setGhost(bool ghost);

private:
	BodyPart* mBodyPart;												///brief BodyPart of our instance.

	CharacterInstance* mOwner;											///brief Character to which the BodyPart belongs.

	BodyPartModelsMap::iterator mCurrentBodyPartModelIterator;			///brief Map iterator on the BodyPartModels pointing on the current BodyPartModel.
    BodyPartModelInstance* mCurrentBodyPartModelInstance;

    bool mGhost;														///brief Ghost flag.
};



/// brief This is the class representing a BodyPart of the character, the Body, Top Clothes or the noze for example. A BodyPart is composed by several BodyPartModel (at least one) and several couple of poses that the user can adjust to obtain a lot of possibles combinations.
/// file BodyPart.h
/// author François FOURNEL
/// date 2007.06.25


class BodyPart
{
    friend class BodyPartInstance;

public:
	///brief Constructor
	///param name Name of the BodyPart, "Noze", "Body" for example.
	///param defaultBodyPartModelSubEntityName Name of the character's mesh's SubEntity representing the default BodyPartModel of the BodyPart.
	///param defaultBodyPartModelCompleteName Complete name of the default BodyPartModel, "Clown's noze" for example.
	///param owner character to which the BodyPart belongs.
	BodyPart(const String& name, const String& defaultBodyPartModelSubEntityName, const String& defaultBodyPartModelCompleteName, Character* owner);
	///brief Destructor
	~BodyPart();



	///brief Method which gives the name of the BodyPart.
	///return The name of the BodyPart.
	const String& getName();

	///brief Method which gives the character to which the BodyPart belongs.
	///return The character to which the BodyPart belongs.
	Character* getOwner();


	///brief Method which tells whether or not the BodyPartModel is modifiable, it means whether or not we can change of BodyPartModel for the BodyPart. In other words, it tells whether or not the number of BodyPartModels is higher to 1. It's important for enabling or disabling BodyPartModel's buttons
	///return true if the BodyPartModel is modifiable, false otherwise
	bool isBodyPartModelModifiable();

	///brief Method which returns the default BodyPartModel of the BodyPart
	///return The default BodyPartModel of the BodyPart
	BodyPartModel* getDefaultBodyPartModel();

	///brief Method which returns a given BodyPartModel of the BodyPart
	///param name Name of the BodyPartModel
	///return The BodyPartModel of the BodyPart whose name is the param "name"
	BodyPartModel* getBodyPartModel(const String& name);

	///brief Method witch returns an Ogre MapIterator on the BodyPartModels of the BodyPart.
	///return An Ogre MapIterator on the BodyPartModels of the BodyPart.
	BodyPartModelsMapIterator getBodyPartModelsMapIterator();

	///brief Method which modify the default BodyPartModel.
	///param bodyPartModelName Name of the new BodyPartModel. This BodyPartModel must be present in the BodyPartModels (even if bodyPartModelName is "none").
	void setDefaultBodyPartModel(const String& bodyPartModelName);

	///brief Method which add a BodyPartModel to the BodyPart. The new BodyPartModel is built according to the given Ogre SubEntity name and to the given BodyPartModel's complete name.
	///param subEntityName Ogre SubEntity name which must be present in the mesh's character. However, it could be "none" and so by default, the SubEntity has no default BodyPartModel.
	///param name Complete name of the BodyPartModel. No BodyPartModel with the same name must be present in the BodyPartModels map.
	void addBodyPartModel(const String& subEntityName,const String& name);

	///brief Method which gives the CoupleOfPoses with a given name.
	///param name Name of the CoupleOfPoses that sould be returned.
	///return The CoupleOfPoses with the given name, or NULL if not found.
	CoupleOfPoses* getCoupleOfPoses(const String& name);

	///brief Method witch returns an Ogre MapIterator on the CouplesOfPoses of the BodyPart.
	///return An Ogre MapIterator on the CouplesOfPoses of the BodyPart.
	CouplesOfPosesMapIterator getCouplesOfPosesMapIterator();

	///brief Method which add a couple of poses to the bodypart.
	///param name Complete name of the Couple of Poses, it could be "Noze'size" for example.
	///param leftPoseIndex Index of the left pose in the mesh, left pose is the pose for the left state.
	///param leftPoseCameraCylindricCoordinates Cylindric coordinates of the camera when rendering the character's left pose.The character's mesh is put in the scene without any translation, scale, rotation and the camera is placed with the leftPoseCameraCylindricCoordinates = Vector3(x,y,z). x is taken as the radius, y as the angle, z as the height of the camera.
	///param leftStateName Complete name of the left state. It could be "Thin noze" for example.
	///param rightPoseIndex Index of the right pose in the mesh, right pose is the pose for the right state.
	///param rightPoseCameraCylindricCoordinates Cylindric coordinates of the camera when rendering the character's right pose.The character's mesh is put in the scene without any translation, scale, rotation and the camera is placed with the rightPoseCameraCylindricCoordinates = Vector3(x,y,z). x is taken as the radius, y as the angle, z as the height of the camera.
	///param rightStateName Complete name of the right state. It could be "Large noze" for example.
	void addCoupleOfPoses(const String& name, ushort leftPoseIndex, const Vector3& leftPoseCameraCylindricCoordinates, const String& leftStateName, ushort rightPoseIndex, const Vector3& rightPoseCameraCylindricCoordinates, const String& rightStateName);
	
	///brief Method which add a couple of poses to the bodypart.
	///param name Complete name of the Couple of poses, it could be "Noze'size" for example.
	///param leftPoseName Name of the left pose in the mesh, left pose is the pose for the left state.
	///param leftPoseCameraCylindricCoordinates Cylindric coordinates of the camera when rendering the character's left pose.The character's mesh is put in the scene without any translation, scale, rotation and the camera is placed with the leftPoseCameraCylindricCoordinates = Vector3(x,y,z). x is taken as the radius, y as the angle, z as the height of the camera.
	///param leftStateName Complete name of the left state. It could be "Thin noze" for example.
	///param rightPoseName Name of the right pose in the mesh, right pose is the pose for the right state.
	///param rightPoseCameraCylindricCoordinates Cylindric coordinates of the camera when rendering the character's right pose.The character's mesh is put in the scene without any translation, scale, rotation and the camera is placed with the rightPoseCameraCylindricCoordinates = Vector3(x,y,z). x is taken as the radius, y as the angle, z as the height of the camera.
	///param rightStateName Complete name of the right state. It could be "Large noze" for example.
	void addCoupleOfPoses(const String& name, const String& leftPoseName, const Vector3& leftPoseCameraCylindricCoordinates, const String& leftStateName, const String& rightPoseName, const Vector3& rightPoseCameraCylindricCoordinates, const String& rightStateName);
	
private:
	String mName;														///brief Name of the BodyPart, "Noze", or "Body" for example.

	Character* mOwner;													///brief character to which the BodyPart belongs.

	CouplesOfPosesMap mCouplesOfPoses;									///brief Map containing all the CouplesOfPoses of the BodyPart.

	BodyPartModelsMap mBodyPartModels;									///brief Map containing all the BodyPartModels of the BodyPart.
	BodyPartModelsMap::iterator mDefaultBodyPartModelIterator;			///brief Map iterator on the BodyPartModels pointing on the default BodyPartModel of the BodyPart (the first found in the character .nature xml file).	
	bool mCanHaveNoBodyPartModel;										///brief Boolean telling wether or not the BodyPart can have no BodyPart, it means if the BodyPart can be "removed" from the character.
};

}

#endif

