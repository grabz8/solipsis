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

#include <FileBuffer.h>
#include <SolipsisErrorHandler.h>
#include "CharacterInstance.h"
#include "CharacterManager.h"
#include "Character.h"
#include "IFaceControllerCreator.h"
#include "IFaceController.h"
#include <VoiceEngineManager.h>

using namespace Solipsis;

//#define ACTIVATE_FACE_CONTROLLER

//---------------------------------------------------------------------------------
CharacterInstance::CharacterInstance(const String& pFileName, const String& pUid, const String& pDefaultCharacterName, SceneManager* pSceneMgr, CharacterManager* pCharacterMgr) :
    mUid(pUid),
    mCharacter(0),
	mSceneMgr(pSceneMgr),
    mCharacterManager(pCharacterMgr),
    mUidPath(NULL),
    mUidZipArchive(NULL),
	mEntity(NULL),
	mSceneNode(NULL),
	mFaceController(NULL),
	mCurrentAnimationState(NULL),
	mCustomizationAnimationState(NULL),
 	mCustomizationKeyFrame(NULL),
	mBodyPartInstance(0),
	mGoodyInstance(0),
	mBone(0)
{
    // Create the instance zip file
    mUidPath = new Path(pFileName);
    mUidZipArchive = new MyZipArchive(mUidPath->getUniversalPath());

    // CONF file exist ?
	if (!mUidZipArchive->isFilePresent(Character::getEditionConfFilename(mUid))) 
    {
        // Load the default character
        mCharacter = pCharacterMgr->loadCharacter(pDefaultCharacterName);
    }
    else
    {
        // Parse CONF xml file
	    FileBuffer xmlConfFile = mUidZipArchive->readFile(Character::getEditionConfFilename(mUid));
	    TiXmlDocument xmlDoc;
	    xmlDoc.Parse(xmlConfFile.getBufferFormatedToText().c_str());
	    if (xmlDoc.Error()) 
	    {
		    mLoadingErrorMessage = Character::getEditionConfFilename(mUid) + "\nisn't a valid xml file !"; 
		    return;
	    }
	    TiXmlElement* characterElement = xmlDoc.FirstChildElement("Character");
	    if (characterElement == NULL)
	    {
		    mLoadingErrorMessage = Character::getEditionConfFilename(mUid) + " :" + "\nNo Character markup found !"; 
		    return;
	    }
	    const char* characterName = characterElement->Attribute("name");
	    if (characterName == NULL)
	    {
		    mLoadingErrorMessage = Character::getEditionConfFilename(mUid) + " :" + "\nNo name's attribute found !"; 
		    return;
	    }
        // Load the character
        if (mCharacter == 0)
            mCharacter = pCharacterMgr->loadCharacter(characterName);
    }
    mCharacter->addInstance(pUid, this);
    mMesh = mCharacter->getMesh();

    // Add instance zip file as resource location
    mResourceGroup = mUid + "Resources";
	ResourceGroupManager::getSingleton().createResourceGroup(mResourceGroup);
    if (mUidZipArchive->isArchivePresent())
    {
	    ResourceGroupManager::getSingleton().addResourceLocation(mUidPath->getUniversalPath(), "Zip", mResourceGroup);
        ResourceGroupManager::getSingleton().initialiseResourceGroup(mResourceGroup);
    }

	//Creating nodes and entities
	mSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(mUid);

    mEntity = mCharacter->getEntity()->clone(mUid);

	mSceneNode->attachObject(mEntity);
	//mSceneNode->scale(SCALE_CHARACTER,SCALE_CHARACTER,SCALE_CHARACTER);

    //Creating the body parts instances
	BodyPartsIterator bodyPartsIterator = mCharacter->getBodyPartsIterator();
	while(bodyPartsIterator.hasMoreElements())
	{
		//Creating BodyPartModel
		BodyPart* bodyPart = bodyPartsIterator.getNext();
        BodyPartInstance* bodyPartInst = new BodyPartInstance(bodyPart, this);
        mBodyPartInstances[bodyPart] = bodyPartInst;
        bodyPartInst->setDefaultBodyPartModelAsCurrent();
    }

	// create the face controller
	#ifdef ACTIVATE_FACE_CONTROLLER
		IFaceControllerCreator* pFaceControllerCreator = mCharacter->getFaceControllerCreator();
		if(pFaceControllerCreator)
		{
			mFaceController = pFaceControllerCreator->createFaceController( this );
		}
	#endif

	//Creating the object necessary to the Couples of poses.
    mCustomizationKeyFrame = mCharacter->getAnimationTrack()->createVertexPoseKeyFrame(0);

	//Initialising the animation state relative to the couple of poses.
	mCustomizationAnimationState = mEntity->getAnimationState(mCharacter->getCustomizationAnimation()->getName());
	mCustomizationAnimationState->setTimePosition(0);
	mCustomizationAnimationState->setEnabled(true);


	//calculateGravityCenter();


	//Setting the animation.
	if (mCharacter->getNumAnimations() > 0)
		setAnimationMode(ANIMATION_MODE_PERIODIC);
	else
		setAnimationMode(ANIMATION_MODE_NO_ANIMATION);

	//loading the configuration saved in a precedent execution of the program
	loadModified();

#if 0
	//re-save the mesh ready to use for the solipsis application.
	saveModified();
#endif

	// register ourselves to receive voice packets
	VoiceEngineManager::getSingleton().getSelectedEngine()->addVoicePacketListener(mUid, this);
}

//---------------------------------------------------------------------------------
CharacterInstance::~CharacterInstance()
{
	// unregister ourselves to no longer receive voice packets
	VoiceEngineManager::getSingleton().getSelectedEngine()->removeVoicePacketListener(mUid);

	mCharacter->removeInstance(mUid);

    // Destroy goodies instances
    for(GoodyInstancesMap::iterator it = mGoodyInstances.begin(); it != mGoodyInstances.end(); it = mGoodyInstances.begin())
    {
        delete it->second;
        mGoodyInstances.erase(it);
    }
    // Destroy body parts instances
    for(BodyPartInstancesMap::iterator it = mBodyPartInstances.begin(); it != mBodyPartInstances.end(); it = mBodyPartInstances.begin())
    {
        delete it->second;
        mBodyPartInstances.erase(it);
    }

	// destroy the face controller
	if(mFaceController)
	{
		delete mFaceController;
	}

    // Destroy entity and scene node
	mSceneNode->detachObject(mEntity);
    mSceneMgr->destroyEntity(mEntity);
    mSceneMgr->destroySceneNode(mUid);

    // Destroy instance zip file as resource location
	ResourceGroupManager::getSingleton().removeResourceLocation(mUidPath->getUniversalPath(), mResourceGroup);
	ResourceGroupManager::getSingleton().destroyResourceGroup(mResourceGroup);

    delete mUidZipArchive;
    delete mUidPath;
}

void CharacterInstance::onVoicePacketReception( VoicePacket* pVoicePacket )
{
	#ifdef ACTIVATE_FACE_CONTROLLER
		if( mFaceController )
		{
			mFaceController->onVoicePacketReception( pVoicePacket );
		}
	#endif
}


//---------------------------------------------------------------------------------
void CharacterInstance::loadModified()
{
	//Parsing XML configuration file
	if (!mUidZipArchive->isFilePresent(Character::getEditionConfFilename(mUid))) 
		return;

	FileBuffer xmlConfFile = mUidZipArchive->readFile(Character::getEditionConfFilename(mUid));
	TiXmlDocument xmlDoc;
	xmlDoc.Parse(xmlConfFile.getBufferFormatedToText().c_str());
	if (xmlDoc.Error()) 
		return;
	
	TiXmlElement* characterElement = xmlDoc.FirstChildElement("Character");
	if (characterElement == NULL) 
		return;

	//Parsing every Bone Element
	Skeleton* skeleton = getEntity()->getSkeleton();
	if( skeleton != NULL)
	{
		TiXmlElement* boneElement = characterElement->FirstChildElement("Bone");
		for(boneElement ; boneElement ; boneElement = boneElement->NextSiblingElement("Bone"))
		{
			//Checking Bone
			const char* name = boneElement->Attribute("name");
			if (name == NULL) 
				break; //return;
			String boneName(name);

			if (skeleton->getBone(boneName) == NULL) 
				break; //return;
			Bone* bone = skeleton->getBone(boneName);

			// enable modification on the bone properties
			if( !bone->isManuallyControlled() ) 
			{
				bone->setManuallyControlled( true );
				for(int i=0; i<skeleton->getNumAnimations(); i++ )
					skeleton->getAnimation( i )->destroyNodeTrack( bone->getHandle() );
			}

			//Checking position
			name = boneElement->Attribute("position");
			if (name == NULL) 
				break; //return;
			bone->setPosition( StringConverter::parseVector3(String(name)) );

			//Checking orientation 
			name = boneElement->Attribute("orientation");
			if (name == NULL) 
				break; //return;
			bone->setOrientation( StringConverter::parseQuaternion(String(name)) );

			//Checking scale
			name = boneElement->Attribute("scale");
			if (name == NULL) 
				break; //return;
			bone->setScale( StringConverter::parseVector3(String(name)) );
		}
	}

	//Parsing every BodyPart Element
	TiXmlElement* bodyPartElement = characterElement->FirstChildElement("BodyPart");
	for(bodyPartElement ; bodyPartElement ; bodyPartElement = bodyPartElement->NextSiblingElement("BodyPart"))
	{
		//Checking BodyPart
		const char* attribute = bodyPartElement->Attribute("name");
		if (attribute == NULL) 
			return;
		String bodyPartName(attribute);

		BodyPart* bodyPart = mCharacter->getBodyPart(bodyPartName);
        if (bodyPart == NULL)
			break; //return;

		//Checking BodyPartModel
		attribute = bodyPartElement->Attribute("bodyPartModel");
		if (attribute == NULL) 
			break; //return;
		String bodyPartModelName(attribute);

		if (bodyPartModelName != "None")
		{
			BodyPartModel* bodyPartModel = bodyPart->getBodyPartModel(bodyPartModelName);
            if (bodyPartModel == NULL) 
				break; //return;
            BodyPartInstance* bodyPartInst = mBodyPartInstances[bodyPart];
            bodyPartInst->setCurrentBodyPartModel(bodyPartModelName);
            BodyPartModelInstance* bodyPartModelInst = bodyPartInst->getCurrentBodyPartModelInstance();

			if (bodyPartModelInst->isColourModifiable())
			{
				//Checking bodyPartModel Colour
				attribute = bodyPartElement->Attribute("colour");
				if (attribute != NULL)
					bodyPartModelInst->setColour(StringConverter::parseColourValue(String(attribute)));

				//Checking bodyPartModel ColourAmbient
				attribute = bodyPartElement->Attribute("ambient");
				if (attribute != NULL)
					bodyPartModelInst->setColourAmbient(StringConverter::parseColourValue(String(attribute)));

				//Checking bodyPartModel ColourDiffuse
				attribute = bodyPartElement->Attribute("diffuse");
				if (attribute != NULL)
					bodyPartModelInst->setColourDiffuse(StringConverter::parseColourValue(String(attribute)));

				//Checking bodyPartModel ColourSpecular
				attribute = bodyPartElement->Attribute("specular");
				if (attribute != NULL)
					bodyPartModelInst->setColourSpecular(StringConverter::parseColourValue(String(attribute)));

				//Checking bodyPartModel Shininess
				attribute = bodyPartElement->Attribute("shininess");
				if (attribute != NULL)
					bodyPartModelInst->setShininess(StringConverter::parseInt(String(attribute)));

				//Checking bodyPartModel Transparency
				attribute = bodyPartElement->Attribute("alpha");
				if (attribute != NULL)
					bodyPartModelInst->setTransparency(StringConverter::parseReal(String(attribute)));
			}

			//Checking bodyPartModel textures
			attribute = bodyPartElement->Attribute("texture");
			if (attribute != NULL)
			{
				String textureName(attribute);
				TexturePtr texture = bodyPartModel->getTexture(textureName);
				if (!texture.isNull())
					bodyPartModelInst->setCurrentTexture(texture);
			}
		}

		//Checking CouplesOfPoses
		TiXmlElement* coupleOfPosesElement = bodyPartElement->FirstChildElement("CoupleOfPoses");
		for(coupleOfPosesElement; coupleOfPosesElement; coupleOfPosesElement = coupleOfPosesElement->NextSiblingElement("CoupleOfPoses"))
		{
			attribute = coupleOfPosesElement->Attribute("name");
			if (attribute == NULL) 
				break; //return;
			String coupleOfPosesName(attribute);
			if (bodyPart->getCoupleOfPoses(coupleOfPosesName) == NULL) 
				break; //return;
			CoupleOfPoses* coupleOfPoses = bodyPart->getCoupleOfPoses(coupleOfPosesName);

			attribute = coupleOfPosesElement->Attribute("position"); 
			if (attribute == NULL) 
				break; //return;
			float position = StringConverter::parseReal(String(attribute));

			coupleOfPoses->setPosition(position);
			updatePoseReference(coupleOfPoses->getLeftPoseIndex(),1.0f - coupleOfPoses->getPosition());
			updatePoseReference(coupleOfPoses->getRightPoseIndex(),coupleOfPoses->getPosition());
		}
	}
	
	//Parsing every Goody Element
	TiXmlElement* goodyElement = characterElement->FirstChildElement("Goody");
	for(goodyElement ; goodyElement ; goodyElement = goodyElement->NextSiblingElement("Goody"))
	{
		//Checking BodyPart
		const char* name = goodyElement->Attribute("name");
		if (name == NULL) 
			break; //return;
		String goodyName(name);

		Goody* goody = mCharacter->getGoody(goodyName);
		if (goody == NULL) 
			break; //return;

		//Checking GoodyModel
		name = goodyElement->Attribute("goodyModel");
		if (name == NULL) 
			break; //return;
		String goodyModelName(name);

		if (goodyModelName != "None")
		{
			GoodyModel* goodyModel = goody->getGoodyModel(goodyModelName);
			if (goodyModel == NULL) 
				break; //return;
            GoodyInstance* goodyInst = new GoodyInstance(goody, this);
            mGoodyInstances[goody] = goodyInst;
            goodyInst->setCurrentGoodyModel(goodyModelName);
            GoodyModelInstance* goodyModelInst = goodyInst->getCurrentGoodyModelInstance();

			//Checking for position.
			name = goodyElement->Attribute("positionScrollPositions");
			if (name != NULL)
			{
				Vector3 positionScrollPositions = StringConverter::parseVector3(String(name));
				goodyInst->setCurrentXScrollPosition(positionScrollPositions.x);
				goodyInst->setCurrentYScrollPosition(positionScrollPositions.y);
				goodyInst->setCurrentZScrollPosition(positionScrollPositions.z);
			}

			//Checking for rotation.
			name = goodyElement->Attribute("rotationsScrollPositions");
			if (name != NULL)
			{
				Vector3 rotationsScrollPositions = StringConverter::parseVector3(String(name));
				goodyInst->setCurrentYawAngleScrollPosition(rotationsScrollPositions.x);
				goodyInst->setCurrentPitchAngleScrollPosition(rotationsScrollPositions.y);
				goodyInst->setCurrentRollAngleScrollPosition(rotationsScrollPositions.z);
			}

			//Checking GoodyModel Colour
			if (goodyModelInst->isColourModifiable())
			{
				//Checking GoodyModel Colour
				name = goodyElement->Attribute("colour");
				if (name != NULL)
					goodyModelInst->setColour(StringConverter::parseColourValue(String(name)));

				//Checking GoodyModel ColourAmbient
				name = goodyElement->Attribute("ambient");
				if (name != NULL)
					goodyModelInst->setColourAmbient(StringConverter::parseColourValue(String(name)));

				//Checking GoodyModel ColourDiffuse
				name = goodyElement->Attribute("diffuse");
				if (name != NULL)
					goodyModelInst->setColourDiffuse(StringConverter::parseColourValue(String(name)));

				//Checking GoodyModel ColourSpecular
				name = goodyElement->Attribute("specular");
				if (name != NULL)
					goodyModelInst->setColourSpecular(StringConverter::parseColourValue(String(name)));

				//Checking GoodyModel Shininess
				name = goodyElement->Attribute("shininess");
				if (name != NULL)
					goodyModelInst->setShininess(StringConverter::parseInt(String(name)));

				//Checking GoodyModel Transparency
				name = goodyElement->Attribute("alpha");
				if (name != NULL)
					goodyModelInst->setTransparency(StringConverter::parseReal(String(name)));
			}

			//Checking GoodyModel textures
			name = goodyElement->Attribute("texture");
			if (name != NULL)
			{
				String textureName(name);
				TexturePtr texture = goodyModel->getTexture(textureName);
				if (!texture.isNull())
					goodyModelInst->setCurrentTexture(texture);
			}
		}
	}
}

//---------------------------------------------------------------------------------
void CharacterInstance::saveModified()
{
	//Creating xml configuration file
	TiXmlElement characterElement("Character");
    characterElement.SetAttribute("name", mCharacter->getName().c_str());

	//Creating Bones
	Skeleton* skeleton = getEntity()->getSkeleton();
	if( skeleton != NULL)
	{
		Skeleton::BoneIterator boneIterator = skeleton->getBoneIterator();
		while(boneIterator.hasMoreElements())
		{
			//Updating Bone
			Bone* bone = boneIterator.getNext();

			// enable modification on the bone properties
			if( bone->isManuallyControlled() ) 
			{
				TiXmlElement boneElement("Bone");
				boneElement.SetAttribute("name",bone->getName().c_str());
				// position
				boneElement.SetAttribute("position",StringConverter::toString(bone->getPosition()).c_str());
				// orientation
				boneElement.SetAttribute("orientation",StringConverter::toString(bone->getOrientation()).c_str());
				// scale
				boneElement.SetAttribute("scale",StringConverter::toString(bone->getScale()).c_str());

				characterElement.InsertEndChild(boneElement);
			}
		}
	}

	//Creating BodyParts
    for(BodyPartInstancesMap::const_iterator it=mBodyPartInstances.begin();it!=mBodyPartInstances.end();++it)
	{
		//Creating BodyPartModel
		BodyPartInstance* bodyPartInst = it->second;
		BodyPartModelInstance* bodyPartModelInst = bodyPartInst->getCurrentBodyPartModelInstance();
		TiXmlElement bodyPartElement("BodyPart");
        bodyPartElement.SetAttribute("name",bodyPartInst->getBodyPart()->getName().c_str());
		bodyPartElement.SetAttribute("bodyPartModel",bodyPartInst->getCurrentBodyPartModelName().c_str());
		if (bodyPartInst->getCurrentBodyPartModel() != NULL)
		{
			BodyPartModel* bodyPartModel = bodyPartInst->getCurrentBodyPartModel();

            if (bodyPartModelInst->isColourModifiable()) 
			{
				bodyPartElement.SetAttribute("colour",StringConverter::toString(bodyPartModelInst->getColour()).c_str());
				//
				bodyPartElement.SetAttribute("ambient",StringConverter::toString(bodyPartModelInst->getColourAmbient()).c_str());
				bodyPartElement.SetAttribute("diffuse",StringConverter::toString(bodyPartModelInst->getColourDiffuse()).c_str());
				bodyPartElement.SetAttribute("specular",StringConverter::toString(bodyPartModelInst->getColourSpecular()).c_str());
				bodyPartElement.SetAttribute("shininess",StringConverter::toString(bodyPartModelInst->getShininess()).c_str());
				bodyPartElement.SetAttribute("alpha",StringConverter::toString(bodyPartModelInst->getTransparency()).c_str());
			}
			if (bodyPartModel->isTextureModifiable()) 
				bodyPartElement.SetAttribute("texture",bodyPartModelInst->getCurrentTexture()->getName().c_str());
		}

#if 0 //GREG
		//Creating couples of poses for the BodyPart
		CouplesOfPosesMapIterator couplesOfPosesMapIterator = bodyPart->getCouplesOfPosesMapIterator();
		while(couplesOfPosesMapIterator.hasMoreElements())
		{
			CoupleOfPoses* coupleOfPoses = couplesOfPosesMapIterator.getNext();
			
			TiXmlElement coupleOfPosesElement("CoupleOfPoses");
			coupleOfPosesElement.SetAttribute("name",coupleOfPoses->getName().c_str());
			coupleOfPosesElement.SetAttribute("position",StringConverter::toString(coupleOfPoses->getPosition()).c_str());
			bodyPartElement.InsertEndChild(coupleOfPosesElement);
		}
#endif //GREG

		characterElement.InsertEndChild(bodyPartElement);
	}

	//Creating Goodies
    for(GoodyInstancesMap::const_iterator it=mGoodyInstances.begin();it!=mGoodyInstances.end();++it)
	{
		//Creating GoodyModel
		GoodyInstance* goodyInst = it->second;
		TiXmlElement goodyElement("Goody");
		goodyElement.SetAttribute("name", goodyInst->getGoody()->getName().c_str());
		goodyElement.SetAttribute("goodyModel", goodyInst->getCurrentGoodyModelName().c_str());
		if (goodyInst->getCurrentGoodyModel() != NULL)
		{
			GoodyModel* goodyModel = goodyInst->getCurrentGoodyModel();
			GoodyModelInstance* goodyModelInst = goodyInst->getCurrentGoodyModelInstance();
			goodyElement.SetAttribute("positionScrollPositions",StringConverter::toString(Vector3(goodyInst->getCurrentXScrollPosition(),goodyInst->getCurrentYScrollPosition(),goodyInst->getCurrentZScrollPosition())).c_str());
			goodyElement.SetAttribute("rotationsScrollPositions",StringConverter::toString(Vector3(goodyInst->getCurrentYawAngleScrollPosition(),goodyInst->getCurrentPitchAngleScrollPosition(),goodyInst->getCurrentRollAngleScrollPosition())).c_str());

			if (goodyModelInst->isColourModifiable()) 
			{
				goodyElement.SetAttribute("colour",StringConverter::toString(goodyModelInst->getColour()).c_str());
				//
				goodyElement.SetAttribute("ambient",StringConverter::toString(goodyModelInst->getColourAmbient()).c_str());
				goodyElement.SetAttribute("diffuse",StringConverter::toString(goodyModelInst->getColourDiffuse()).c_str());
				goodyElement.SetAttribute("specular",StringConverter::toString(goodyModelInst->getColourSpecular()).c_str());
				goodyElement.SetAttribute("shininess",StringConverter::toString(goodyModelInst->getShininess()).c_str());
				goodyElement.SetAttribute("alpha",StringConverter::toString(goodyModelInst->getTransparency()).c_str());
			}
			if (goodyModel->isTextureModifiable()) 
				goodyElement.SetAttribute("texture",goodyModelInst->getCurrentTexture()->getName().c_str());
		}

		characterElement.InsertEndChild(goodyElement);
	}
		

	FileBuffer xmlNatureFile = mUidZipArchive->readFile(Character::getEditionNatureFilename(mUid));
	TiXmlDocument xmlDoc;
	xmlDoc.Parse(xmlNatureFile.getBufferFormatedToText().c_str());



	//Finally saving to the character archive
	TiXmlDocument confXmlDoc; 
	confXmlDoc.InsertEndChild(characterElement);
	confXmlDoc.SaveFile(Character::getEditionConfFilename(mUid).c_str());

	FILE* confXmlFile = fopen(Character::getEditionConfFilename(mUid).c_str(), "rb");

	fseek(confXmlFile,0,SEEK_END);
	size_t confXmlFileSize = ftell(confXmlFile);
	fseek(confXmlFile,0,SEEK_SET);
	void* confXmlFileData = new unsigned char[confXmlFileSize];
	fread(confXmlFileData,confXmlFileSize,sizeof(unsigned char),confXmlFile);
	mUidZipArchive->writeFile(Character::getEditionConfFilename(mUid).c_str(), FileBuffer(confXmlFileData, confXmlFileSize));

	fclose(confXmlFile);

	SOLdeleteFile(Character::getEditionConfFilename(mUid).c_str());

#if 0
    //Creating a mesh very easily usable for others applications (Sollipsis).
	String modifiedMeshName(mUid + "Modified.mesh");

	//Creating new .mesh and .material
	MeshPtr mesh = getMesh()->clone(modifiedMeshName, mResourceGroup);
	for(unsigned short idxSubMesh = 0 ; idxSubMesh < mesh->getNumSubMeshes() ;idxSubMesh++)
	{
		SubMesh* smesh = mesh->getSubMesh(idxSubMesh);
		smesh->setMaterialName(mEntity->getSubEntity(idxSubMesh)->getMaterialName());
		if (!mEntity->getSubEntity(idxSubMesh)->isVisible())
		{
			//dirty way to disable a SubMesh from an Ogre Mesh beause there is no clean way to remove a submesh from a mesh.
			smesh->vertexData->vertexCount = 0;
			smesh->indexData->indexCount = 0;
		}
	}

	//Adding every Goody
	goodiesIterator = getGoodiesIterator();
	while(goodiesIterator.hasMoreElements())
	{
		Goody* goody = goodiesIterator.getNext();
		if (goody->getCurrentGoodyModelName() != "None")
		{
			for(unsigned int idxSubEntity=0 ; idxSubEntity < goody->getCurrentGoodyModel()->getEntity()->getNumSubEntities() ; idxSubEntity++)
			{
				addSubMesh(
					mesh,
					goody->getBoneName(),
					goody->getCurrentGoodyModel()->getEntity()->getSubEntity(idxSubEntity)->getSubMesh(),
					goody->getName()+StringConverter::toString(idxSubEntity),
					goody->getCurrentPosition(),
					goody->getCurrentOrientation());
			}
		}
	}

	//Modifing vertex data according to couples of poses.
	BodyPartsIterator bodyPartIterator = getBodyPartsIterator();
	while(bodyPartIterator.hasMoreElements())
	{
		BodyPart* bodyPart = bodyPartIterator.getNext();
		CouplesOfPosesMapIterator couplesOfPosesMapIterator = bodyPart->getCouplesOfPosesMapIterator();
		while(couplesOfPosesMapIterator.hasMoreElements())
		{
			CoupleOfPoses* coupleOfPoses = couplesOfPosesMapIterator.getNext();

			//Modifying SubMesh according to left pose
			Pose* leftPose = mesh->getPose(coupleOfPoses->getLeftPoseIndex());
			Pose::VertexOffsetIterator leftPoseVertexOffsetIterator = leftPose->getVertexOffsetIterator();
			SubMesh* smesh = mesh->getSubMesh(leftPose->getTarget()-1);
			const VertexElement* positionElement = smesh->vertexData->vertexDeclaration->findElementBySemantic(VES_POSITION);
			HardwareVertexBufferSharedPtr vertexBuffer = smesh->vertexData->vertexBufferBinding->getBuffer(positionElement->getSource());
			size_t vertexBufferSize = vertexBuffer->getSizeInBytes();
			size_t nbVertices = vertexBuffer->getNumVertices();
			size_t offset = positionElement->getOffset();
			size_t vertexSize = vertexBuffer->getVertexSize();
			void* vertexBufferData = new unsigned char[vertexBufferSize];
			vertexBuffer->readData(0,vertexBufferSize,vertexBufferData);


			while(leftPoseVertexOffsetIterator.hasMoreElements())
			{
				size_t idxVertex = leftPoseVertexOffsetIterator.peekNextKey();
				const Vector3& vertexOffset = leftPoseVertexOffsetIterator.getNext();

				float* point = ((float*)(((unsigned char*)vertexBufferData) + offset + vertexSize*idxVertex));
				point[0] += vertexOffset.x*(1.0f - coupleOfPoses->getPosition());
				point[1] += vertexOffset.y*(1.0f - coupleOfPoses->getPosition());
				point[2] += vertexOffset.z*(1.0f - coupleOfPoses->getPosition());					
			}

			vertexBuffer->lock(HardwareBuffer::LockOptions::HBL_NORMAL);
			vertexBuffer->writeData(0,vertexBufferSize,vertexBufferData);
			delete[] vertexBufferData;


			//Modifying SubMesh according to right pose
			Pose* rightPose = mesh->getPose(coupleOfPoses->getRightPoseIndex());
			Pose::VertexOffsetIterator rightPoseVertexOffsetIterator = rightPose->getVertexOffsetIterator();
			smesh = mesh->getSubMesh(rightPose->getTarget()-1);
			positionElement = smesh->vertexData->vertexDeclaration->findElementBySemantic(VES_POSITION);
			vertexBuffer = smesh->vertexData->vertexBufferBinding->getBuffer(positionElement->getSource());
			vertexBufferSize = vertexBuffer->getSizeInBytes();
			nbVertices = vertexBuffer->getNumVertices();
			offset = positionElement->getOffset();
			vertexSize = vertexBuffer->getVertexSize();
			vertexBufferData = new unsigned char[vertexBufferSize];
			vertexBuffer->readData(0,vertexBufferSize,vertexBufferData);


			while(rightPoseVertexOffsetIterator.hasMoreElements())
			{
				size_t idxVertex = rightPoseVertexOffsetIterator.peekNextKey();
				const Vector3& vertexOffset = rightPoseVertexOffsetIterator.getNext();

				float* point = ((float*)(((unsigned char*)vertexBufferData) + offset + vertexSize*idxVertex));
				point[0] += vertexOffset.x*coupleOfPoses->getPosition();
				point[1] += vertexOffset.y*coupleOfPoses->getPosition();
				point[2] += vertexOffset.z*coupleOfPoses->getPosition();					
			}

			vertexBuffer->lock(HardwareBuffer::LockOptions::HBL_NORMAL);
			vertexBuffer->writeData(0,vertexBufferSize,vertexBufferData);
			delete[] vertexBufferData;
		}
	}
	
	//Saving mesh and materials to the zip archive.
	MeshSerializer meshSerializer;
	meshSerializer.exportMesh(mesh.get(),mUid + ".mesh");
	
	FILE* meshFile = fopen((mUid + ".mesh").c_str(), "rb");

	fseek(meshFile,0,SEEK_END);
	size_t meshFileSize = ftell(meshFile);
	fseek(meshFile,0,SEEK_SET);
	void* meshFileData = new unsigned char[meshFileSize];
	fread(meshFileData,meshFileSize,sizeof(unsigned char),meshFile);
	mUidZipArchive->writeFile((mUid + ".mesh").c_str(), FileBuffer(meshFileData, meshFileSize));


	fclose(meshFile);

	SOLdeleteFile(Ogre::String(mUid + ".mesh").c_str());



	//material now
	MaterialSerializer materialSerializer;
	bool emptyQueue = true;
	for(unsigned short idxSubMesh = 0 ; idxSubMesh < mesh->getNumSubMeshes() ; idxSubMesh++)
	{
		emptyQueue = false;
		materialSerializer.queueForExport(MaterialManager::getSingleton().getByName(mesh->getSubMesh(idxSubMesh)->getMaterialName()));
	}
	if (!emptyQueue) materialSerializer.exportQueued(mUid + ".material");

	FILE* materialFile = fopen((mUid + ".material").c_str(), "rb");

	fseek(materialFile,0,SEEK_END);
	size_t materialFileSize = ftell(materialFile);
	fseek(materialFile,0,SEEK_SET);
	void* materialFileData = new unsigned char[materialFileSize];
	fread(materialFileData,materialFileSize,sizeof(unsigned char),materialFile);
	mUidZipArchive->writeFile((mUid + ".material").c_str(), FileBuffer(materialFileData, materialFileSize));

	fclose(materialFile);

	SOLdeleteFile(Ogre::String(mUid + ".material").c_str());

	//Unloading created mesh and removing it from the resource manager
	mesh->unload();
	MeshManager::getSingleton().remove(mesh->getName());
#endif
}

//---------------------------------------------------------------------------------
void CharacterInstance::deleteModified()
{
    mUidZipArchive->removeFile(Character::getEditionNatureFilename(mUid));
    mUidZipArchive->removeFile(Character::getEditionConfFilename(mUid));
    mUidZipArchive->removeFile(mUid + ".mesh");
    mUidZipArchive->removeFile(mUid + ".material");
}

//---------------------------------------------------------------------------------
void CharacterInstance::addPoseReference(ushort poseIndex, Real influence)
{
	mCustomizationKeyFrame->addPoseReference(poseIndex,influence);
}

//---------------------------------------------------------------------------------
void CharacterInstance::updatePoseReference(ushort indexPose,float influence)
{
	mCustomizationKeyFrame->updatePoseReference(indexPose,influence);
	mCustomizationAnimationState->getParent()->_notifyDirty();
}

//---------------------------------------------------------------------------------
unsigned short CharacterInstance::getAnimationMode()
{
	return mAnimationMode;
}

//---------------------------------------------------------------------------------
void CharacterInstance::setAnimationMode(unsigned short animationMode)
{
    if (mCharacter->getNumAnimations() == 0) return; //Nothing to do, the current mode is and still will be ANIMATION_NO_ANIMATION.

	assert((animationMode >= 0) && (animationMode < 4));

	mAnimationMode = animationMode;
	if ((mAnimationMode != ANIMATION_MODE_NO_ANIMATION) && (mCurrentAnimationState == NULL)) 
		setCurrentAnimation(0); //If the current anim state is NULL (and the mode is different from no animation), as it is the case at the beginning of the application, we must set an animation. 
}

//---------------------------------------------------------------------------------
unsigned short CharacterInstance::getCurrentAnimation()
{
	return mCurrentAnimation;
}

//---------------------------------------------------------------------------------
void CharacterInstance::setCurrentAnimation(unsigned short idAnimation)
{
	if (mCharacter->getNumAnimations() == 0) return;

	assert((idAnimation >= 0) && (idAnimation < mCharacter->getNumAnimations()));

	//stopping the old animation
	if (mCurrentAnimationState != NULL)
	{
		mCurrentAnimationState->setTimePosition(0);
		mCurrentAnimationState->setEnabled(false);
		mCurrentAnimationState->setLoop(false);
	}

	//setting the new animation	
	mCurrentAnimation = idAnimation;
	const String& animationName = mEntity->getSkeleton()->getAnimation(idAnimation)->getName();
	mCurrentAnimationState = mEntity->getAnimationState(animationName);
	mCurrentAnimationState->setTimePosition(0);
	mCurrentAnimationState->setEnabled(true);
	mCurrentAnimationState->setLoop(true);
}

//---------------------------------------------------------------------------------
void CharacterInstance::animate(Real elapsedTime)
{
	if (mCharacter->getNumAnimations() == 0) return;

	static Real timeSinceLastCurrentAnimationChange = 0;	//only available in ANIMATION_MODE_PERIODIC mode.

	switch(mAnimationMode)
	{
	case ANIMATION_MODE_NO_ANIMATION:
		mCurrentAnimationState->setTimePosition(0);
		mCurrentAnimationState->setEnabled(true);
		mCurrentAnimationState->setLoop(true);
	case ANIMATION_MODE_EDITION_MANUAL:
		return;
	case ANIMATION_MODE_PERIODIC:
		if (mCharacter->getNumAnimations() > 1) //In this case we want that in case of changement of animation, the new animation is different from the old one (if not the same animation will seem 'cut')
		{
			timeSinceLastCurrentAnimationChange += elapsedTime;
			if (timeSinceLastCurrentAnimationChange >= ANIMATION_PERIOD) //the period time has been reached, we have to change randomly the animation.
			{
				timeSinceLastCurrentAnimationChange -= ANIMATION_PERIOD;
				unsigned short idAnimation = mCurrentAnimation;
				while(idAnimation == mCurrentAnimation)
				{
					idAnimation = (unsigned short) (((float)rand()/RAND_MAX) * mCharacter->getNumAnimations());
				}
				setCurrentAnimation(idAnimation);
			}
		}//Otherwise nothing to do, the animation is still the same.
	case ANIMATION_MODE_EDITION_AUTOMATIC:
		mCurrentAnimationState->addTime(elapsedTime);
		break;
	}
}

//---------------------------------------------------------------------------------
float CharacterInstance::getAnimationPosition()
{
	if (mCharacter->getNumAnimations() == 0) return 0;

	return (mCurrentAnimationState->getTimePosition() / mCurrentAnimationState->getLength());
}

//---------------------------------------------------------------------------------
void CharacterInstance::setAnimationPosition(Real position)
{
	if ((mAnimationMode != ANIMATION_MODE_EDITION_MANUAL)||(mCharacter->getNumAnimations() == 0)) return;

	assert((position >= 0) && (position <= 1));

	mCurrentAnimationState->setTimePosition(position * mCurrentAnimationState->getLength());
}

//---------------------------------------------------------------------------------
void CharacterInstance::resetModifications()
{
}

//---------------------------------------------------------------------------------
BodyPartInstance* CharacterInstance::getCurrentBodyPart()
{
	if (mBodyPartInstance == 0)
    {
		BodyPart* bodyPart = (*mCharacter->getBodyParts().begin()).second;
        mBodyPartInstance = mBodyPartInstances[bodyPart];
    }

	return mBodyPartInstance;
}

//---------------------------------------------------------------------------------
BodyPartInstance* CharacterInstance::setNextBodyPartAsCurrent()
{
    const BodyPartsMap& bodyParts = mCharacter->getBodyParts();
	BodyPartsMap::const_iterator iter = bodyParts.begin();
	while(iter != bodyParts.end())
	{
        if((*iter).second == mBodyPartInstance->getBodyPart())
		{
			iter++;
			if(iter == bodyParts.end())
				iter = bodyParts.begin();
			break;
		}

		iter++;
	}
    BodyPart* bodyPart = (*iter).second;
    mBodyPartInstance = mBodyPartInstances[bodyPart];

	return mBodyPartInstance;
}

//---------------------------------------------------------------------------------
BodyPartInstance* CharacterInstance::setPreviousBodyPartAsCurrent()
{
    const BodyPartsMap& bodyParts = mCharacter->getBodyParts();
	BodyPartsMap::const_iterator iter = bodyParts.begin();
	while(iter != bodyParts.end())
	{
        if((*iter).second == mBodyPartInstance->getBodyPart())
		{
			if(iter == bodyParts.begin())
				iter = bodyParts.end();
			iter--;
			break;
		}

		iter++;
	}
    BodyPart* bodyPart = (*iter).second;
    mBodyPartInstance = mBodyPartInstances[bodyPart];

	return mBodyPartInstance;
}

//---------------------------------------------------------------------------------
Bone* CharacterInstance::getCurrentBone()
{
	return getEntity()->getSkeleton()->getBone( mBone );;
}

//---------------------------------------------------------------------------------
Bone* CharacterInstance::setNextBoneAsCurrent()
{
	mBone++;
	if( mBone >= getEntity()->getSkeleton()->getNumBones() )
		mBone = 0;

	return getCurrentBone();
}

//---------------------------------------------------------------------------------
Bone* CharacterInstance::setPreviousBoneAsCurrent()
{
	if( mBone <= 0 )
		mBone = getEntity()->getSkeleton()->getNumBones();
	mBone--;

	return getCurrentBone();
}

//---------------------------------------------------------------------------------
void CharacterInstance::setCurrentGoody(Goody* goody)
{
    mGoodyInstance = mGoodyInstances[goody];
    if (mGoodyInstance == 0)
    {
        mGoodyInstance = new GoodyInstance(goody, this);
        mGoodyInstances[goody] = mGoodyInstance;
    }
}

//---------------------------------------------------------------------------------
GoodyInstance* CharacterInstance::getCurrentGoody()
{
	if (mGoodyInstance == 0)
    {
		Goody* goody = (*mCharacter->getGoodies().begin()).second;
        setCurrentGoody(goody);
    }

	return mGoodyInstance;
}

//---------------------------------------------------------------------------------
GoodyInstance* CharacterInstance::setNextGoodyAsCurrent()
{
    const GoodiesMap& goodies = mCharacter->getGoodies();
	GoodiesMap::const_iterator iter = goodies.begin();
	while(iter != goodies.end())
	{
        if((*iter).second == mGoodyInstance->getGoody())
		{
			iter++;
			if(iter == goodies.end())
				iter = goodies.begin();
			break;
		}

		iter++;
	}
    Goody* goody = (*iter).second;
    setCurrentGoody(goody);

	return mGoodyInstance;
}

//---------------------------------------------------------------------------------
GoodyInstance* CharacterInstance::setPreviousGoodyAsCurrent()
{
    const GoodiesMap& goodies = mCharacter->getGoodies();
	GoodiesMap::const_iterator iter = goodies.begin();
	while(iter != goodies.end())
	{
        if((*iter).second == mGoodyInstance->getGoody())
		{
			if(iter == goodies.begin())
				iter = goodies.end();
			iter--;
			break;
		}

		iter++;
	}
    Goody* goody = (*iter).second;
    setCurrentGoody(goody);

	return mGoodyInstance;
}

//---------------------------------------------------------------------------------
