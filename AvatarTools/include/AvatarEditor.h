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

/**
	\file 
		AvatarEditor.h
	\brief
		Solipsis AvatarEditor
	\author
		ARTEFACTO - François FOURNEL
*/

#ifndef __AvatarEditor_h__
#define __AvatarEditor_h__

#include <Ogre.h>
#include <MyZipArchive.h>

using namespace Ogre;

namespace Solipsis {

class CharacterManager;
class CharacterInstance;

class AvatarEditor
{
public:
	AvatarEditor(const String& pPath, SceneManager* pSceneMgr);
    ~AvatarEditor();

    /** Get the singleton for the haracter editor. */
    static AvatarEditor* getSingletonPtr();

	/** Get list of .SAF files from the path directory. */
	void buildListSAF(const String& pPathDirectory = "");

	/** Set the character instance of the user character */    
	void setCharacterInstance(CharacterInstance* pCharacterInstance);

	/** The character manager. */
	CharacterManager* getManager();
	/** Get as current character from his name. */
	bool setCurrentByName(const String& pName);
	/** */
	void setNextAsCurrent();
	/** */
	void setPrevAsCurrent();

	/** */
	SceneNode* getSceneNode();
	/** */
	Entity* getEntity();




	/** */
	String getName();
	/** */
	String getMeshName();
	/** */
	String getSkeletonName();



	/// The execution path (to go back home each time)
	Ogre::String		mExecPath;

	int selectType;
	
private:
	/** Update the current character' datas */
	void updateCurrent(const String& pName);

    static AvatarEditor* ms_singletonPtr;
	String mPath;
	SceneManager* mSceneMgr;
    MyZipArchive* archive;
	CharacterManager* mCharacters;

    String mUidString;
	String mCurrentName;
	SceneNode* mSceneNode;
	Entity* mEntity;

	String mMeshFilename;
    String mSkeletonFilename;
};

} // namespace Solipsis

#endif //__AvatarEditor_h__