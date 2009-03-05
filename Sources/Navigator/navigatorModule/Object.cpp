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
#include "OgreHelpers.h"
#include "Navigator.h"
#include "Modeler.h"
#include <CTStringHelpers.h>
#include <Navi.h>

using namespace Solipsis;
using namespace CommonTools;

//-------------------------------------------------------------------------------------
#ifdef POOL
Object::Object(RefCntPoolPtr<XmlEntity>& xmlEntity, bool isLocal, Object3D* object3D) :
#else
Object::Object(XmlEntity* xmlEntity, bool isLocal, Object3D* object3D) :
#endif
    OgrePeer(xmlEntity, isLocal),
    mObject3D(object3D)
{
    mResourceGroup = xmlEntity->getUid() + "Resources";
    ResourceGroupManager::getSingleton().createResourceGroup(mResourceGroup);
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

        if (!mResourceLocation.empty())
	        ResourceGroupManager::getSingleton().removeResourceLocation(mResourceLocation, mResourceGroup);
        if (!mResourceGroup.empty())
            ResourceGroupManager::getSingleton().destroyResourceGroup(mResourceGroup);
    }
}

//-------------------------------------------------------------------------------------
void Object::onObjectSave()
{
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
#ifdef POOL
bool Object::action(RefCntPoolPtr<XmlAction>& xmlAction)
#else
bool Object::action(XmlAction* xmlAction)
#endif
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
#ifdef POOL
bool Object::update(RefCntPoolPtr<XmlEntity>& xmlEntity)
#else
bool Object::update(XmlEntity* xmlEntity)
#endif
{
    XmlEntity::DefinedAttributes definedAttributes = xmlEntity->getDefinedAttributes();

    if (definedAttributes & XmlEntity::DAContent)
    {
        OGRE_LOG("Avatar::update() Destroy/Load new object uid:" + mXmlEntity->getUid());

        Modeler* modeler = Modeler::getSingletonPtr();
        if (mObject3D != 0)
        {
            Selection *selection = modeler->getSelection();
            selection->remove3DObject(mObject3D);

            if (!mResourceLocation.empty())
	            ResourceGroupManager::getSingleton().removeResourceLocation(mResourceLocation, mResourceGroup);
            if (!mResourceGroup.empty())
                ResourceGroupManager::getSingleton().destroyResourceGroup(mResourceGroup);
        }

        String pathname = "";
        XmlLodContent::LodContentFileList& lodContentFileList = xmlEntity->getContent()->getContentLodMap()[0]->getLodContentFileList();
        for (XmlLodContent::LodContentFileList::const_iterator it = lodContentFileList.begin(); it != lodContentFileList.end(); ++it)
            if (it->mFilename.find(".sof") == it->mFilename.length() - 4)
                pathname = Navigator::getSingletonPtr()->getMediaCachePath() + "\\" + it->mFilename;

        mResourceLocation = pathname;
        ResourceGroupManager::getSingleton().addResourceLocation(mResourceLocation, "Zip", mResourceGroup);
        ResourceGroupManager::getSingleton().initialiseResourceGroup(mResourceGroup);

        Object3DPtrList newObjects;
        if (!modeler->XMLLoad(pathname, newObjects))
            throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to load .sof object file !", "Object::update");
        mObject3D = *(newObjects.begin());
    }

    return true;
}

//-------------------------------------------------------------------------------------
