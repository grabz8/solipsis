#include "AvatarEditor.h"

using namespace Solipsis;

AvatarEditor* AvatarEditor::ms_singletonPtr = 0;

//-------------------------------------------------------------------------------------
AvatarEditor::AvatarEditor() :
    archive(0),
    mMeshFilename(""),
    mSkeletonFilename("")
{
    ms_singletonPtr = this;
}
//-------------------------------------------------------------------------------------
AvatarEditor::~AvatarEditor()
{
    delete ms_singletonPtr;
    ms_singletonPtr = 0;
}
//-------------------------------------------------------------------------------------
AvatarEditor* AvatarEditor::getSingletonPtr()
{
    return ms_singletonPtr;
}
//-------------------------------------------------------------------------------------
bool AvatarEditor::XMLLoad()
{
    // open a file browser to select a SAF file and load it !
    return XMLLoad("", "");
}
//-------------------------------------------------------------------------------------
bool AvatarEditor::XMLLoad(std::string pPathDirectory, std::string pFilename)
{
    mMeshFilename = "";
    mSkeletonFilename = "";
    std::string resourceGrp = "";

    //try
    {
        if (pFilename == "")
            return false;
        
        //std::string fullFilename();
        Path path(pPathDirectory + pFilename);
        Path filenameExt(pFilename);
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
    }

    return true;
}
//-------------------------------------------------------------------------------------
bool AvatarEditor::XMLSave(std::string pPathDirectory, std::string pFilename)
{
    return true;
}
//-------------------------------------------------------------------------------------
std::string AvatarEditor::getMeshFilename()
{
    return mMeshFilename;
}
//-------------------------------------------------------------------------------------
std::string AvatarEditor::getSkeletonFilename()
{
    return mSkeletonFilename;
}
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
    