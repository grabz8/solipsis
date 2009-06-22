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

#include "Scene.h"
#include "MainApplication/Navigator.h"
#include "OgreOSMScene.h"
#include "OgreTools/OgreHelpers.h"
#include <CTIO.h>
#include <XmlSceneLodContent.h>

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

Scene::Scene(RefCntPoolPtr<XmlEntity>& xmlEntity, bool isLocal) :
    OgrePeer(xmlEntity, isLocal),
    mSceneNode(0),
    mStaticGeometry(0),
    mOgreMaxScene(0)
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
    if(mOgreMaxScene != 0)
    {
        mOgreMaxScene->Update( timeSinceLastFrame );
#if 1 // GILLES
        // Load all animation states
        std::vector<Ogre::AnimationState*>::iterator animationState = mVertexAnimationStates.begin();
        for( ; animationState != mVertexAnimationStates.end(); animationState++ )
        {
            Ogre::AnimationState* animState = (*animationState);
            animState->addTime( timeSinceLastFrame );
        }
#endif
    }

}

//-------------------------------------------------------------------------------------

bool Scene::updateEntity(RefCntPoolPtr<XmlEntity>& xmlEntity)
{
    XmlEntity::DefinedAttributes definedAttributes = xmlEntity->getDefinedAttributes();

    if ((definedAttributes & XmlEntity::DAContent) && (xmlEntity->getDownloadProgress() >= 1.0f))
    {
        OGRE_LOG("Scene::updateEntity() Destroy/Load new scene uid:" + mXmlEntity->getUid());

        destroy();

        SceneManager* sceneMgr = Navigator::getSingletonPtr()->getOgrePeerManager()->getSceneManager();
        if (sceneMgr == 0)
            throw Exception(Exception::ERR_INTERNAL_ERROR, "No scene manager !", "Scene::updateEntity");

        // Get the scene content for LOD 0
        XmlContent::ContentLodMap& contentLodMap = xmlEntity->getContent()->getContentLodMap();
        RefCntPoolPtr<XmlSceneLodContent> xmlSceneLodContent0 = RefCntPoolPtr<XmlSceneLodContent>(contentLodMap[0]->getDatas());

        // Find .ssf file
        XmlLodContent::LodContentFileList::const_iterator lodContent0File = contentLodMap[0]->getLodContentFileList().begin();
        for(;lodContent0File!=contentLodMap[0]->getLodContentFileList().end();++lodContent0File)
            if (lodContent0File->mFilename.find(".ssf") == lodContent0File->mFilename.length() - 4)
                break;
        if (lodContent0File == contentLodMap[0]->getLodContentFileList().end())
            throw Exception(Exception::ERR_INTERNAL_ERROR, "No .ssf scene file found !", "Scene::updateEntity");

        // Create the resource group
        mResourceGroup = xmlEntity->getUid() + "Resources";
        mResourceLocation = Navigator::getSingletonPtr()->getMediaCachePath() + IO::getPathSeparator() + lodContent0File->mFilename;
        ResourceGroupManager::getSingleton().createResourceGroup(mResourceGroup);
        ResourceGroupManager::getSingleton().addResourceLocation(mResourceLocation, "Zip", mResourceGroup);
        ResourceGroupManager::getSingleton().initialiseResourceGroup(mResourceGroup);

        // Create the scene node
        SceneNode* sceneNode = sceneMgr->getRootSceneNode()->createChildSceneNode(xmlEntity->getUid() + "Scene");

        // Check the scene type (.OSM with oFusion or .SCENE with ogreMax)
        std::string sceneFilename( xmlSceneLodContent0->getMainFilename().c_str() );
        if( sceneFilename.find(".osm") == sceneFilename.size() - 4)
        {
            // Load from the .osm
            OSMScene osmScene(sceneMgr, Navigator::getSingletonPtr()->getRenderWindowPtr());
            OgrePeerManagerOSMSceneCallbacks osmSceneCallbacks;
            //if (!osmScene.initialise(xmlSceneLodContent0->getMainFilename().c_str(), &osmSceneCallbacks))
            if (!osmScene.initialise(sceneFilename.c_str(), &osmSceneCallbacks))
                throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to load OSM file scene " + String(xmlSceneLodContent0->getMainFilename()), "Scene::updateEntity");
            osmScene.declareResources();
            if (!osmScene.createScene(sceneNode))
                throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to create OSM file scene " + String(xmlSceneLodContent0->getMainFilename()), "Scene::updateEntity");
        }
        else if( sceneFilename.find(".scene") == sceneFilename.size() - 6 )
        {
            // Load from the .scene
            mOgreMaxScene = new OgreMax::OgreMaxScene();
            mOgreMaxScene->Load( 
                sceneFilename, 
                Navigator::getSingletonPtr()->getRenderWindowPtr(),
                (NO_OGREMAX_STATIC_GEOM==1) ? OgreMax::OgreMaxScene::NO_OPTIONS : OgreMax::OgreMaxScene::NO_ANIMATION_STATES,
                sceneMgr,
                sceneNode,
                0,
                mResourceGroup );

#if 1 // GILLES
            // Load all animation states
            mVertexAnimationStates.clear();
            scanSceneNode( sceneNode );
#endif

            if (NO_OGREMAX_STATIC_GEOM) mSceneNode = sceneNode;
        }
        else
            throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to create any file scene " + String(xmlSceneLodContent0->getMainFilename()), "Scene::updateEntity");

        /*
        // Load from the .osm
        OSMScene osmScene(sceneMgr, Navigator::getSingletonPtr()->getRenderWindowPtr());
        OgrePeerManagerOSMSceneCallbacks osmSceneCallbacks;
        if (!osmScene.initialise(xmlSceneLodContent0->getMainFilename().c_str(), &osmSceneCallbacks))
            throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to load OSM file scene " + String(xmlSceneLodContent0->getMainFilename()), "Scene::updateEntity");
        osmScene.declareResources();
        if (!osmScene.createScene(sceneNode))
            throw Exception(Exception::ERR_INTERNAL_ERROR, "Unable to create OSM file scene " + String(xmlSceneLodContent0->getMainFilename()), "Scene::updateEntity");
        */

        if (Navigator::getSingletonPtr()->getCastShadows())
        {
            if(0 && mOgreMaxScene != 0)
            {
                // use the ogreScene's shadow parameters
            }
            else
            {
                //sceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE);
                sceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
                sceneMgr->setShadowTextureSelfShadow(false);
                //sceneMgr->setShadowTextureSettings(512, 1, PixelFormat::PF_A4R4G4B4);
                sceneMgr->setShadowTextureSettings(1024, 4, PF_A4R4G4B4);
                SharedPtr<LiSPSMShadowCameraSetup> shadowCameraSetup = SharedPtr<LiSPSMShadowCameraSetup>(new LiSPSMShadowCameraSetup());
                //sceneMgr->setShadowColour(ColourValue(.6, .65, .7, 1.));
                sceneMgr->setShadowColour(ColourValue(.7, .75, .85, 1.));
                sceneMgr->setShadowFarDistance(100.);
                sceneMgr->setShadowCameraSetup(shadowCameraSetup);
            }
        }

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
        if (NO_OGREMAX_STATIC_GEOM && mOgreMaxScene != 0) return true;
        // Optimize by converting it into static geometry
        convertToStaticGeometry(sceneNode);
    }
    if (definedAttributes & XmlEntity::DADownloadProgress)
    {
        if (xmlEntity->getDownloadProgress() < 1.0f)
        {
            Avatar* pUserAvatar = Navigator::getSingletonPtr()->getUserAvatar();
            if (pUserAvatar)
            {
                if (mProgressBar == 0)
                {
                    mProgressBar = new ProgressBarWithText(mXmlEntity->getUid(), "Scene downloading : ", false);
                    mProgressBar->setBarSize(2.5, 0.3);
                    mProgressBar->setTxtVerticalPos(0);
                    mProgressBar->setTxtHozizontalPosition(false, -120);
                    mProgressBar->setPosition(1.7);
                    mProgressBar->setPosition(pUserAvatar->getEntity()->getBoundingBox().getSize().y);
                    mProgressBar->showRemainingTime(true);
                    mProgressBar->setFont("BerlinSans32", 1, ColourValue::White, 1);
                    mProgressBar->setTxtScale(0.1f);
                    mProgressBar->attach(pUserAvatar->getSceneNode());
                }
                mProgressBar->setProgress(xmlEntity->getDownloadProgress());
            } 
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
   
        mXmlEntity->setDownloadProgress(xmlEntity->getDownloadProgress());
        OGRE_LOG("Download progress for Scene " +
            xmlEntity->getUid() + " : " +
            StringConverter::toString((Real)mXmlEntity->getDownloadProgress()));
    }
    if (definedAttributes & XmlEntity::DAUploadProgress)
    {
        mXmlEntity->setUploadProgress(xmlEntity->getUploadProgress());
        OGRE_LOG("Upload progress for Scene " +
            xmlEntity->getUid() + " : " +
            StringConverter::toString((Real)mXmlEntity->getUploadProgress()));
    }

    return true;
}

#if 1 // GILLES
//-------------------------------------------------------------------------------------
void Scene::scanSceneNode(SceneNode* pSceneNode)
{
    if (pSceneNode->numChildren() == 0)
    {
        // for each entities, get their check for animationStates
        Ogre::SceneNode::ObjectIterator object = pSceneNode->getAttachedObjectIterator();
        for( ; object.hasMoreElements(); object.getNext() )
        {
            MovableObject* movable = object.peekNextValue();
            if (movable->getMovableType().compare("Entity") == 0)
            {
                Entity* entity = (Entity*)movable;
                if (entity->hasSkeleton() || entity->hasVertexAnimation())
                {
                    Ogre::AnimationStateSet* animations = entity->getAllAnimationStates();
                    Ogre::AnimationStateIterator animStateIter = animations->getAnimationStateIterator();
                    for( ; animStateIter.hasMoreElements(); animStateIter.getNext())
                    {
                        // get the animationState and activate it
                        Ogre::AnimationState* animState = animStateIter.peekNextValue();
                        animState->setEnabled( true );
                        animState->setLoop( true );
                        animState->setTimePosition(0.);
                        // add this animationState to the list
                        mVertexAnimationStates.push_back( animState );
                    }
                }
            }
        }
    }
    else
    {
        // for each childNode, scan them to find their entities
        Ogre::Node::ChildNodeIterator child = pSceneNode->getChildIterator();
        for( ; child.hasMoreElements(); child.getNext() )
        {
            SceneNode* sceneNode = (SceneNode*)child.peekNextValue();
            scanSceneNode( sceneNode );
        }
    }
}
#endif

//-------------------------------------------------------------------------------------

bool Scene::action(RefCntPoolPtr<XmlAction>& xmlAction)
{
    return true;
}

//-------------------------------------------------------------------------------------
void Scene::destroy()
{
    SceneManager* sceneMgr = Navigator::getSingletonPtr()->getOgrePeerManager()->getSceneManager();
    if (sceneMgr == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR, "No scene manager !", "Scene::updateEntity");

    bool resetAmbientViewportsSkiesAndLights = false;

    if (mStaticGeometry != 0)
    {
        sceneMgr->destroyStaticGeometry(mStaticGeometry);
        mStaticGeometry = 0;

        if (mOgreMaxScene != 0)
        {
            // remove all aniamtions & animations states from the objects attached to the scene
            OgreMax::OgreMaxScene::AnimationStates animations = mOgreMaxScene->GetAnimationStates();
            OgreMax::OgreMaxScene::AnimationStates::iterator anim = animations.begin();
            while( anim != animations.end() )
            {
                (*anim).second->setEnabled(false);
                (*anim).second->setLoop(false);
                sceneMgr->destroyAnimation((*anim).first);
                sceneMgr->destroyAnimationState((*anim).first);
                anim++;
            }
#if 1 // GILLES
            // remove all animation states (vertex & skeleton animations)
            std::vector<Ogre::AnimationState*>::iterator animationState = mVertexAnimationStates.begin();
            while( animationState != mVertexAnimationStates.end() )
            {
                Ogre::AnimationState* animState = (*animationState);
                sceneMgr->destroyAnimation(animState->getAnimationName());
                sceneMgr->destroyAnimationState(animState->getAnimationName());
                animationState++;
            }
            mVertexAnimationStates.clear();
#endif
            // remove the scene
            delete mOgreMaxScene;
            mOgreMaxScene = 0;
        }

        resetAmbientViewportsSkiesAndLights = true;

	    ResourceGroupManager::getSingleton().removeResourceLocation(mResourceLocation, mResourceGroup);
        ResourceGroupManager::getSingleton().destroyResourceGroup(mResourceGroup);
        // Here we unload the archive manually because removeResourceLocation() missed it (see Ogre forums)
        ArchiveManager::getSingleton().unload(mResourceLocation);
    } 

    else if (NO_OGREMAX_STATIC_GEOM && mOgreMaxScene != 0)
    {
        // remove all aniamtions & animations states from the objects attached to the scene
        OgreMax::OgreMaxScene::AnimationStates animations = mOgreMaxScene->GetAnimationStates();
        OgreMax::OgreMaxScene::AnimationStates::iterator anim = animations.begin();
        while( anim != animations.end() )
        {
            (*anim).second->setEnabled(false);
            (*anim).second->setLoop(false);
            sceneMgr->destroyAnimation((*anim).first);
            sceneMgr->destroyAnimationState((*anim).first);
            anim++;
        }
        // remove the scene
        delete mOgreMaxScene;
        mOgreMaxScene = 0;

        ResourceGroupManager::getSingleton().removeResourceLocation(mResourceLocation, mResourceGroup);
        ResourceGroupManager::getSingleton().destroyResourceGroup(mResourceGroup);
        // Here we unload the archive manually because removeResourceLocation() missed it (see Ogre forums)
        ArchiveManager::getSingleton().unload(mResourceLocation);
    }

    if (mSceneNode != 0)
    {
        OgreHelpers::removeAndDestroySceneNode(mSceneNode);
        mSceneNode = 0;
    }

    if (resetAmbientViewportsSkiesAndLights)
    {
        // Reset ambient light to black
        sceneMgr->setAmbientLight(ColourValue::Black);
        // Reset background color to black
        RenderWindow *win = Navigator::getSingletonPtr()->getRenderWindowPtr();
        int numViewports = win->getNumViewports();
        if (numViewports)
        {
	        for(int i=0; i<numViewports; ++i)
                win->getViewport(i)->setBackgroundColour(ColourValue::Black);
        }
        // Destroy skies
        sceneMgr->setSkyPlane(false, Plane(-Vector3::UNIT_Y, 0), "");
        sceneMgr->setSkyBox(false, "");
        sceneMgr->setSkyDome(false, "");
        // Destroy lights
        sceneMgr->destroyAllLights();
    }
}

//-------------------------------------------------------------------------------------
void Scene::convertToStaticGeometry(SceneNode* sceneNode)
{
    LOGHANDLER_LOGF(LogHandler::VL_INFO, "Scene::convertToStaticGeometry() scene uid:%s converting into static geometry", mXmlEntity->getUid().c_str());

    destroy();

    if (sceneNode == 0)
        throw Exception(Exception::ERR_INVALIDPARAMS, "No scene node !", "Scene::convertToStaticGeometry");
    SceneManager* sceneMgr = Navigator::getSingletonPtr()->getOgrePeerManager()->getSceneManager();
    if (sceneMgr == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR, "No scene manager !", "Scene::convertToStaticGeometry");

    mSceneNode = sceneNode;

    // Add all visible entities (1 loop because StaticGeometry::addSceneNode() add invisible entities)
    mStaticGeometry = sceneMgr->createStaticGeometry(mSceneNode->getName());
    std::list<MovableObject*> movableObjectsList;
    OgreHelpers::getMovableObjectsList(mSceneNode, "Entity", movableObjectsList);
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

    LOGHANDLER_LOGF(LogHandler::VL_INFO, "Scene::convertToStaticGeometry() scene:%s building static geometry", mXmlEntity->getUid().c_str());
    mStaticGeometry->build();
    LOGHANDLER_LOGF(LogHandler::VL_INFO, "Scene::convertToStaticGeometry() scene:%s static geometry built", mXmlEntity->getUid().c_str());

    // Destroy all entities converted into static geometry (no more used)
    OgreHelpers::removeAndDestroyMovableObjects(mSceneNode, movableObjectsList);
}

//-------------------------------------------------------------------------------------
