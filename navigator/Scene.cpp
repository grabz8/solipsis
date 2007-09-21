#include "Scene.h"

Scene::Scene(Peer* peer, SceneNode* sceneNode, RaySceneQuery* raySceneQuery) :
    OgrePeer(peer),
    mSceneNode(sceneNode),
    mRaySceneQuery(raySceneQuery)
{
}

Scene::~Scene()
{
    if (mSceneNode == 0) return;
    if (mRaySceneQuery != 0)
        mSceneNode->getCreator()->destroyQuery(mRaySceneQuery);
    SceneNode::ObjectIterator oit = mSceneNode->getAttachedObjectIterator();
    while (oit.hasMoreElements())
    {
        MovableObject* m = oit.getNext();
        mSceneNode->getCreator()->destroyMovableObject(m);
    }
    mSceneNode->getCreator()->destroySceneNode(mSceneNode->getName());
}

SceneNode* Scene::getSceneNode()
{
    return mSceneNode;
}

void Scene::update(Ogre::Real timeSinceLastFrame)
{
}
