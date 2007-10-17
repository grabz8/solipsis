/*
		COPYRIGHTS (c) ARTEFACTO 
		
			!! Licence GPL !!	
*/

#ifndef __MeshModifier_h__
#define __MeshModifier_h__

#include <Ogre.h>

using namespace Ogre;
using namespace std;


//-------------------------------------------------------------------------------------
class MeshModifier
{
public:
	MeshModifier() {};
	~MeshModifier() {};

	/// brief Check for an intersection (HitPos) between a line(LP1,LP2) and a triangle face (TP1,TP2,TP3)
	/// file MeshModifier.h
	static bool interLineTriangle( Vector3 TP1, Vector3 TP2, Vector3 TP3, Vector3 LP1, Vector3 LP2, Vector3 &HitPos )
	{
		Vector3 normal, intersectPos, vTest;

		// Find Triangle Normal
		normal = (TP2-TP1).crossProduct( TP3-TP1 );
		normal.normalise(); // not really needed

		// Find distance from LP1 and LP2 to the plane defined by the triangle
		float dist1 = (LP1-TP1).dotProduct( normal );
		float dist2 = (LP2-TP1).dotProduct( normal );
		if ( (dist1 * dist2) >= 0.0f) return false;  // line doesn't cross the triangle.
		if ( dist1 == dist2) return false;// line and plane are parallel

		// Find point on the line that intersects with the plane
		intersectPos = LP1 + (LP2-LP1) * ( -dist1 / (dist2-dist1) );

		// Find if the interesection point lies inside the triangle by testing it against all edges
		vTest = normal.crossProduct( TP2-TP1 );
		if ( vTest.dotProduct( intersectPos-TP1 ) < 0.0f ) return false;
		vTest = normal.crossProduct( TP3-TP2 );
		if ( vTest.dotProduct( intersectPos-TP2 ) < 0.0f ) return false;
		vTest = normal.crossProduct( TP1-TP3 );
		if ( vTest.dotProduct( intersectPos-TP1 ) < 0.0f ) return false;

		HitPos = intersectPos;
		return true;
	}

	/// brief Check for an intersection (HitPos) between a line(LP1,LP2) and a rectangle face (TP1,TP2,TP3,TP4)
	/// file MeshModifier.h
	static bool interLineRectangle( Vector3 TP1, Vector3 TP2, Vector3 TP3, Vector3 TP4, Vector3 LP1, Vector3 LP2, Vector3 &HitPos)
	{
		return ( 
			interLineTriangle( TP1, TP2, TP3, LP1, LP2, HitPos ) || 
			interLineTriangle( TP3, TP2, TP4, LP1, LP2, HitPos ));
	}

	/// brief ...
	/// file MeshModifier.h
	static int interPointPoint( float fDst1, float fDst2, Vector3 P1, Vector3 P2, Vector3 &Hit ) 
	{
		if ( (fDst1 * fDst2) >= 0.0f) return 0;
		if ( fDst1 == fDst2) return 0; 
		Hit = P1 + (P2-P1) * ( -fDst1/(fDst2-fDst1) );
		return 1;
	}

	/// brief ...
	/// file MeshModifier.h
	static int pointInBox( Vector3 Hit, Vector3 B1, Vector3 B2, const int Axis ) 
	{
		if ( Axis==1 && Hit.z > B1.z && Hit.z < B2.z && Hit.y > B1.y && Hit.y < B2.y) return 1;
		if ( Axis==2 && Hit.z > B1.z && Hit.z < B2.z && Hit.x > B1.x && Hit.x < B2.x) return 1;
		if ( Axis==3 && Hit.x > B1.x && Hit.x < B2.x && Hit.y > B1.y && Hit.y < B2.y) return 1;
		return 0;
	}

	/// brief returns true and intersection point in Hit if line (L1, L2) intersects with the box (B1, B2)
	/// file MeshModifier.h
	static int interLineBox( Vector3 B1, Vector3 B2, Vector3 L1, Vector3 L2, Vector3 &Hit )
	{
		if (L2.x < B1.x && L1.x < B1.x) return false;
		if (L2.x > B2.x && L1.x > B2.x) return false;
		if (L2.y < B1.y && L1.y < B1.y) return false;
		if (L2.y > B2.y && L1.y > B2.y) return false;
		if (L2.z < B1.z && L1.z < B1.z) return false;
		if (L2.z > B2.z && L1.z > B2.z) return false;
		if (L1.x > B1.x && L1.x < B2.x &&
			L1.y > B1.y && L1.y < B2.y &&
			L1.z > B1.z && L1.z < B2.z) 
		{
			Hit = L1; 
			return true;
		}

		if ( (interPointPoint( L1.x-B1.x, L2.x-B1.x, L1, L2, Hit) && pointInBox( Hit, B1, B2, 1 ))
			|| (interPointPoint( L1.y-B1.y, L2.y-B1.y, L1, L2, Hit) && pointInBox( Hit, B1, B2, 2 )) 
			|| (interPointPoint( L1.z-B1.z, L2.z-B1.z, L1, L2, Hit) && pointInBox( Hit, B1, B2, 3 )) 
			|| (interPointPoint( L1.x-B2.x, L2.x-B2.x, L1, L2, Hit) && pointInBox( Hit, B1, B2, 1 )) 
			|| (interPointPoint( L1.y-B2.y, L2.y-B2.y, L1, L2, Hit) && pointInBox( Hit, B1, B2, 2 )) 
			|| (interPointPoint( L1.z-B2.z, L2.z-B2.z, L1, L2, Hit) && pointInBox( Hit, B1, B2, 3 )))
		{
			return true;
		}

		return false;
	}

	/// brief Get some informations from a mesh (vertex data, nb vertex, nb index face, index faces)
	/// file MeshModifier.h
	static void getMeshInformation(
		Mesh* mesh, 
		size_t &vertex_count, Vector3* &vertices,
		size_t &index_count, unsigned* &indices,
		const Vector3 &position = Vector3::ZERO,
		const Quaternion &orient = Quaternion::IDENTITY, const Vector3 &scale = Vector3::UNIT_SCALE )
	{
		vertex_count = index_count = 0;

		bool added_shared = false;
		size_t current_offset = vertex_count;
		size_t shared_offset = vertex_count;
		size_t next_offset = vertex_count;
		size_t index_offset = index_count;
		size_t prev_vert = vertex_count;
		size_t prev_ind = index_count;

		// Calculate how many vertices and indices we're going to need
		for(int i = 0;i < mesh->getNumSubMeshes();i++)
		{
			SubMesh* submesh = mesh->getSubMesh(i);

			// We only need to add the shared vertices once
			if(submesh->useSharedVertices)
			{
				if(!added_shared)
				{
					VertexData* vertex_data = mesh->sharedVertexData;
					vertex_count += vertex_data->vertexCount;
					added_shared = true;
				}
			}
			else
			{
				VertexData* vertex_data = submesh->vertexData;
				vertex_count += vertex_data->vertexCount;
			}

			// Add the indices
			Ogre::IndexData* index_data = submesh->indexData;
			index_count += index_data->indexCount;
		}

		// Allocate space for the vertices and indices
		vertices = new Vector3[vertex_count];
		indices = new unsigned[index_count];

		added_shared = false;

		// Run through the submeshes again, adding the data into the arrays
		for(int i = 0;i < mesh->getNumSubMeshes();i++)
		{
			SubMesh* submesh = mesh->getSubMesh(i);

			Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;
			if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
			{
				if(submesh->useSharedVertices)
				{
					added_shared = true;
					shared_offset = current_offset;
				}

				const Ogre::VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);
				Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
				unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
				Ogre::Real* pReal;

				for(size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
				{
					posElem->baseVertexPointerToElement(vertex, &pReal);

					Vector3 pt;

					pt.x = (*pReal++);
					pt.y = (*pReal++);
					pt.z = (*pReal++);

					pt = (orient * (pt * scale)) + position;

					vertices[current_offset + j].x = pt.x;
					vertices[current_offset + j].y = pt.y;
					vertices[current_offset + j].z = pt.z;
				}
				vbuf->unlock();
				next_offset += vertex_data->vertexCount;
			}

			Ogre::IndexData* index_data = submesh->indexData;

			size_t numTris = index_data->indexCount / 3;
			unsigned short* pShort;
			unsigned int* pInt;
			Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;
			bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);
			if (use32bitindexes) pInt = static_cast<unsigned int*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
			else pShort = static_cast<unsigned short*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

			for(size_t k = 0; k < numTris; ++k)
			{
				size_t offset = (submesh->useSharedVertices)?shared_offset:current_offset;

				unsigned int vindex = use32bitindexes? *pInt++ : *pShort++;
				indices[index_offset + 0] = vindex + offset;
				vindex = use32bitindexes? *pInt++ : *pShort++;
				indices[index_offset + 1] = vindex + offset;
				vindex = use32bitindexes? *pInt++ : *pShort++;
				indices[index_offset + 2] = vindex + offset;

				index_offset += 3;
			}
			ibuf->unlock();
			current_offset = next_offset;
		}
	}

	// ...

	/// brief ...
	static void getCenter( Vector3 P1, Vector3 P2, Vector3 P3, Vector3 &Hit )
	{
		Hit = ( P1 + P2 + P3 ) / 3;
	}
};

#endif //__MeshModifier_h__