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

class AvatarEditor
{
public:
    AvatarEditor();
    ~AvatarEditor();

    /** Get the singleton for the avatar editor. */
    static AvatarEditor* getSingletonPtr();

    /** Load a SAF file from the filebrowser. */
    bool XMLLoad();
    /** Load a SAF file from the cache directory. */
    bool XMLLoad(std::string pPathDirectory, std::string pFilename);
    /** Save in the SAF file to the cache directory. */
    bool XMLSave(std::string pPathDirectory, std::string pFilename);

    /** */
    std::string getMeshFilename();
    /** */
    std::string getSkeletonFilename();

private:
    static AvatarEditor* ms_singletonPtr;

    MyZipArchive* archive;

    std::string mMeshFilename;
    std::string mSkeletonFilename;
};

} // namespace Solipsis

#endif //__AvatarEditor_h__