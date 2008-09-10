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

#include "Scene.h"
#include "Navigator.h"
#include "OgreOSMScene.h"
#include "OgreHelpers.h"

using namespace Solipsis;
using namespace CommonTools;

// this internal OSM-loader callbacks class is used to force OFF shadows casting of entities
class OgrePeerManagerOSMSceneCallbacks : public OSMSceneCallbacks
{
    virtual void OnLightCreate(Light *pLight, TiXmlElement* pLightDesc)
    {
        pLight->setCastShadows(false);
    }
    virtual void OnEntityCreate(Entity *pEntity, TiXmlElement* pEntityDesc)
    {
        pEntity->setCastShadows(false);
    }
};

//-------------------------------------------------------------------------------------
#ifdef POOL
Scene::Scene(RefCntPoolPtr<XmlEntity>& xmlEntity, bool isLocal) :
#else
Scene::Scene(XmlEntity* xmlEntity, bool isLocal) :
#endif
    OgrePeer(xmlEntity, isLocal),
    mStaticGeometry(0)
{
}

//-------------------------------------------------------------------------------------
Scene::~Scene()
{
    destroy();
}

//-------------------------------------------------------------------------------------
void Scene::update(Ogre::Real timeSinceLastFrame)
{
}

//-------------------------------------------------------------------------------------
#ifdef POOL
bool Scene::update(RefCntPoolPtr<XmlEntity>& xmlEntity)
#else
bool Scene::update(XmlEntity* xmlEntity)
#endif
{
    XmlEntity::DefinedAttributes definedAttributes = xmlEntity->getDefinedAttributes();

    if (definedAttributes & XmlEntity::DAContent)
    {
        OGRE_LOG("Scene::update() Destroy/Load new scene uid:" + mXmlEntity->getUidString());

        destroy();

        SceneManager* sceneMgr = Navigator::getSingletonPtr()->getOgrePeerManager()->getSceneManager();
        if (sceneMgr == 0)
            throw Exception(Exception::ERR_INTERNAL_ERROR, "No scene manager !", "Scene::update");

        // Get the scene content for LOD 0
        XmlContent::ContentLodMap& contentLodMap = xmlEntity->getContent()->getContentLodMap();
        RefCntPoolPtr<XmlSceneLodContent> xmlSceneLodContent0 = RefCntPoolPtr<XmlSceneLodContent>(contentLodMap[0]->getDatas());

        // Find .ssf file
        XmlLodContent::LodContentFileList::const_iterator lodContent0File = contentLodMap[0]->getLodContentFileList().begin();
        for(;lodContent0File!=contentLodMap[0]->getLodContentFileList().end();++lodContent0File)
            if (lodContent0File->mFilename.find(".ssf") == lodContent0File->mFilename.length() - 4)
                break;
        if (lodContent0File == contentLodMap[0]->getLodContentFileList().end())
            throw Exception(Exception::ERR_INTERNAL_ERROR, "No .ssf scene file found !", "Scene::update");

        // Create the resource group
        mResourceGroup = xmlEntity->getUidString() + "Resources";
        mResourceLocation = Navigator::getSingletonPtr()->getMediaCachePath() + "\\" + lodContent0File->mFilename;
        ResourceGroupManager::getSingleton().createResourceGroup(mResourceGroup);
        ResourceGroupManager::getSingleton().addResourceLocation(mResourceLocation, "Zip", mResourceGroup);
        ResourceGroupManager::getSingleton().initialiseResourceGroup(mResourceGroup);

        // Create the scene node
        SceneNode* sceneNode = sceneMgr->getRootSceneNode()->createChildSceneNode(xmlEntity->getUidString() + "Scene");

        // Load from the .osm
        OSMScene osmScene(sceneMgr);
        OgrePeerManagerOSMSceneCallbacks osmSceneCallbacks;
        if (!osmScene.initialise(xmlSceneLodContent0->getMainFilename().c_str(), &osmSceneCallbacks))
            throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to load OSM file scene " + String(xmlSceneLodContent0->getMainFilename()), "Scene::update");
        osmScene.declareResources();
        if (!osmScene.createScene(sceneNode))
            throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to create OSM file scene " + String(xmlSceneLodContent0->getMainFilename()), "Scene::update");

#ifdef SHADOWS
        sceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE);
        sceneMgr->setShadowTextureSettings(512, 1, PixelFormat::PF_A4R4G4B4);
        Ogre::SharedPtr<LiSPSMShadowCameraSetup> shadowCameraSetup = Ogre::SharedPtr<LiSPSMShadowCameraSetup>(new LiSPSMShadowCameraSetup());
        sceneMgr->setShadowCameraSetup(shadowCameraSetup);
#endif

        // Destroy the scene collision mesh
        if (!xmlSceneLodContent0->getCollision().empty())
        {
//            sceneMgr->destroySceneNode(xmlSceneLodContent0->getCollision());
            SceneNode* collisionSceneNode = sceneMgr->getSceneNode(xmlSceneLodContent0->getCollision());
            OgreHelpers::removeAndDestroySceneNode(collisionSceneNode);
        }

        if (definedAttributes & XmlEntity::DAPosition)
            sceneNode->setPosition(xmlEntity->getPosition());
        if (definedAttributes & XmlEntity::DAOrientation)
            sceneNode->setOrientation(xmlEntity->getOrientation());

        // Optimize by converting it into static geometry
        convertToStaticGeometry(sceneNode);
    }

    return true;
}

//-------------------------------------------------------------------------------------
#ifdef POOL
bool Scene::action(RefCntPoolPtr<XmlAction>& xmlAction)
#else
bool Scene::action(XmlAction* xmlAction)
#endif
{
    return true;
}

//-------------------------------------------------------------------------------------
void Scene::destroy()
{
    SceneManager* sceneMgr = Navigator::getSingletonPtr()->getOgrePeerManager()->getSceneManager();
    if (sceneMgr == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR, "No scene manager !", "Scene::update");

    if (mStaticGeometry != 0)
    {
        sceneMgr->destroyStaticGeometry(mStaticGeometry);
        mStaticGeometry = 0;

	    ResourceGroupManager::getSingleton().removeResourceLocation(mResourceLocation, mResourceGroup);
        ResourceGroupManager::getSingleton().destroyResourceGroup(mResourceGroup);
    }
}

//-------------------------------------------------------------------------------------
void Scene::convertToStaticGeometry(SceneNode* sceneNode)
{
    LOGHANDLER_LOGF(LogHandler::VL_INFO, "Scene::convertToStaticGeometry() scene uid:%s converting into static geometry", mXmlEntity->getUidString().c_str());

    destroy();

    if (sceneNode == 0)
        throw Exception(Exception::ERR_INVALIDPARAMS, "No scene node !", "Scene::convertToStaticGeometry");
    SceneManager* sceneMgr = Navigator::getSingletonPtr()->getOgrePeerManager()->getSceneManager();
    if (sceneMgr == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR, "No scene manager !", "Scene::convertToStaticGeometry");

    // Add all visible entities (1 loop because StaticGeometry::addSceneNode() add invisible entities)
    mStaticGeometry = sceneMgr->createStaticGeometry(sceneNode->getName());
    std::list<MovableObject*> movableObjectsList;
    OgreHelpers::getMovableObjectsList(sceneNode, "Entity", movableObjectsList);
    for (std::list<MovableObject*>::iterator movableObject = movableObjectsList.begin();movableObject != movableObjectsList.end();++movableObject)
    {
        Entity* entity = static_cast<Entity*>(*movableObject);
        SceneNode* sceneNode = entity->getParentSceneNode();
        if (entity->isVisible())
            mStaticGeometry->addEntity(entity,
                sceneNode->_getDerivedPosition(),
                sceneNode->_getDerivedOrientation(),
                sceneNode->_getDerivedScale());
    }

    LOGHANDLER_LOGF(LogHandler::VL_INFO, "Scene::convertToStaticGeometry() scene:%s building static geometry", mXmlEntity->getUidString().c_str());
    mStaticGeometry->build();
    LOGHANDLER_LOGF(LogHandler::VL_INFO, "Scene::convertToStaticGeometry() scene:%s static geometry built", mXmlEntity->getUidString().c_str());

/*    SceneNode::ObjectIterator objectIterator = sceneNode->getAttachedObjectIterator();
    while (objectIterator.hasMoreElements())
    {
        MovableObject* m = objectIterator.getNext();
        sceneMgr->destroyMovableObject(m);
    }
    sceneMgr->destroySceneNode(sceneNode->getName());*/
    OgreHelpers::removeAndDestroySceneNode(sceneNode);
}

//-------------------------------------------------------------------------------------
