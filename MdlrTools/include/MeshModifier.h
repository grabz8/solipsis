/*
		COPYRIGHTS (c) ARTEFACTO 
		
			!! Licence GPL !!	
*/

#ifndef __MeshModifier_h__
#define __MeshModifier_h__

#include <Ogre.h>
#include "SolipsisErrorHandler.h"


using namespace Ogre;
using namespace std;

namespace Solipsis {

//-------------------------------------------------------------------------------------
class MeshModifier
{
public:
	MeshModifier() {};
	~MeshModifier() {};



	/// brief ...
	static bool inline check_same_clock_dir(Vector3 pt1, Vector3 pt2, Vector3 pt3, Vector3 norm)
	{  
		float testi, testj, testk;
		float dotprod;
		// normal of trinagle
		testi = (((pt2.y - pt1.y)*(pt3.z - pt1.z)) - ((pt3.y - pt1.y)*(pt2.z - pt1.z)));
		testj = (((pt2.z - pt1.z)*(pt3.x - pt1.x)) - ((pt3.z - pt1.z)*(pt2.x - pt1.x)));
		testk = (((pt2.x - pt1.x)*(pt3.y - pt1.y)) - ((pt3.x - pt1.x)*(pt2.y - pt1.y)));

		// Dot product with triangle normal
		dotprod = testi*norm.x + testj*norm.y + testk*norm.z;

		//answer
		if(dotprod < 0) return false; //DIFF_CLOCKNESS;
		else return true; //SAME_CLOCKNESS;
	}

	/// brief ...
	static int check_intersect_tri(Vector3 pt1, Vector3 pt2, Vector3 pt3, Vector3 linept, Vector3 vect, Vector3 &pt_int)
	{
		float V1x, V1y, V1z;
		float V2x, V2y, V2z;
		Vector3 norm;
		float dotprod;
		float t;

		// vector form triangle pt1 to pt2
		V1x = pt2.x - pt1.x;
		V1y = pt2.y - pt1.y;
		V1z = pt2.z - pt1.z;

		// vector form triangle pt2 to pt3
		V2x = pt3.x - pt2.x;
		V2y = pt3.y - pt2.y;
		V2z = pt3.z - pt2.z;

		// vector normal of triangle
		norm.x = V1y*V2z-V1z*V2y;
		norm.y = V1z*V2x-V1x*V2z;
		norm.z = V1x*V2y-V1y*V2x;

		// dot product of normal and line's vector if zero line is parallel to triangle
		dotprod = norm.x*vect.x + norm.y*vect.y + norm.z*vect.z;

//		if(dotprod < 0)
		if(dotprod)
		{
			//Find point of intersect to triangle plane.
			//find t to intersect point
			t = -(norm.x*(linept.x-pt1.x)+norm.y*(linept.y-pt1.y)+norm.z*(linept.z-pt1.z))/
				(norm.x*vect.x+norm.y*vect.y+norm.z*vect.z);

			// if ds is neg line started past triangle so can't hit triangle.
			if(t < 0) return false;

			pt_int.x = linept.x + vect.x*t;
			pt_int.y = linept.y + vect.y*t;
			pt_int.z = linept.z + vect.z*t;

			if(check_same_clock_dir(pt1, pt2, pt_int, norm))
				if(check_same_clock_dir(pt2, pt3, pt_int, norm))
					if(check_same_clock_dir(pt3, pt1, pt_int, norm))
						// answer in pt_int is insde triangle
						return true;
		}
		return false;
	}

	/// brief Determine whether or not the line segment p1,p2
	// Intersects the 3 vertex facet bounded by pa,pb,pc
	// Return true/false and the intersection point p
	//
	// The equation of the line is p = p1 + mu (p2 - p1)
	// The equation of the plane is a x + b y + c z + d = 0
	// n.x x + n.y y + n.z z + d = 0
	static bool interSegTri( Vector3 p1, Vector3 p2, Vector3 pa, Vector3 pb, Vector3 pc, Vector3 &p)
	{
static const int EPS = 0;
static const int RTOD = 0;

		double d;
		double a1,a2,a3;
		double total,denom,mu;
		Vector3 n,pa1,pa2,pa3;

		// Calculate the parameters for the plane
		n.x = (pb.y - pa.y)*(pc.z - pa.z) - (pb.z - pa.z)*(pc.y - pa.y);
		n.y = (pb.z - pa.z)*(pc.x - pa.x) - (pb.x - pa.x)*(pc.z - pa.z);
		n.z = (pb.x - pa.x)*(pc.y - pa.y) - (pb.y - pa.y)*(pc.x - pa.x);
//		Normalise(&n);
		n = n.normalisedCopy();
		d = - n.x * pa.x - n.y * pa.y - n.z * pa.z;

		// Calculate the position on the line that intersects the plane
		denom = n.x * (p2.x - p1.x) + n.y * (p2.y - p1.y) + n.z * (p2.z - p1.z);
		if( abs(denom) < EPS )   // Line and plane don't intersect
			return false;
		mu = - (d + n.x * p1.x + n.y * p1.y + n.z * p1.z) / denom;
		p.x = p1.x + mu * (p2.x - p1.x);
		p.y = p1.y + mu * (p2.y - p1.y);
		p.z = p1.z + mu * (p2.z - p1.z);
		if( mu < 0 || mu > 1 )   // Intersection not along line segment
			return false;

		// Determine whether or not the intersection point is bounded by pa,pb,pc
		pa1.x = pa.x - p.x;
		pa1.y = pa.y - p.y;
		pa1.z = pa.z - p.z;
//		Normalise(&pa1);
		pa1 = pa1.normalisedCopy();
		pa2.x = pb.x - p.x;
		pa2.y = pb.y - p.y;
		pa2.z = pb.z - p.z;
//		Normalise(&pa2);
		pa2 = pa2.normalisedCopy();
		pa3.x = pc.x - p.x;
		pa3.y = pc.y - p.y;
		pa3.z = pc.z - p.z;
//		Normalise(&pa3);
		pa3 = pa3.normalisedCopy();
		a1 = pa1.x*pa2.x + pa1.y*pa2.y + pa1.z*pa2.z;
		a2 = pa2.x*pa3.x + pa2.y*pa3.y + pa2.z*pa3.z;
		a3 = pa3.x*pa1.x + pa3.y*pa1.y + pa3.z*pa1.z;
		total = ( acos(a1) + acos(a2) + acos(a3) ) * RTOD;

		if ( abs(total - 360) > EPS )
			return false;

		return true;
	}

	/// brief Check for an intersection (HitPos) between a line(LP1,LP2) and a triangle face (TP1,TP2,TP3)
	/// file MeshModifier.h
	static bool interLineTri( Vector3 TP1, Vector3 TP2, Vector3 TP3, Vector3 LP1, Vector3 LP2, Vector3 &HitPos )
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
	static bool interLineRect( Vector3 TP1, Vector3 TP2, Vector3 TP3, Vector3 TP4, Vector3 LP1, Vector3 LP2, Vector3 &HitPos)
	{
		return ( 
			interLineTri( TP1, TP2, TP3, LP1, LP2, HitPos ) || 
			interLineTri( TP3, TP2, TP4, LP1, LP2, HitPos ));
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
		size_t &vertex_count, realvector* vertices,
		size_t &index_count, uintvector* indices,
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

		size_t vertexDecl;

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
					// ...
					vertexDecl = vertex_data->vertexDeclaration->getVertexSize(0);
				}
			}
			else
			{
				VertexData* vertex_data = submesh->vertexData;
				vertex_count += vertex_data->vertexCount;
				// ...
				vertexDecl = vertex_data->vertexDeclaration->getVertexSize(0);
			}

			// Add the indices
			Ogre::IndexData* index_data = submesh->indexData;
			index_count += index_data->indexCount;
		}

		// Allocate space for the vertices and indices
		indices->resize(index_count);

		added_shared = false;

		Real *tmpVertices;
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
				//unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
				size_t tt = vbuf->getNumVertices();

//vertices->resize(tt * vertexDecl/4);
				vertices->clear();
                tmpVertices = static_cast<Real*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
				for (int ind = 0 ; ind < vertex_data->vertexCount * vertexDecl/4 ; ind ++)
					vertices->push_back(tmpVertices[ind]);

				vbuf->unlock();
				next_offset += vertex_data->vertexCount;
				tmpVertices = NULL;
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
				//indices[index_offset + 0] = vindex + offset;
				//vindex = use32bitindexes? *pInt++ : *pShort++;
				//indices[index_offset + 1] = vindex + offset;
				//vindex = use32bitindexes? *pInt++ : *pShort++;
				//indices[index_offset + 2] = vindex + offset;
				(*indices)[index_offset + 0] = vindex + offset;
				vindex = use32bitindexes? *pInt++ : *pShort++;
				(*indices)[index_offset + 1] = vindex + offset;
				vindex = use32bitindexes? *pInt++ : *pShort++;
				(*indices)[index_offset + 2] = vindex + offset;

				index_offset += 3;
			}
			ibuf->unlock();
			current_offset = next_offset;
		}
	}

	/// brief ...
	static void getCenter( Vector3 P1, Vector3 P2, Vector3 P3, Vector3 &Hit )
	{
		Hit = ( P1 + P2 + P3 ) / 3;
	}

	/// brief ...
	static bool genCylinder( Object3D::Command command,
						realvector* pVertex, unsigned int &pVertexCount, 
						unsigned int pVertexDecl, 
						uintvector* pIndex, unsigned int &pIndexCount,
						unsigned int pSides, Real pRadius, Real pLength,
						Vector3 pSizeMax, Vector3 pSizeMin )
	{
		if( pSides < 3 ) pSides = 3;
		if( pRadius >= 1 ) pRadius = .95;
		if( pSides == 4 ) pRadius *= 1.35;		// a bit smaller than sqrt(2) of a square side
		int indVertex = 0;

		pVertexCount = 0;
		pIndexCount = 0;

		Real angleStep = Math::PI * 2 / pSides;
		
		Vector3 rootTrans = (pSizeMax + pSizeMin) / 2;
		pLength *= 1.02;

		// A. Build the vertexs
		static Vector3 pt1, pt2, pt3, pt4, ptA, ptB;
		vector< Vector3 > points;
		{
			pt1 = Vector3( rootTrans + pSizeMax * Vector3( 0, 0, pLength ) ) - rootTrans;
			pt2 = Vector3( rootTrans + pSizeMin * Vector3( 0, 0, pLength ) ) - rootTrans;
			pt3	= Vector3( rootTrans + pSizeMax	* Vector3( -pRadius, 0, pLength ) ) - rootTrans;
			pt4 = Vector3( rootTrans + pSizeMin * Vector3( +pRadius, 0, pLength ) ) - rootTrans;

			pLength = (1-pLength/1.02) / 2 * (pSizeMax.z - pSizeMin.z);
			pt1.z += pLength;
			pt2.z += pLength;
			pt3.z += pLength;
			pt4.z += pLength;

			VectorModifier::rotateZ( pt3, Math::PI / pSides );
			VectorModifier::rotateZ( pt4, Math::PI / pSides );

			pt1 += rootTrans;	points.push_back( pt1 );
			pt2 += rootTrans;	points.push_back( pt2 );
			pt3 += rootTrans;	points.push_back( pt3 );
			pt4 += rootTrans;	points.push_back( pt4 );

			for( unsigned int i=1; i<pSides; i++ )
			{
				ptA = pt3 - rootTrans;  VectorModifier::rotateZ( ptA, angleStep * i );
				ptB = pt4 - rootTrans;  VectorModifier::rotateZ( ptB, angleStep * i );

				ptA += rootTrans; points.push_back( ptA );
				ptB += rootTrans; points.push_back( ptB );
			}
		}

		// A'. Turn the points of 90° on X
		for( vector<Vector3>::iterator pt = points.begin(); pt != points.end(); pt++ )
		{
			(*pt) -= rootTrans;
			VectorModifier::rotateX( (*pt), -Math::PI / 2 ); 
			(*pt) += rootTrans;
		}

		// B. Create vertex data
		{
			pVertex->resize(points.size() * pVertexDecl);
			int ind=0;
			for( vector<Vector3>::iterator pt = points.begin(); pt != points.end(); pt++,indVertex+=pVertexDecl)
			{
				(*pVertex)[indVertex] = (*pt).x;	(*pVertex)[indVertex+1] = (*pt).y;		(*pVertex)[indVertex+2] = (*pt).z;	// vertex position
				//pVertex += 3;																// normal
				(*pVertex)[indVertex+6] = 0;												// color
				(*pVertex)[indVertex+7] = 0;		(*pVertex)[indVertex+8] = 1;			// texture coodrinates
			}
			
			//pVertex -= ( points.size() * pVertexDecl );
			indVertex -= ( points.size() * pVertexDecl );
			pVertexCount = unsigned int(points.size());
		}

		// C. Create index data
		{
			pIndexCount = size_t( 2 * points.size() - 4 ) * 3;
			pIndex->resize(pIndexCount);
			unsigned int id = 2;

			// the faces
            unsigned int ifaces;
			for( ifaces = 0; ifaces < points.size()-2; ifaces+=2 )
			{
				(*pIndex)[ifaces*3] = id++;		(*pIndex)[ifaces*3+1] = id+1;	(*pIndex)[ifaces*3+2] = id;
				(*pIndex)[ifaces*3+3] = id++;	(*pIndex)[ifaces*3+4] = id++;	(*pIndex)[ifaces*3+5] = id--;
			}
			ifaces -= 2;
			(*pIndex)[ifaces*3+1] = 2;		(*pIndex)[ifaces*3+4] = 2;		(*pIndex)[ifaces*3+5] = 3;

			// the sides
			id = 2;
            size_t i;
			for( i = points.size()-2; i < (2*points.size()-4); i+=2 )
			{
				(*pIndex)[i*3] = 0;			(*pIndex)[i*3+1] = id+2;	(*pIndex)[i*3+2] = id;
				(*pIndex)[i*3+3] = 1;		(*pIndex)[i*3+4] = id+1;	(*pIndex)[i*3+5] = id+3;
				id+=2;
			}
			i -= 2;
			(*pIndex)[i*3+1] = 2;		(*pIndex)[i*3+5] = 3;
		}

		// D. Compute the normals
		if(0)
		for(unsigned int n = 0; n < pIndexCount/3; n++)
		{
			static unsigned int id1, id2, id3, max;
			static Vector3 v1, v2, v3, normal;

			// get the normal from the face witch include the current vertex
			id1 = (*pIndex)[3*n];		v1 = points[ id1 ];		id1 *= pVertexDecl;
			id2 = (*pIndex)[3*n+1];		v2 = points[ id2 ];		id2 *= pVertexDecl;
			id3 = (*pIndex)[3*n+2];		v3 = points[ id3 ];		id3 *= pVertexDecl;
			normal = VectorModifier::getNormal( v1, v2, v3 );

			// normalize
			normal.normalise();
			
			// apply the normal face to the 3 vertex of it
			(*pVertex)[ id1 + 3 ] = (*pVertex)[ id2 + 3 ] = (*pVertex)[ id3 + 3 ] = normal.x;
			(*pVertex)[ id1 + 4 ] = (*pVertex)[ id2 + 4 ] = (*pVertex)[ id3 + 4 ] = normal.y;
			(*pVertex)[ id1 + 5 ] = (*pVertex)[ id2 + 5 ] = (*pVertex)[ id3 + 5 ] = normal.z;
		}

		return true;
	}
	
	/// brief ...
	static bool genCylinderCut( Object3D::Command command,
						realvector* pVertex, unsigned int &pVertexCount, 
						unsigned int pVertexDecl, 
						uintvector* pIndex, unsigned int &pIndexCount,
						Real pAngleBegin, Real pAngleEnd,
						Vector3 pSizeMax, Vector3 pSizeMin )
	{
		if( 0 > pAngleBegin || pAngleBegin > 1) return false;
		if( (pAngleBegin == 0) && (pAngleEnd == 1) ) return false;
		if( pAngleBegin > pAngleEnd ) return false;	
		int indVertex = 0;

		pVertexCount = 0;
		pIndexCount = 0;

		Real angleBegin = 2 * Math::PI * pAngleBegin;
		Real angleEnd = 2 * Math::PI * (pAngleEnd - 1);

		Vector3 rootTrans = (pSizeMax + pSizeMin) / 2;

		// A. Update the 2 cut planes ( position & orientation )
		static Vector3 pt1, pt2, pt3, pt4, pt5, pt6;
		vector< Vector3 > points;
		{
			// create 6 points ( 2 for each cut planes plus 2 commun )
			pt1 = Vector3( rootTrans + pSizeMax * Vector3( 0, 0, 1.05 ) ) - rootTrans;
			pt2 = Vector3( rootTrans + pSizeMin * Vector3( 0, 0, 1.05 ) ) - rootTrans;
			pt3	= Vector3( rootTrans + pSizeMax	* Vector3( -1.55, 0, 1.05 ) ) - rootTrans;
			pt4 = Vector3( rootTrans + pSizeMin * Vector3( +1.55, 0, 1.05 ) ) - rootTrans;
			pt5 = pt3;
			pt6 = pt4;

			VectorModifier::rotateZ( pt3, angleBegin );
			VectorModifier::rotateZ( pt4, angleBegin );
			VectorModifier::rotateZ( pt5, angleEnd );
			VectorModifier::rotateZ( pt6, angleEnd );

			pt1 += rootTrans;	points.push_back( pt1 );
			pt2 += rootTrans;	points.push_back( pt2 );
			pt3 += rootTrans;	points.push_back( pt3 );
			pt4 += rootTrans;	points.push_back( pt4 );
			pt5 += rootTrans;	//points.push_back( pt5 );
			pt6 += rootTrans;	//points.push_back( pt6 );

			// create the new points between the 2 cut planes
			unsigned int step = 1 + abs( pAngleBegin + 1 - pAngleEnd ) * 8;
			if( step > 1 )
			{
				Real angleStep = -abs( angleBegin - angleEnd ) / step;
				Vector3 ptA, ptB;
				for( unsigned int i=1; i<step; i++ )
				{
					ptA = pt3 - rootTrans;  VectorModifier::rotateZ( ptA, angleStep * i );
					ptB = pt4 - rootTrans;  VectorModifier::rotateZ( ptB, angleStep * i );

					ptA += rootTrans; points.push_back( ptA );
					ptB += rootTrans; points.push_back( ptB );
				}
			}

			points.push_back( pt5 );
			points.push_back( pt6 );
		}

		// A'. Turn the points of 90° on X in case of command is DIMPLE or PATH_CUT
		if( command == Object3D::PATH_CUT_BEGIN || command == Object3D::PATH_CUT_END )
			for( vector<Vector3>::iterator pt = points.begin(); pt != points.end(); pt++ )
			{
				(*pt) -= rootTrans;
				VectorModifier::rotateX( (*pt), -Math::PI / 2 ); 
				(*pt) += rootTrans;
			}

		// B. Create vertex data
		{
			int ind = 0;
			pVertex->resize(points.size() * pVertexDecl);
			for( vector<Vector3>::iterator pt = points.begin(); pt != points.end(); pt++,indVertex+=pVertexDecl )
			{
				(*pVertex)[indVertex] = (*pt).x;	(*pVertex)[indVertex+1] = (*pt).y;		(*pVertex)[indVertex+2] = (*pt).z;	// vertex position
				//pVertex += 3;																// normal
				(*pVertex)[indVertex+6] = 0;												// color
				(*pVertex)[indVertex+7] = 0;		(*pVertex)[indVertex+8] = 1;			// texture coodrinates
			}

			//pVertex -= ( points.size() * pVertexDecl );
			indVertex -= ( points.size() * pVertexDecl );
			pVertexCount = unsigned int(points.size());
		}

		// C. Create index data
		{
			pIndexCount = size_t( 2 * points.size() - 4 ) * 3;

			pIndex->resize(pIndexCount);
			unsigned int id = 0;

			// the first (begin) cut plane
			(*pIndex)[0] = id++;		(*pIndex)[1] = id++;		(*pIndex)[2] = id+1;
			(*pIndex)[3] = id+1;		(*pIndex)[4] = id--;		(*pIndex)[5] = id++ -1;


			// the other faces
            unsigned int i;
			for( i = 2; i < points.size(); i+=2 )
			{
				(*pIndex)[i*3] = id++;		(*pIndex)[i*3+1] = id++;	(*pIndex)[i*3+2] = id++ +1;
				(*pIndex)[i*3+3] = id--;	(*pIndex)[i*3+4] = id--;	(*pIndex)[i*3+5] = id++ -1;
			}

			// the second (end) cut plane
			i = points.size()-2;
			(*pIndex)[i*3+2] = 1;		(*pIndex)[i*3+3] = 1;		(*pIndex)[i*3+4] = 0;

			// the two sides
			id = 2;
			for( size_t i = points.size(); i < (2*points.size()-4); i+=2 )
			{
				(*pIndex)[i*3] = 0;			(*pIndex)[i*3+1] = id;		(*pIndex)[i*3+2] = id+2;
				(*pIndex)[i*3+3] = 1;		(*pIndex)[i*3+4] = id+3;	(*pIndex)[i*3+5] = id+1;
				id+=2;
			}
		}

		// D. Compute the normals
		if(0)
		for(unsigned int n = 0; n < pIndexCount/3; n++)
		{
			static unsigned int id1, id2, id3, max;
			static Vector3 v1, v2, v3, normal;

			// get the normal from the face witch include the current vertex
			id1 = (*pIndex)[3*n];		v1 = points[ id1 ];		id1 *= pVertexDecl;
			id2 = (*pIndex)[3*n+1];		v2 = points[ id2 ];		id2 *= pVertexDecl;
			id3 = (*pIndex)[3*n+2];		v3 = points[ id3 ];		id3 *= pVertexDecl;
			normal = VectorModifier::getNormal( v1, v2, v3 );

			// normalize
			normal.normalise();
			
			// apply the normal face to the 3 vertex of it
			(*pVertex)[ id1 + 3 ] = (*pVertex)[ id2 + 3 ] = (*pVertex)[ id3 + 3 ] = normal.x;
			(*pVertex)[ id1 + 4 ] = (*pVertex)[ id2 + 4 ] = (*pVertex)[ id3 + 4 ] = normal.y;
			(*pVertex)[ id1 + 5 ] = (*pVertex)[ id2 + 5 ] = (*pVertex)[ id3 + 5 ] = normal.z;
		}

		return true;
	}

	/// brief ...
	static bool genCylinderSkew( Object3D::Command command,
						realvector* pVertex, unsigned int &pVertexCount, 
						unsigned int pVertexDecl, 
						uintvector* pIndex, unsigned int &pIndexCount,
						unsigned int pSides, Real pRadius, Real pLength,
						Real pSkew, Real pRadiusDelta, unsigned int pRevolutions, Object3D::Type pType,
						Vector3 pSizeMax, Vector3 pSizeMin )
	{
		pVertexCount = 0;
		pIndexCount = 0;
		int indVertex = 0;

		Vector3 rootTrans = (pSizeMax + pSizeMin) / 2;
		vector<Vector3>::iterator it;

		// scale between the begin and the end cap
		if( pSkew == 0 ) pSkew = .00001;
		Real scale = ( pSkew > 0 && pSkew < 1 ) ? ( 1 / pSkew ) - 1 : 0.01;
		
		// height between the base of each cap
		Real heigth = ( pSizeMax.y - pSizeMin.y ) * (1-pSkew/2);

		// A. Create the profil
		static vector< Vector3 > profil, points;
		{
			Vector3 temp;
			Vector3 max = pSizeMax;		max.z = 0;				//max.x *= pRadius;
			Vector3 min = pSizeMin;		min.z = min.x = 0;		min.y = max.y - ((max.y - min.y) * pSkew / pRevolutions);
			unsigned int level = 5;		// number of levels between the top & bottom faces
			int indVertex = 0;

			switch( pType )
			{
			case Object3D::CYLINDER:
				{
					temp = max;
					profil.push_back( temp );
					Real step = (max.y - min.y) / level;
                    unsigned int i;
					for(i=0; i<level; i++)
					{
						temp.y -= step;
						profil.push_back( temp );
					}
					temp.x = 0;
					profil.push_back( temp );
					for(i=0; i<level; i++)
					{
						temp.y += step;
						profil.push_back( temp );
					}
					break;
				}
			case Object3D::TUBE:
				{
					temp = max;
					profil.push_back( temp );
					Real step = (max.y - min.y) / level;
                    unsigned int i;
					for(i=0; i<level; i++)
					{
						temp.y -= step;
						profil.push_back( temp );
					}
					temp.x *= .5;								//temp.x *= pRadiusDelta;	
					profil.push_back( temp );
					for(i=0; i<level; i++)
					{
						temp.y += step;
						profil.push_back( temp );
					}
					break;
				}
			case Object3D::TORUS:
				{
					max.x /= 2;
					profil.push_back( max );	max.x *= 2;		max.y /= 2;
					profil.push_back( max );	min.x = max.x / 2;
					profil.push_back( min );	max.x = 0;
					profil.push_back( max );
					break;
				}
			}
		}

		// create the vertex data
		pVertex->resize(profil.size() * (pSides*pRevolutions + 3) * pVertexDecl);
		// create the index data
		pIndex->resize(( (profil.size() * 2 * pSides*pRevolutions) + (profil.size() - 2) * 2 ) * 3 );

		// B. Build the 2 cap planes
		{
			// vertex : first cap
			for(it=profil.begin(); it!=profil.end(); it++,indVertex+=9)
			{
				(*pVertex)[indVertex] = (*it).x;	(*pVertex)[indVertex+1] = (*it).y;	(*pVertex)[indVertex+2] = (*it).z;	// position
				(*pVertex)[indVertex+3] = 0;		(*pVertex)[indVertex+4] = 0;		(*pVertex)[indVertex+5] = 1;		// normal
				(*pVertex)[indVertex+6] = 0;																				// colour
				(*pVertex)[indVertex+7] = 0;		(*pVertex)[indVertex+8] = 1;											// tex. coord
			}

			Vector3 v;
			// vertex : second cap
			for(it=profil.begin(); it!=profil.end(); it++,indVertex+=9)
			{
				v = (*it);
				v.y -= ( pSizeMax.y - pSizeMin.y );
				v.y *= scale;
				v.y += ( pSizeMax.y - pSizeMin.y ) * (1-pSkew) / pRevolutions;

				(*pVertex)[indVertex] = v.x;		(*pVertex)[indVertex+1] = v.y;		(*pVertex)[indVertex+2] = v.z;	// position
				(*pVertex)[indVertex+3] = 0;		(*pVertex)[indVertex+4] = 0;		(*pVertex)[indVertex+5] = -1;	// normal
				(*pVertex)[indVertex+6] = 0;																			// colour
				(*pVertex)[indVertex+7] = 0;		(*pVertex)[indVertex+8] = 1;										// tex. coord
			}

			// index : first cap
			unsigned int first = pVertexCount;
			unsigned int next = pVertexCount + 1;
            unsigned int i;
			for(i=2; i<profil.size(); i++)
			{
				(*pIndex)[pIndexCount++] = first;
				(*pIndex)[pIndexCount++] = pVertexCount + i;
				(*pIndex)[pIndexCount++] = next;
				next = pVertexCount + i;
			}
			pVertexCount += profil.size();

			// index : second cap
			first = pVertexCount;
			next = pVertexCount + 1;
			for(i=2; i<profil.size(); i++)
			{
				(*pIndex)[pIndexCount++] = first;
				(*pIndex)[pIndexCount++] = next;
				(*pIndex)[pIndexCount++] = pVertexCount + i;
				next = pVertexCount + i;
			}
			pVertexCount += profil.size();
		}

		//if( pSkew == 0 ) pSkew = 1;

		// C. Build the mesh (vertex & faces)
		{
			// first segment
			// vertex
			for(it=profil.begin(); it!=profil.end(); it++)
			{
				(*pVertex)[indVertex++] = (*it).x;	(*pVertex)[indVertex++] = (*it).y;	(*pVertex)[indVertex++] = (*it).z;	// position
				indVertex += 3;																								// normal
				(*pVertex)[indVertex++] = 0;																				// colour
				(*pVertex)[indVertex++] = 0;		(*pVertex)[indVertex++] = 1;											// tex. coord

			}
			// other segments
			unsigned int id = pVertexCount;
			Real angleStep = Math::TWO_PI / pSides;
			for(unsigned int s=0; s<(pSides * pRevolutions); s++)
			{
				for(it=profil.begin(); it!=profil.end(); it++)
				{
					// vertex
					VectorModifier::rotateY( (*it), angleStep );// / pRevolutions);
					// TODO : ajouter ici le code permettant de modifier la largeur en fonction de 'pRadiusDelta'

					Vector3 v = (*it);
					Real ratio = Real(s+1) / Real(pSides * pRevolutions);

					v.y -= heigth;
					v.y *= ratio * (scale - 1) + 1;
					v.y += heigth;
					v.y -= ( pSizeMax.y - pSizeMin.y ) * ratio * ( 0.5 + (1-pSkew) * (pRevolutions-1) / pRevolutions );

					(*pVertex)[indVertex++] = v.x;	(*pVertex)[indVertex++] = v.y;		(*pVertex)[indVertex++] = v.z;	// position
					indVertex += 3;																						// normal
					(*pVertex)[indVertex++] = 0;																		// colour
					(*pVertex)[indVertex++] = 0;	(*pVertex)[indVertex++] = 1;										// tex. coord


					// index
					(*pIndex)[pIndexCount++] = id++;
					(*pIndex)[pIndexCount++] = id--;
					(*pIndex)[pIndexCount++] = id + profil.size();

					(*pIndex)[pIndexCount++] = id++ + profil.size();
					(*pIndex)[pIndexCount++] = id;
					(*pIndex)[pIndexCount++] = id + profil.size();
				}

				(*pIndex)[pIndexCount-5] = pVertexCount;
				(*pIndex)[pIndexCount-2] = pVertexCount;
				(*pIndex)[pIndexCount-1] = pVertexCount + profil.size();
				pVertexCount += profil.size();
			}

			pVertexCount += profil.size();

			// normals
		}
		indVertex = 0;

/*		// D. Compute the normals
		// TODO : ne pas calculer les normales des 2 faces CAP !!!
		for(unsigned int n = 0; n < pIndexCount/3; n++)
		{
			static unsigned int id1, id2, id3, max;
			static Vector3 v1, v2, v3, normal;

			// get the normal from the face witch include the current vertex
			id1 = (*pIndex)[3*n];	v1 = points[ id1 ];		id1 *= pVertexDecl;
			id2 = (*pIndex)[3*n+1];	v2 = points[ id2 ];		id2 *= pVertexDecl;
			id3 = (*pIndex)[3*n+2];	v3 = points[ id3 ];		id3 *= pVertexDecl;
			normal = VectorModifier::getNormal( v1, v2, v3 );

			// normalize
			normal.normalise();
			
			// apply the normal face to the 3 vertex of it
			(*pVertex)[ id1 + 3 ] = (*pVertex)[ id2 + 3 ] = (*pVertex)[ id3 + 3 ] = normal.x;
			(*pVertex)[ id1 + 4 ] = (*pVertex)[ id2 + 4 ] = (*pVertex)[ id3 + 4 ] = normal.y;
			(*pVertex)[ id1 + 5 ] = (*pVertex)[ id2 + 5 ] = (*pVertex)[ id3 + 5 ] = normal.z;
		}
*/
		profil.clear();
		points.clear();

		return true;
	}
};

} //namespace

#endif //__MeshModifier_h__
