#include "CharacterManager.h"
#include "Character.h"
#include <Path.h>
#include <FileBrowser.h>
#include <MyZipArchive.h>
 
using namespace Solipsis;

CharacterManager* CharacterManager::ms_singletonPtr = 0;

//-------------------------------------------------------------------------------------
CharacterManager::CharacterManager(String pPath, SceneManager* pSceneMgr) :
	mSceneMgr(pSceneMgr),
	mCurrent(NULL)
{
	mPath = pPath + "/";
	ms_singletonPtr = this;
}
//-------------------------------------------------------------------------------------
CharacterManager::~CharacterManager() {}
//-------------------------------------------------------------------------------------
CharacterManager* CharacterManager::getSingletonPtr()
{
    return ms_singletonPtr;
}
//-------------------------------------------------------------------------------------
void CharacterManager::setUid(String pUid)
{
	mUidString = pUid;
}
//-------------------------------------------------------------------------------------
bool CharacterManager::addCharacter(String pName)
{
	Path completName(mPath + pName);
	String characterName = completName.getLastFileName(false);
	
	mCharacters[characterName] = NULL;
	//setCurrentCharacter(character->getName());

	return true;
}
//-------------------------------------------------------------------------------------
Character* CharacterManager::loadCharacter(std::string pName)
{
	Character* character = mCharacters[pName];
	if (character == NULL) 
	{
		mCharacters.erase(pName);
	
		Path completName(mPath + pName + ".saf");

		character = new Character(completName.getUniversalPath(), mUidString+"Avatar", mSceneMgr);
		//If the loading of the avatar has passed, we displayed an error message and delete the Avatar.
		if (!character->isLoadingSuccessful())
		{
			FileBrowser::displayMessageWindow("Error loading the character",character->getLoadingErrorMessage().c_str());
			delete character;
			return NULL;
		}
		mCharacters[pName] = character;
	}
	
	mCurrent = character;
	return mCurrent;
}
//-------------------------------------------------------------------------------------
Character* CharacterManager::getCurrent()
{
	return mCurrent;
}
//-------------------------------------------------------------------------------------
Character* CharacterManager::getByName(String pName)
{
	//CharactersMap::iterator iterator = mCharacters.find(pName);
	//return (*iterator).second;
	
	return loadCharacter(pName);
}
//-------------------------------------------------------------------------------------
Character* CharacterManager::getNextFromName(String pName)
{
	CharactersMap::iterator iterator = mCharacters.find(pName);

	iterator++;
	if(iterator == mCharacters.end()) 
		iterator = mCharacters.begin();

	return loadCharacter((*iterator).first);
}
//-------------------------------------------------------------------------------------
Character* CharacterManager::getPrevFromName(String pName)
{
	CharactersMap::iterator iterator = mCharacters.find(pName);

	if(iterator == mCharacters.begin()) 
		iterator = mCharacters.end();
	iterator--;

	return loadCharacter((*iterator).first);
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
//-------------------------------------------------------------------------------------

