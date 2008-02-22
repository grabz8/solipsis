#include "AvatarNode.h"
#include "Site.h"
#include "Peer.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
AvatarNode::AvatarNode(const NodeId& nodeId, XmlEntity* xmlEntity) :
    Node(nodeId, "avatar"),
    mMutex(PTHREAD_MUTEX_INITIALIZER),
    mAvatar(xmlEntity)
{
#ifdef PHYSICSPLUGINS
    IPhysicsEngine* engine = PhysicsEngineManager::getSingleton().getSelectedEngine();
    // Create the physical scene
    mPhysicsScene = engine->createScene();
    if (!mPhysicsScene->create())
        Exception(Exception::ERR_INTERNAL_ERROR,
        "Unable to create the PhysX scene !",
        "PhysXScene::PhysXScene");
#endif

    addAwareEntity(&mAvatar);

    Peer::getSingleton().addTimeListener(this);
}

//-------------------------------------------------------------------------------------
AvatarNode::~AvatarNode()
{
    pthread_mutex_lock(&mMutex);

    Peer::getSingleton().removeTimeListener(this);

#ifdef PHYSICSPLUGINS
    if (mPhysicsScene != 0)
        PhysicsEngineManager::getSingleton().getSelectedEngine()->destroyScene(mPhysicsScene);
    mPhysicsScene = 0;
#endif

    pthread_mutex_unlock(&mMutex);
}

//-------------------------------------------------------------------------------------
Avatar& AvatarNode::getEntity()
{
    return mAvatar;
}

//-------------------------------------------------------------------------------------
Entity::EntityMap& AvatarNode::getOwnedEntities()
{
    return mOwnedEntities;
}

#ifdef PHYSICSPLUGINS
//-------------------------------------------------------------------------------------
IPhysicsScene* AvatarNode::getPhysicsScene()
{
    return mPhysicsScene;
}
#endif

//-------------------------------------------------------------------------------------
bool AvatarNode::addAwareEntity(Entity* entity)
{
    pthread_mutex_lock(&mMutex);

    mAwareEntities[entity->getXmlEntity()->getUid()] = entity;
    if (entity->getXmlEntity()->getOwner().compare(mNodeId) == 0)
        mOwnedEntities[entity->getXmlEntity()->getUid()] = entity;
    else
        entity->addEntityListener(this);

#ifdef PHYSICSPLUGINS
    entity->createPhysics(mPhysicsScene);
    if (entity->getXmlEntity()->getType() == ETSite)
        mAvatar.setGravity(true);
#endif

    pthread_mutex_unlock(&mMutex);

    XmlEvt* evt = new XmlEvt(ETNewEntity);
    evt->setDatas(entity->getXmlEntity());
    pthread_mutex_lock(&mEvtsMutex);
    mEvtsToHandleList.push_back(evt);
    pthread_mutex_unlock(&mEvtsMutex);

    return true;
}

//-------------------------------------------------------------------------------------
bool AvatarNode::removeAwareEntity(Entity* entity)
{
    pthread_mutex_lock(&mMutex);

    if (entity->getXmlEntity()->getOwner().compare(mNodeId) != 0)
        entity->removeEntityListener(this);

    mAwareEntities.erase(entity->getXmlEntity()->getUid());

#ifdef PHYSICSPLUGINS
    entity->destroyPhysics();
#endif

    pthread_mutex_unlock(&mMutex);

    XmlEvt* evt = new XmlEvt(ETLostEntity);
    evt->setDatas(entity->getXmlEntity());
    pthread_mutex_lock(&mEvtsMutex);
    mEvtsToHandleList.push_back(evt);
    pthread_mutex_unlock(&mEvtsMutex);

    return true;
}

//-------------------------------------------------------------------------------------
bool AvatarNode::processEvt(XmlEvt& xmlEvt, std::string& xmlRespStr)
{
    static int c;
    static unsigned long l = (unsigned long)-1;
    if (xmlEvt.getType() == ETUpdatedEntity)
    {
        XmlEntity* xmlEntity = (XmlEntity*)xmlEvt.getDatas();
        if (xmlEntity == 0)
        {
            xmlRespStr = "No entity found in event !";
            return false;
        }
        Entity* entity = mAwareEntities[xmlEntity->getUid()];
        if (entity == 0)
            return true;
        pthread_mutex_lock(&mMutex);
        if (entity->getXmlEntity()->getType() == ETAvatar)
        {
            Avatar* avatar = (Avatar*)entity;
            if (xmlEntity->getDefinedAttributes() & XmlEntity::DAFlags)
            {
                EntityFlags diff = avatar->getXmlEntity()->getFlags() ^ xmlEntity->getFlags();
                if (diff & EFGravity)
                    avatar->setGravity(xmlEntity->getFlags() & EFGravity);
                avatar->getXmlEntity()->setFlags(xmlEntity->getFlags());
            }
            if (xmlEntity->getDefinedAttributes() & XmlEntity::DADisplacement)
            {
                avatar->getXmlEntity()->setDisplacement(xmlEntity->getDisplacement());
#ifdef LOGSNDRCV
                LogManager::getSingleton().logMessage("RCV " + StringConverter::toString(xmlEntity->getDisplacement()));
#endif
                unsigned long n = Root::getSingleton().getTimer()->getMilliseconds();
                if (l == (unsigned long)-1) { l = n; c = 0; }
                c++;
                if (n - l > 10000)
                {
                    Real fr = (Real)c/10.0f;
                    LogManager::getSingleton().logMessage("AvatarNode::processEvt() fr=" + StringConverter::toString(fr));
                    l = n; c = 0;
                }
            }
            if (xmlEntity->getDefinedAttributes() & XmlEntity::DAOrientation)
            {
                avatar->getXmlEntity()->setOrientation(xmlEntity->getOrientation());
                mAvatar.throwUpdateToEntityListeners(*this, mAvatar, xmlEvt);
            }
        }
        pthread_mutex_unlock(&mMutex);
    }

    return true;
}

//-------------------------------------------------------------------------------------
bool AvatarNode::freeEvt(XmlEvt* evt)
{
    pthread_mutex_lock(&mEvtsMutex);
    if (evt->getDatas() == &mAvatar.mUpdatedXmlEntity)
        mAvatar.mUpdatedXmlEntity.setDefinedAttributes(XmlEntity::DANone);
    pthread_mutex_unlock(&mEvtsMutex);

    return Node::freeEvt(evt);
}

//-------------------------------------------------------------------------------------
bool AvatarNode::tick(Real timeSinceLastTick)
{
    pthread_mutex_lock(&mMutex);

#ifdef PHYSICSPLUGINS
    // Step physics part 1
    if (mPhysicsScene != 0)
        mPhysicsScene->preStep(timeSinceLastTick);
#endif

    // Update entities
    for (Entity::EntityMap::iterator obj = mAwareEntities.begin();obj != mAwareEntities.end();++obj)
        obj->second->update(timeSinceLastTick);

#ifdef PHYSICSPLUGINS
    // Step physics part 2
    if (mPhysicsScene != 0)
        mPhysicsScene->postStep();
#endif

//    pthread_mutex_unlock(&mMutex);

/*static int c=0;
c++;
    if (mAvatar.mDirty && ((c % 5) == 0))*/
    if (mAvatar.mDirty)
    {
        pthread_mutex_lock(&mEvtsMutex);
        if (!mAvatar.mUpdatedXmlEntity.getDefinedAttributes() & XmlEntity::DAUid)
        {
            XmlEvt* evt = new XmlEvt(ETUpdatedEntity);
            mAvatar.mUpdatedXmlEntity.setUid(mAvatar.getXmlEntity()->getUid());
            mAvatar.mUpdatedXmlEntity.setPosition(mAvatar.getXmlEntity()->getPosition());
#ifdef LOGSNDRCV
            LogManager::getSingleton().logMessage("SND " + StringConverter::toString(mAvatar.getXmlEntity()->getPosition()));
#endif
            evt->setDatas(&mAvatar.mUpdatedXmlEntity);
            mEvtsToHandleList.push_back(evt);
            mAvatar.throwUpdateToEntityListeners(*this, mAvatar, *evt);
            mAvatar.mDirty = false;
        }
        pthread_mutex_unlock(&mEvtsMutex);
    }

    pthread_mutex_unlock(&mMutex);

    return true;
}

//-------------------------------------------------------------------------------------
bool AvatarNode::updated(const Node& node, Entity& entity, XmlEvt& xmlEvt)
{
    pthread_mutex_lock(&mEvtsMutex);
//    XmlEvt* evt = new XmlEvt(ETUpdatedEntity);
//    evt->setDatas(xmlEvt.getDatas());
//    mEvtsToHandleList.push_back(evt);
    if (mXmlEntityMap.find(entity.getXmlEntity()->getUid()) == mXmlEntityMap.end())
        mXmlEntityMap[entity.getXmlEntity()->getUid()] = new XmlEntity(entity.getXmlEntity()->getUid());
    XmlEntity* local = mXmlEntityMap[entity.getXmlEntity()->getUid()];
    local->setDefinedAttributes(((XmlEntity*)xmlEvt.getDatas())->getDefinedAttributes());
    if (local->getDefinedAttributes() & XmlEntity::DAPosition)
        local->setPosition(((XmlEntity*)xmlEvt.getDatas())->getPosition());
    if (local->getDefinedAttributes() & XmlEntity::DAOrientation)
        local->setOrientation(((XmlEntity*)xmlEvt.getDatas())->getOrientation());
    XmlEvt* evt = new XmlEvt(ETUpdatedEntity);
    evt->setDatas(local);
    mEvtsToHandleList.push_back(evt);
    pthread_mutex_unlock(&mEvtsMutex);

    return true;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
