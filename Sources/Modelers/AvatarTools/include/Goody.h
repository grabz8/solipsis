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

/*
		COPYRIGHTS (c) ARTEFACTO 
		
			!! Licence GPL !!	
*/

#ifndef __Goody_h__
#define __Goody_h__

#include <OgrePrerequisites.h>

#if (OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR < 6)
	#include <OgreNoMemoryMacros.h>
	#include <OgreMemoryMacros.h>
#endif
#include <Ogre.h>

#include "GoodyModel.h"

using namespace Ogre;

namespace Solipsis {

class Character;
class CharacterInstance;

typedef std::map<String,GoodyModel*> GoodyModelsMap;
typedef MapIterator<GoodyModelsMap> GoodyModelsMapIterator;

class Goody;
class GoodyInstance
{
public:
	///Constructor, instantiate the goody.
	GoodyInstance(Goody* pGoody, CharacterInstance* owner);
	///brief Destructor
	~GoodyInstance();

    Goody* getGoody() { return mGoody; };
    CharacterInstance* getOwner() { return mOwner; }

	///brief Method which tells whether or not the colour of the Goody is modifiable. Note that this entirely depends on the current GoodyModel. It's important fo enabling or disabling colours scrollbars.
	///return true is the Goody's colour is modifiable, false otherwise (false too if there isn't default GoodyModel).
	bool isColourModifiable();

	///brief Method which sets wether or not the colour of the Goody is modifiable. In fact, this only affects the current GoodyModel, so this one must not be NULL.
	///param colourModifiable If true the Goody's colour will be modifiable, else not.
	void setColourModifiable(bool isColourModifiable);

	///brief Method which gives the Goody's colour. In fact, it is the current GoodyModel's colour. So this one must not be NULL.
	///return The Goody's colour.
	const ColourValue& getColour();

	///brief Method which modify the Goody's colour. In fact, it is the current GoodyModel's colour. So this one must not be NULL.
	///param colour The new Goody's colour.
	void setColour(const ColourValue& colour);



	///brief Method which tells whether or not the Goody's texture is modifiable (if we can change the texture). Note that this entirely depends on the current GoodyModel. It's important fo enabling or disabling texture's buttons.
	///return true is the texture is modifiable, false otherwise (false too if there is no current GoodyModel).
	bool isTextureModifiable();

	///brief Method which return the current texture of the current GoodyModel. So, this one must not be NULL.
	///return the current texture of the current GoodyModel.
	TexturePtr getCurrentTexture();

	///brief Method which navigates through the collection of textures of the Goody, more precisely the current GoodyModel (So this one must be not be NULL), and set the previous texture as the current texture applied on the current GoodyModel.
	void setPreviousTextureAsCurrent();	

	///brief Method which navigates through the collection of textures of the Goody, more precisely the current GoodyModel (So this one must be not be NULL), and set the next texture as the current texture applied on the current GoodyModel.
	void setNextTextureAsCurrent();

	///brief Method which adds a texture to the Goody, more precisely the current GoodyModel (So this one must be not be NULL), this texture will be choosable by the user who will be able to apply it on the GoodyModel.
	///param texture Ogre texture to add
	void addTexture(const TexturePtr& texture);



	///brief Method which update the position at the screen of the Goody according to the concerned character's bone and the current position and rotation. Goody must have a current GoodyModel (different from "None" GoodyModel).
	void updateGoodyPosition();

	///brief Method which return the current position of the Goody, calculated according to the current positions scroll positions.
	///return the current position of the Goody.
	const Vector3& getCurrentPosition();

	///brief Method which return the current rotations angles of the Goody, calculated according to the current rotations angles scroll positions.
	///return the current rotations angles of the Goody.
	const Vector3& getCurrentRotationsAngles();

	///brief Method which return the current orientation (quaternion) of the Goody according to the bone.
	///return The current orientation (quaternion) of the Goody.
	const Quaternion& getCurrentOrientation();

	///brief Method which return the current scroll position of the x component of the goody's position.
	///return The current scroll position of the x component of the goody's position.
	float getCurrentXScrollPosition();

	///brief Method which return the current scroll position of the y component of the goody's position.
	///return The current scroll position of the y component of the goody's position.
	float getCurrentYScrollPosition();

	///brief Method which return the current scroll position of the z component of the goody's position.
	///return The current scroll position of the z component of the goody's position.
	float getCurrentZScrollPosition();

	///brief Method which return the current scroll position of the yaw angle of the goody's rotation.
	///return The current scroll position of the yaw angle of the goody's rotation.
	float getCurrentYawAngleScrollPosition();

	///brief Method which return the current scroll position of the pitch angle of the goody's rotation.
	///return The current scroll position of the pitch angle of the goody's rotation.
	float getCurrentPitchAngleScrollPosition();

	///brief Method which return the current scroll position of the roll angle of the goody's rotation.
	///return The current scroll position of the roll angle of the goody's rotation.
	float getCurrentRollAngleScrollPosition();

	///brief Method which modifies the x component of the current position of the Goody, and replace as a consequence the GoodyModel (if different from "None").
	///param position If 0 the x position will be the minimum x position, if 0.5 it will be the default x position, if 1, it will be the max x position. All the other x values are calculated by linear interpolation of these 3 values. position must be between 0 and 1. 
	void setCurrentXScrollPosition(float position);

	///brief Method which modifies the y component of the current position of the Goody, and replace as a consequence the GoodyModel (if different from "None").
	///param position If 0 the y position will be the minimum y position, if 0.5 it will be the default y position, if 1, it will be the max yx position. All the other y values are calculated by linear interpolation of these 3 values. position must be between 0 and 1. 
	void setCurrentYScrollPosition(float position);

	///brief Method which modifies the z component of the current position of the Goody, and replace as a consequence the GoodyModel (if different from "None").
	///param position If 0 the z position will be the minimum z position, if 0.5 it will be the default z position, if 1, it will be the max z position. All the other z values are calculated by linear interpolation of these 3 values. position must be between 0 and 1. 
	void setCurrentZScrollPosition(float position);

	///brief Method which reset the current position, it means set the current position as the default position. in other words it set the mCurrentPositionScrollPositions at (0.5,0.5,0.5)
	void resetCurrentPosition();

	///brief Method which modifies the yaw current rotation angle of the Goody.
	///param position If 0 the current yaw angle will be the minimum yaw angle, if 0.5 it will be the default yaw angle, if 1, it will be the max yaw angle. All the other angles values are calculated by linear interpolation of these 3 values. position must be between 0 and 1. 
	void setCurrentYawAngleScrollPosition(float position);

	///brief Method which modifies the pitch current rotation angle of the Goody.
	///param position If 0 the current pitch angle will be the minimum yaw angle, if 0.5 it will be the default pitch angle, if 1, it will be the max pitch angle. All the other angles values are calculated by linear interpolation of these 3 values. position must be between 0 and 1. 
	void setCurrentPitchAngleScrollPosition(float position);

	///brief Method which modifies the roll current rotation angle of the Goody.
	///param position If 0 the current roll angle will be the minimum roll angle, if 0.5 it will be the default roll angle, if 1, it will be the max roll angle. All the other angles values are calculated by linear interpolation of these 3 values. position must be between 0 and 1. 
	void setCurrentRollAngleScrollPosition(float position);

	///brief Method which reset the current rotations, it means set the current rotations angles as the default rotations angles. in other words it set the mCurrentRotationsAnglesScrollPositions at (0.5,0.5,0.5)
	void resetCurrentRotation();



	///brief Method which return the current GoodyModel.
	///return the current GoodyModel.
	GoodyModel* getCurrentGoodyModel();

	///brief Method which return the name of the current model of the Goody. If there is no body part model, return "None"
	///return The name of the current body part model, or "None" if there isn't any body part model
	const String& getCurrentGoodyModelName();

    GoodyModelInstance* getCurrentGoodyModelInstance() { return mCurrentGoodyModelInstance; }


	///brief Method which set a GoodyModel of a given name as current GoodyModel. This GoodyModel must be already present in the GoodyModels map.
	///param goodyModelName Name of the GoodyModel to set as current one.
	void setCurrentGoodyModel(const String& goodyModelName);

	///brief Method which set the previous GoodyModel in the map as CurrentGoodyModel.
	void setPreviousGoodyModelAsCurrent();

	///brief Method which set the next GoodyModel in the map as CurrentGoodyModel.
	void setNextGoodyModelAsCurrent();

	///brief Method which set the default GoodyModel as CurrentGoodyModel.
	void setDefaultGoodyModelAsCurrent();



	///brief Method which reset all modifications of the Goody i. e. which reset all GoodyModels, set the default GoodyModel.
	void resetModifications();

    /// brief Set the ghost attribute
    void setGhost(bool ghost);

private:
	Goody* mGoody;														///brief Goody of our instance.

	CharacterInstance* mOwner;											///brief Character to which the Goody belongs.

    GoodyModelsMap::const_iterator mCurrentGoodyModelIterator;			///brief Map iterator on the GoodyModels pointing on the current GoodyModel.
    GoodyModelInstance* mCurrentGoodyModelInstance;

	Vector3 mCurrentRotationsAnglesScrollPositions;						///brief Current rotations angles. In fact it is the 3 scroll positions, between 0 and 1, representing the current yaw, pitch and roll angles values. Each component of this vector is between 0 and 1 : if it is 0 the corresponding rotation angle (x component : yaw angle, y : pitch,  z: roll) is the minimum angle (taken in the vector mMinRotationsAngles). If it is 0.5 the angle is the default angle (mDefaultRotationsAngles) and if it's 1 the angle is the max angle (mMaxRotationsAngles). All the others angles values are calculated by a linear interpolation.		

	Vector3 mCurrentPositionScrollPositions;							///brief Current position of the GoodyModel. In fact it is the 3 scroll positions, between 0 and 1, representing the current x, y, z values of the current position. Each component of this vector is between 0 and 1 : if it is 0 the corresponding position component is the minimum x position (taken in the vector mMinPositions). If it is 0.5 the position component is the default position (mDefaultPositions) and if it's 1 the position component is the max positions (mMaxPosition). All the others position component values are calculated by a linear interpolation.		

    bool mGhost;														///brief Ghost flag.
};

/// brief This is the class representing a goody of an character. A goody is an accessory, which will be attached to a bone of the mesh of the character. The goody will be represented by any of its GoodyModel (It has a collection of GoodyModels). For example, we can have a Goody named "Watch" and GoodyModel "Rolex","Swatch"...  The user will be able to move this Goody and orientate as he wants. He will also able to change his colour, if the xml nature file allows it, which will be taken as the colour of the submesh 0 of the mesh of the current GoodyModel used to represent the Goody. Same thing for the texture.
/// file Goody.h
/// author François FOURNEL
/// date 2007.06.25

class Goody
{
    friend class GoodyInstance;

public:
	///brief Constructor; by default, there is no true GoodyModel, it means the default GoodyModel is none.
	///param name Name of the Goody, "Hat", "Watch" for example.
	///param boneName Name of the bone of the mesh of the character to which the Goody must be attached.
	///param minRotationsAngles Min rotation angles in degrees of the Goody from the bone to which the Goody is attached. x : Yaw angle, y : Pitch angle, z : Roll angle.
	///param defaultRotationsAngles Default rotation angles in degrees of the Goody from the bone to which the Goody is attached. x : Yaw angle, y : Pitch angle, z : Roll angle.
	///param maxRotationsAngles Max rotation angles in degrees of the Goody from the bone to which the Goody is attached. x : Yaw angle, y : Pitch angle, z : Roll angle.
	///param minPosition Min position of the Goody from the bone to which the Goody is attached.
	///param defaultPosition Default position of the Goody from the bone to which the Goody is attached.
	///param maxPosition Max position of the Goody from the bone to which the Goody is attached.
	///param owner character to which the Goody belongs.
	Goody(const String& name,
		const String& boneName,
		const Vector3& minRotationsAngles, const Vector3& defaultRotationsAngles, const Vector3& maxRotationsAngles,
		const Vector3& minPosition, const Vector3& defaultPosition, const Vector3& maxPosition,
		Character* owner); 
	///brief Destructor
	~Goody();

	///brief Method which gives the name of the Goody.
	///return The name of the Goody	
	const String& getName();
	
	///brief Method which gives the character to which the Goody belongs.
	///return The character to which the Goody belongs.
	Character* getOwner();



	///brief Method which return the name of the bone of the character to which the Goody is attached.
	///return The name of the bone of the character to which the Goody is attached.
	const String& getBoneName();

	///brief Method which tell whether or not the GoodyModel is modifiable, it means whether or not we can change of GoodyModel for the Goody. In other words, it tells whether or not the number of GoodyModels is higher to 1. It's important for enabling or disabling GoodyModel's buttons
	///return true if the GoodyModel is modifiable, false otherwise
	bool isGoodyModelModifiable();

	///brief Method which returns the default GoodyModel of the Goody
	///return The default GoodyModel of the Goody
	GoodyModel* getDefaultGoodyModel();

	///brief Method which returns a given GoodyModel of the Goody
	///param name Name of the GoodyModel
	///return The GoodyModel of the Goody whose name is the param "name"
	GoodyModel* getGoodyModel(const String& name);

	///brief Method witch returns an Ogre MapIterator on the GoodyModels of the Goody.
	///return An Ogre MapIterator on the GoodyModels of the Goody.
	GoodyModelsMapIterator getGoodyModelsMapIterator();

	///brief Method which modify the default GoodyModel.
	///param GoodyModelName Name of the new GoodyModel. This GoodyModel must be present in the GoodyModels (even if GoodyModelName is "none").
	void setDefaultGoodyModel(const String& GoodyModelName);

	///brief Method which add a GoodyModel to the Goody. The mesh (and so the Entity) used to build the GoodyModel is loaded according to the name of the mesh passed in parameter (loaded via createEntity). The GoodyModel is then built according to this entity and a complete name passed in parameter.
	///param meshName Name of the mesh loaded to build the new GoodyModel.
	///param name Complete name of the GoodyModel. No GoodyModel with the same name must be present in the GoodyModels map. It's also the name of the entity created.
	///param sceneMgr SceneManager used to create the entity associated to the goody model.
	void addGoodyModel(const String& meshName,const String& name, SceneManager* sceneMgr);


	

private:

	String mName;														///brief Name of the Goody, "Hat", or "Watch" for example.

	Character* mOwner;													///brief Character to which the Goody belongs.

	GoodyModelsMap mGoodyModels;										///brief Map containing all the GoodyModels of the Goody.
	GoodyModelsMap::iterator mDefaultGoodyModelIterator;				///brief Map iterator on the GoodyModels pointing on the default GoodyModel of the Goody (the first found in the character .nature xml file).	

	String mBoneName;													///brief Bone of the mesh of the character to which the Goody must be attached.

	Vector3 mMinRotationsAngles;										///brief Min rotation angles in degrees of the Goody from the bone to which the Goody is attached. x : Yaw angle, y : Pitch angle, z : Roll angle.
	Vector3 mDefaultRotationsAngles;									///brief Default rotation angles in degrees of the Goody from the bone to which the Goody is attached. x : Yaw angle, y : Pitch angle, z : Roll angle.
	Vector3 mMaxRotationsAngles;										///brief Max rotation angles in degrees of the Goody from the bone to which the Goody is attached. x : Yaw angle, y : Pitch angle, z : Roll angle.

	Vector3 mMinPosition;												///brief Min position of the Goody from the bone to which the Goody is attached.
	Vector3 mDefaultPosition;											///brief Default position of the Goody from the bone to which the Goody is attached.
	Vector3 mMaxPosition;												///brief Max position of the Goody from the bone to which the Goody is attached.
};

}

#endif
