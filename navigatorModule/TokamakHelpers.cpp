#include "TokamakHelpers.h"

#ifdef TOKAMAK

#include "OgreHelpers.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
neTriangleMesh TokamakHelpers::convertMesh(const MeshPtr mesh,
                                            const Vector3& position,
                                            const Quaternion& orientation,
                                            const Vector3& scale)
{
    size_t vertexCount;
    size_t indexCount;
    Vector3* vertices;
    Vector2* texCoords;
    unsigned long *indices;

    // Get mesh datas
    OgreHelpers::getMeshInformation(mesh, vertexCount, vertices, texCoords, indexCount, indices,
                                    position,
                                    orientation,
                                    scale);
    neTriangleMesh triangleMesh;
    triangleMesh.vertexCount = (s32)vertexCount;
    triangleMesh.vertices = new neV3[triangleMesh.vertexCount];
    triangleMesh.triangleCount = (s32)indexCount/3;
    triangleMesh.triangles = new neTriangle[triangleMesh.triangleCount];
    for (int v=0; v<(int)triangleMesh.vertexCount; ++v)
        triangleMesh.vertices[v].Set(vertices[v].x, vertices[v].y, vertices[v].z);
    for (int t=0; t<(int)triangleMesh.triangleCount; ++t)
    {
        triangleMesh.triangles[t].indices[0] = indices[t*3 + 0];
        triangleMesh.triangles[t].indices[1] = indices[t*3 + 1];
        triangleMesh.triangles[t].indices[2] = indices[t*3 + 2];
        triangleMesh.triangles[t].materialID = 0;
        triangleMesh.triangles[t].flag = neTriangle::NE_TRI_TRIANGLE;
    }
    delete[] vertices;
    delete[] texCoords;
    delete[] indices;

    return triangleMesh;
}

//-------------------------------------------------------------------------------------

#endif
