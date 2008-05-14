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
Character::Character(String pFileName, String pName, SceneManager* pSceneMgr) :
	mSceneMgr(pSceneMgr),
	mPath(NULL),
	mZipArchive(NULL),
	mEntity(NULL),
	mNode(NULL),
	mName(pFileName),
	mSkeletonName(""),
	mMeshName(""),
	mCurrentAnimationState(NULL),
	mCustomizationAnimationState(NULL),
 	mCustomizationKeyFrame(NULL),
	mLoadingSuccessful(false),
	mBodyPart(NULL)
{
	/*
	std::string mMeshFilename = "";
	std::string mSkeletonFilename = "";
	std::string resourceGrp = "";

	if (pName == "")
		return false;

	//std::string fullFilename();
	Path path(mPath + pName);
	Path filenameExt(pName);
	std::string filename(filenameExt.getLastFileName(false));

	// verify the SAF extension
	if (filenameExt.getExtension() != "saf")
		return false;

	archive = new MyZipArchive(path.getFormatedPath());
	if (!archive->isArchivePresent())
		return false;

	// mesh filename
	if (!archive->isFilePresent(filename + ".mesh"))
		return false;
	mMeshFilename = filename + ".mesh";

	// removing the modified meshes and materials in order that Ogre doesn't load them, we will recreate them after.
	//	mZipArchive->removeFile(mName + ".mesh");
	//	mZipArchive->removeFile(mName + ".material");

	// adding the zip to the ressource location and load all the medias in the zip.
	bool find = false;
	resourceGrp = filename + "Resources";
	StringVector lstGroup = Ogre::ResourceGroupManager::getSingleton().getResourceGroups();
	for( StringVector::iterator r=lstGroup.begin(); r!=lstGroup.end(); r++ )
	{
		if((*r) == resourceGrp)
		{
			find = true;
			break;
		}
	}
	if(!find)
	{
		Ogre::ResourceGroupManager::getSingleton().createResourceGroup(resourceGrp);
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path.getUniversalPath(),"Zip",resourceGrp);
		Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(resourceGrp);
	}
	Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().load(mMeshFilename, resourceGrp);

	// skeleton filename
	if (!archive->isFilePresent(mesh->getSkeletonName()))
		return false;
	mSkeletonFilename = mesh->getSkeletonName();

	//Ogre::MeshManager::getSingleton().unload(mMeshFilename);
	delete archive;
	archive = 0;
	*/


	//Checking the validity of the archive, the presence of the edition.mesh and edition.material files.
	mPath = new Path(pFileName);
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
	if (!mZipArchive->isFilePresent(mName + "_edition.mesh"))
	{
		mLoadingErrorMessage = mName + "_edition.mesh" + "\nnot found in the character archive !"; 
		return;
	}
	mMeshName = mName + "_edition.mesh";

	//removing the modified meshes and materials in order that Ogre doesn't load them, we will recreate them after.
	mZipArchive->removeFile(mName + ".mesh");
	mZipArchive->removeFile(mName + ".material");

	//Adding the zip to the ressource location and load all the medias in the zip.
	ResourceGroupManager::getSingleton().createResourceGroup(mName + "Resources");
	ResourceGroupManager::getSingleton().addResourceLocation(mPath->getUniversalPath(),"Zip",mName + "Resources");
	ResourceGroupManager::getSingleton().initialiseResourceGroup(mName + "Resources");

	MeshPtr mesh = MeshManager::getSingleton().load(mName+"_edition.mesh", mName + "Resources");
	mMesh = mesh;

	// skeleton filename
	if (!mZipArchive->isFilePresent(mesh->getSkeletonName()))
	{
		mLoadingErrorMessage = "Skeleton file\nnot found in the character archive !"; 
		return;
	}
	mSkeletonName = mesh->getSkeletonName();

	// generate the NATURE file if does not exist
	if (!mZipArchive->isFilePresent(mName + "_edition.nature"))
	{
		if (mZipArchive->isFilePresent(mName + "_edition.conf"))
			mZipArchive->removeFile(mName + "_edition.conf");

		TiXmlDeclaration decl( "1.0", "", "" );
		TiXmlElement avatarElement("Avatar");
		avatarElement.SetAttribute("name",mName.c_str());
		avatarElement.SetAttribute("avatarCameraCylindricCoordinates","3 45 -0.5");

		//Creating BodyParts
		char name[64];
		int id = 0;
		HashMap<String,ushort> subMeshNameMap = mesh->getSubMeshNameMap();
		for(HashMap<String,ushort>::iterator sm = subMeshNameMap.begin(); sm != subMeshNameMap.end(); sm++)
		{
			sprintf(name, "%s_part%i", mName.data(), id++);

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

			avatarElement.InsertEndChild(bodyPartElement);
		}

		//Finally saving to the avatar archive
		TiXmlDocument confXmlDoc;
		confXmlDoc.InsertEndChild(decl);
		confXmlDoc.InsertEndChild(avatarElement);
		confXmlDoc.SaveFile((mName+"_edition.nature").c_str());

		FILE* confXmlFile = fopen((mName+"_edition.nature").c_str(),"rb");

		fseek(confXmlFile,0,SEEK_END);
		size_t confXmlFileSize = ftell(confXmlFile);
		fseek(confXmlFile,0,SEEK_SET);
		void* confXmlFileData = new unsigned char[confXmlFileSize];
		fread(confXmlFileData,confXmlFileSize,sizeof(unsigned char),confXmlFile);
		mZipArchive->writeFile((mName+"_edition.nature").c_str(), FileBuffer(confXmlFileData, confXmlFileSize));

		fclose(confXmlFile);

		SOLdeleteFile(Ogre::String(mName+"_edition.nature").c_str());
	}

	///Parsing xml nature file in order to perhaps load other additionnal meshs in order to add them to our mesh.
	FileBuffer xmlNatureFile = mZipArchive->readFile(mName + "_edition.nature");
	String s = xmlNatureFile.getBufferFormatedToText();
	TiXmlDocument xmlDoc;
	xmlDoc.Parse(xmlNatureFile.getBufferFormatedToText().c_str());
	if (xmlDoc.Error())
	{
		mLoadingErrorMessage = mName + "_edition.nature" + "\nisn't a valid xml file !"; 
		return;
	}

	TiXmlElement* additionnalSubMeshElement = xmlDoc.FirstChildElement("AdditionnalSubMesh");
	for(additionnalSubMeshElement ; additionnalSubMeshElement != NULL ; additionnalSubMeshElement = additionnalSubMeshElement->NextSiblingElement("AdditionnalSubMesh"))
	{
		const char* attribute = additionnalSubMeshElement->Attribute("name");
		if (attribute == NULL)
		{
			mLoadingErrorMessage = mName + "_edition.nature :" + "\nAdditionnal SubMesh must have a name !"; return;
		}
		String additionnalSubMeshName = attribute;

		attribute = additionnalSubMeshElement->Attribute("parentMesh");
		if (attribute == NULL)
		{
			mLoadingErrorMessage = mName + "_edition.nature :" + "\nAdditionnal SubMesh must have a parent mesh !"; return;
		}
		String additionnalSubMeshParentMeshName = attribute;

		attribute = additionnalSubMeshElement->Attribute("index");
		if (attribute == NULL)
		{
			mLoadingErrorMessage = mName + "_edition.nature :" + "\nAdditionnal SubMesh must have an index !"; return;
		}
		int additionnalSubMeshIndex = StringConverter::parseInt(String(attribute));

		attribute = additionnalSubMeshElement->Attribute("boneToBeAttached");
		if (attribute == NULL)
		{
			mLoadingErrorMessage = mName + "_edition.nature :" + "\nAdditionnal SubMesh must have a boneToBeAttached !"; return;
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

		addSubMesh(mesh,additionnalSubMeshBoneToBeAttachedName,additionnalSubMesh,additionnalSubMeshName,offsetPosition,offsetOrientation);

		additionnalSubMeshParentMesh->unload();
	}




	//Creating the object necessary to the Couples of poses.
	Animation* mCustomizationAnimation = mesh->createAnimation(mName+"_CustomizationAnimation",0);
	VertexAnimationTrack* mAnimationTrack;
	if (mCustomizationAnimation->hasVertexTrack(1))
		mAnimationTrack = mCustomizationAnimation->getVertexTrack(1);
	else
		mAnimationTrack = mCustomizationAnimation->createVertexTrack(1,VAT_POSE);

	mCustomizationKeyFrame = mAnimationTrack->createVertexPoseKeyFrame(0);

	//Creating nodes and entities
	if(mSceneMgr->hasSceneNode(pName))
		mNode = mSceneMgr->getSceneNode(pName);
	else
		mNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(pName);

	mEntity = mSceneMgr->createEntity(pFileName,mName+"_edition.mesh");

	mEntity->setVisible(false);
	mEntity->setNormaliseNormals(true);
	mNode->attachObject(mEntity);
	//mNode->scale(SCALE_CHARACTER,SCALE_CHARACTER,SCALE_CHARACTER);

	//All the subentities are insvisible, only the subentities parts of a BodyPart will be set visibles after.
	for(unsigned int idxSubEntity = 0 ; idxSubEntity < mEntity->getNumSubEntities() ; idxSubEntity++)
		mEntity->getSubEntity(idxSubEntity)->setVisible(false);

	//Initialising the animation state relative to the couple of poses.
	mCustomizationAnimationState = mEntity->getAnimationState(mName+"_CustomizationAnimation");
	mCustomizationAnimationState->setTimePosition(0);
	mCustomizationAnimationState->setEnabled(true);


	//calculateGravityCenter();


	//Setting the animation.
	if (getNumAnimations() > 0)
		setAnimationMode(ANIMATION_MODE_PERIODIC);
	else
		setAnimationMode(ANIMATION_MODE_NO_ANIMATION);

	//We will need the following variables for parsing the xml nature file
	Vector3 avatarCameraCylindricCoordinates; //we will memorise it one time we met him in the file. But we won't immediately add the avatar to render to texture because the loading can abort after and so we will destroy the avatar.

	std::list<CoupleOfPosesToAdd> couplesOfPosesToAdd;			//same thing as avatarCameraCylindricCoordinates, we want to save the information for adding a couple of poses without adding it.


	//Parsing xml nature file
	TiXmlElement* avatarElement = xmlDoc.FirstChildElement("Avatar");
	if (avatarElement == NULL)
	{
		mLoadingErrorMessage = mName + "_edition.nature :" + "\nNo Character markup found !"; 
		return;
	}

	const char* pName = avatarElement->Attribute("name");
	if (pName == NULL)
	{
		mLoadingErrorMessage = mName + "_edition.nature :" + "\nNo name's attribute found !"; 
		return;
	}
	mCompleteName = pName;

	pName = avatarElement->Attribute("avatarCameraCylindricCoordinates");
	if (pName == NULL)
	{
		mLoadingErrorMessage = mName + "_edition.nature :" + "\nNo avatarCameraCylindricCoordinates found !\nneeded for taking avatar photo for the gallery."; 
		return;
	}
	avatarCameraCylindricCoordinates = StringConverter::parseVector3(String(pName));


	//Parsing every BodyPart Element
	TiXmlElement* bodyPartElement = avatarElement->FirstChildElement("BodyPart");
	for(bodyPartElement ; bodyPartElement != NULL ; bodyPartElement = bodyPartElement->NextSiblingElement("BodyPart"))
	{
		//BodyPart's name
		pName = bodyPartElement->Attribute("name");
		if (pName == NULL)
		{
			mLoadingErrorMessage = mName + "_edition.nature :" + "\nBody part must have a name !"; 
			return;
		}
		String bodyPartName(pName);

		//Parsing every BodyPartModel Element
		TiXmlElement* bodyPartModelElement = bodyPartElement->FirstChildElement("BodyPartModel");
		if (bodyPartModelElement == NULL)
		{
			mLoadingErrorMessage = mName + "_edition.nature :" + "\nBody part must have at least one model !"; 
			return;
		}
		bool firstBodyPartModel = true;
		for(bodyPartModelElement ; bodyPartModelElement != NULL ; bodyPartModelElement = bodyPartModelElement->NextSiblingElement("BodyPartModel"))
		{
			//BodyPartModel's name
			pName = bodyPartModelElement->Attribute("name");
			if (pName == NULL)
			{
				mLoadingErrorMessage = mName + "_edition.nature :" + "\nBody part model must have a name !"; 
				return;
			}
			String bodyPartModelName(pName);

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
					mLoadingErrorMessage = mName + "_edition.nature :" + "\nBody part model must have one (and only one) SubMesh !"; 
					return;
				}

				pName = subMesh->Attribute("name");
				if (pName == NULL)
				{
					mLoadingErrorMessage = mName + "_edition.nature :" + "\nSubmesh must have a name !"; 
					return;
				}
				subMeshName = pName;

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
					pName = textureElement->Attribute("name");
					if (pName == NULL)
					{
						mLoadingErrorMessage = mName + "_edition.nature :" + "\nTexture must have a name !"; return;
					}
					String textureName = pName;
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
				mLoadingErrorMessage = mName + "_edition.nature :" + "\nCoupleOfPoses must have a name !"; return;
			}
			String coupleOfPosesName(attribute);

			attribute = coupleOfPosesElement->Attribute("leftPoseName");
			if (attribute == NULL)
			{
				mLoadingErrorMessage = mName + "_edition.nature :" + "\nTexture must have a leftPoseName !"; return;
			}
			String leftPoseName(attribute);

			attribute = coupleOfPosesElement->Attribute("leftPoseCameraCylindricCoordinates");
			if (attribute == NULL)
			{
				mLoadingErrorMessage = mName + "_edition.nature :" + "\nTexture must have a leftPoseCameraCylindricCoordinates !"; return;
			}
			const Vector3& leftPoseCameraCylindricCoordinates = StringConverter::parseVector3(String(attribute));

			attribute = coupleOfPosesElement->Attribute("leftStateName");
			if (attribute == NULL)
			{
				mLoadingErrorMessage = mName + "_edition.nature :" + "\nTexture must have a leftStateName !"; return;
			}
			String leftStateName(attribute);

			attribute = coupleOfPosesElement->Attribute("rightPoseName");
			if (attribute == NULL)
			{
				mLoadingErrorMessage = mName + "_edition.nature :" + "\nTexture must have a rightPoseName !"; return;
			}
			String rightPoseName(attribute);

			attribute = coupleOfPosesElement->Attribute("rightPoseCameraCylindricCoordinates");
			if (attribute == NULL)
			{
				mLoadingErrorMessage = mName + "_edition.nature :" + "\nTexture must have a rightPoseCameraCylindricCoordinates !"; return;
			}
			const Vector3& rightPoseCameraCylindricCoordinates = StringConverter::parseVector3(String(attribute));

			attribute = coupleOfPosesElement->Attribute("rightStateName");
			if (attribute == NULL)
			{
				mLoadingErrorMessage = mName + "_edition.nature :" + "\nTexture must have a rightStateName !"; return;
			}
			String rightStateName(attribute);


			CoupleOfPosesToAdd coupleOfPosesToAdd = { bodyPartName, coupleOfPosesName, leftPoseName, leftPoseCameraCylindricCoordinates, leftStateName, rightPoseName, rightPoseCameraCylindricCoordinates, rightStateName };
			couplesOfPosesToAdd.push_back(coupleOfPosesToAdd);
		}
	}


	//Parsing every Goody Element
	TiXmlElement* goodyElement = avatarElement->FirstChildElement("Goody");
	for(goodyElement ; goodyElement != NULL ; goodyElement = goodyElement->NextSiblingElement("Goody"))
	{
		//Goody's name
		pName = goodyElement->Attribute("name");
		if (pName == NULL)
		{
			mLoadingErrorMessage = mName + "_edition.nature :" + "\nGoody must have a name !"; return;
		}
		String goodyName(pName);

		//Goody's boneName
		pName = goodyElement->Attribute("boneName");
		assert( (pName != NULL) && "Goody must have a boneName !");
		if (pName == NULL)
		{
			mLoadingErrorMessage = mName + "_edition.nature :" + "\nGoody must have a boneName !"; return;
		}
		String boneName(pName);

		//Goody's minRotations
		pName = goodyElement->Attribute("minRotations");
		if (pName == NULL)
		{
			mLoadingErrorMessage = mName + "_edition.nature :" + "\nGoody must have minRotations !"; return;
		}
		Vector3 minRotations = StringConverter::parseVector3(String(pName));

		//Goody's defaultRotations
		pName = goodyElement->Attribute("defaultRotations");
		if (pName == NULL)
		{
			mLoadingErrorMessage = mName + "_edition.nature :" + "\nGoody must have defaultRotations !"; return;
		}
		Vector3 defaultRotations = StringConverter::parseVector3(String(pName));

		//Goody's maxRotations
		pName = goodyElement->Attribute("maxRotations");
		if (pName == NULL)
		{
			mLoadingErrorMessage = mName + "_edition.nature :" + "\nGoody must have maxRotations !"; return;
		}
		Vector3 maxRotations = StringConverter::parseVector3(String(pName));

		//Goody's minPosition
		pName = goodyElement->Attribute("minPosition");
		if (pName == NULL)
		{
			mLoadingErrorMessage = mName + "_edition.nature :" + "\nGoody must have minPosition !"; return;
		}
		Vector3 minPosition = StringConverter::parseVector3(String(pName));

		//Goody's defaultPosition
		pName = goodyElement->Attribute("defaultPosition");
		if (pName == NULL)
		{
			mLoadingErrorMessage = mName + "_edition.nature :" + "\nGoody must have defaultPosition !"; return;
		}
		Vector3 defaultPosition = StringConverter::parseVector3(String(pName));

		//Goody's maxPosition
		pName = goodyElement->Attribute("maxPosition");
		if (pName == NULL)
		{
			mLoadingErrorMessage = mName + "_edition.nature :" + "\nGoody must have maxPosition !"; return;
		}
		Vector3 maxPosition = StringConverter::parseVector3(String(pName));


		//Creating Goody
		addGoody(goodyName,boneName,
			minRotations,defaultRotations,maxRotations,
			minPosition,defaultPosition,maxPosition);
		Goody* goody = getGoody(goodyName);



		//Parsing every GoodyModel Element
		TiXmlElement* goodyModelElement = goodyElement->FirstChildElement("GoodyModel");
		if (goodyModelElement == NULL)
		{
			mLoadingErrorMessage = mName + "_edition.nature :" + "\nGoody must have at least one model !"; return;
		}
		for(goodyModelElement ; goodyModelElement != NULL ; goodyModelElement = goodyModelElement->NextSiblingElement("GoodyModel"))
		{
			//GoodyModel's name
			pName = goodyModelElement->Attribute("name");
			if (pName == NULL)
			{
				mLoadingErrorMessage = mName + "_edition.nature :" + "\nGoody model must have a name !"; return;
			}
			String goodyModelName(pName);

			//GoodyModel's MeshName
			pName = goodyModelElement->Attribute("meshName");
			if (pName == NULL)
			{
				mLoadingErrorMessage = mName + "_edition.nature :" + "\nGoody model must have a meshName !"; return;
			}
			String goodyModelMeshName(pName);


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
				pName = textureElement->Attribute("name");
				if (pName == NULL)
				{
					mLoadingErrorMessage = mName + "_edition.nature :" + "\nTexture must have a name !" ; return;
				}
				String textureName = pName;
				TexturePtr texture = TextureManager::getSingleton().load(textureName,ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
				goodyModel->addTexture(texture);
			}
		}
	}


	//The loading has been successful, we can finally render the avatar and all his couples of poses to textures.
//GILLES	ScreenshotManager::getSingleton().addAvatarToRenderToTexture(this,avatarCameraCylindricCoordinates);
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


	//loading the configuration saved in a precedent execution of the program
	loadModified();	

	mLoadingSuccessful = true;

	//re-save the mesh ready to use for the solipsis application.
//	saveModified();
}
//---------------------------------------------------------------------------------
Character::~Character()
{}
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
void Character::loadModified()
{
	//Parsing XML configuration file
	if (!(mZipArchive->isFilePresent(mName + "_edition.conf"))) 
		return;

	FileBuffer xmlConfFile = mZipArchive->readFile(mName + "_edition.conf");
	TiXmlDocument xmlDoc;
	xmlDoc.Parse(xmlConfFile.getBufferFormatedToText().c_str());
	if (xmlDoc.Error()) 
		return;
	
	TiXmlElement* avatarElement = xmlDoc.FirstChildElement("Avatar");
	if (avatarElement == NULL) 
		return;

	//Parsing every BodyPart Element
	TiXmlElement* bodyPartElement = avatarElement->FirstChildElement("BodyPart");
	for(bodyPartElement ; bodyPartElement ; bodyPartElement = bodyPartElement->NextSiblingElement("BodyPart"))
	{
		//Checking BodyPart
		const char* pName = bodyPartElement->Attribute("name");
		if (pName == NULL) 
			return;
		String bodyPartName(pName);

		if (getBodyPart(bodyPartName) == NULL) 
			return;
		BodyPart* bodyPart = getBodyPart(bodyPartName);

		//Checking BodyPartModel
		pName = bodyPartElement->Attribute("bodyPartModel");
		if (pName == NULL) 
			return;
		String bodyPartModelName(pName);

		if (bodyPartModelName != "None")
		{
			if (bodyPart->getBodyPartModel(bodyPartModelName) == NULL) 
				return;
			BodyPartModel* bodyPartModel = bodyPart->getBodyPartModel(bodyPartModelName);
			bodyPart->setCurrentBodyPartModel(bodyPartModelName);

			//Checking BodyPartModel Colour
			pName = bodyPartElement->Attribute("colour");
			if (pName != NULL)
			{
				String stringColour(pName);
				const ColourValue& bodyPartColour = StringConverter::parseColourValue(stringColour);
				if (bodyPartModel->isColourModifiable())
					bodyPartModel->setColour(bodyPartColour);
			}

			//Checking BodyPartModel textures
			pName = bodyPartElement->Attribute("texture");
			if (pName != NULL)
			{
				String textureName(pName);
				TexturePtr texture = bodyPartModel->getTexture(textureName);
				if (!texture.isNull())
					bodyPartModel->setCurrentTexture(texture);
			}
		}

		//Checking CouplesOfPoses
		TiXmlElement* coupleOfPosesElement = bodyPartElement->FirstChildElement("CoupleOfPoses");
		for(coupleOfPosesElement; coupleOfPosesElement; coupleOfPosesElement = coupleOfPosesElement->NextSiblingElement("CoupleOfPoses"))
		{
			pName = coupleOfPosesElement->Attribute("name");
			if (pName == NULL) 
				return;
			String coupleOfPosesName(pName);
			if (bodyPart->getCoupleOfPoses(coupleOfPosesName) == NULL) 
				return;
			CoupleOfPoses* coupleOfPoses = bodyPart->getCoupleOfPoses(coupleOfPosesName);

			pName = coupleOfPosesElement->Attribute("position"); 
			if (pName == NULL) 
				return;
			float position = StringConverter::parseReal(String(pName));

			coupleOfPoses->setPosition(position);
			updatePoseReference(coupleOfPoses->getLeftPoseIndex(),1.0f - coupleOfPoses->getPosition());
			updatePoseReference(coupleOfPoses->getRightPoseIndex(),coupleOfPoses->getPosition());
		}
	}
	
	//Parsing every Goody Element
	TiXmlElement* goodyElement = avatarElement->FirstChildElement("Goody");
	for(goodyElement ; goodyElement ; goodyElement = goodyElement->NextSiblingElement("Goody"))
	{
		//Checking BodyPart
		const char* name = goodyElement->Attribute("name");
		if (name == NULL) 
			return;
		String goodyName(name);

		if (getGoody(goodyName) == NULL) 
			return;
		Goody* goody = getGoody(goodyName);

		//Checking GoodyModel
		name = goodyElement->Attribute("goodyModel");
		if (name == NULL) 
			return;
		String goodyModelName(name);

		if (goodyModelName != "None")
		{
			if (goody->getGoodyModel(goodyModelName) == NULL) 
				return;
			GoodyModel* goodyModel = goody->getGoodyModel(goodyModelName);
			goody->setCurrentGoodyModel(goodyModelName);

			//Checking for position.
			name = goodyElement->Attribute("positionScrollPositions");
			if (name != NULL)
			{
				Vector3 positionScrollPositions = StringConverter::parseVector3(String(name));
				goody->setCurrentXScrollPosition(positionScrollPositions.x);
				goody->setCurrentYScrollPosition(positionScrollPositions.y);
				goody->setCurrentZScrollPosition(positionScrollPositions.z);
			}

			//Checking for rotation.
			name = goodyElement->Attribute("rotationsScrollPositions");
			if (name != NULL)
			{
				Vector3 rotationsScrollPositions = StringConverter::parseVector3(String(name));
				goody->setCurrentYawAngleScrollPosition(rotationsScrollPositions.x);
				goody->setCurrentPitchAngleScrollPosition(rotationsScrollPositions.y);
				goody->setCurrentRollAngleScrollPosition(rotationsScrollPositions.z);
			}

			//Checking GoodyModel Colour
			name = goodyElement->Attribute("colour");
			if (name != NULL)
			{
				String stringColour(name);
				const ColourValue& goodyColour = StringConverter::parseColourValue(stringColour);
				if (goodyModel->isColourModifiable())
					goodyModel->setColour(goodyColour);
			}

			//Checking GoodyModel textures
			name = goodyElement->Attribute("texture");
			if (name != NULL)
			{
				String textureName(name);
				TexturePtr texture = goodyModel->getTexture(textureName);
				if (!texture.isNull())
					goodyModel->setCurrentTexture(texture);
			}
		}
	}
}
//---------------------------------------------------------------------------------
void Character::saveModified()
{
	//Creating xml configuration file
	TiXmlElement avatarElement("Avatar");
	avatarElement.SetAttribute("name",mName.c_str());

	//Creating BodyParts
	BodyPartsIterator bodyPartsIterator = getBodyPartsIterator();
	while(bodyPartsIterator.hasMoreElements())
	{
		//Creating BodyPartModel
		BodyPart* bodyPart = bodyPartsIterator.getNext();
		TiXmlElement bodyPartElement("BodyPart");
		bodyPartElement.SetAttribute("name",bodyPart->getName().c_str());
		bodyPartElement.SetAttribute("bodyPartModel",bodyPart->getCurrentBodyPartModelName().c_str());
		if (bodyPart->getCurrentBodyPartModel() != NULL)
		{
			BodyPartModel* bodyPartModel = bodyPart->getCurrentBodyPartModel();

            if (bodyPartModel->isColourModifiable()) bodyPartElement.SetAttribute("colour",StringConverter::toString(bodyPartModel->getColour()).c_str());
			if (bodyPartModel->isTextureModifiable()) bodyPartElement.SetAttribute("texture",bodyPartModel->getCurrentTexture()->getName().c_str());
		}

		//Creating couples of poses for the BodyPart
		CouplesOfPosesIterator couplesOfPosesIterator = bodyPart->getCouplesOfPosesIterator();
		while(couplesOfPosesIterator.hasMoreElements())
		{
			CoupleOfPoses* coupleOfPoses = couplesOfPosesIterator.getNext();
			
			TiXmlElement coupleOfPosesElement("CoupleOfPoses");
			coupleOfPosesElement.SetAttribute("name",coupleOfPoses->getName().c_str());
			coupleOfPosesElement.SetAttribute("position",StringConverter::toString(coupleOfPoses->getPosition()).c_str());
			bodyPartElement.InsertEndChild(coupleOfPosesElement);
		}

		avatarElement.InsertEndChild(bodyPartElement);
	}

	//Creating Goodies
	GoodiesIterator goodiesIterator = getGoodiesIterator();
	while(goodiesIterator.hasMoreElements())
	{
		//Creating GoodyModel
		Goody* goody = goodiesIterator.getNext();
		TiXmlElement goodyElement("Goody");
		goodyElement.SetAttribute("name",goody->getName().c_str());
		goodyElement.SetAttribute("goodyModel",goody->getCurrentGoodyModelName().c_str());
		if (goody->getCurrentGoodyModel() != NULL)
		{
			GoodyModel* goodyModel = goody->getCurrentGoodyModel();
			goodyElement.SetAttribute("positionScrollPositions",StringConverter::toString(Vector3(goody->getCurrentXScrollPosition(),goody->getCurrentYScrollPosition(),goody->getCurrentZScrollPosition())).c_str());
			goodyElement.SetAttribute("rotationsScrollPositions",StringConverter::toString(Vector3(goody->getCurrentYawAngleScrollPosition(),goody->getCurrentPitchAngleScrollPosition(),goody->getCurrentRollAngleScrollPosition())).c_str());

			if (goodyModel->isColourModifiable()) goodyElement.SetAttribute("colour",StringConverter::toString(goodyModel->getColour()).c_str());
			if (goodyModel->isTextureModifiable()) goodyElement.SetAttribute("texture",goodyModel->getCurrentTexture()->getName().c_str());
		}

		avatarElement.InsertEndChild(goodyElement);
	}
		

	FileBuffer xmlNatureFile = mZipArchive->readFile(mName + "_edition.nature");
	TiXmlDocument xmlDoc;
	xmlDoc.Parse(xmlNatureFile.getBufferFormatedToText().c_str());



	//Finally saving to the avatar archive
	TiXmlDocument confXmlDoc; 
	confXmlDoc.InsertEndChild(avatarElement);
	confXmlDoc.SaveFile((mName+"_edition.conf").c_str());

	FILE* confXmlFile = fopen((mName+"_edition.conf").c_str(),"rb");

	fseek(confXmlFile,0,SEEK_END);
	size_t confXmlFileSize = ftell(confXmlFile);
	fseek(confXmlFile,0,SEEK_SET);
	void* confXmlFileData = new unsigned char[confXmlFileSize];
	fread(confXmlFileData,confXmlFileSize,sizeof(unsigned char),confXmlFile);
	mZipArchive->writeFile((mName+"_edition.conf").c_str(), FileBuffer(confXmlFileData, confXmlFileSize));

	fclose(confXmlFile);

	SOLdeleteFile(Ogre::String(mName+"_edition.conf").c_str());


//Creating a mesh very easily usable for others applications (Sollipsis).
	String modifiedMeshName(mName+"Modified.mesh");

	//Creating new .mesh and .material
	MeshPtr mesh = getMesh()->clone(modifiedMeshName);
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
				addSubMesh(mesh,goody->getBoneName(),goody->getCurrentGoodyModel()->getEntity()->getSubEntity(idxSubEntity)->getSubMesh(),goody->getName()+StringConverter::toString(idxSubEntity),goody->getCurrentPosition(),goody->getCurrentOrientation());
			}
		}
	}

	//Modifing vertex data according to couples of poses.
	BodyPartsIterator bodyPartIterator = getBodyPartsIterator();
	while(bodyPartIterator.hasMoreElements())
	{
		BodyPart* bodyPart = bodyPartIterator.getNext();
		CouplesOfPosesIterator couplesOfPosesIterator = bodyPart->getCouplesOfPosesIterator();
		while(couplesOfPosesIterator.hasMoreElements())
		{
			CoupleOfPoses* coupleOfPoses = couplesOfPosesIterator.getNext();

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
	meshSerializer.exportMesh(mesh.get(),mName + ".mesh");
	
	FILE* meshFile = fopen((mName + ".mesh").c_str(),"rb");

	fseek(meshFile,0,SEEK_END);
	size_t meshFileSize = ftell(meshFile);
	fseek(meshFile,0,SEEK_SET);
	void* meshFileData = new unsigned char[meshFileSize];
	fread(meshFileData,meshFileSize,sizeof(unsigned char),meshFile);
	mZipArchive->writeFile((mName + ".mesh").c_str(), FileBuffer(meshFileData, meshFileSize));


	fclose(meshFile);

	SOLdeleteFile(Ogre::String(mName+".mesh").c_str());



	//material now
	MaterialSerializer materialSerializer;
	bool emptyQueue = true;
	for(unsigned short idxSubMesh = 0 ; idxSubMesh < mesh->getNumSubMeshes() ; idxSubMesh++)
	{
		emptyQueue = false;
		materialSerializer.queueForExport(MaterialManager::getSingleton().getByName(mesh->getSubMesh(idxSubMesh)->getMaterialName()));
	}
	if (!emptyQueue) materialSerializer.exportQueued(mName + ".material");

	FILE* materialFile = fopen((mName + ".material").c_str(),"rb");

	fseek(materialFile,0,SEEK_END);
	size_t materialFileSize = ftell(materialFile);
	fseek(materialFile,0,SEEK_SET);
	void* materialFileData = new unsigned char[materialFileSize];
	fread(materialFileData,materialFileSize,sizeof(unsigned char),materialFile);
	mZipArchive->writeFile((mName + ".material").c_str(), FileBuffer(materialFileData, materialFileSize));

	fclose(materialFile);

	SOLdeleteFile(Ogre::String(mName+".material").c_str());
	
	//Unloading created mesh and removing it from the resource manager
	mesh->unload();
	MeshManager::getSingleton().remove(mesh->getName());
}
//---------------------------------------------------------------------------------
void Character::addSubMesh(const MeshPtr& mesh, const String& boneName, SubMesh* subMesh, const String& subMeshName, const Vector3 &offsetPosition, const Quaternion &offsetOrientation)
{
	//getting the index of the bone of the avatar's mesh to which the submesh must be attached
	Skeleton::BoneIterator boneIterator = mesh->getSkeleton()->getBoneIterator();
	Bone* bone;
	int idxBone = 0;
	while((idxBone < mesh->getSkeleton()->getNumBones())&&((bone = boneIterator.getNext())->getName() != boneName)) idxBone++;

	assert( (idxBone < mesh->getSkeleton()->getNumBones()) && "Bone not found !");

	//Copying the submesh to a new submesh of the Avatar.
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
void Character::addPoseReference(ushort poseIndex, Real influence)
{
	mCustomizationKeyFrame->addPoseReference(poseIndex,influence);
}

//---------------------------------------------------------------------------------
void Character::updatePoseReference(ushort indexPose,float influence)
{
	mCustomizationKeyFrame->updatePoseReference(indexPose,influence);
	mCustomizationAnimationState->getParent()->_notifyDirty();
}
//---------------------------------------------------------------------------------
unsigned short Character::getNumAnimations()
{
	return (mEntity->hasSkeleton())? mEntity->getSkeleton()->getNumAnimations() : 0;
}

//---------------------------------------------------------------------------------
unsigned short Character::getAnimationMode()
{
	return mAnimationMode;
}

//---------------------------------------------------------------------------------
void Character::setAnimationMode(unsigned short animationMode)
{
	if (getNumAnimations() == 0) return; //Nothing to do, the current mode is and still will be ANIMATION_NO_ANIMATION.

	assert((animationMode >= 0) && (animationMode < 4));

	mAnimationMode = animationMode;
	if ((mAnimationMode != Character::ANIMATION_MODE_NO_ANIMATION) && (mCurrentAnimationState == NULL)) 
		setCurrentAnimation(0); //If the current anim state is NULL (and the mode is different from no animation), as it is the case at the beginning of the application, we must set an animation. 
}

//---------------------------------------------------------------------------------
unsigned short Character::getCurrentAnimation()
{
	return mCurrentAnimation;
}

//---------------------------------------------------------------------------------
void Character::setCurrentAnimation(unsigned short idAnimation)
{
	if (getNumAnimations() == 0) return;

	assert((idAnimation >= 0) && (idAnimation < getNumAnimations()));

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
void Character::animate(Real elapsedTime)
{
	if (getNumAnimations() == 0) return;

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
		if (getNumAnimations() > 1) //In this case we want that in case of changement of animation, the new animation is different from the old one (if not the same animation will seem 'cut')
		{
			timeSinceLastCurrentAnimationChange += elapsedTime;
			if (timeSinceLastCurrentAnimationChange >= ANIMATION_PERIOD) //the period time has been reached, we have to change randomly the animation.
			{
				timeSinceLastCurrentAnimationChange -= ANIMATION_PERIOD;
				unsigned short idAnimation = mCurrentAnimation;
				while(idAnimation == mCurrentAnimation)
				{
					idAnimation = (unsigned short) (((float)rand()/RAND_MAX) * getNumAnimations());
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
float Character::getAnimationPosition()
{
	if (getNumAnimations() == 0) return 0;

	return (mCurrentAnimationState->getTimePosition() / mCurrentAnimationState->getLength());
}

//---------------------------------------------------------------------------------
void Character::setAnimationPosition(Real position)
{
	if ((mAnimationMode != ANIMATION_MODE_EDITION_MANUAL)||(getNumAnimations() == 0)) return;

	assert((position >= 0) && (position <= 1));

	mCurrentAnimationState->setTimePosition(position * mCurrentAnimationState->getLength());
}

//---------------------------------------------------------------------------------
void Character::resetModifications()
{}
//---------------------------------------------------------------------------------
String Character::getName()
{
	return mName;
}
//---------------------------------------------------------------------------------
String Character::getCompleteName()
{
	return mCompleteName;
}
//---------------------------------------------------------------------------------
Entity* Character::getEntity()
{
	return mEntity;
}
//---------------------------------------------------------------------------------
SceneNode* Character::getNode()
{
	return mNode;
}
//---------------------------------------------------------------------------------
MeshPtr Character::getMesh()
{
	return mMesh;
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
BodyPart* Character::getCurrentBodyPart()
{
	if(!mBodyPart)
		mBodyPart = (*mBodyParts.begin()).second;

	return mBodyPart;
}

//---------------------------------------------------------------------------------
BodyPart* Character::setNextBodyPartAsCurrent()
{
	std::map<String,BodyPart*>::iterator iter = mBodyParts.begin();
	mBodyPart = getCurrentBodyPart();
	while(iter != mBodyParts.end())
	{
		if((*iter).second == mBodyPart)
		{
			iter++;
			if(iter == mBodyParts.end())
				iter = mBodyParts.begin();
			break;
		}

		iter++;
	}
	mBodyPart = (*iter).second;

	return mBodyPart;
}

//---------------------------------------------------------------------------------
BodyPart* Character::setPreviousBodyPartAsCurrent()
{
	std::map<String,BodyPart*>::iterator iter = mBodyParts.begin();
	mBodyPart = getCurrentBodyPart();
	while(iter != mBodyParts.end())
	{
		if((*iter).second == mBodyPart)
		{
			if(iter == mBodyParts.begin())
				iter = mBodyParts.end();
			iter--;
			break;
		}

		iter++;
	}
	mBodyPart = (*iter).second;

	return mBodyPart;
}

//---------------------------------------------------------------------------------
void Character::addBodyPart(String name, String defaultBodyPartModelSubEntityName, String defaultBodyPartModelName)
{
	assert( (getBodyPart(name) == NULL) && "BodyPart already present in Avatar!" );

	mBodyParts[name] = new BodyPart(name,defaultBodyPartModelSubEntityName,defaultBodyPartModelName,this);
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
	assert( (getGoody(name) == NULL) && "Goody already present in Avatar!" );

	mGoodies[name] = new Goody(name,
		boneName,
		minRotations, defaultRotations, maxRotations,
		minPosition, defaultPosition, maxPosition,
		this);
}
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
