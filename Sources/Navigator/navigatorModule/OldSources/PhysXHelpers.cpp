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

#include "PhysXHelpers.h"

#ifdef PHYSX

#include <stdio.h>
#include "NxCooking.h"
#include "OgreHelpers.h"

UserStream::UserStream(const char* filename, bool load) : fp(NULL)
	{
	fp = fopen(filename, load ? "rb" : "wb");
	}

UserStream::~UserStream()
	{
	if(fp)	fclose(fp);
	}

// Loading API
NxU8 UserStream::readByte() const
	{
	NxU8 b;
	size_t r = fread(&b, sizeof(NxU8), 1, fp);
	NX_ASSERT(r);
	return b;
	}

NxU16 UserStream::readWord() const
	{
	NxU16 w;
	size_t r = fread(&w, sizeof(NxU16), 1, fp);
	NX_ASSERT(r);
	return w;
	}

NxU32 UserStream::readDword() const
	{
	NxU32 d;
	size_t r = fread(&d, sizeof(NxU32), 1, fp);
	NX_ASSERT(r);
	return d;
	}

float UserStream::readFloat() const
	{
	NxReal f;
	size_t r = fread(&f, sizeof(NxReal), 1, fp);
	NX_ASSERT(r);
	return f;
	}

double UserStream::readDouble() const
	{
	NxF64 f;
	size_t r = fread(&f, sizeof(NxF64), 1, fp);
	NX_ASSERT(r);
	return f;
	}

void UserStream::readBuffer(void* buffer, NxU32 size)	const
	{
	size_t w = fread(buffer, size, 1, fp);
	NX_ASSERT(w);
	}

// Saving API
NxStream& UserStream::storeByte(NxU8 b)
	{
	size_t w = fwrite(&b, sizeof(NxU8), 1, fp);
	NX_ASSERT(w);
	return *this;
	}

NxStream& UserStream::storeWord(NxU16 w)
	{
	size_t ww = fwrite(&w, sizeof(NxU16), 1, fp);
	NX_ASSERT(ww);
	return *this;
	}

NxStream& UserStream::storeDword(NxU32 d)
	{
	size_t w = fwrite(&d, sizeof(NxU32), 1, fp);
	NX_ASSERT(w);
	return *this;
	}

NxStream& UserStream::storeFloat(NxReal f)
	{
	size_t w = fwrite(&f, sizeof(NxReal), 1, fp);
	NX_ASSERT(w);
	return *this;
	}

NxStream& UserStream::storeDouble(NxF64 f)
	{
	size_t w = fwrite(&f, sizeof(NxF64), 1, fp);
	NX_ASSERT(w);
	return *this;
	}

NxStream& UserStream::storeBuffer(const void* buffer, NxU32 size)
	{
	size_t w = fwrite(buffer, size, 1, fp);
	NX_ASSERT(w);
	return *this;
	}




MemoryWriteBuffer::MemoryWriteBuffer() : currentSize(0), maxSize(0), data(NULL)
	{
	}

MemoryWriteBuffer::~MemoryWriteBuffer()
	{
	NX_DELETE_ARRAY(data);
	}

void MemoryWriteBuffer::clear()
	{
	currentSize = 0;
	}

NxStream& MemoryWriteBuffer::storeByte(NxU8 b)
	{
	storeBuffer(&b, sizeof(NxU8));
	return *this;
	}
NxStream& MemoryWriteBuffer::storeWord(NxU16 w)
	{
	storeBuffer(&w, sizeof(NxU16));
	return *this;
	}
NxStream& MemoryWriteBuffer::storeDword(NxU32 d)
	{
	storeBuffer(&d, sizeof(NxU32));
	return *this;
	}
NxStream& MemoryWriteBuffer::storeFloat(NxReal f)
	{
	storeBuffer(&f, sizeof(NxReal));
	return *this;
	}
NxStream& MemoryWriteBuffer::storeDouble(NxF64 f)
	{
	storeBuffer(&f, sizeof(NxF64));
	return *this;
	}
NxStream& MemoryWriteBuffer::storeBuffer(const void* buffer, NxU32 size)
	{
	NxU32 expectedSize = currentSize + size;
	if(expectedSize > maxSize)
		{
		maxSize = expectedSize + 4096;

		NxU8* newData = new NxU8[maxSize];
		NX_ASSERT(newData!=NULL);

		if(data)
			{
			memcpy(newData, data, currentSize);
			delete[] data;
			}
		data = newData;
		}
	memcpy(data+currentSize, buffer, size);
	currentSize += size;
	return *this;
	}


MemoryReadBuffer::MemoryReadBuffer(const NxU8* data) : buffer(data)
	{
	}

MemoryReadBuffer::~MemoryReadBuffer()
	{
	// We don't own the data => no delete
	}

NxU8 MemoryReadBuffer::readByte() const
	{
	NxU8 b;
	memcpy(&b, buffer, sizeof(NxU8));
	buffer += sizeof(NxU8);
	return b;
	}

NxU16 MemoryReadBuffer::readWord() const
	{
	NxU16 w;
	memcpy(&w, buffer, sizeof(NxU16));
	buffer += sizeof(NxU16);
	return w;
	}

NxU32 MemoryReadBuffer::readDword() const
	{
	NxU32 d;
	memcpy(&d, buffer, sizeof(NxU32));
	buffer += sizeof(NxU32);
	return d;
	}

float MemoryReadBuffer::readFloat() const
	{
	float f;
	memcpy(&f, buffer, sizeof(float));
	buffer += sizeof(float);
	return f;
	}

double MemoryReadBuffer::readDouble() const
	{
	double f;
	memcpy(&f, buffer, sizeof(double));
	buffer += sizeof(double);
	return f;
	}

void MemoryReadBuffer::readBuffer(void* dest, NxU32 size) const
	{
	memcpy(dest, buffer, size);
	buffer += size;
	}

using namespace Solipsis;

NxPhysicsSDK* PhysXHelpers::mPhysicsSDK = 0;

//-------------------------------------------------------------------------------------
bool PhysXHelpers::init()
{
    if (mPhysicsSDK != 0)
        return true;

    // Init the SDK
    NxPhysicsSDKDesc desc;
    NxSDKCreateError errorCode = NXCE_NO_ERROR;
    mPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, 0, 0, desc, &errorCode);
    if (mPhysicsSDK == 0)
    {
        throw Exception(Exception::ERR_INTERNAL_ERROR,
            "Unable to initialize the PhysX SDK ! errorCode=" + StringConverter::toString((int)errorCode),
            "OgrePeerManager::CreateSceneNode");
    }
    mPhysicsSDK->setParameter(NX_SKIN_WIDTH, 0.01f);
    mPhysicsSDK->setParameter(NX_DEFAULT_SLEEP_LIN_VEL_SQUARED, 0.15f*0.15f);

    return true;
}

//-------------------------------------------------------------------------------------
bool PhysXHelpers::shutdown()
{
    if (mPhysicsSDK == 0)
        return true;

    NxReleasePhysicsSDK(mPhysicsSDK);
    mPhysicsSDK = 0;

    return true;
}

//-------------------------------------------------------------------------------------
/*#include <fstream>*/
NxTriangleMesh* PhysXHelpers::cookMesh(const MeshPtr mesh,
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
    NxVec3* NxVerts = new NxVec3[vertexCount];
    NxU32* NxIndices = new NxU32[indexCount];
    for (int v=0; v<(int)vertexCount; ++v)
        NxVerts[v].set(vertices[v].x, vertices[v].y, vertices[v].z);
    for (int i=0; i<(int)indexCount; ++i)
        NxIndices[i] = indices[i];
    delete[] vertices;
    delete[] texCoords;
    delete[] indices;

    // Create the descriptor
	NxTriangleMeshDesc triangleMeshDesc;
	triangleMeshDesc.numVertices = (NxU32)vertexCount;
	triangleMeshDesc.numTriangles = (NxU32)indexCount/3;
	triangleMeshDesc.pointStrideBytes = sizeof(NxVec3);
	triangleMeshDesc.triangleStrideBytes = 3*sizeof(NxU32);
	triangleMeshDesc.points = NxVerts;
	triangleMeshDesc.triangles = NxIndices;
	triangleMeshDesc.flags = 0;

    // Cooking from memory
    NxInitCooking();
    MemoryWriteBuffer buf;
    NxCookTriangleMesh(triangleMeshDesc, buf);
/*std::ofstream ofs("terrain.dat", std::ios_base::binary);
ofs.write((char*)buf.data, buf.currentSize);
ofs.close();*/
    NxTriangleMeshShapeDesc triangleMeshShapeDesc;
    NxTriangleMesh* triangleMesh = mPhysicsSDK->createTriangleMesh(MemoryReadBuffer(buf.data));
    NxCloseCooking();

    return triangleMesh;
}

//-------------------------------------------------------------------------------------

#endif
