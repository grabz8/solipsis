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

#include "CharacterManager.h"
#include "Character.h"
#include "CharacterInstance.h"
#include <Path.h>
#include <FileBrowser.h>
#include <MyZipArchive.h>
 
using namespace Solipsis;

CharacterManager* CharacterManager::ms_singletonPtr = 0;

//-------------------------------------------------------------------------------------
CharacterManager::CharacterManager(String pPath, SceneManager* pSceneMgr) :
	mSceneMgr(pSceneMgr),
	mCurrentInstance(0)
{
	mPath = pPath + "/";
	ms_singletonPtr = this;
    mDefaultCharacterName = "";
}
//-------------------------------------------------------------------------------------
CharacterManager::~CharacterManager() {}
//-------------------------------------------------------------------------------------
CharacterManager* CharacterManager::getSingletonPtr()
{
    return ms_singletonPtr;
}
//-------------------------------------------------------------------------------------
bool CharacterManager::addCharacter(const String& pName)
{
    if (mDefaultCharacterName.empty())
        mDefaultCharacterName = pName;
	mCharacters[pName] = NULL;
	mNameList.push_back(pName);
	return true;
}
//-------------------------------------------------------------------------------------
Character* CharacterManager::loadCharacter(const String& pName)
{
	Character* character = mCharacters[pName];
	if (character == NULL) 
	{
		Path completName(mPath + pName + ".saf");

        character = new Character(completName.getUniversalPath(), mSceneMgr);
		//If the loading of the character has passed, we displayed an error message and delete the character.
		if (!character->isLoadingSuccessful())
		{
			FileBrowser::displayMessageWindow("Error loading the character",character->getLoadingErrorMessage().c_str());
			delete character;
			return NULL;
		}
		mCharacters[pName] = character;
	}
	
	return character;
}
//---------------------------------------------------------------------------------
CharacterInstance* CharacterManager::loadCharacterInstance(const String& pUidString, const String& pDefaultCharacterName)
{
    String defaultCharacterName = pDefaultCharacterName;
    if (defaultCharacterName.empty())
        defaultCharacterName = mDefaultCharacterName;
    CharacterInstance* instance = new CharacterInstance(mPath + pUidString + ".sif", pUidString, defaultCharacterName, mSceneMgr, this);
    return instance;
}
//---------------------------------------------------------------------------------
void CharacterManager::destroyCharacterInstance(CharacterInstance* pCharacterInstance)
{
    if (mCurrentInstance == pCharacterInstance)
        mCurrentInstance = 0;
    delete pCharacterInstance;
}
//-------------------------------------------------------------------------------------
void CharacterManager::setCurrentInstance(CharacterInstance* pCharacterInstance)
{
	mCurrentInstance = pCharacterInstance;
}
//-------------------------------------------------------------------------------------
CharacterInstance* CharacterManager::getCurrentInstance()
{
	return mCurrentInstance;
}
//-------------------------------------------------------------------------------------
String CharacterManager::getNextFromName(const String& pName)
{
	CharactersMap::iterator iterator = mCharacters.find(pName);

	iterator++;
	if(iterator == mCharacters.end()) 
		iterator = mCharacters.begin();

	return (*iterator).first;
}
//-------------------------------------------------------------------------------------
String CharacterManager::getPrevFromName(const String& pName)
{
	CharactersMap::iterator iterator = mCharacters.find(pName);

	if(iterator == mCharacters.begin()) 
		iterator = mCharacters.end();
	iterator--;

	return (*iterator).first;
}
//-------------------------------------------------------------------------------------
std::vector<String>* CharacterManager::getNameList()
{
	return &mNameList;
}
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

