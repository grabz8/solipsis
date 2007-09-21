#include "Avatar.h"

String Avatar::mDefaultStateAnimName[SCount] = {
    "",
    "Idle",
    "Walk",
    "Run",
    "Fly",
    "Swim"
};

#define MAX_SPEED 400
#define STARTING_IMPULSE_SPEED 25
#define EPSILON_DIST 0.01

Avatar::Avatar(Peer* peer, SceneNode* sceneNode, Entity* entity, RaySceneQuery* raySceneQuery) :
    OgrePeer(peer),
    mState(SNone),
    mGravity(false),
    mSceneNode(sceneNode),
    mEntity(entity),
    mRaySceneQuery(raySceneQuery),
    time(0),
    mDirection(Vector3(EPSILON_DIST,0,0)),
    mBackwardDist(0),
    mUpKeyPressed(false),
    mDownKeyPressed(false),
    mLeftKeyPressed(false),
    mRightKeyPressed(false),
    mPgupKeyPressed(false),
    mPgdownKeyPressed(false)
{
    for (int a = 0;a < SCount; ++a)
        mStateAnimName[a] = mDefaultStateAnimName[a];

    mSceneNode->attachObject(entity);
    lookAtTheGoodDirection();

    // Set Name Label
    mNameLabel = new MovableText("Label" + peer->getLogin(), peer->getLogin(), false);
    mNameLabel->setCharacterHeight(6);
    mNameLabel->setColor(Ogre::ColourValue::White);
    mNameLabel->setTextAlignment(MovableText::H_CENTER, MovableText::V_ABOVE); // Center horizontally and display above the node
    Real aabbHeightDiv2 = entity->getBoundingBox().getSize().y*0.5;
    Real scale = mSceneNode->getScale().y;
    mNameLabel->setAdditionalHeight(aabbHeightDiv2*(1 + scale));
    mSceneNode->attachObject(mNameLabel);
}

Avatar::~Avatar()
{
    if (mSceneNode == 0) return;
    if (mRaySceneQuery != 0)
        mSceneNode->getCreator()->destroyQuery(mRaySceneQuery);
    if (mEntity != 0) {
        mSceneNode->detachObject(mEntity);
        mSceneNode->getCreator()->destroyEntity(mEntity);
    }
    mSceneNode->getCreator()->destroySceneNode(mSceneNode->getName());
}

SceneNode* Avatar::getSceneNode()
{
    return mSceneNode;
}

Entity* Avatar::getEntity()
{
    return mEntity;
}

void Avatar::setName(const String& name)
{
    mNameLabel->setCaption(name);
}

void Avatar::setNameVisibility(bool visible)
{
    mNameLabel->setVisible(visible);
}

void Avatar::setGravity(bool enabled) {
    mGravity = enabled;
}

void Avatar::setState(State state)
{
    if (mStateAnimName[state].length() > 0)
        startAnimation(mStateAnimName[state]);
    else if (mStateAnimName[mState].length() > 0)
    {
        mAnimationState = mEntity->getAnimationState(mStateAnimName[mState]);
        mAnimationState->setLoop(false);
        mAnimationState->setEnabled(false);
    }
    mState = state;
}

Avatar::State Avatar::getState()
{
    return mState;
}

void Avatar::setStateAnimName(State state, const String& name)
{
    mStateAnimName[state] = name;
}

void Avatar::update(Ogre::Real timeSinceLastFrame)
{
    animate(timeSinceLastFrame);
}

void Avatar::startAnimation(const String &name, bool loop)
{
    mAnimationState = mEntity->getAnimationState(name);
    mAnimationState->setLoop(loop);
    mAnimationState->setEnabled(true);
}

void Avatar::animate(Ogre::Real timeSinceLastFrame)
{
    AxisAlignedBox waabb = mSceneNode->_getWorldAABB();
    time += timeSinceLastFrame;
    if (time > 0.02)
    {
        //Every half second..
        time = 0;
        bool doIdle = false;

        Real distance = mDirection.normalise();
        if (mUpKeyPressed)
        {
            if (distance < MAX_SPEED)
            {
                distance *= 1.5;
                if ((distance > MAX_SPEED/2) && (getState() != SRun))
                    setState(SRun);
            }
        }
        else
        {
            if (distance > EPSILON_DIST)
            {
                distance *= 0.75;
                if (distance <= EPSILON_DIST)
                    doIdle = true;
            }
        }
        mDirection = mDirection*distance; //come back to unnormalized value

        if (mDownKeyPressed)
        {
            if (mBackwardDist < MAX_SPEED/2)
            {
                //No more than half of walk speed
                mBackwardDist *= 1.5;
                if (mBackwardDist == 0)
                    mBackwardDist = STARTING_IMPULSE_SPEED;
            }
        } 
        else if (mBackwardDist > EPSILON_DIST)
        {
            mBackwardDist *= 0.75;
            if (mBackwardDist <= EPSILON_DIST)
                doIdle = true;
        }

        if (mLeftKeyPressed || mRightKeyPressed)
        {
           mYaw = ((Ogre::Math::HALF_PI/16)*((mRightKeyPressed) ? -1 : 1));
           mDirection = Ogre::Quaternion(Radian(mYaw),Vector3::UNIT_Y)*mDirection;
           lookAtTheGoodDirection();
        }

        if (mPgdownKeyPressed || mPgupKeyPressed)
        {
            mSceneNode->translate(Vector3::UNIT_Y*(((mPgdownKeyPressed) ? -1 : +1)*(MAX_SPEED/2)*timeSinceLastFrame));
        }

        /* Fly
        if (mPgupKeyPressed)
        {
            if (mDirection.y < MAX_SPEED)
            {
                mDirection.y *= 1.15;
                if (mDirection.y == 0)
                    mDirection.y = STARTING_IMPULSE_SPEED;
            }
        } else if (mDirection.y>EPSILON_DIST)
        {
            mDirection.y *= 0.6;
            if (mDirection.y <= EPSILON_DIST)
                mDirection.y = 0;
        }*/

        if (mGravity && (mRaySceneQuery != 0))
        {
            //Here is a fake gravity, follow ground if any
            Vector3 pos = mSceneNode->getPosition();
            Vector3 avatarSize = mEntity->getBoundingBox().getSize();
            Ray ray(pos + avatarSize/2, Vector3::NEGATIVE_UNIT_Y); //Ray from the middle of avatar, direction:floor
            mRaySceneQuery->setRay(ray);
            mRaySceneQuery->setSortByDistance(true);
            RaySceneQueryResult &result = mRaySceneQuery->execute();
            RaySceneQueryResult::iterator itray = result.begin();
            if (itray!=result.end() && itray->worldFragment)
            {
                Real height = itray->worldFragment->singleIntersection.y;
                mSceneNode->setPosition(pos.x,height+10.0f,pos.z);
            }
/*            Vector3 hit;
            if (RaycastFromPoint(ray, result, String("station"), hit))
                mSceneNode->setPosition(pos.x,hit.y,pos.z);*/
        }

        if (doIdle)
            setState(SIdle);
    }

    Real animOffset = 0;
    if (mBackwardDist > EPSILON_DIST)
    {
        mDirection.normalise();
        mSceneNode->translate(mDirection*(-mBackwardDist*timeSinceLastFrame));
        animOffset = - (timeSinceLastFrame*(mBackwardDist/(MAX_SPEED/4))); //(MAX_SPEED/4) is speed where walk anim is in real time
    }
    else if (mDirection.length() > EPSILON_DIST)
    {
        mSceneNode->translate(mDirection*timeSinceLastFrame);
        animOffset = timeSinceLastFrame*(mDirection.length()/(MAX_SPEED/4)); //(MAX_SPEED/4) is speed where walk anim is in real time
    }
    else
        animOffset = timeSinceLastFrame;

    mAnimationState->addTime(animOffset);

}

void Avatar::movementKeyPressed(OIS::KeyCode code)
{
    using namespace OIS;
    switch (code) {
       case KC_UP:
           mUpKeyPressed = true;
       break;
       case KC_DOWN:
           mDownKeyPressed = true;
       break;
       case KC_LEFT:
           mLeftKeyPressed = true;
       break;
       case KC_RIGHT:
           mRightKeyPressed = true;
       break;
       case KC_PGUP:
           mPgupKeyPressed = true;
       break;
       case KC_PGDOWN:
           mPgdownKeyPressed = true;
       break;
    }

    //Walk anim if needed
    if ((mUpKeyPressed || mDownKeyPressed || mLeftKeyPressed || mRightKeyPressed) && (getState() == SIdle))
    {
        time = 1000;
        setState(SWalk);
        //Starting Impulse
        mDirection.normalise();
        mDirection *= STARTING_IMPULSE_SPEED; 
    }
}

void Avatar::lookAtTheGoodDirection()
{
    Vector3 src = mSceneNode->getOrientation()*Vector3::UNIT_X;
    Vector3 direction = mDirection;
    direction.y = 0; //Facing horizon !
    if ((1.0f + src.dotProduct(direction)) < 0.0001f) {
        mSceneNode->yaw(Degree(180));
    }
    else
    {
        Ogre::Quaternion quat = src.getRotationTo(direction);
        mSceneNode->rotate(quat);
    } 
}

void Avatar::movementKeyReleased(OIS::KeyCode code)
{
    using namespace OIS;
    switch (code) {
       case KC_UP:
           mUpKeyPressed = false;
       break;
       case KC_DOWN:
           mDownKeyPressed = false;
       break;
       case KC_LEFT:
           mLeftKeyPressed = false;
       break;
       case KC_RIGHT:
           mRightKeyPressed = false;
       break;
       case KC_PGUP:
           mPgupKeyPressed = false;
       break;
       case KC_PGDOWN:
           mPgdownKeyPressed = false;
       break;
    }
}

/*
// raycast from a point in to the scene.
// returns success or failure.
// on success the point is returned in the result.
bool Avatar::RaycastFromPoint(Ray& ray,
                              RaySceneQueryResult& query_result,
                              String& entity_name,
                              Vector3& result)
{
    // at this point we have raycast to a series of different objects bounding boxes.
    // we need to test these different objects to see which is the first polygon hit.
    // there are some minor optimizations (distance based) that mean we wont have to
    // check all of the objects most of the time, but the worst case scenario is that
    // we need to test every triangle of every object.
    Ogre::Real closest_distance = -1.0f;
    Ogre::Vector3 closest_result;
    for (size_t qr_idx = 0; qr_idx < query_result.size(); qr_idx++)
    {
        // stop checking if we have found a raycast hit that is closer
        // than all remaining entities
        if ((closest_distance >= 0.0f) &&
            (closest_distance < query_result[qr_idx].distance))
        {
             break;
        }
       
        // only check this result if its a hit against an entity
        if ((query_result[qr_idx].movable != NULL) &&
            (query_result[qr_idx].movable->getMovableType().compare("Entity") == 0) &&
            (static_cast<Ogre::Entity*>(query_result[qr_idx].movable)->getName().compare(entity_name) == 0))
        {
            // get the entity to check
            Ogre::Entity *pentity = static_cast<Ogre::Entity*>(query_result[qr_idx].movable);           

            // mesh data to retrieve         
            size_t vertex_count;
            size_t index_count;
            Ogre::Vector3 *vertices;
            unsigned long *indices;

            // get the mesh information
            GetMeshInformation(pentity->getMesh(), vertex_count, vertices, index_count, indices,             
                              pentity->getParentNode()->getWorldPosition(),
                              pentity->getParentNode()->getWorldOrientation(),
                              pentity->getParentNode()->getScale());

            // test for hitting individual triangles on the mesh
            bool new_closest_found = false;
            for (int i = 0; i < static_cast<int>(index_count); i += 3)
            {
                // check for a hit against this triangle
                std::pair<bool, Ogre::Real> hit = Ogre::Math::intersects(ray, vertices[indices[i]],
                    vertices[indices[i+1]], vertices[indices[i+2]], true, false);

                // if it was a hit check if its the closest
                if (hit.first)
                {
                    if ((closest_distance < 0.0f) ||
                        (hit.second < closest_distance))
                    {
                        // this is the closest so far, save it off
                        closest_distance = hit.second;
                        new_closest_found = true;
                    }
                }
            }

         // free the verticies and indicies memory
            delete[] vertices;
            delete[] indices;

            // if we found a new closest raycast for this object, update the
            // closest_result before moving on to the next object.
            if (new_closest_found)
            {
                closest_result = ray.getPoint(closest_distance);               
            }
        }       
    }

    // return the result
    if (closest_distance >= 0.0f)
    {
        // raycast success
        result = closest_result.x;
        return (true);
    }
    else
    {
        // raycast failed
        return (false);
    }
}

// Get the mesh information for the given mesh.
// Code found on this forum link: http://www.ogre3d.org/wiki/index.php/RetrieveVertexData
void Avatar::GetMeshInformation(const Ogre::MeshPtr mesh,
                                size_t &vertex_count,
                                Ogre::Vector3* &vertices,
                                size_t &index_count,
                                unsigned long* &indices,
                                const Ogre::Vector3 &position,
                                const Ogre::Quaternion &orient,
                                const Ogre::Vector3 &scale)
{
    bool added_shared = false;
    size_t current_offset = 0;
    size_t shared_offset = 0;
    size_t next_offset = 0;
    size_t index_offset = 0;

    vertex_count = index_count = 0;

    // Calculate how many vertices and indices we're going to need
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh( i );

        // We only need to add the shared vertices once
        if(submesh->useSharedVertices)
        {
            if( !added_shared )
            {
                vertex_count += mesh->sharedVertexData->vertexCount;
                added_shared = true;
            }
        }
        else
        {
            vertex_count += submesh->vertexData->vertexCount;
        }

        // Add the indices
        index_count += submesh->indexData->indexCount;
    }


    // Allocate space for the vertices and indices
    vertices = new Ogre::Vector3[vertex_count];
    indices = new unsigned long[index_count];

    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        Ogre::SubMesh* submesh = mesh->getSubMesh(i);

        Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

        if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
        {
            if(submesh->useSharedVertices)
            {
                added_shared = true;
                shared_offset = current_offset;
            }

            const Ogre::VertexElement* posElem =
                vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

            Ogre::HardwareVertexBufferSharedPtr vbuf =
                vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

            unsigned char* vertex =
                static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

            // There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
            //  as second argument. So make it float, to avoid trouble when Ogre::Real will
            //  be comiled/typedefed as double:
            //      Ogre::Real* pReal;
            float* pReal;

            for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
            {
                posElem->baseVertexPointerToElement(vertex, &pReal);

                Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

                vertices[current_offset + j] = (orient * (pt * scale)) + position;
            }

            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }


        Ogre::IndexData* index_data = submesh->indexData;
        size_t numTris = index_data->indexCount / 3;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

        bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

        unsigned long*  pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
        unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);


        size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

        if ( use32bitindexes )
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
            }
        }
        else
        {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
                indices[index_offset++] = static_cast<unsigned long>(pShort[k]) +
                    static_cast<unsigned long>(offset);
            }
        }

        ibuf->unlock();
        current_offset = next_offset;
    }
}*/
