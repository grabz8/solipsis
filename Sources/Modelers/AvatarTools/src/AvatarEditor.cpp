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

#include "AvatarEditor.h"
#include "CharacterManager.h"
#include "Character.h"
#include "CharacterInstance.h"
#include <SolipsisErrorHandler.h>
#include <CTLog.h>
#include <FileBrowser.h>

using namespace CommonTools;
using namespace Solipsis;

AvatarEditor* AvatarEditor::ms_singletonPtr = 0;

//-------------------------------------------------------------------------------------
AvatarEditor::AvatarEditor(const String& pPath, SceneManager* pSceneMgr) :
	mPath(pPath),
	mSceneMgr(pSceneMgr),
    archive(0),
	mCharacters(0),
    mUid(""),
	mCurrentName(""),
    mMeshFilename(""),
    mSkeletonFilename(""),
	mSceneNode(0),
	mEntity(0),
	selectType(-1)
{
    ms_singletonPtr = this;
	mCharacters = new CharacterManager(pPath, mSceneMgr);
	mExecPath = _getcwd(NULL, 0);
}
//-------------------------------------------------------------------------------------
AvatarEditor::~AvatarEditor()
{
	delete mCharacters; 
    ms_singletonPtr = 0;
}
//-------------------------------------------------------------------------------------
AvatarEditor* AvatarEditor::getSingletonPtr()
{
    return ms_singletonPtr;
}
//-------------------------------------------------------------------------------------
void AvatarEditor::buildListSAF(const String& pPathDirectory)
{
    Path charactersFolderPath(pPathDirectory.empty() ? mPath : pPathDirectory); //Folder where to find the zip archives representing the characters.

	if (SOLisDirectory(charactersFolderPath.getFormatedPath().c_str()))
	{
		std::vector<String> fileList;
		SOLlistDirectoryFiles(charactersFolderPath.getFormatedPath().c_str(),&fileList);
		std::vector<String>::iterator itFiles = fileList.begin();

		while (itFiles != fileList.end())
		{
			Path characterPath( String((*itFiles).c_str()));
			if (characterPath.getExtension() == "saf")
			{
				mCharacters->addCharacter(characterPath.getLastFileName(false));
                LOGHANDLER_LOGF(LogHandler::VL_INFO, "AvatarEditor::buildListSAF() Adding character from file %s", characterPath.getUniversalPath().c_str());
			}
			itFiles++;
		}
	}
	else
		FileBrowser::displayMessageWindow("Error","The directory for the .SAF files doesn't exist.");
}
//-------------------------------------------------------------------------------------
void AvatarEditor::setCharacterInstance(CharacterInstance* pCharacterInstance)
{
    mUid = pCharacterInstance->getUid();
    mCharacters->setCurrentInstance(pCharacterInstance);

	mSceneNode = mCharacters->getCurrentInstance()->getSceneNode();
	mEntity = mCharacters->getCurrentInstance()->getEntity();
    mMeshFilename = pCharacterInstance->getCharacter()->getMeshName();
	mSkeletonFilename = pCharacterInstance->getCharacter()->getSkeletonName();
	mCurrentName = pCharacterInstance->getCharacter()->getName();
}
//-------------------------------------------------------------------------------------
CharacterManager* AvatarEditor::getManager()
{
	return mCharacters;
}
//-------------------------------------------------------------------------------------
void AvatarEditor::updateCurrent(const String& pName)
{
    Vector3 savedPosition = mSceneNode->getPosition();
    Quaternion savedOrientation = mSceneNode->getOrientation();

    if (!mCurrentName.empty())
    {
        mCharacters->getCurrentInstance()->deleteModified();
        mCharacters->destroyCharacterInstance(mCharacters->getCurrentInstance());
    }

    CharacterInstance* characterInstance = mCharacters->loadCharacterInstance(mUid, pName);
    mCharacters->setCurrentInstance(characterInstance);

	mSceneNode = characterInstance->getSceneNode();
    mSceneNode->setPosition(savedPosition);
    mSceneNode->setOrientation(savedOrientation);
	mEntity = characterInstance->getEntity();
    mMeshFilename = characterInstance->getCharacter()->getMeshName();
	mSkeletonFilename = characterInstance->getCharacter()->getSkeletonName();
	mCurrentName = pName;
}
//-------------------------------------------------------------------------------------
bool AvatarEditor::setCurrentByName(const String& pName)
{
	if (mCurrentName != pName)
        updateCurrent(pName);
	return true;
}
//-------------------------------------------------------------------------------------
void AvatarEditor::setNextAsCurrent()
{
	String name = mCharacters->getNextFromName(mCurrentName);
	if (name == mCurrentName)
		return;
	updateCurrent(name);
}
//-------------------------------------------------------------------------------------
void AvatarEditor::setPrevAsCurrent()
{
    String name = mCharacters->getPrevFromName(mCurrentName);
	if (name == mCurrentName)
		return;
	updateCurrent(name);
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
SceneManager* AvatarEditor::getSceneManager()
{
	return mSceneMgr;
}
//-------------------------------------------------------------------------------------
SceneNode* AvatarEditor::getSceneNode()
{
	return mSceneNode;
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
    