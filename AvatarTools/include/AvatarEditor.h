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
class Character;

class AvatarEditor
{
public:
	AvatarEditor(std::string pPath, SceneManager* pSceneMgr);
    ~AvatarEditor();

    /** Get the singleton for the avatar editor. */
    static AvatarEditor* getSingletonPtr();

	/** Get list of .SAF files from the path directory. */
	void buildListSAF(std::string pPathDirectory = "");

	/** */    
	void setUid(String pUid);

	/** The avatar manager. */
	CharacterManager* getManager();
	/** Get as current avatar from his name. */
	bool setCurrentByName(std::string pName);
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



private:
	/** Update the current avatar' datas */
	void updateCurrent(Character* pAvatar);

    static AvatarEditor* ms_singletonPtr;
	String mPath;
	SceneManager* mSceneMgr;
    MyZipArchive* archive;
	CharacterManager* mAvatars;

	String mUidString;
	String mCurrentName;
	SceneNode* mNode;
	Entity* mEntity;



	String mMeshFilename;
    String mSkeletonFilename;
};

} // namespace Solipsis

#endif //__AvatarEditor_h__