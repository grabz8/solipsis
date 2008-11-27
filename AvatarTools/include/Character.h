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

#ifndef __Character_h__
#define __Character_h__

#include <Ogre.h>
#include <tinyxml.h>
#include <FileBrowser.h>
#include <Path.h>
#include <myZipArchive.h>

#include "BodyPart.h"
#include "Goody.h"

/// brief This is the class representing a Character, containing informations about all its BodyParts and its BodyPartModels, Goodies, and other informations like his name.
/// file Character.h
/// author François FOURNEL
/// date 2007.06.25

using namespace Ogre;

namespace Solipsis {

class MyZipArchive;
class CharacterInstance;
class IFaceControllerCreator;

typedef std::map<String,BodyPart*> BodyPartsMap;
typedef MapIterator<BodyPartsMap> BodyPartsIterator;

typedef std::map<String,Goody*> GoodiesMap;
typedef MapIterator<GoodiesMap> GoodiesIterator;

typedef std::map<String,CharacterInstance*> CharInstancesMap;
typedef MapIterator<CharInstancesMap> CharInstancesIterator;

class Character
{
public:
	///Constructor, load the mesh of the Character, create an entity and a sceneNode associated to the Character and load all Goodies and BodyParts according to the nature xml file.
	Character(const String pName, SceneManager* pSceneMgr);
	///brief Destructor
	~Character();

	///brief Method which tell whether or not the loading of the character has been successful.
	///return true if the loading of the character has been successful, false else.
	bool isLoadingSuccessful();
	///brief Method which return the loading error message.
	///return the loading error message if the loading hasn't been successful, "" else.
	String getLoadingErrorMessage();

    ///brief Return the NATURE edition filename
    static String getEditionNatureFilename(const String& prefix);
    ///brief Return the CONF edition filename
    static String getEditionConfFilename(const String& prefix);
    ///brief Return the MESH edition filename
    static String getEditionMeshFilename(const String& prefix);

	///brief Add an instance of the character.
    void addInstance(const String& pUidString, CharacterInstance* pCharacterInstance);
	///brief Remove an instance of the character.
    void removeInstance(const String& pUidString);

	///brief Method that return the resource group of the Character.
	///return Resource group.
    const String& getResourceGroup() { return mResourceGroup; }
	///brief Method that return the mesh of the Character.
	///return Mesh.
    MeshPtr getMesh() { return mMesh; }
    Animation* getCustomizationAnimation() { return mCustomizationAnimation; }
    VertexAnimationTrack* getAnimationTrack() { return mAnimationTrack; };

	///brief Method that return the entity associated to the Character.
	///return entity associated with the Character.
    Entity* getEntity() { return mEntity; }
	///brief Method that return the pathname of the Character.
	///return Character's pathname.
    Path* getPath() { return mPath; }
	///brief Method that return the identifiant name of the Character (root of the .mesh file).
	///return Character's root.
    String getName() { return mName; }
	///brief Method that return the mesh name of the Character.
	///return Mesh name.
	String getMeshName() { return mMeshName; }
	///brief Method that return the skeleton name of the Character.
	///return Skeleton name.
	String getSkeletonName() { return mSkeletonName; }

	///brief Methohd which add a subMesh to a mesh and attach it to a bone. It does exactly the same thing as Entity::attachObjectToBone except here it modifies directly the mesh.
	///param mesh Mesh to add the submesh.
	///param boneName Name of the bone of the mesh to wich the new submesh must be attached.
	///param subMesh SubMesh to add to the mesh.
	///param subMeshName Name to give to the SubMesh.
	///param offsetPosition Offset position to add to the subMesh's position one time attached to the bone.
	///param offsetOrientation Offset orientation to set to the subMesh one time attached to the bone.
	void addSubMesh(const MeshPtr& mesh, const String& boneName, SubMesh* subMesh, const String& subMeshName, const Vector3 &offsetPosition = Vector3::ZERO, const Quaternion &offsetOrientation = Quaternion::IDENTITY);

	///brief Method which return the number of animations of the entity. Usefull in order to know if the character is animated (<=> getNumAnimation() > 0).
	///return The number of animations of the entity.
	unsigned short getNumAnimations();

    const BodyPartsMap& getBodyParts() { return mBodyParts; }
	///brief Method which return a BodyPart with a given name of the character.
	///param bodyPartName Name of the BodyPart.
	///return The BodyPart with the given name of NULL if this one doesn't exist. 
	BodyPart* getBodyPart(String bodyPartName);
	///brief Method which return the number of BodyParts of the character.
	///return The number of BodyParts of the character.
	size_t getNumBodyParts();
	///brief Method which gives an Ogre::MapIterator on all the BodyParts of the character.
	///return An Ogre::MapIterator on all the BodyParts of the character.
	BodyPartsIterator getBodyPartsIterator();
	///brief Method which add a BodyPart to the character, it could be the noze, the hands, the clothes...
	///param name Name of the BodyPart, name that will be displayed on the screen
	///param defaultBodyPartModelSubEntityName Ogre name of the subEntity of the default BodyPartModel representing the BodyPart. For example, if the BodyPart is the noze, this entity could be a clown's noze subEntity. The SubEntity must be a SubEntity of the character's Entity.
	///param defaultBodyPartModelName Complete name of default BodyPartModel of the BodyPart, name which is displayed on the screen. It can be "None" and then by default the Bodypart has no BodyPartModel (it means it is not present).
	void addBodyPart(String name, String defaultBodyPartModelSubEntityName, String defaultBodyPartModelName);

	///brief Method which gives an Ogre::MapIterator on all the Bones of the character.
	///return An Ogre::MapIterator on all the Bones of the character.
	Skeleton::BoneIterator getBonesIterator();

    const GoodiesMap& getGoodies() { return mGoodies; }
	///brief Method which return a Goody with a given name of the character.
	///param goodyName Name of the Goody.
	///return The Goody with the given name of NULL if this one doesn't exist. 
	Goody* getGoody(String goodyName);
	///brief Method which return the number of Goodies of the character.
	///return The number of Goodies of the character.
	size_t getNumGoodies();
	///brief Method which gives an Ogre::MapIterator on all the Goodies of the character.
	///return An Ogre::MapIterator on all the Goodies of the character.
	GoodiesIterator getGoodiesIterator();
	///brief Method which add a Goody to the character (Hat, Watch,...)
	///param name Name of the Goody, "Hat", "Watch" for example.
	///param boneName Name of the bone of the mesh of the character to which the Goody must be attached.
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

	/**
		@brief	gets access to the face controller creator associated with this character
	*/
	const IFaceControllerCreator*	getFaceControllerCreator( void ) const { return mFaceControllerCreator; }
	IFaceControllerCreator*	getFaceControllerCreator( void ) { return mFaceControllerCreator; }

protected:
	Path* mPath;										///brief Path informing us where is placed the character on the disks.
	MyZipArchive* mZipArchive;							///brief Zip archive containing our character.

	SceneManager* mSceneMgr;							///brief SceneManger of our scene.

    String mResourceGroup;

	String mName;										///brief Identifiant name of the character. It's the name of the folder/zip archive containing the character
	String mCompleteName;								///brief Complete name of the character.	
	String mSkeletonName;
	String mMeshName;

    MeshPtr mMesh;
	Animation* mCustomizationAnimation;
	VertexAnimationTrack* mAnimationTrack;

    Entity* mEntity;									///brief Ogre entity associated to the character.

	BodyPartsMap mBodyParts;							///brief Map of all the BodyParts of the character (For example "Head","Arms","Legs",...).
	GoodiesMap mGoodies;								///brief Map of all the Goodies of the character (For example "Watch","Hat"...).
	IFaceControllerCreator* mFaceControllerCreator;		///brief The creator that is used to create the face controller for this character's instances

	bool mLoadingSuccessful;							///brief Boolean telling whether or not te loading of the character has been successful.
	String mLoadingErrorMessage;						///brief Description of the problem which has avoided the character to load correctly.

    CharInstancesMap mCharInstancesMap;					///brief Map of all instances.
};

}

#endif
