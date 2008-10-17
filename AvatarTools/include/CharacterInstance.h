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

#ifndef __CharacterInstance_h__
#define __CharacterInstance_h__

#include <Ogre.h>
#include <tinyxml.h>
#include <FileBrowser.h>
#include <Path.h>
#include <myZipArchive.h>

#include "BodyPart.h"
#include "Goody.h"

/// brief This is the class representing a Character instance, containing informations about all its BodyParts and its BodyPartModels, Goodies, and other informations like his name.
/// file Character.h
/// author François FOURNEL
/// date 2007.06.25

using namespace Ogre;

namespace Solipsis {

class CharacterManager;
class Character;

typedef std::map<BodyPart*,BodyPartInstance*> BodyPartInstancesMap;
typedef std::map<Goody*,GoodyInstance*> GoodyInstancesMap;

class CharacterInstance
{
public:
	static const int SCALE_CHARACTER = 1;					///brief Scale applied to the character in order to adapt his size to the Camera and to avoid problems with a Near plane "too far" which will "cut" the character if this one is too small.
	static const int ANIMATION_MODE_NO_ANIMATION = 0;		///brief No animation mode, it is the mode where the character is immobile, set when the user clicks on the button "stop animation" of a character, it's also the constant animation mode for the unanimated characters.
	static const int ANIMATION_MODE_PERIODIC = 1;			///brief It is the periodic animation mode, set when the user clicks on the button "start animation". An animation is played (randomly choosed) during a given period and then a new animation is played randomly, as infinite.
	static const int ANIMATION_MODE_EDITION_AUTOMATIC = 2;	///brief It is the animation mode set when the user clicks on the button "Choose animation" and on the "Automatic" button radio is checked. In this case, a single character animation is playing as infinite, chosed with the radio buttons by the user.
	static const int ANIMATION_MODE_EDITION_MANUAL = 3;		///brief It is the animation mode set when the user clicks on the button "Choose animation" and on the "Manual" button radio is checked. In this case, a single character animation is selected and paused, the the user can forward or backward the animation thanks to a scrollbar.
	static const int ANIMATION_PERIOD = 5;					///brief Period (duration time, in seconds) of an animation in the ANIMATION_MODE_PERIODIC mode. Every period, the current animation is changed.

public:
	///Constructor, instantiate the mesh of the Character, create an entity and a sceneNode associated to the Character and load all Goodies and BodyParts according to the nature xml file.
	CharacterInstance(const String& pFileName, const String& pUid, const String& pDefaultCharacterName, SceneManager* pSceneMgr, CharacterManager* pCharacterMgr);
	///brief Destructor
	~CharacterInstance();

    SceneManager* getSceneMgr() { return mSceneMgr; }

	///brief Method which load the modified Character from the CharacterName.conf file. It will do nothing if this xml file contains errors.
	void loadModified();
	///brief Method which saves the current modified Character. The mesh is saved in the Character archive with the name "Characterrootname.mesh", ready to use fot the Solipsis application. The material is saved in the Character archive with the name "Characterrootname.material" (containing all the materials of all the submeshes).
	void saveModified();
	///brief Method which deletes the current modified Character.
	void deleteModified();

	///brief Method which add a pose reference. It directly call the same method of the CustomizationKeyFrame member of the class.
	///param poseIndex Index of the pose whose reference have to be added.
	///param influence New influence of the pose. Must be between 0 and 1.
	void addPoseReference(ushort poseIndex, Real influence);
	///brief Method which update the influence of a pose. 
	///param indexPose Index of the pose in the Mesh's character. This pose must be in the mIndexPosesCustomizationKeyFrame, it means the pose must belong to a CoupleOfStates of the character. 
	///param influence New influence of the pose. Must be between 0 and 1.
	void updatePoseReference(ushort indexPose,float influence);

    ///brief Method which give the current animation mode of the character.
	///return The current animation mode of the character.
	unsigned short getAnimationMode();
	///brief Method which modify the animation mode with the given value. If the character is unanimated, the animationMode will always be ANIMATION_MODE_NO_ANIMATION. (It doens't change the current animation).
	///param animationMode New animation mode, must be between 0 and 3.
	void setAnimationMode(unsigned short animationMode);
	///brief Method which return the current animation of the character (0 if unanimated).
	///return The current animation of the character (0 if unanimated).
	unsigned short getCurrentAnimation();
	///brief Method which modify the current animation
	///param idAnimation Index of the animation in the mesh, must be between 0 and getNumAnimations()-1, if the character is unanimated, the function will do nothing. 
	void setCurrentAnimation(unsigned short idAnimation);
	///brief Method which animate the character according to the elapsed time since the last call of the method. The current animation will be automatically changed every ANIMATION_PERIOD seconds if the current animation mode if the periodic one. this method won't do anything if the character is immobile or if the mode is ANIMATION_MODE_EDITION_MANUAL.
	///param elapsedTime time in seconds elapsed since the last frame.
	void animate(Real elapsedTime);
	///brief Method which return the current relative (between 0 and 1) position of the animation. For example, if we are currently at the half of the animation, the function will return 0.5. If the character isn't animated, will return 0.
	///return The current relative (between 0 and 1) position of the animation, or 0 if the character is unanimated.
	float getAnimationPosition();
	///brief Method which has only an effect in ANIMATION_MODE_EDITION_MANUAL animation mode. It will modify the position of the current animation, 0 means the beginning of the animation and 1 the end.
	///param position New position of the animation, must be between 0 and 1.
	void setAnimationPosition(Real position);

	///brief Reset all the modifications of the Character, i. e. all the modfications of all the BodyParts of the Character.
	void resetModifications();

    Character* getCharacter() { return mCharacter; }
    const String& getUid() { return mUid; }
    const String& getResourceGroup() { return mResourceGroup; }
	///brief Method that return the pathname of the Character.
	///return Character's pathname.
    Path* getUidPath() { return mUidPath; }
	///brief Method that return the mesh pointer associated with the Character.
	///return mesh pointer associated with the Character.
    MeshPtr getMesh() { return mMesh; }
	///brief Method that return the entity associated to the Character.
	///return entity associated with the Character.
	Entity* getEntity() { return mEntity; }
	///brief Method that return the scene node on which the Character is attached.
	///return scene node on which the Character is attached.
	SceneNode* getSceneNode() { return mSceneNode; }

	///brief Get the current BodyPart name
	///return The current BodyPart
	BodyPartInstance* getCurrentBodyPart();
	///brief Set the next BoyPart as Current
	///return The newly current BodyPart
	BodyPartInstance* setNextBodyPartAsCurrent();
	///brief Set the previous BoyPart as Current
	///return The newly current BodyPart
	BodyPartInstance* setPreviousBodyPartAsCurrent();

	///brief Get the current Bone name
	///return The current Bone
	Bone* getCurrentBone();
	///brief Set the next Bone as Current
	///return The newly current Bone
	Bone* setNextBoneAsCurrent();
	///brief Set the previous Bone as Current
	///return The newly current Bone
	Bone* setPreviousBoneAsCurrent();

	///brief Set the current Goody name
    void setCurrentGoody(Goody* goody);
	///brief Get the current Goody name
	///return The current Goody
	GoodyInstance* getCurrentGoody();
	///brief Set the next Goody as Current
	///return The newly current Goody
	GoodyInstance* setNextGoodyAsCurrent();
	///brief Set the previous Goody as Current
	///return The newly current Goody
	GoodyInstance* setPreviousGoodyAsCurrent();

protected:
    String mUid;										///brief UID string.
	Character* mCharacter;								///brief Character of our instance.

    CharacterManager* mCharacterManager;				///brief CharacterManager of our instance.
	SceneManager* mSceneMgr;							///brief SceneManager of our scene.

    Path* mUidPath;										///brief Path informing us where is placed the instance on the disks.
    MyZipArchive* mUidZipArchive;						///brief Zip archive containing our instance.

    String mResourceGroup;

    MeshPtr mMesh;
	Entity* mEntity;									///brief Ogre entity associated to the character.
	SceneNode* mSceneNode;								///brief Scene node on which the entity is attached.

    BodyPartInstancesMap mBodyPartInstances;
	BodyPartInstance* mBodyPartInstance;
    GoodyInstancesMap mGoodyInstances;
	GoodyInstance* mGoodyInstance;
	unsigned int mBone;
	
	unsigned short mAnimationMode;						///brief Current animation mode, see the possible animation modes (The four constants in the top of the class) for more informations.
	unsigned short mCurrentAnimation;					///brief Current animation index of the character (will be 0 if the character is unanimated).
	AnimationState* mCurrentAnimationState;				///brief Current animation state of the character, will be always NULL if the character is unanimated.

	AnimationState* mCustomizationAnimationState;		///brief Animation state used for the couples of poses. It contains all the infuences of all the poses of the couples of poses.
	VertexPoseKeyFrame* mCustomizationKeyFrame;			///brief VertexPoseKeyFrame used for the animation of the couples of poses.
	std::vector<int> mIndexPosesCustomizationKeyFrame;	///brief List of the indices of the poses of all the couples of poses used for the modification of the character.

	String mLoadingErrorMessage;						///brief Description of the problem which has avoided the character to load correctly.
};

}

#endif
