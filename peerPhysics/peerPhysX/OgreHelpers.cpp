#include "OgreHelpers.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
void OgreHelpers::getMeshInformation(const MeshPtr mesh,
                                     size_t &vertex_count,
                                     Vector3*& vertices,
                                     Vector2*& texCoords,
                                     size_t &index_count,
                                     unsigned long*& indices,
                                     const Vector3& position,
                                     const Quaternion& orientation,
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
                vertices[current_offset + j] = (orientation*(pt*scale)) + position;
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
