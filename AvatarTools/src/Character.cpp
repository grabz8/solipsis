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
#include "Character.h"

using namespace Solipsis;

class CoupleOfPosesToAdd{
public:
	String bodyPartName;
	String coupleOfPosesName;
	String leftPoseName;
	Vector3 leftPoseCameraCylindricCoordinates;
	String leftStateName;
	String rightPoseName;
	Vector3 rightPoseCameraCylindricCoordinates;
	String rightStateName;
};

//---------------------------------------------------------------------------------
Character::Character(String pName, SceneManager* pSceneMgr) :
	mPath(NULL),
	mZipArchive(NULL),
	mSceneMgr(pSceneMgr),
	mName(pName),
	mSkeletonName(""),
	mMeshName(""),
	mLoadingSuccessful(false)
{
	//Checking the validity of the archive, the presence of the edition.mesh and edition.material files.
	mPath = new Path(pName);
	if (mPath->getExtension() != "saf")
	{
		mLoadingErrorMessage = "The character archive must be a saf file !"; 
		return;
	}

	mName = mPath->getLastFileName(false);
	if (mName == "")
	{
		mLoadingErrorMessage = "The character archive must have a name !"; 
		return;
	}

	mZipArchive = new MyZipArchive(mPath->getUniversalPath());
	if (!mZipArchive->isArchivePresent())
	{
		mLoadingErrorMessage = "Character archive not found !"; 
		return;
	}

    // mesh filename
	if (!mZipArchive->isFilePresent(getEditionMeshFilename(mName)))
	{
		mLoadingErrorMessage = getEditionMeshFilename(mName) + "\nnot found in the character archive !"; 
		return;
	}
	mMeshName = getEditionMeshFilename(mName);

	//Adding the zip to the ressource location and load all the medias in the zip.
    mResourceGroup = mName + "Resources";
	ResourceGroupManager::getSingleton().createResourceGroup(mResourceGroup);
	ResourceGroupManager::getSingleton().addResourceLocation(mPath->getUniversalPath(), "Zip", mResourceGroup);
	ResourceGroupManager::getSingleton().initialiseResourceGroup(mResourceGroup);

	MeshPtr mesh = MeshManager::getSingleton().load(getEditionMeshFilename(mName), mResourceGroup);
	mMesh = mesh;

	mCustomizationAnimation = mMesh->createAnimation(mName + "_CustomizationAnimation",0);
	if (mCustomizationAnimation->hasVertexTrack(1))
		mAnimationTrack = mCustomizationAnimation->getVertexTrack(1);
	else
		mAnimationTrack = mCustomizationAnimation->createVertexTrack(1,VAT_POSE);

    // generate the NATURE file if it doesn't exist
    if (!mZipArchive->isFilePresent(getEditionNatureFilename(mName)))
	{
        if (mZipArchive->isFilePresent(getEditionConfFilename(mName)))
			mZipArchive->removeFile(getEditionConfFilename(mName));

	    TiXmlDeclaration decl( "1.0", "", "" );
	    TiXmlElement characterElement("Character");
        characterElement.SetAttribute("name", mName.c_str());
	    characterElement.SetAttribute("characterCameraCylindricCoordinates", "3 45 -0.5");

	    //Creating BodyParts
	    char name[64];
	    int id = 0;
	    HashMap<String,ushort> subMeshNameMap = mMesh->getSubMeshNameMap();
	    for(HashMap<String,ushort>::iterator sm = subMeshNameMap.begin(); sm != subMeshNameMap.end(); sm++)
	    {
		    Path subName( (*sm).first );
		    sprintf(name, "%s", subName.getLastFileName(true).data() );

		    TiXmlElement bodyPartElement("BodyPart");
		    bodyPartElement.SetAttribute("name",name);

		    TiXmlElement bodyPartModel("BodyPartModel");
		    bodyPartModel.SetAttribute("name",name);
		    bodyPartModel.SetAttribute("colourModifiable","true");
		    TiXmlElement subMesh("SubMesh");
		    subMesh.SetAttribute("name",(*sm).first.c_str());
		    bodyPartModel.InsertEndChild(subMesh);
		    bodyPartElement.InsertEndChild(bodyPartModel);

		    TiXmlElement bodyPartModelNone("BodyPartModel");
		    bodyPartModelNone.SetAttribute("name","None");
		    bodyPartElement.InsertEndChild(bodyPartModelNone);

		    characterElement.InsertEndChild(bodyPartElement);
	    }

	    //Finally saving to the character archive
	    TiXmlDocument confXmlDoc;
	    confXmlDoc.InsertEndChild(decl);
	    confXmlDoc.InsertEndChild(characterElement);
	    confXmlDoc.SaveFile(getEditionNatureFilename(mName).c_str());

	    FILE* confXmlFile = fopen(getEditionNatureFilename(mName).c_str(), "rb");

	    fseek(confXmlFile,0,SEEK_END);
	    size_t confXmlFileSize = ftell(confXmlFile);
	    fseek(confXmlFile,0,SEEK_SET);
	    void* confXmlFileData = new unsigned char[confXmlFileSize];
	    fread(confXmlFileData,confXmlFileSize,sizeof(unsigned char),confXmlFile);
	    mZipArchive->writeFile(getEditionNatureFilename(mName).c_str(), FileBuffer(confXmlFileData, confXmlFileSize));

	    fclose(confXmlFile);

	    SOLdeleteFile(getEditionNatureFilename(mName).c_str());
	}

	// skeleton filename
	if (!mZipArchive->isFilePresent(mesh->getSkeletonName()))
	{
		mLoadingErrorMessage = "Skeleton file\nnot found in the character archive !"; 
		return;
	}
	mSkeletonName = mesh->getSkeletonName();

	//Creating entity
	mEntity = mSceneMgr->createEntity(mName, getEditionMeshFilename(mName));
	mEntity->setNormaliseNormals(true);

	//All the subentities are insvisible, only the subentities parts of a BodyPart will be set visibles after.
	for(unsigned int idxSubEntity = 0 ; idxSubEntity < mEntity->getNumSubEntities() ; idxSubEntity++)
		mEntity->getSubEntity(idxSubEntity)->setVisible(false);

	///Parsing xml nature file in order to perhaps load other additionnal meshs in order to add them to our mesh.
	FileBuffer xmlNatureFile = mZipArchive->readFile(Character::getEditionNatureFilename(mName));
	String s = xmlNatureFile.getBufferFormatedToText();
	TiXmlDocument xmlDoc;
	xmlDoc.Parse(xmlNatureFile.getBufferFormatedToText().c_str());
	if (xmlDoc.Error())
	{
		mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + "\nisn't a valid xml file !"; 
		return;
	}

	//We will need the following variables for parsing the xml nature file
	Vector3 characterCameraCylindricCoordinates; //we will memorise it one time we met him in the file. But we won't immediately add the character to render to texture because the loading can abort after and so we will destroy the character.

	//Parsing xml nature file
	TiXmlElement* characterElement = xmlDoc.FirstChildElement("Character");
	if (characterElement == NULL)
	{
		mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nNo Character markup found !"; 
		return;
	}

	const char* characterName = characterElement->Attribute("name");
	if (characterName == NULL)
	{
		mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nNo name's attribute found !"; 
		return;
	}

	const char* attribute = characterElement->Attribute("characterCameraCylindricCoordinates");
	if (attribute == NULL)
	{
		mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nNo characterCameraCylindricCoordinates found !\nneeded for taking character photo for the gallery."; 
		return;
	}
	characterCameraCylindricCoordinates = StringConverter::parseVector3(String(attribute));

	TiXmlElement* additionnalSubMeshElement = xmlDoc.FirstChildElement("AdditionnalSubMesh");
	for(additionnalSubMeshElement ; additionnalSubMeshElement != NULL ; additionnalSubMeshElement = additionnalSubMeshElement->NextSiblingElement("AdditionnalSubMesh"))
	{
		attribute = additionnalSubMeshElement->Attribute("name");
		if (attribute == NULL)
		{
			mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nAdditionnal SubMesh must have a name !"; return;
		}
		String additionnalSubMeshName = attribute;

		attribute = additionnalSubMeshElement->Attribute("parentMesh");
		if (attribute == NULL)
		{
			mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nAdditionnal SubMesh must have a parent mesh !"; return;
		}
		String additionnalSubMeshParentMeshName = attribute;

		attribute = additionnalSubMeshElement->Attribute("index");
		if (attribute == NULL)
		{
			mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nAdditionnal SubMesh must have an index !"; return;
		}
		int additionnalSubMeshIndex = StringConverter::parseInt(String(attribute));

		attribute = additionnalSubMeshElement->Attribute("boneToBeAttached");
		if (attribute == NULL)
		{
			mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nAdditionnal SubMesh must have a boneToBeAttached !"; return;
		}
		String additionnalSubMeshBoneToBeAttachedName = attribute;

		Vector3 offsetPosition = Vector3::ZERO;
		//Optional offset position
		attribute = additionnalSubMeshElement->Attribute("offsetPosition");
		if (attribute != NULL)
		{
			offsetPosition = StringConverter::parseVector3(String(attribute));
		}

		Quaternion offsetOrientation = Quaternion::IDENTITY;
		//Optional x rotation
		attribute = additionnalSubMeshElement->Attribute("xRotationAngle");
		if (attribute != NULL)
		{
			float angle = StringConverter::parseReal(String(attribute));
			offsetOrientation.FromAngleAxis(Degree(angle),Vector3(1,0,0));
		}
		//Optional y rotation
		attribute = additionnalSubMeshElement->Attribute("yRotationAngle");
		if (attribute != NULL)
		{
			float angle = StringConverter::parseReal(String(attribute));
			offsetOrientation = offsetOrientation * Quaternion(Degree(angle),Vector3(0,1,0));
		}
		//Optional z rotation
		attribute = additionnalSubMeshElement->Attribute("zRotationAngle");
		if (attribute != NULL)
		{
			float angle = StringConverter::parseReal(String(attribute));
			offsetOrientation = offsetOrientation * Quaternion(Degree(angle),Vector3(0,0,1));
		}


		MeshPtr additionnalSubMeshParentMesh = MeshManager::getSingleton().load(additionnalSubMeshParentMeshName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		SubMesh* additionnalSubMesh = additionnalSubMeshParentMesh->getSubMesh(additionnalSubMeshIndex);

		addSubMesh(mMesh,additionnalSubMeshBoneToBeAttachedName,additionnalSubMesh,additionnalSubMeshName,offsetPosition,offsetOrientation);

		additionnalSubMeshParentMesh->unload();
	}

	std::list<CoupleOfPosesToAdd> couplesOfPosesToAdd;			//same thing as characterCameraCylindricCoordinates, we want to save the information for adding a couple of poses without adding it.

	//Parsing every BodyPart Element
	TiXmlElement* bodyPartElement = characterElement->FirstChildElement("BodyPart");
	for(bodyPartElement ; bodyPartElement != NULL ; bodyPartElement = bodyPartElement->NextSiblingElement("BodyPart"))
	{
		//BodyPart's name
		attribute = bodyPartElement->Attribute("name");
		if (attribute == NULL)
		{
			mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nBody part must have a name !"; 
			return;
		}
		String bodyPartName(attribute);

		//Parsing every BodyPartModel Element
		TiXmlElement* bodyPartModelElement = bodyPartElement->FirstChildElement("BodyPartModel");
		if (bodyPartModelElement == NULL)
		{
			mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nBody part must have at least one model !"; 
			return;
		}
		bool firstBodyPartModel = true;
		for(bodyPartModelElement ; bodyPartModelElement != NULL ; bodyPartModelElement = bodyPartModelElement->NextSiblingElement("BodyPartModel"))
		{
			//BodyPartModel's name
			attribute = bodyPartModelElement->Attribute("name");
			if (attribute == NULL)
			{
				mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nBody part model must have a name !"; 
				return;
			}
			String bodyPartModelName(attribute);

			//SubMesh
			String subMeshName;
			if (bodyPartModelName == "None")
			{
				subMeshName = "none"; //Totally arbitrary since this body part model won't exist in reality.
			}
			else
			{
				TiXmlElement* subMesh = bodyPartModelElement->FirstChildElement("SubMesh");
				if (subMesh == NULL)
				{
					mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nBody part model must have one (and only one) SubMesh !"; 
					return;
				}

				attribute = subMesh->Attribute("name");
				if (attribute == NULL)
				{
					mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nSubmesh must have a name !"; 
					return;
				}
				subMeshName = attribute;

				HashMap<String,ushort> subMeshNameMap = getMesh()->getSubMeshNameMap();
				if (subMeshNameMap.find(subMeshName) == subMeshNameMap.end())
				{
					mLoadingErrorMessage = mName + "\nSubmesh "+subMeshName+" not found !"; 
					return;
				}
			}

			//Adding the BodyPartModel (and perhaps adding the BodyPart)
			if (firstBodyPartModel) //The first body part model found is taken as the default one for the body part, we have then to create the body part and it's first model.
			{
				firstBodyPartModel = false;
				addBodyPart(bodyPartName,subMeshName,bodyPartModelName);
			}
			else
			{
				getBodyPart(bodyPartName)->addBodyPartModel(subMeshName,bodyPartModelName);
			}
			BodyPartModel* bodyPartModel = getBodyPart(bodyPartName)->getBodyPartModel(bodyPartModelName);


			if (bodyPartModelName != "none")  //BodyPartModel specifications, so "none" bodyPartModel isn't concerned
			{
				//Is BodyPartModel's ColourModifiable ?
				const char* colourModifiableAttribute = bodyPartModelElement->Attribute("colourModifiable");
				if ((colourModifiableAttribute != NULL)&&(strcmp(colourModifiableAttribute,"true") == 0))
				{
					getBodyPart(bodyPartName)->getBodyPartModel(bodyPartModelName)->setColourModifiable(true);
				}

				//Textures
				TiXmlElement* textureElement = bodyPartModelElement->FirstChildElement("Texture");
				for(textureElement ; textureElement != NULL ; textureElement = textureElement->NextSiblingElement("Texture"))
				{
					attribute = textureElement->Attribute("name");
					if (attribute == NULL)
					{
						mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nTexture must have a name !"; return;
					}
					String textureName = attribute;
					TexturePtr texture = TextureManager::getSingleton().load(textureName,ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
					bodyPartModel->addTexture(texture);
				}
			}
		}

		//CouplesOfPoses
		TiXmlElement* coupleOfPosesElement = bodyPartElement->FirstChildElement("CoupleOfPoses");
		for(coupleOfPosesElement; coupleOfPosesElement != NULL ; coupleOfPosesElement = coupleOfPosesElement->NextSiblingElement("CoupleOfPoses"))
		{
			const char* attribute = coupleOfPosesElement->Attribute("name");
			if (attribute == NULL)
			{
				mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nCoupleOfPoses must have a name !"; return;
			}
			String coupleOfPosesName(attribute);

			attribute = coupleOfPosesElement->Attribute("leftPoseName");
			if (attribute == NULL)
			{
				mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nTexture must have a leftPoseName !"; return;
			}
			String leftPoseName(attribute);

			attribute = coupleOfPosesElement->Attribute("leftPoseCameraCylindricCoordinates");
			if (attribute == NULL)
			{
				mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nTexture must have a leftPoseCameraCylindricCoordinates !"; return;
			}
			const Vector3& leftPoseCameraCylindricCoordinates = StringConverter::parseVector3(String(attribute));

			attribute = coupleOfPosesElement->Attribute("leftStateName");
			if (attribute == NULL)
			{
				mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nTexture must have a leftStateName !"; return;
			}
			String leftStateName(attribute);

			attribute = coupleOfPosesElement->Attribute("rightPoseName");
			if (attribute == NULL)
			{
				mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nTexture must have a rightPoseName !"; return;
			}
			String rightPoseName(attribute);

			attribute = coupleOfPosesElement->Attribute("rightPoseCameraCylindricCoordinates");
			if (attribute == NULL)
			{
				mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nTexture must have a rightPoseCameraCylindricCoordinates !"; return;
			}
			const Vector3& rightPoseCameraCylindricCoordinates = StringConverter::parseVector3(String(attribute));

			attribute = coupleOfPosesElement->Attribute("rightStateName");
			if (attribute == NULL)
			{
				mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nTexture must have a rightStateName !"; return;
			}
			String rightStateName(attribute);


			CoupleOfPosesToAdd coupleOfPosesToAdd = { bodyPartName, coupleOfPosesName, leftPoseName, leftPoseCameraCylindricCoordinates, leftStateName, rightPoseName, rightPoseCameraCylindricCoordinates, rightStateName };
			couplesOfPosesToAdd.push_back(coupleOfPosesToAdd);
		}
	}

	//Parsing every Goody Element
	TiXmlElement* goodyElement = characterElement->FirstChildElement("Goody");
	for(goodyElement ; goodyElement != NULL ; goodyElement = goodyElement->NextSiblingElement("Goody"))
	{
		//Goody's name
		attribute = goodyElement->Attribute("name");
		if (attribute == NULL)
		{
			mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nGoody must have a name !"; return;
		}
		String goodyName(attribute);

		//Goody's boneName
		attribute = goodyElement->Attribute("boneName");
		assert( (attribute != NULL) && "Goody must have a boneName !");
		if (attribute == NULL)
		{
			mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nGoody must have a boneName !"; return;
		}
		String boneName(attribute);

		//Goody's minRotations
		attribute = goodyElement->Attribute("minRotations");
		if (attribute == NULL)
		{
			mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nGoody must have minRotations !"; return;
		}
		Vector3 minRotations = StringConverter::parseVector3(String(attribute));

		//Goody's defaultRotations
		attribute = goodyElement->Attribute("defaultRotations");
		if (attribute == NULL)
		{
			mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nGoody must have defaultRotations !"; return;
		}
		Vector3 defaultRotations = StringConverter::parseVector3(String(attribute));

		//Goody's maxRotations
		attribute = goodyElement->Attribute("maxRotations");
		if (attribute == NULL)
		{
			mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nGoody must have maxRotations !"; return;
		}
		Vector3 maxRotations = StringConverter::parseVector3(String(attribute));

		//Goody's minPosition
		attribute = goodyElement->Attribute("minPosition");
		if (attribute == NULL)
		{
			mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nGoody must have minPosition !"; return;
		}
		Vector3 minPosition = StringConverter::parseVector3(String(attribute));

		//Goody's defaultPosition
		attribute = goodyElement->Attribute("defaultPosition");
		if (attribute == NULL)
		{
			mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nGoody must have defaultPosition !"; return;
		}
		Vector3 defaultPosition = StringConverter::parseVector3(String(attribute));

		//Goody's maxPosition
		attribute = goodyElement->Attribute("maxPosition");
		if (attribute == NULL)
		{
			mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nGoody must have maxPosition !"; return;
		}
		Vector3 maxPosition = StringConverter::parseVector3(String(attribute));


		//Creating Goody
		addGoody(goodyName,boneName,
			minRotations,defaultRotations,maxRotations,
			minPosition,defaultPosition,maxPosition);
		Goody* goody = getGoody(goodyName);



		//Parsing every GoodyModel Element
		TiXmlElement* goodyModelElement = goodyElement->FirstChildElement("GoodyModel");
		if (goodyModelElement == NULL)
		{
			mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nGoody must have at least one model !"; return;
		}
		for(goodyModelElement ; goodyModelElement != NULL ; goodyModelElement = goodyModelElement->NextSiblingElement("GoodyModel"))
		{
			//GoodyModel's name
			attribute = goodyModelElement->Attribute("name");
			if (attribute == NULL)
			{
				mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nGoody model must have a name !"; return;
			}
			String goodyModelName(attribute);

			//GoodyModel's MeshName
			attribute = goodyModelElement->Attribute("meshName");
			if (attribute == NULL)
			{
				mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nGoody model must have a meshName !"; return;
			}
			String goodyModelMeshName(attribute);


			goody->addGoodyModel(goodyModelMeshName,goodyModelName,mSceneMgr);
			GoodyModel* goodyModel = goody->getGoodyModel(goodyModelName);

			//Is GoodyModel's ColourModifiable ?
			const char* colourModifiableAttribute = goodyModelElement->Attribute("colourModifiable");
			if ((colourModifiableAttribute != NULL)&&(strcmp(colourModifiableAttribute,"true") == 0))
			{
				goodyModel->setColourModifiable(true);
			}

			//Textures
			TiXmlElement* textureElement = goodyModelElement->FirstChildElement("Texture");
			for(textureElement ; textureElement != NULL ; textureElement = textureElement->NextSiblingElement("Texture"))
			{
				attribute = textureElement->Attribute("name");
				if (attribute == NULL)
				{
					mLoadingErrorMessage = Character::getEditionNatureFilename(mName) + " :" + "\nTexture must have a name !" ; return;
				}
				String textureName = attribute;
				TexturePtr texture = TextureManager::getSingleton().load(textureName,ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
				goodyModel->addTexture(texture);
			}
		}
	}

//GILLES	ScreenshotManager::getSingleton().addCharacterToRenderToTexture(this,characterCameraCylindricCoordinates);
	std::list<CoupleOfPosesToAdd>::iterator iterator;
	for(iterator = couplesOfPosesToAdd.begin() ; iterator != couplesOfPosesToAdd.end() ; iterator++)
	{
		getBodyPart(iterator->bodyPartName)->addCoupleOfPoses(iterator->coupleOfPosesName,
			iterator->leftPoseName,
			iterator->leftPoseCameraCylindricCoordinates,
			iterator->leftStateName,
			iterator->rightPoseName,
			iterator->rightPoseCameraCylindricCoordinates,
			iterator->rightStateName);
	}

	mLoadingSuccessful = true;
}

//---------------------------------------------------------------------------------
Character::~Character()
{
    // Destroy body parts
	for(BodyPartsMap::iterator bodyPartsIterator = mBodyParts.begin();bodyPartsIterator != mBodyParts.end();bodyPartsIterator = mBodyParts.begin())
	{
        delete bodyPartsIterator->second;
        mBodyParts.erase(bodyPartsIterator);
    }
    // Destroy goodies
	for(GoodiesMap::iterator goodiesIterator = mGoodies.begin();goodiesIterator != mGoodies.end();goodiesIterator = mGoodies.begin())
	{
        delete goodiesIterator->second;
        mGoodies.erase(goodiesIterator);
    }

    // Destroy entity
    mSceneMgr->destroyEntity(mEntity);

    mMesh->removeAnimation(mName + "_CustomizationAnimation");
}

//---------------------------------------------------------------------------------
bool Character::isLoadingSuccessful()
{
	return mLoadingSuccessful;
}

//---------------------------------------------------------------------------------
String Character::getLoadingErrorMessage()
{
	return mLoadingErrorMessage;
}

//---------------------------------------------------------------------------------
String Character::getEditionNatureFilename(const String& prefix)
{
    return prefix + "_edition.nature";
}

//---------------------------------------------------------------------------------
String Character::getEditionConfFilename(const String& prefix)
{
    return prefix + "_edition.conf";
}

//---------------------------------------------------------------------------------
String Character::getEditionMeshFilename(const String& prefix)
{
    return prefix + "_edition.mesh";
}

//---------------------------------------------------------------------------------
void Character::addInstance(const String& pUidString, CharacterInstance* pCharacterInstance)
{
    mCharInstancesMap[pUidString] = pCharacterInstance; 
}

//---------------------------------------------------------------------------------
void Character::removeInstance(const String& pUidString)
{
    mCharInstancesMap.erase(pUidString);
}

//---------------------------------------------------------------------------------
void Character::addSubMesh(const MeshPtr& mesh, const String& boneName, SubMesh* subMesh, const String& subMeshName, const Vector3 &offsetPosition, const Quaternion &offsetOrientation)
{
	//getting the index of the bone of the character's mesh to which the submesh must be attached
	Skeleton::BoneIterator boneIterator = mesh->getSkeleton()->getBoneIterator();
	Bone* bone;
	int idxBone = 0;
	while((idxBone < mesh->getSkeleton()->getNumBones())&&((bone = boneIterator.getNext())->getName() != boneName)) idxBone++;

	assert( (idxBone < mesh->getSkeleton()->getNumBones()) && "Bone not found !");

	//Copying the submesh to a new submesh of the character.
	SubMesh* smeshDest = mesh->createSubMesh(subMeshName);
	smeshDest->indexData = subMesh->indexData->clone();
	smeshDest->mLodFaceList = subMesh->mLodFaceList;
	smeshDest->operationType = subMesh->operationType;
	smeshDest->parent = mesh.get();
	smeshDest->useSharedVertices = false;
	smeshDest->vertexData = subMesh->vertexData->clone();
	smeshDest->setMaterialName(subMesh->getMaterialName());
	smeshDest->clearBoneAssignments();

	//assigning all the vertices of the new submesh to the bone idxBone.
	for(unsigned int i =0 ; i < smeshDest->vertexData->vertexCount ; i++)
	{
        VertexBoneAssignment vba;
		vba.boneIndex = idxBone;
		vba.vertexIndex = i;
		vba.weight = 1.0f;
		smeshDest->addBoneAssignment(vba);
	}


	//Modifying the position and the orientation of the submesh directly in the vertexbuffer.
	const VertexElement* positionElement = smeshDest->vertexData->vertexDeclaration->findElementBySemantic(VES_POSITION);
	HardwareVertexBufferSharedPtr vertexBuffer = smeshDest->vertexData->vertexBufferBinding->getBuffer(positionElement->getSource());
	size_t vertexBufferSize = vertexBuffer->getSizeInBytes();
	size_t nbVertices = vertexBuffer->getNumVertices();
	size_t offset = positionElement->getOffset();
	size_t vertexSize = vertexBuffer->getVertexSize();
	void* vertexBufferData = new unsigned char[vertexBufferSize];
	vertexBuffer->readData(0,vertexBufferSize,vertexBufferData);

	for(unsigned int idxVertex = 0 ; idxVertex < nbVertices ; idxVertex++)
	{
		float* point = ((float*)(((unsigned char*)vertexBufferData) + offset + vertexSize*idxVertex));
		Vector3 positionIn(point[0],point[1],point[2]);

		Vector3 positionOut = (offsetOrientation*positionIn) + offsetPosition;
		positionOut = (bone->getWorldOrientation()*positionOut) + bone->getWorldPosition();

		point[0] = positionOut.x;
		point[1] = positionOut.y;
		point[2] = positionOut.z;
	}

	vertexBuffer->lock(HardwareBuffer::LockOptions::HBL_NORMAL);
	vertexBuffer->writeData(0,vertexBufferSize,vertexBufferData);
	delete[] vertexBufferData;
}

//---------------------------------------------------------------------------------
unsigned short Character::getNumAnimations()
{
	return (mEntity->hasSkeleton())? mEntity->getSkeleton()->getNumAnimations() : 0;
}

//---------------------------------------------------------------------------------
BodyPart* Character::getBodyPart(String bodyPartName)
{
	BodyPart* bodyPart = mBodyParts[bodyPartName];
	if (bodyPart == NULL) mBodyParts.erase(bodyPartName);

	return bodyPart;
}

//---------------------------------------------------------------------------------
size_t Character::getNumBodyParts()
{
	return mBodyParts.size();
}

//---------------------------------------------------------------------------------
BodyPartsIterator Character::getBodyPartsIterator()
{
	return BodyPartsIterator(mBodyParts.begin(),mBodyParts.end());
}

//---------------------------------------------------------------------------------
void Character::addBodyPart(String name, String defaultBodyPartModelSubEntityName, String defaultBodyPartModelName)
{
	assert( (getBodyPart(name) == NULL) && "BodyPart already present in character!" );

	mBodyParts[name] = new BodyPart(name,defaultBodyPartModelSubEntityName,defaultBodyPartModelName,this);
}

//---------------------------------------------------------------------------------
Skeleton::BoneIterator Character::getBonesIterator()
{
	Skeleton* skeleton = getEntity()->getSkeleton();
	if( skeleton != NULL )
		return skeleton->getBoneIterator();

	//Skeleton::BoneIterator( 0 );
	return skeleton->getBoneIterator();;
}

//---------------------------------------------------------------------------------
Goody* Character::getGoody(String goodyName)
{
	Goody* goody = mGoodies[goodyName];
	if (goody == NULL) mGoodies.erase(goodyName);

	return goody;
}

//---------------------------------------------------------------------------------
size_t Character::getNumGoodies()
{
	return mGoodies.size();
}

//---------------------------------------------------------------------------------
GoodiesIterator Character::getGoodiesIterator()
{
	return GoodiesIterator(mGoodies.begin(),mGoodies.end());
}

//---------------------------------------------------------------------------------
void Character::addGoody(String name,
		const String& boneName,
		const Vector3& minRotations, const Vector3& defaultRotations, const Vector3& maxRotations,
		const Vector3& minPosition, const Vector3& defaultPosition, const Vector3& maxPosition)
{
	assert( (getGoody(name) == NULL) && "Goody already present in character!" );

	mGoodies[name] = new Goody(name,
		boneName,
		minRotations, defaultRotations, maxRotations,
		minPosition, defaultPosition, maxPosition,
		this);
}

//---------------------------------------------------------------------------------
