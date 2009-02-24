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

#include "TokamakHelpers.h"

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
