#include "Scene.h"

Scene::Scene(Peer* peer, SceneNode* sceneNode, RaySceneQuery* raySceneQuery) :
    OgrePeer(peer),
    mSceneNode(sceneNode),
    mStaticGeometry(0),
    mRaySceneQuery(raySceneQuery)
{
}

Scene::~Scene()
{
    destroy();
}

SceneNode* Scene::getSceneNode()
{
    return mSceneNode;
}

void Scene::update(Ogre::Real timeSinceLastFrame)
{
    // Optimize by converting it into static geometry
    if (mStaticGeometry == 0)
        convertToStaticGeometry();
}

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

void Scene::convertToStaticGeometry()
{
    delete mStaticGeometry;

    if (mSceneNode == 0) return;
    SceneManager* sceneMgr = mSceneNode->getCreator();

    mStaticGeometry = sceneMgr->createStaticGeometry(mSceneNode->getName());
    mStaticGeometry->addSceneNode(mSceneNode);
    mStaticGeometry->build();

    std::list<MovableObject*> movableObjectsList;
    getMovableObjectsList(mSceneNode, "Entity", movableObjectsList);
    for (std::list<MovableObject*>::iterator movableObject = movableObjectsList.begin();movableObject != movableObjectsList.end();++movableObject)
        (*movableObject)->setVisible(false);
}

void Scene::getMovableObjectsList(SceneNode* node, const String movableType, std::list<MovableObject*> &movableObjectsList)
{
    SceneNode::ObjectIterator objectIterator = node->getAttachedObjectIterator();
    while (objectIterator.hasMoreElements())
    {
        MovableObject* movableObject = objectIterator.getNext();
        if (movableObject->getMovableType().compare(movableType) == 0)
            movableObjectsList.push_back(movableObject);
    }
    Node::ChildNodeIterator childNodeIterator = node->getChildIterator();
    while (childNodeIterator.hasMoreElements())
    {
        Node* childNode = childNodeIterator.getNext();
        getMovableObjectsList((SceneNode*)childNode, movableType, movableObjectsList);
    }
}
