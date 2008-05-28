/*
		COPYRIGHTS (c) ARTEFACTO 
		
			!! Licence GPL !!	
*/

#ifndef __Character_h__
#define __Character_h__

#include <Ogre.h>
#include <tinyxml.h>
#include <FileBrowser.h>
#include <Path.h>
#include <myZipArchive.h>

#include "BodyPart.h"
#include "Goody.h"


/// brief This is the class representing an Character, containing informations about all its BodyParts and its BodyPartModels, Goodies, and other informations like his name.
/// file Character.h
/// author François FOURNEL
/// date 2007.06.25

using namespace Ogre;
namespace Solipsis {

typedef std::map<String,BodyPart*> BodyPartsMap ;
typedef MapIterator<BodyPartsMap> BodyPartsIterator;

typedef std::map<String,Goody*> GoodiesMap ;
typedef MapIterator<GoodiesMap> GoodiesIterator;


class Character
{
public:

	static const int SCALE_CHARACTER = 1;					///brief Scale applied to the Avatar in order to adapt his size to the Camera and to avoid problems with a Near plane "too far" which will "cut" the Avatar if this one is too small.
	static const int ANIMATION_MODE_NO_ANIMATION = 0;		///brief No animation mode, it is the mode where the avatar is immobile, set when the user clicks on the button "stop animation" of an avatar, it's also the constant animation mode for the unanimated avatars.
	static const int ANIMATION_MODE_PERIODIC = 1;			///brief It is the periodic animation mode, set when the user clicks on the button "start animation". An animation is played (randomly choosed) during a given period and then a new animation is played randomly, as infinite.
	static const int ANIMATION_MODE_EDITION_AUTOMATIC = 2;	///brief It is the animation mode set when the user clicks on the button "Choose animation" and on the "Automatic" button radio is checked. In this case, a single avatar animation is playing as infinite, chosed with the radio buttons by the user.
	static const int ANIMATION_MODE_EDITION_MANUAL = 3;		///brief It is the animation mode set when the user clicks on the button "Choose animation" and on the "Manual" button radio is checked. In this case, a single avatar animation is selected and paused, the the user can forward or backward the animation thanks to a scrollbar.
	static const int ANIMATION_PERIOD = 5;					///brief Period (duration time, in seconds) of an animation in the ANIMATION_MODE_PERIODIC mode. Every period, the current animation is changed.


	///Constructor, load the mesh of the Character, create an entity and a sceneNode associated to the Character and load all Goodies and BodyParts according to the nature xml file.
	Character(const String pFileName, String pName, SceneManager* pSceneMgr);
	///brief Destructor
	~Character();

	///brief Method which tell whether or not the loading of the avatar has been successful.
	///return true if the loading of the avatar has been successful, false else.
	bool isLoadingSuccessful();
	///brief Method which return the loading error message.
	///return the loading error message if the loading hasn't been successful, "" else.
	String getLoadingErrorMessage();

	///brief Method which load the modified Character from the CharacterName.conf file. It will do nothing if this xml file contains errors.
	void loadModified();
	///brief Method which saves the current modified Character. The mesh is saved in the Character archive with the name "Characterrootname.mesh", ready to use fot the Solipsis application. The material is saved in the Character archive with the name "Characterrootname.material" (containing all the materials of all the submeshes).
	void saveModified();

	///brief Methohd which add a subMesh to a mesh and attach it to a bone. It does exactly the same thing as Entity::attachObjectToBone except here it modifies directly the mesh.
	///param mesh Mesh to add the submesh.
	///param boneName Name of the bone of the mesh to wich the new submesh must be attached.
	///param subMesh SubMesh to add to the mesh.
	///param subMeshName Name to give to the SubMesh.
	///param offsetPosition Offset position to add to the subMesh's position one time attached to the bone.
	///param offsetOrientation Offset orientation to set to the subMesh one time attached to the bone.
	void addSubMesh(const MeshPtr& mesh, const String& boneName, SubMesh* subMesh, const String& subMeshName, const Vector3 &offsetPosition = Vector3::ZERO, const Quaternion &offsetOrientation = Quaternion::IDENTITY);

	///brief Method which add a pose reference. It directly call the same method of the CustomizationKeyFrame member of the class.
	///param poseIndex Index of the pose whose reference have to be added.
	///param influence New influence of the pose. Must be between 0 and 1.
	void addPoseReference(ushort poseIndex, Real influence);
	///brief Method which update the influence of a pose. 
	///param indexPose Index of the pose in the Mesh's Avatar. This pose must be in the mIndexPosesCustomizationKeyFrame, it means the pose must belong to a CoupleOfStates of the Avatar. 
	///param influence New influence of the pose. Must be between 0 and 1.
	void updatePoseReference(ushort indexPose,float influence);

	///hrief Method which return the number of animations of the entity. Usefull in order to know if the avatar is animated (<=> getNumAnimation() > 0).
	///return The number of animations of the entity.
	unsigned short getNumAnimations();
	///brief Method which give the current animation mode of the avatar.
	///return The current animation mode of the avatar.
	unsigned short getAnimationMode();
	///brief Method which modify the animation mode with the given value. If the avatar is unanimated, the animationMode will always be ANIMATION_MODE_NO_ANIMATION. (It doens't change the current animation).
	///param animationMode New animation mode, must be between 0 and 3.
	void setAnimationMode(unsigned short animationMode);
	///brief Method which return the current animation of the avatar (0 if unanimated).
	///return The current animation of the avatar (0 if unanimated).
	unsigned short getCurrentAnimation();
	///brief Method which modify the current animation
	///param idAnimation Index of the animation in the mesh, must be between 0 and getNumAnimations()-1, if the avatar is unanimated, the function will do nothing. 
	void setCurrentAnimation(unsigned short idAnimation);
	///brief Method which animate the avatar according to the elapsed time since the last call of the method. The current animation will be automatically changed every ANIMATION_PERIOD seconds if the current animation mode if the periodic one. this method won't do anything if the avatar is immobile or if the mode is ANIMATION_MODE_EDITION_MANUAL.
	///param elapsedTime time in seconds elapsed since the last frame.
	void animate(Real elapsedTime);
	///brief Method which return the current relative (between 0 and 1) position of the animation. For example, if we are currently at the half of the animation, the function will return 0.5. If the avatar isn't animated, will return 0.
	///return The current relative (between 0 and 1) position of the animation, or 0 if the avatar is unanimated.
	float getAnimationPosition();
	///brief Method which has only an effect in ANIMATION_MODE_EDITION_MANUAL animation mode. It will modify the position of the current animation, 0 means the beginning of the animation and 1 the end.
	///param position New position of the animation, must be between 0 and 1.
	void setAnimationPosition(Real position);

	///brief Reset all the modifications of the Character, i. e. all the modfications of all the BodyParts of the Character.
	void resetModifications();

	///brief Method that return the identifiant name of the Character (root of the .mesh file).
	///return Character's root.
	String getName();
	///brief Method that return the complete name of the Character, the one displayed on the left window.
	///return Character's name.
	String getCompleteName();
	String getMeshName() { return mMeshName; }
	String getSkeletonName() { return mSkeletonName; }
	///brief Method that return the node on which the Character is attached.
	///return node on which the Character is attached.
	SceneNode* getNode();
	///brief Method that return the entity associated to the Character.
	///return entity associated with the Character.
	Entity* getEntity();
	///brief Method that return the mesh pointer associated with the Character.
	///return mesh pointer associated with the Character.
	MeshPtr getMesh();

	///brief Method which return a BodyPart with a given name of the avatar.
	///param bodyPartName Name of the BodyPart.
	///return The BodyPart with the given name of NULL if this one doesn't exist. 
	BodyPart* getBodyPart(String bodyPartName);
	///brief Method which return the number of BodyParts of the Avatar.
	///return The number of BodyParts of the Avatar.
	size_t getNumBodyParts();
	///brief Method which gives an Ogre::MapIterator on all the BodyParts of the Avatar.
	///return An Ogre::MapIterator on all the BodyParts of the Avatar.
	BodyPartsIterator getBodyPartsIterator();
	///brief Get the current BodyPart name
	///return The current BodyPart
	BodyPart* getCurrentBodyPart();
	///brief Set the next BoyPart as Current
	///return The newly current BodyPart
	BodyPart* setNextBodyPartAsCurrent();
	///brief Set the previous BoyPart as Current
	///return The newly current BodyPart
	BodyPart* setPreviousBodyPartAsCurrent();
	///brief Method which add a BodyPart to the avatar, it could be the noze, the hands, the clothes...
	///param name Name of the BodyPart, name that will be displayed on the screen
	///param defaultBodyPartModelSubEntityName Ogre name of the subEntity of the default BodyPartModel representing the BodyPart. For example, if the BodyPart is the noze, this entity could be a clown's noze subEntity. The SubEntity must be a SubEntity of the avatar's Entity.
	///param defaultBodyPartModelName Complete name of default BodyPartModel of the BodyPart, name which is displayed on the screen. It can be "None" and then by default the Bodypart has no BodyPartModel (it means it is not present).
	void addBodyPart(String name, String defaultBodyPartModelSubEntityName, String defaultBodyPartModelName);

	///brief Method which gives an Ogre::MapIterator on all the Bones of the Avatar.
	///return An Ogre::MapIterator on all the Bones of the Avatar.
	Skeleton::BoneIterator getBonesIterator();
	///brief Get the current Bone name
	///return The current Bone
	Bone* getCurrentBone();
	///brief Set the next Bone as Current
	///return The newly current Bone
	Bone* setNextBoneAsCurrent();
	///brief Set the previous Bone as Current
	///return The newly current Bone
	Bone* setPreviousBoneAsCurrent();

	///brief Method which return a Goody with a given name of the avatar.
	///param goodyName Name of the Goody.
	///return The Goody with the given name of NULL if this one doesn't exist. 
	Goody* getGoody(String goodyName);
	///brief Method which return the number of Goodies of the Avatar.
	///return The number of Goodies of the Avatar.
	size_t getNumGoodies();
	///brief Method which gives an Ogre::MapIterator on all the Goodies of the Avatar.
	///return An Ogre::MapIterator on all the Goodies of the Avatar.
	GoodiesIterator getGoodiesIterator();
	///brief Get the current Goody name
	///return The current Goody
	Goody* getCurrentGoody();
	///brief Set the next Goody as Current
	///return The newly current Goody
	Goody* setNextGoodyAsCurrent();
	///brief Set the previous Goody as Current
	///return The newly current Goody
	Goody* setPreviousGoodyAsCurrent();
	///brief Method which add a Goody to the Avatar (Hat, Watch,...)
	///param name Name of the Goody, "Hat", "Watch" for example.
	///param boneName Name of the bone of the mesh of the avatar to which the Goody must be attached.
	///param minRotations Min rotation angles in degrees of the Goody from the bone to which the Goody is attached. x : Yaw angle, y : Pitch angle, z : Roll angle.
	///param defaultRotations Default rotation angles in degrees of the Goody from the bone to which the Goody is attached. x : Yaw angle, y : Pitch angle, z : Roll angle.
	///param maxRotations Max rotation angles in degrees of the Goody from the bone to which the Goody is attached. x : Yaw angle, y : Pitch angle, z : Roll angle.
	///param minPosition Min position of the Goody from the bone to which the Goody is attached.
	///param defaultPosition Default position of the Goody from the bone to which the Goody is attached.
	///param maxPosition Max position of the Goody from the bone to which the Goody is attached.
	void addGoody(String name,
		const String& boneName,
		const Vector3& minRotations, const Vector3& defaultRotations, const Vector3& maxRotations,
		const Vector3& minPosition, const Vector3& defaultPosition, const Vector3& maxPosition); 

private:
	SceneManager* mSceneMgr;							///brief SceneManger of our scene.

	Path* mPath;										///brief Path informing us where is placed the Avatar on the disks.
	MyZipArchive* mZipArchive;							///brief Zip archive containing our avatar.

	String mName;										///brief Identifiant name of the avatar. It's the name of the folder/zip archive containing the avatar
	String mCompleteName;								///brief Complete name of the avatar.	
	Entity* mEntity;									///brief Ogre entity associated to the avatar.
	SceneNode* mNode;									///brief Node on which the entity is attached.
//	Vector3 mGravityCenter;								///brief Gravity center of the avatar (barycenter of all his vertices).
//	CEGUI::Imageset* mImageSet;							///brief CEGUI::Imageset containing the image representing the avatar (used for the avatars window).
	String mSkeletonName;
	String mMeshName;
	MeshPtr mMesh;
	BodyPart* mBodyPart;
	Goody* mGoody;
	unsigned int mBone;

	BodyPartsMap mBodyParts;							///brief Map of all the BodyParts of the Avatar (For example "Head","Arms","Legs",...).
	GoodiesMap mGoodies;								///brief Map of all the Goodies of the Avatar (For example "Watch","Hat"...).
	
	unsigned short mAnimationMode;						///brief Current animation mode, see the possible animation modes (The four constants in the top of the class) for more informations.
	unsigned short mCurrentAnimation;					///brief Current animation index of the avatar (will be 0 if the avatar is unanimated).
	AnimationState* mCurrentAnimationState;				///brief Current animation state of the avatar, will be always NULL if the avatar is unanimated.
	
	AnimationState* mCustomizationAnimationState;		///brief Animation state used for the couples of poses. It contains all the infuences of all the poses of the couples of poses.
	VertexPoseKeyFrame* mCustomizationKeyFrame;			///brief VertexPoseKeyFrame used for the animation of the couples of poses.
	std::vector<int> mIndexPosesCustomizationKeyFrame;	///brief List of the indices of the poses of all the couples of poses used for the modification of the avatar.

	bool mLoadingSuccessful;							///brief Boolean telling whether or not te loading of the avatar has been successful.
	String mLoadingErrorMessage;						///brief Description of the problem which has avoided the avatar to load correctly.
};

}

#endif
