#include "AvatarEditor.h"
#include "CharacterManager.h"
#include "Character.h"
#include <SolipsisErrorHandler.h>
#include <FileBrowser.h>

using namespace Solipsis;

AvatarEditor* AvatarEditor::ms_singletonPtr = 0;

//-------------------------------------------------------------------------------------
AvatarEditor::AvatarEditor(std::string pPath, SceneManager* pSceneMgr) :
	mPath(pPath),
	mSceneMgr(pSceneMgr),
    archive(0),
	mAvatars(0),
	mUidString(""),
	mCurrentName(""),
    mMeshFilename(""),
    mSkeletonFilename(""),
	mNode(0),
	mEntity(0)
{
    ms_singletonPtr = this;
	mAvatars = new CharacterManager(pPath, mSceneMgr);
	mExecPath = _getcwd(NULL, 0);
}
//-------------------------------------------------------------------------------------
AvatarEditor::~AvatarEditor()
{
	delete mAvatars; 

	delete ms_singletonPtr;
    ms_singletonPtr = 0;
}
//-------------------------------------------------------------------------------------
AvatarEditor* AvatarEditor::getSingletonPtr()
{
    return ms_singletonPtr;
}
//-------------------------------------------------------------------------------------
void AvatarEditor::buildListSAF(std::string pPathDirectory)
{
	if(pPathDirectory == "") 
		pPathDirectory = mPath;

	Path avatarsFolderPath(pPathDirectory); //Folder where to find the zip archives representing the avatars.

	if (SOLisDirectory(avatarsFolderPath.getFormatedPath().c_str()))
	{
		std::vector<std::string> fileList;
		SOLlistDirectoryFiles(avatarsFolderPath.getFormatedPath().c_str(),&fileList);
		std::vector<std::string>::iterator itFiles = fileList.begin();

		while (itFiles != fileList.end())
		{
			Path avatarPath( String((*itFiles).c_str()));
			if (avatarPath.getExtension() == "saf")
			{
				mAvatars->addCharacter(avatarPath.getLastFileName(true));
				SOLIPSISINFO("Adding avatar from file :",avatarPath.getUniversalPath().c_str());
			}
			itFiles++;
		}
	}
	else
		FileBrowser::displayMessageWindow("Error","The directory for the .SAF files doesn't existe.");
}
//-------------------------------------------------------------------------------------
void AvatarEditor::setUid(String pUid)
{
	mUidString = pUid;
	mAvatars->setUid( pUid );
}
//-------------------------------------------------------------------------------------
CharacterManager* AvatarEditor::getManager()
{
	return mAvatars;
}
//-------------------------------------------------------------------------------------
void AvatarEditor::updateCurrent(Character* pAvatar)
{
	mMeshFilename = pAvatar->getMeshName();
	mSkeletonFilename = pAvatar->getSkeletonName();
	mCurrentName = pAvatar->getName(); //pName;

	mNode = mAvatars->getCurrent()->getNode();
	if(mEntity)
		mEntity->setVisible(false);
	mEntity = mAvatars->getCurrent()->getEntity();
	mEntity->setVisible(true);
}
//-------------------------------------------------------------------------------------
bool AvatarEditor::setCurrentByName(std::string pName)
{
	if(mCurrentName != pName)
	{
		Character* avatar = mAvatars->getByName(pName);
		updateCurrent(avatar);
	}

	return true;
}
//-------------------------------------------------------------------------------------
void AvatarEditor::setNextAsCurrent()
{
	Character* avatar = mAvatars->getNextFromName(mCurrentName);
	if(avatar->getName() == mCurrentName)
		return;

	updateCurrent(avatar);
}
//-------------------------------------------------------------------------------------
void AvatarEditor::setPrevAsCurrent()
{
	Character* avatar = mAvatars->getPrevFromName(mCurrentName);
	if(avatar->getName() == mCurrentName)
		return;

	updateCurrent(avatar);
}
//-------------------------------------------------------------------------------------
String AvatarEditor::getName()
{
	return mCurrentName;
}
//-------------------------------------------------------------------------------------
String AvatarEditor::getMeshName()
{
    return mMeshFilename;
}
//-------------------------------------------------------------------------------------
String AvatarEditor::getSkeletonName()
{
    return mSkeletonFilename;
}
//-------------------------------------------------------------------------------------
SceneNode* AvatarEditor::getSceneNode()
{
	return mNode;
}
//-------------------------------------------------------------------------------------
Entity* AvatarEditor::getEntity()
{
	return mEntity;
}
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
    