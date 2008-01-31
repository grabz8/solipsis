#include "Scene.h"
#include "OgreHelpers.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
Scene::Scene(XmlObject* object, bool isLocal, SceneNode* sceneNode, RaySceneQuery* raySceneQuery) :
    OgrePeer(object, isLocal),
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
bool Scene::update(XmlObject* updateObject)
{
    if (updateObject->getDefinedAttributes().Test(XmlObject::DAPosition))
    {
        mSceneNode->setPosition(updateObject->getPosition());
    }
    if (updateObject->getDefinedAttributes().Test(XmlObject::DAOrientation))
    {
        mSceneNode->setOrientation(updateObject->getOrientation());
    }

    return true;
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
