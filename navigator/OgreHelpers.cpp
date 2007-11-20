#include "OgreHelpers.h"

using namespace Solipsis;

void OgreHelpers::getMovableObjectsList(SceneNode* node, const String movableType, std::list<MovableObject*> &movableObjectsList)
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

//-------------------------------------------------------------------------------------
bool OgreHelpers::convertString2Real(const String& real, Real& r)
{
    Real result = 0;

    if (1 != sscanf(real.c_str(), "%f", &result))
        return false;
    r = result;

    return true;
}

//-------------------------------------------------------------------------------------
bool OgreHelpers::convertString2Vector3(const String& vector, Vector3& v)
{
    Vector3 result = Vector3::ZERO;
    size_t prev, comma;

    comma = vector.find_first_of("(", 0);
    if (comma == String::npos)
    {
        comma = -1;
        prev = 0;
    }
    else
    {
        comma += 1;
        prev = comma;
    }
    comma = vector.find_first_of(",", comma + 1);
    if (comma == String::npos) return false;
    if (1 != sscanf(vector.substr(prev, comma).c_str(), "%f", &result.x))
        return false;
    prev = comma + 1;
    comma = vector.find_first_of(",", comma + 1);
    if (comma == String::npos) return false;
    if (1 != sscanf(vector.substr(prev, comma).c_str(), "%f", &result.y))
        return false;
    prev = comma + 1;
    comma = vector.find_first_of(")", comma + 1);
    if (comma != String::npos)
        comma -= 1;
    if (1 != sscanf(vector.substr(prev, comma).c_str(), "%f", &result.z))
        return false;
    v = result;

    return true;
}

//-------------------------------------------------------------------------------------
void OgreHelpers::getMeshInformation(const MeshPtr mesh,
                                     size_t &vertex_count,
                                     Vector3*& vertices,
                                     Vector2*& texCoords,
                                     size_t &index_count,
                                     unsigned long*& indices,
                                     const Vector3& position,
                                     const Quaternion& orient,
                                     const Vector3& scale)
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
        SubMesh* submesh = mesh->getSubMesh(i);

        // We only need to add the shared vertices once
        if (submesh->useSharedVertices)
        {
            if (!added_shared)
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


    // Allocate space for the vertices, texture coordinates and indices
    vertices = new Vector3[vertex_count];
    texCoords = new Vector2[vertex_count];
    indices = new unsigned long[index_count];

    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        SubMesh* submesh = mesh->getSubMesh(i);

        VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

        if ((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared))
        {
            if (submesh->useSharedVertices)
            {
                added_shared = true;
                shared_offset = current_offset;
            }

            const VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(VES_POSITION);
            const VertexElement* texCoordsElem = vertex_data->vertexDeclaration->findElementBySemantic(VES_TEXTURE_COORDINATES);
            HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
            unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(HardwareBuffer::HBL_READ_ONLY));

            // There is _no_ baseVertexPointerToElement() which takes an Real or a double
            //  as second argument. So make it float, to avoid trouble when Real will
            //  be comiled/typedefed as double:
            //      Real* pReal;
            float* pReal;
            for (size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
            {
                posElem->baseVertexPointerToElement(vertex, &pReal);
                Vector3 pt(pReal[0], pReal[1], pReal[2]);
                vertices[current_offset + j] = (orient*(pt*scale)) + position;
                if (texCoordsElem != NULL)
                {
                    texCoordsElem->baseVertexPointerToElement(vertex, &pReal);
                    Vector2 tc(pReal[0], pReal[1]);
                    texCoords[current_offset + j] = tc;
                }
                else
                    texCoords[current_offset + j] = Vector2::ZERO;
            }

            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }


        IndexData* index_data = submesh->indexData;
        size_t numTris = index_data->indexCount/3;
        HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

        bool use32bitindexes = (ibuf->getType() == HardwareIndexBuffer::IT_32BIT);

        unsigned long*  pLong = static_cast<unsigned long*>(ibuf->lock(HardwareBuffer::HBL_READ_ONLY));
        unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);

        size_t offset = (submesh->useSharedVertices) ? shared_offset : current_offset;
        if ( use32bitindexes )
        {
            for (size_t k = 0; k < numTris*3; ++k)
                indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
        }
        else
        {
            for (size_t k = 0; k < numTris*3; ++k)
                indices[index_offset++] = static_cast<unsigned long>(pShort[k]) + static_cast<unsigned long>(offset);
        }

        ibuf->unlock();
        current_offset = next_offset;
    }
}

//-------------------------------------------------------------------------------------
bool OgreHelpers::getIntersection(const Ray& ray,
                                  const Vector3& a,
                                  const Vector3& b,
                                  const Vector3& c,
                                  Real& distance,
                                  Vector2& uv)
{
    // Compute two edges sharing a
    Vector3 edge1 = b - a;
    Vector3 edge2 = c - a;

    // Compute determinant - also used to calculate U parameter
    Vector3 pvec = ray.getDirection().crossProduct(edge2);

    // If determinant is near zero, ray lies in plane of triangle
    Real det = edge1.dotProduct(pvec);

    Vector3 tvec;
    if (det > 0)
        tvec = ray.getOrigin() - a;
    else
    {
        tvec = a - ray.getOrigin();
        det = -det;
    }

    if (det < 0.0001f)
        return false;

    // Compute U parameter and test bounds
    uv.x = tvec.dotProduct(pvec);
    if ((uv.x < 0.0f) || (uv.x > det))
        return false;

    // Prepare to test V parameter
    Vector3 qvec = tvec.crossProduct(edge1);

    // Compute V parameter and test bounds
    uv.y = ray.getDirection().dotProduct(qvec);
    if ((uv.y < 0.0f) || (uv.x + uv.y > det))
        return false;

    // Compute distance, scale parameters, ray intersects triangle
    distance = edge2.dotProduct(qvec);
    Real invDet = 1.0f/det;
    distance *= invDet;
    uv *= invDet;

    return true;
}

//-------------------------------------------------------------------------------------
bool OgreHelpers::isEntityHitByMouse(const Ray& ray, Entity* entity,
                                     Real& closestDistance,
                                     Vector2& closestUV,
                                     Vector2& closestTriUV0, Vector2& closestTriUV1, Vector2& closestTriUV2)
{
    // mesh data to retrieve
    size_t vertexCount;
    size_t indexCount;
    Vector3 *vertices;
    Vector2 *texCoords;
    unsigned long *indices;

    // get the mesh information
    OgreHelpers::getMeshInformation(entity->getMesh(), vertexCount, vertices, texCoords, indexCount, indices,
                                    entity->getParentNode()->getWorldPosition(),
                                    entity->getParentNode()->getWorldOrientation(),
                                    entity->getParentNode()->getScale());

    // test for hitting individual triangles on the mesh
    bool newClosestFound = false;
    for (int i = 0; i < static_cast<int>(indexCount); i += 3)
    {
        // check for a hit against this triangle
        Real distance;
        Vector2 uv;
        // if it was a hit check if its the closest
        if (OgreHelpers::getIntersection(ray, vertices[indices[i]], vertices[indices[i + 1]], vertices[indices[i + 2]], distance, uv))
        {
            if ((closestDistance < 0.0f) || (distance < closestDistance))
            {
                OGRE_LOG("v0=" + StringConverter::toString(vertices[indices[i]]) + ", tc=" + StringConverter::toString(texCoords[indices[i]]));
                OGRE_LOG("v1=" + StringConverter::toString(vertices[indices[i + 1]]) + ", tc=" + StringConverter::toString(texCoords[indices[i + 1]]));
                OGRE_LOG("v2=" + StringConverter::toString(vertices[indices[i + 2]]) + ", tc=" + StringConverter::toString(texCoords[indices[i + 2]]));
                // this is the closest so far, save it off
                closestDistance = distance;
                closestUV = uv;
                closestTriUV0 = texCoords[indices[i]];
                closestTriUV1 = texCoords[indices[i + 1]];
                closestTriUV2 = texCoords[indices[i + 2]];
                newClosestFound = true;
            }
        }
    }

    // free the verticies and indicies memory
    delete[] vertices;
    delete[] texCoords;
    delete[] indices;

    return newClosestFound;
}
