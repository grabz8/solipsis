/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

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

#include "Prerequisites.h"

#include "Object.h"
#include "OgreTools/OgreHelpers.h"
#include "MainApplication/Navigator.h"
#include "World/Modeler.h"
#include <CTStringHelpers.h>
#include <CTIO.h>
#include <Navi.h>

using namespace Solipsis;
using namespace CommonTools;

//-------------------------------------------------------------------------------------

Object::Object(RefCntPoolPtr<XmlEntity>& xmlEntity, bool isLocal, Object3D* object3D) :
    OgrePeer(xmlEntity, isLocal),
    mObject3D(object3D)

{
}

//-------------------------------------------------------------------------------------
Object::~Object()
{
    Modeler *modeler = Modeler::getSingletonPtr();
    if (modeler == 0)
        return;
    if (mObject3D != 0)
    {
        Selection *selection = modeler->getSelection();
        selection->remove3DObject(mObject3D);
    }

    if (!mResourceLocation.empty())
    {
        ResourceGroupManager::getSingleton().removeResourceLocation(mResourceLocation, mResourceGroup);
        if (!mResourceGroup.empty())
            ResourceGroupManager::getSingleton().destroyResourceGroup(mResourceGroup);
        // Here we unload the archive manually because removeResourceLocation() missed it (see Ogre forums)
        ArchiveManager::getSingleton().unload(mResourceLocation);
        mResourceLocation.clear();
        mResourceGroup.clear();
    }
}

//-------------------------------------------------------------------------------------
void Object::onObjectSave()
{
    // Update entity position
    mXmlEntity->setPosition(mObject3D->getPosition());
    AxisAlignedBox aabbox;
    mObject3D->getAABoundingBox(aabbox);
    mXmlEntity->setAABoundingBox(aabbox);
    if (mLocalNode != 0)
        mLocalNode->setPosition(mXmlEntity->getPosition());

    // Get the content for LOD 0
    XmlContent::ContentLodMap& contentLodMap = mXmlEntity->getContent()->getContentLodMap();
    XmlLodContent::LodContentFileList& lodContent0FileList = contentLodMap[0]->getLodContentFileList();
    XmlLodContent::LodContentFileList::iterator lodContent0File;
    for (lodContent0File = lodContent0FileList.begin(); lodContent0File != lodContent0FileList.end(); ++lodContent0File)
        if (lodContent0File->mFilename.find(".sof") == lodContent0File->mFilename.length() - 4)
        {
            lodContent0File->mVersion++;
            break;
        }
    if (lodContent0File == lodContent0FileList.end())
        throw Exception(Exception::ERR_INTERNAL_ERROR, "No .sof object file found !", "Object::onObjectSave");
}

//-------------------------------------------------------------------------------------
void Object::update(Real timeSinceLastFrame)
{
}

//-------------------------------------------------------------------------------------
bool Object::action(RefCntPoolPtr<XmlAction>& xmlAction)
{
    // Action applied by our local avatar ?
    if (xmlAction->getSourceEntityUid() == Navigator::getSingletonPtr()->getUserAvatar()->getXmlEntity()->getUid())
        return true;

    if (!Navigator::getSingletonPtr()->isNaviSupported())
        return true;
    std::string action = StringHelpers::convertWStringToString(xmlAction->getDesc());
    std::string::size_type comma = action.find_first_of(",");
    if ((comma == std::string::npos) || (comma < 1) || (action.length() - comma <= 1))
    {
        OGRE_LOG("Object::action() Navi " + mXmlEntity->getUid() + " not found !");
        return true;
    }
    std::string naviName = action.substr(0, comma);
    std::string url = action.substr(comma + 1);
    NaviLibrary::Navi* navi = NaviLibrary::NaviManager::Get().getNavi(naviName);
    if (navi == 0)
    {
        OGRE_LOG("Object::action() Navi " + mXmlEntity->getUid() + " not found !");
        return true;
    }
    if (navi->getCurrentLocation() != url)
    {
        Navigator::getSingletonPtr()->addNaviURLUpdatePending(naviName, url);
        navi->navigateTo(url);
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool Object::updateEntity(RefCntPoolPtr<XmlEntity>& xmlEntity)
{
    XmlEntity::DefinedAttributes definedAttributes = xmlEntity->getDefinedAttributes();

    if (mLocalNode == 0)
    {
        mLocalNode = Modeler::getSceneManager()->getRootSceneNode()->createChildSceneNode(String(xmlEntity->getUid()) + "_localNode");
        mLocalNode->setPosition(mXmlEntity->getPosition());
    }

    if ((definedAttributes & XmlEntity::DAContent) && (xmlEntity->getDownloadProgress() >= 1.0f))
    {
        OGRE_LOG("Object::updateEntity() Destroy/Load new object uid:" + mXmlEntity->getUid());

        Modeler* modeler = Modeler::getSingletonPtr();
        if (mObject3D != 0)
        {
            Selection *selection = modeler->getSelection();
            selection->remove3DObject(mObject3D);

            if (!mResourceLocation.empty())
            {
                ResourceGroupManager::getSingleton().removeResourceLocation(mResourceLocation, mResourceGroup);
                if (!mResourceGroup.empty())
                    ResourceGroupManager::getSingleton().destroyResourceGroup(mResourceGroup);
                // Here we unload the archive manually because removeResourceLocation() missed it (see Ogre forums)
                ArchiveManager::getSingleton().unload(mResourceLocation);
                mResourceLocation.clear();
                mResourceGroup.clear();
            }
        }

        String pathname = "";
        XmlLodContent::LodContentFileList& lodContentFileList = xmlEntity->getContent()->getContentLodMap()[0]->getLodContentFileList();
        for (XmlLodContent::LodContentFileList::const_iterator it = lodContentFileList.begin(); it != lodContentFileList.end(); ++it)
            if (it->mFilename.find(".sof") == it->mFilename.length() - 4)
                pathname = Navigator::getSingletonPtr()->getMediaCachePath() + IO::getPathSeparator() + it->mFilename;

        mResourceLocation = pathname;
        try
        {
            mResourceGroup = xmlEntity->getUid() + "Resources";
            ResourceGroupManager::getSingleton().createResourceGroup(mResourceGroup);
            ResourceGroupManager::getSingleton().addResourceLocation(mResourceLocation, "Zip", mResourceGroup);
            ResourceGroupManager::getSingleton().initialiseResourceGroup(mResourceGroup);
        }
        catch (Ogre::Exception e)
        {
            OGRE_LOG("Object::updateEntity() caught Ogre exception : " + e.getFullDescription());
            if (!mResourceLocation.empty())
            {
                ResourceGroupManager::getSingleton().removeResourceLocation(mResourceLocation, mResourceGroup);
                if (!mResourceGroup.empty())
                    ResourceGroupManager::getSingleton().destroyResourceGroup(mResourceGroup);
                // Here we unload the archive manually because removeResourceLocation() missed it (see Ogre forums)
                ArchiveManager::getSingleton().unload(mResourceLocation);
                mResourceLocation.clear();
                mResourceGroup.clear();
            }
            return false;
        }

        Object3DPtrList newObjects;

        if (!modeler->XMLLoad(pathname, mResourceGroup, newObjects))
        {
            OGRE_LOG("Object::updateEntity() Unable to load .sof object file !");
            ResourceGroupManager::getSingleton().removeResourceLocation(mResourceLocation, mResourceGroup);
            ResourceGroupManager::getSingleton().destroyResourceGroup(mResourceGroup);
            // Here we unload the archive manually because removeResourceLocation() missed it (see Ogre forums)
            ArchiveManager::getSingleton().unload(mResourceLocation);
            mResourceLocation.clear();
            mResourceGroup.clear();
            return false;
        }
        mObject3D = *(newObjects.begin());
    }
    if (definedAttributes & XmlEntity::DAAABoundingBox)
    {
//         if (mBBox == NULL)
//         {
//             const AxisAlignedBox & bbBox = mXmlEntity->getAABoundingBox();
//             Vector3 size = bbBox.getSize();
//             if (size.x == 0)    
//                 size.x = 1;
//             if (size.y == 0)    
//                 size.y = 1;
//             if (size.z == 0)    
//                 size.z = 1;
// 
//             size.y = 800;
// 
//             mBBox = new MovableBox(mXmlEntity->getUid()+"_BBOX", size, false);
//             mLocalNode->attachObject(mBBox);
//         }
    }
    if (definedAttributes & XmlEntity::DAPosition)
    {
        mLocalNode->setPosition(mXmlEntity->getPosition());
    }
    if (definedAttributes & XmlEntity::DADownloadProgress)
    {
        mXmlEntity->setDownloadProgress(xmlEntity->getDownloadProgress());
        OGRE_LOG("Download progress for Object " +
            xmlEntity->getUid() + " : " +
            StringConverter::toString((Real)mXmlEntity->getDownloadProgress()));
    }
    if (definedAttributes & XmlEntity::DAUploadProgress)
    {
        if (xmlEntity->getUploadProgress() < 1.0f)
        {
            if (mProgressBar == 0)
            {
                mProgressBar = new ProgressBarWithText(mXmlEntity->getUid(), "Object uploading : ", false);
                mProgressBar->setBarSize(2.5,0.3);
                mProgressBar->setTxtVerticalPos(0);
                mProgressBar->setTxtHozizontalPosition(false, -120);
                mProgressBar->setPosition(mXmlEntity->getAABoundingBox().getSize().y);
                mProgressBar->showRemainingTime(true);
                mProgressBar->setFont("BerlinSans32", 1, ColourValue::White, 1);
                mProgressBar->setTxtScale(0.1f);
                mProgressBar->attach(mLocalNode);
            }
            mProgressBar->setProgress(xmlEntity->getUploadProgress());
        }
        else
        {
            if (mProgressBar != 0)
            {
                mProgressBar->detach();
                delete mProgressBar;
                mProgressBar = 0;
            }
        }

        mXmlEntity->setUploadProgress(xmlEntity->getUploadProgress());
        OGRE_LOG("Upload progress for Object " +
            xmlEntity->getUid() + " : " +
            StringConverter::toString((Real)mXmlEntity->getUploadProgress()));
    }

    return true;
}

//-------------------------------------------------------------------------------------
