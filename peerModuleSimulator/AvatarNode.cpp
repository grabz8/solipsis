#include "AvatarNode.h"
#include "Site.h"
#include "Peer.h"
#include "OgreHelpers.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
AvatarNode::AvatarNode(const NodeId& nodeId, XmlEntity* xmlEntity) :
    Node(nodeId, "avatar"),
    mMutex(PTHREAD_MUTEX_INITIALIZER),
    mAvatar(xmlEntity),
    mPhysicsScene(0)
{
    OGRE_LOG("AvatarNode::AvatarNode() uid:" + StringConverter::toString(mAvatar.getXmlEntity()->getUid()));

#ifdef PHYSICSPLUGINS
    IPhysicsEngine* engine = PhysicsEngineManager::getSingleton().getSelectedEngine();
    if (engine == 0)
        throw Exception(Exception::ERR_INTERNAL_ERROR,
        "No physics engine selected !",
        "AvatarNode::AvatarNode");
    OGRE_LOG("AvatarNode::AvatarNode() uid:" + StringConverter::toString(mAvatar.getXmlEntity()->getUid()) + " creating physics scene with engine:" + engine->getName());
    // Create the physical scene
    mPhysicsScene = engine->createScene();
    if (!mPhysicsScene->create())
    {
        throw Exception(Exception::ERR_INTERNAL_ERROR,
        "Unable to create the PhysX scene !",
        "AvatarNode::AvatarNode");
    }
#endif

    addAwareEntity(&mAvatar);

    Peer::getSingleton().addTimeListener(this);
}

//-------------------------------------------------------------------------------------
AvatarNode::~AvatarNode()
{
    removeAwareEntity(&mAvatar);

    pthread_mutex_lock(&mMutex);

    if (!mFrozen)
        Peer::getSingleton().removeTimeListener(this);

#ifdef PHYSICSPLUGINS
    OGRE_LOG("AvatarNode::~AvatarNode() uid:" + StringConverter::toString(mAvatar.getXmlEntity()->getUid()) + " destroying physics scene");
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
    OGRE_LOG("AvatarNode::addAwareEntity() uid:" + StringConverter::toString(mAvatar.getXmlEntity()->getUid()) + " adding entity uid:" + StringConverter::toString(entity->getXmlEntity()->getUid()));

    pthread_mutex_lock(&mMutex);

    mAwareEntities[entity->getXmlEntity()->getUid()] = entity;
    if (entity->getXmlEntity()->getOwner().compare(mNodeId) == 0)
    {
        OGRE_LOG("AvatarNode::addAwareEntity() uid:" + StringConverter::toString(mAvatar.getXmlEntity()->getUid()) + " owned entity uid:" + StringConverter::toString(entity->getXmlEntity()->getUid()));
        mOwnedEntities[entity->getXmlEntity()->getUid()] = entity;
    }
    else
    {
        OGRE_LOG("AvatarNode::addAwareEntity() uid:" + StringConverter::toString(mAvatar.getXmlEntity()->getUid()) + " adding listener on entity uid:" + StringConverter::toString(entity->getXmlEntity()->getUid()));
        entity->addEntityListener(this);
    }

#ifdef PHYSICSPLUGINS
    // for instance we create physics of scenes + my own avatar
    if (!((entity->getXmlEntity()->getType() == ETAvatar) && (entity->getXmlEntity()->getOwner().compare(mNodeId) != 0)))
    {
        OGRE_LOG("AvatarNode::addAwareEntity() uid:" + StringConverter::toString(mAvatar.getXmlEntity()->getUid()) + " creating physics of entity uid:" + StringConverter::toString(entity->getXmlEntity()->getUid()));
        entity->createPhysics(mPhysicsScene);
        if (entity->getXmlEntity()->getType() == ETSite)
            mAvatar.setGravity(true);
    }
#endif

    pthread_mutex_unlock(&mMutex);

#ifdef POOL
    RefCntPoolPtr<XmlEvt> evt;
    evt->setType(ETNewEntity);
    evt->setDatas(RefCntPoolPtr<XmlData>(entity->getXmlEntity()));
    pthread_mutex_lock(&mEvtsMutex);
    mEvtsToHandleList.push_back(evt);
    pthread_mutex_unlock(&mEvtsMutex);
#else
    XmlEvt* evt = new XmlEvt(ETNewEntity);
    evt->setDatas(entity->getXmlEntity());
    pthread_mutex_lock(&mEvtsMutex);
    mEvtsToHandleList.push_back(evt);
    pthread_mutex_unlock(&mEvtsMutex);
#endif

    return true;
}

//-------------------------------------------------------------------------------------
bool AvatarNode::removeAwareEntity(Entity* entity)
{
    OGRE_LOG("AvatarNode::removeAwareEntity() uid:" + StringConverter::toString(mAvatar.getXmlEntity()->getUid()) + " removing entity uid:" + StringConverter::toString(entity->getXmlEntity()->getUid()));

    pthread_mutex_lock(&mMutex);

    if (entity->getXmlEntity()->getOwner().compare(mNodeId) != 0)
    {
        OGRE_LOG("AvatarNode::removeAwareEntity() uid:" + StringConverter::toString(mAvatar.getXmlEntity()->getUid()) + " removing listener on entity uid:" + StringConverter::toString(entity->getXmlEntity()->getUid()));
        entity->removeEntityListener(this);
    }

    mAwareEntities.erase(entity->getXmlEntity()->getUid());

#ifdef PHYSICSPLUGINS
    if (!((entity->getXmlEntity()->getType() == ETAvatar) && (entity->getXmlEntity()->getOwner().compare(mNodeId) != 0)))
    {
        OGRE_LOG("AvatarNode::removeAwareEntity() uid:" + StringConverter::toString(mAvatar.getXmlEntity()->getUid()) + " destroying physics of entity uid:" + StringConverter::toString(entity->getXmlEntity()->getUid()));
        entity->destroyPhysics();
    }
#endif

    pthread_mutex_unlock(&mMutex);

#ifdef POOL
    RefCntPoolPtr<XmlEvt> evt;
    evt->setType(ETLostEntity);
    evt->setDatas(RefCntPoolPtr<XmlData>(entity->getXmlEntity()));
    pthread_mutex_lock(&mEvtsMutex);
    mEvtsToHandleList.push_back(evt);
    pthread_mutex_unlock(&mEvtsMutex);
#else
    XmlEvt* evt = new XmlEvt(ETLostEntity);
    evt->setDatas(entity->getXmlEntity());
    pthread_mutex_lock(&mEvtsMutex);
    mEvtsToHandleList.push_back(evt);
    pthread_mutex_unlock(&mEvtsMutex);
#endif

    return true;
}

//-------------------------------------------------------------------------------------
bool AvatarNode::processEvt(XmlEvt& xmlEvt, std::string& xmlRespStr)
{
    static int c;
    static unsigned long l = (unsigned long)-1;
    if (xmlEvt.getType() == ETUpdatedEntity)
    {
#ifdef POOL
        XmlEntity* xmlEntity = (XmlEntity*)xmlEvt.getDatas().get();
#else
        XmlEntity* xmlEntity = (XmlEntity*)xmlEvt.getDatas();
#endif
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
                OGRE_LOG("RCV uid:" + StringConverter::toString(xmlEntity->getUid()) + " " + StringConverter::toString(xmlEntity->getDisplacement()));
#endif
                unsigned long n = Root::getSingleton().getTimer()->getMilliseconds();
                if (l == (unsigned long)-1) { l = n; c = 0; }
                c++;
                if (n - l > 10000)
                {
                    Real fr = (Real)c/10.0f;
                    OGRE_LOG("AvatarNode::processEvt() fr=" + StringConverter::toString(fr));
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
#ifdef POOL
bool AvatarNode::freeEvt(RefCntPoolPtr<XmlEvt>& evt)
#else
bool AvatarNode::freeEvt(XmlEvt* evt)
#endif
{
    pthread_mutex_lock(&mEvtsMutex);
#ifdef POOL
    if (evt->getDatas() == mAvatar.mUpdatedXmlEntity)
        mAvatar.mUpdatedXmlEntity->setDefinedAttributes(XmlEntity::DANone);
#else
    if (evt->getDatas() == &mAvatar.mUpdatedXmlEntity)
        mAvatar.mUpdatedXmlEntity.setDefinedAttributes(XmlEntity::DANone);
#endif
    pthread_mutex_unlock(&mEvtsMutex);

    return Node::freeEvt(evt);
}

//-------------------------------------------------------------------------------------
bool AvatarNode::freeze(bool frozen)
{
    pthread_mutex_lock(&mMutex);
    if (frozen != mFrozen)
    {
        if (frozen)
            Peer::getSingleton().removeTimeListener(this);
        else
            Peer::getSingleton().addTimeListener(this);
        mFrozen = frozen;
    }
    pthread_mutex_unlock(&mMutex);

    return true;
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
    for (Entity::EntityMap::iterator obj = mOwnedEntities.begin();obj != mOwnedEntities.end();++obj)
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
#ifdef POOL
        if (!mAvatar.mUpdatedXmlEntity->getDefinedAttributes() & XmlEntity::DAUid)
        {
            RefCntPoolPtr<XmlEvt> evt;
            evt->setType(ETUpdatedEntity);
            mAvatar.mUpdatedXmlEntity->setUid(mAvatar.getXmlEntity()->getUid());
            mAvatar.mUpdatedXmlEntity->setPosition(mAvatar.getXmlEntity()->getPosition());
#ifdef LOGSNDRCV
            OGRE_LOG("SND uid:" + StringConverter::toString(mAvatar.getXmlEntity()->getUid()) + " " + StringConverter::toString(mAvatar.getXmlEntity()->getPosition()));
#endif
            evt->setDatas(RefCntPoolPtr<XmlData>(mAvatar.mUpdatedXmlEntity));
#else
        if (!mAvatar.mUpdatedXmlEntity.getDefinedAttributes() & XmlEntity::DAUid)
        {
            XmlEvt* evt = new XmlEvt(ETUpdatedEntity);
            mAvatar.mUpdatedXmlEntity.setUid(mAvatar.getXmlEntity()->getUid());
            mAvatar.mUpdatedXmlEntity.setPosition(mAvatar.getXmlEntity()->getPosition());
#ifdef LOGSNDRCV
            OGRE_LOG("SND uid:" + StringConverter::toString(mAvatar.getXmlEntity()->getUid()) + " " + StringConverter::toString(mAvatar.getXmlEntity()->getPosition()));
#endif
            evt->setDatas(&mAvatar.mUpdatedXmlEntity);
#endif
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
#ifdef POOL
    if (mXmlEntityMap.find(entity.getXmlEntity()->getUid()) == mXmlEntityMap.end())
        mXmlEntityMap[entity.getXmlEntity()->getUid()] = RefCntPoolPtr<XmlEntity>(new XmlEntity(entity.getXmlEntity()->getUid()));
    RefCntPoolPtr<XmlEntity>& local = mXmlEntityMap[entity.getXmlEntity()->getUid()];
    XmlEntity* evtEntity = (XmlEntity*)xmlEvt.getDatas().get();
    local->setDefinedAttributes(evtEntity->getDefinedAttributes());
    if (local->getDefinedAttributes() & XmlEntity::DAPosition)
        local->setPosition(evtEntity->getPosition());
    if (local->getDefinedAttributes() & XmlEntity::DAOrientation)
        local->setOrientation(evtEntity->getOrientation());
    RefCntPoolPtr<XmlEvt> evt;
    evt->setType(ETUpdatedEntity);
    evt->setDatas(RefCntPoolPtr<XmlData>(local));
    mEvtsToHandleList.push_back(evt);
#else
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
#endif
    pthread_mutex_unlock(&mEvtsMutex);

    return true;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
