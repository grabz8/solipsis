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
#include "OgreHelpers.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
Scene::Scene(Peer* peer, SceneNode* sceneNode, RaySceneQuery* raySceneQuery) :
    OgrePeer(peer, "scene"),
    mSceneNode(sceneNode),
    mStaticGeometry(0),
    mRaySceneQuery(raySceneQuery)
{
}

//-------------------------------------------------------------------------------------
Scene::~Scene()
{
    destroy();
}

//-------------------------------------------------------------------------------------
SceneNode* Scene::getSceneNode()
{
    return mSceneNode;
}

//-------------------------------------------------------------------------------------
void Scene::update(Ogre::Real timeSinceLastFrame)
{
    // Optimize by converting it into static geometry
    if (mStaticGeometry == 0)
        convertToStaticGeometry();
}

//-------------------------------------------------------------------------------------
void Scene::destroy()
{
    if (mSceneNode == 0) return;
    SceneManager* sceneMgr = mSceneNode->getCreator();

    sceneMgr->destroyStaticGeometry(mStaticGeometry);

    if (mRaySceneQuery != 0)
        sceneMgr->destroyQuery(mRaySceneQuery);
    SceneNode::ObjectIterator objectIterator = mSceneNode->getAttachedObjectIterator();
    while (objectIterator.hasMoreElements())
    {
        MovableObject* m = objectIterator.getNext();
        sceneMgr->destroyMovableObject(m);
    }
    sceneMgr->destroySceneNode(mSceneNode->getName());
    mSceneNode = 0;
}

//-------------------------------------------------------------------------------------
void Scene::convertToStaticGeometry()
{
    delete mStaticGeometry;

    if (mSceneNode == 0) return;
    SceneManager* sceneMgr = mSceneNode->getCreator();

    // Add all visible entities (1 loop because StaticGeometry::addSceneNode() add invisible entities)
    mStaticGeometry = sceneMgr->createStaticGeometry(mSceneNode->getName());
    std::list<MovableObject*> movableObjectsList;
    OgreHelpers::getMovableObjectsList(mSceneNode, "Entity", movableObjectsList);
    for (std::list<MovableObject*>::iterator movableObject = movableObjectsList.begin();movableObject != movableObjectsList.end();++movableObject)
    {
        Entity* entity = static_cast<Entity*>(*movableObject);
        SceneNode* sceneNode = entity->getParentSceneNode();
        if (entity->isVisible()) mStaticGeometry->addEntity(entity,
            sceneNode->_getDerivedPosition(),
            sceneNode->_getDerivedOrientation(),
            sceneNode->_getDerivedScale());
        entity->setVisible(false);
    }
    mStaticGeometry->build();
}

//-------------------------------------------------------------------------------------
