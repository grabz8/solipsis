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
class CharacterInstance;

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

	///brief Open an character from the list 
	bool addCharacter(const String& pName);
	///brief Remove the character from the list
	bool removeCharacter(const String& pName) { return true; }
	///brief Set the default character
	void setDefaultCharacter(const String& pName);

	///brief Load a SAF file if haven't loaded before
	Character* CharacterManager::loadCharacter(const String& pName);
	///brief Load SIF/SAF files of a character instance
    CharacterInstance* CharacterManager::loadCharacterInstance(const String& pUidString, const String& pDefaultCharacterName);
	///brief Destroy a character instance
    void destroyCharacterInstance(CharacterInstance* pCharacterInstance);
	///brief Set the current character instance
    void setCurrentInstance(CharacterInstance* pCharacterInstance);
	///brief Get the current character instance
	CharacterInstance* getCurrentInstance();
	///brief Get the next character after the named one
	String getNextFromName(const String& pName);
	///brief Get the previews character after the named one
	String getPrevFromName(const String& pName);

	///brief Get the name list of the added characters
	std::vector<String>* getNameList();





private:
	static CharacterManager* ms_singletonPtr;
	SceneManager* mSceneMgr;
	String mPath;

	String mDefaultCharacterName;
	std::vector<String> mNameList;
	CharactersMap mCharacters;					///brief Map containing our Characters.
	CharacterInstance* mCurrentInstance;		///brief Character instance which is currently being modified.
	bool mCurrentCharacterJustChanged;			///brief Tells us if the current character just changed, it is used by CEGUI and MyApp in order to they know current character has changed.
};

}

#endif
