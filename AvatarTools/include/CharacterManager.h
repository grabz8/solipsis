/*
		COPYRIGHTS (c) ARTEFACTO 
		
			!! Licence GPL !!	
*/

#ifndef __CharacterManager_h__
#define __CharacterManager_h__

#include <Ogre.h>
#include <OgreNoMemoryMacros.h>
#include <OgreMemoryMacros.h>

using namespace Ogre;
namespace Solipsis {

class Character;

typedef std::map<String,Character*> CharactersMap;
typedef MapIterator<CharactersMap> CharactersIterator;

class CharacterManager
{
public:
	CharacterManager(String pPath, SceneManager* pSceneMgr);
	~CharacterManager();

	///brief Method returning the singleton of our class
	///return The single instance of the classe
	static CharacterManager* getSingletonPtr();

	///brief Set the UI
	void setUid(String pUid);
	///brief Open an character from a filebrowser and add it to the list
	bool addCharacter() { return true; }
	///brief Open an character from the list 
	bool addCharacter(String pName);
	///brief Remove the character from the list
	bool removeCharacter(String pName) { return true; }

	///brief Load a SAF file if haven't loaded before
	Character* CharacterManager::loadCharacter(std::string pName);
	///brief Get the current character
	Character* getCurrent();
	///brief Get the character from his name
	Character* getByName(String pName);
	///brief Get the next character after the named one
	Character* getNextFromName(String pName);
	///brief Get the previews character after the named one
	Character* getPrevFromName(String pName);





private:
	static CharacterManager* ms_singletonPtr;
	SceneManager* mSceneMgr;
	String mPath;
	String mUidString;

	CharactersMap mCharacters;					///brief Map containing our Characters.
	Character* mCurrent;						///brief Character which is currently being modified.
	bool mCurrentCharacterJustChanged;			///brief Tells us if the current avatar just changed, it is used by CEGUI and MyApp in order to they know current avatar has changed.
};

}

#endif
