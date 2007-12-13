//

#include "SolidBoolOp.h"


// *** APPLY BOOLEAN OPERATION ***
static SOLID* global_othersolid;

// The algorithm requires to remember coplanar face pairs until partitioning
// and inside/outside mark propagation has been done
typedef std::pair<BREP_FACE*, BREP_FACE*> COPLANAR_FACES;
typedef std::vector<COPLANAR_FACES> COPLANAR_FACES_LIST;
COPLANAR_FACES_LIST coplanar_faces;

// Given a pair of coplanar faces, IterateMatchingContours() calls a given
// function for each pair of matching contours. Two contours match after
// partitioning as soon as they have one wing connecting the same vertices
// and their enclosed region is on the same side.
// See M. Segal, IEEE CG&A Jan '88 p 61-62
static void (*matching_contour_func)(BREP_CONTOUR*, BREP_CONTOUR*);
static BREP_CONTOUR* matching_contourA;
static BREP_FACE* matching_contour_faceB;

// *** WRITE NEW SOLID ***
std::vector< std::vector<unsigned int> > faces;
std::vector<unsigned int> currentface;

typedef std::map<BREP_VERTEX*, unsigned long> PointMap;
PointMap pointMap;

unsigned long vertexid;







// Creates a vertex
void SBO::AddVertex(VECTOR vector)
{
	VERTEX* v = new VERTEX();
	v->point = vector;

	v->DataCloseCallback();

	currentSolid->vertices->AddWithDuplicates((BREP_VERTEX*)v);
	points.push_back(v);
}


// Reads the objects and collects them into one SOLID
SOLID* SBO::dataLoad(float* vertex, 
					 unsigned int numVertex, 
					 unsigned int vertexDeclSize, 
					 unsigned int* index, 
					 unsigned int numIndex )
{
	currentSolid = new SOLID();
	currentSolid->filename = "";

	// New surface
	newObject = true;

	// Extract point and polygon numbers
	unsigned int npoints = numVertex;
	unsigned int npols = numIndex;

	// Load objects ...
	while(npoints && npols)
	{
		// Read points
		points.clear();
		for(unsigned int n = 0; n < npoints; n++)
		{
			// Extract a record type ...
			VECTOR v;
			v.x = *vertex++;
			v.y = *vertex++;
			v.z = *vertex++;
			AddVertex(v);

			vertex += vertexDeclSize - 3;
		}
		vertex -= vertexDeclSize * npoints;

		// Faces
		unsigned int* indexStart = index;
		for(unsigned int n = 0; n < npols; n++)
		{
			// New face
			if(newObject)
			{
				currentShell = new SHELL(currentSolid);
				newObject = false;
			}

			currentFace = new FACE(currentShell);

			currentContour = new CONTOUR(currentFace);

			// First vertex
			unsigned long idx;
			idx = *index++;

			unsigned int id = 2;

			VERTEX* first = points[idx];
			VERTEX* cur = first;

			// Decode edge vertices into points from point map
			do
			{
				idx = *index++;
				id--;

				VERTEX* prev = cur;
				cur = points[idx];
				currentContour->CreateWing((BREP_VERTEX*)prev, (BREP_VERTEX*)cur);
			}
			//while( indexStart + numIndex >= index );
			while( id );

			// Close the loop
			currentContour->CreateWing((BREP_VERTEX*)cur, (BREP_VERTEX*)first);
		}
		index = indexStart;

		npoints = npols = 0;
	}

	currentSolid->DataCloseCallback();

	return currentSolid;
}




// Apply the boolean operation between two solids A & B
void SBO::applyBoolOp(Operation op, SOLID* solidA, SOLID* solidB)
{
	solidA->IterateContours(unset_mark);
	solidB->IterateContours(unset_mark);

	if(!DisjunctBounds(((SOLID*)(solidA))->bounds, ((SOLID*)(solidB))->bounds))
	{
		BREP_SHELL_RING::iterator shellA;
		for(shellA = solidA->shells.begin(); shellA != solidA->shells.end(); shellA++)
		{
			BREP_SHELL_RING::iterator shellB;
			for(shellB = solidB->shells.begin(); shellB != solidB->shells.end(); shellB++)
			{
				SHELL* shA = (SHELL*)(*shellA);
				SHELL* shB = (SHELL*)(*shellB);
				if(!DisjunctBounds(shA->bounds, shB->bounds))
					partition_shells(*shellA, *shellB);
			}
		}
	}
	else
	{
		solidA->IterateContours(mark_with_outmask);
		solidB->IterateContours(mark_with_outmask);
	}

	csg_propagate_mark(solidA);
	csg_mark_unmarked(solidA, solidB);
	csg_propagate_mark(solidB);
	csg_mark_unmarked(solidB, solidA);

	COPLANAR_FACES_LIST::iterator cfaces;
	switch(op)
	{
	case OP_UNION:
		solidA->IterateContours(mark_out);
		solidB->IterateContours(mark_out);

		for(cfaces = coplanar_faces.begin(); cfaces != coplanar_faces.end(); cfaces++)
			IterateMatchingContours((*cfaces).first, (*cfaces).second, keep_one_same);
		break;

	case OP_INTERSECTION:
		solidA->IterateContours(mark_in);
		solidB->IterateContours(mark_in);

		for(cfaces = coplanar_faces.begin(); cfaces != coplanar_faces.end(); cfaces++)
			IterateMatchingContours((*cfaces).first, (*cfaces).second, keep_one_same);
		break;

	case OP_DIFFERENCE:
		solidA->IterateContours(mark_out);
		solidB->IterateContours(mark_in_reverse);

		for(cfaces = coplanar_faces.begin(); cfaces != coplanar_faces.end(); cfaces++)
			IterateMatchingContours((*cfaces).first, (*cfaces).second, keep_one_opposite);
		break;
	}
}

// Read vertex and index datas from the modified solid
void SBO::OutputSolid(BREP_SOLID* solid,
					  float* &vertex, unsigned int &vertexCount, unsigned int vertexDecl,
					  unsigned int* &index, unsigned int &indexCount )
{
	pointMap.clear();
	vertexid = 0;
	faces.clear();
	currentface.clear();
	indexCount = 0;

	// Get solid information
	BREP_SHELL_RING::iterator shell;
	for(shell = solid->shells.begin(); shell != solid->shells.end(); shell++)
		OutputShell(*shell);

	// Output solid information
	for(unsigned int n = 0; n < faces.size(); n++)
		indexCount += ( faces[n].size() - 2 ) * 3;
	index = new unsigned int[ indexCount ];
	vertexCount = pointMap.size();
	vertex = new float[ vertexCount * vertexDecl ];

	// Output solid with 0-indexed points
	std::vector<VECTOR> vectors;
	vectors.resize(vertexid);
	for(PointMap::iterator mp = pointMap.begin(); mp != pointMap.end(); mp++)
		vectors[mp->second] = ((VERTEX*)(mp->first))->point;

	// vertex data
	for(unsigned long n = 0; n < vertexid; n++)
	{
		VECTOR v = vectors[n];
		*vertex++ = v.x;		*vertex++ = v.y;		*vertex++ = v.z;	// position
		*vertex++ = 0;			*vertex++ = 0;			*vertex++ = 0;		// normal
		*vertex++ = 0;														// colour
		*vertex++ = 0;			*vertex++ = 1;								// tex. coord
	}
	vertex -= vertexCount * vertexDecl;

	// index data
	unsigned int id = 0;
    unsigned int n;
	for(n = 0; n < faces.size(); n++)
	{
		index[id++] = faces[n][0];
		for(unsigned int f = 1; f < faces[n].size(); f++)
		{
			if(f>2)
			{
				index[id++] = faces[n][0];
				index[id++] = faces[n][f-1];
			}
			index[id++] = faces[n][f];
		}
	}
	
	// normal data
	for(n = 0; n < indexCount/3; n++)
	{
		static unsigned int id1, id2, id3, max;
		static VECTOR v1, v2, v3, normal;

		// get the normal from the face witch include the current vertex
		id1 = index[3*n];		v1 = vectors[ id1 ];	id1 *= vertexDecl;
		id2 = index[3*n+1];		v2 = vectors[ id2 ];	id2 *= vertexDecl;
		id3 = index[3*n+2];		v3 = vectors[ id3 ];	id3 *= vertexDecl;
		normal = GetNormal( v1, v2, v3 );

		// normalize
		max = abs(normal.x) > abs(normal.y) ? normal.x : normal.y;
		max = abs(normal.z) > max ? normal.z : max;
		if( max )
		{
			normal.x /= max;
			normal.y /= max;
			normal.z /= max;
		}

		// apply the normal face to the 3 vertex of it
		vertex[ id1 + 3 ] = vertex[ id2 + 3 ] = vertex[ id3 + 3 ] = normal.x;
		vertex[ id1 + 4 ] = vertex[ id2 + 4 ] = vertex[ id3 + 4 ] = normal.y;
		vertex[ id1 + 5 ] = vertex[ id2 + 5 ] = vertex[ id3 + 5 ] = normal.z;
	}
}

// ------------------------------------------------------------------------
void do_matching_contours(BREP_WING* wingB)
{
	BREP_CONTOUR* contourB = wingB->contour;

	if(contourB == NULL || contourB->face != matching_contour_faceB)
		return;

	// wingB has endpoints at the same location as the first wing of
	// matching_contourA. The contours match if the enclosed region is on the same
	// side, i.o.w. the first vertex of wingB is at the same location as the
	// first vertex of the first wing of matching_contourA
	if(VertexCompareLocation(matching_contourA->wings->vertex, wingB->vertex) == 8)
		matching_contour_func(matching_contourA, contourB);
}

void IterateMatchingContours(BREP_FACE* faceA, BREP_FACE* faceB,
							 void (*func)(BREP_CONTOUR* contourA, BREP_CONTOUR* contourB))
{
	void (*old_matching_contour_func)(BREP_CONTOUR*, BREP_CONTOUR*) = matching_contour_func;
	matching_contour_func = func;
	matching_contour_faceB = faceB;

	BREP_CONTOUR_RING::iterator contourA;
	for(contourA = faceA->outer_contour.begin(); contourA != faceA->outer_contour.end(); contourA++)
		if((*contourA)->wings && (*contourA)->wings->next != (*contourA)->wings->prev)
			{
				matching_contourA = *contourA;
				BrepIterateWingsBetweenLocations((*contourA)->wings->vertex,
					(*contourA)->wings->next->vertex,
					faceB->shell->solid->vertices,
					do_matching_contours);
			}

	matching_contour_func = old_matching_contour_func;
}

// Clears the contour masks
void unset_mark(BREP_CONTOUR* contour)
{
	((CONTOUR*)(contour))->ClearMarks();
}

// Returns the inside/outside mask of the contour
unsigned int inoutmark(BREP_CONTOUR* contour)
{
	return ((CONTOUR*)(contour))->InOut();
}

// Marks a contour with the given mask
void mark_contour(BREP_CONTOUR* contour, unsigned int mask)
{
	((CONTOUR*)(contour))->SetInOut(mask);
}

// ContourA and contourB are matching contours in coplanar faces
// Keep one if they have the same orientation
void keep_one_same(BREP_CONTOUR* contourA, BREP_CONTOUR* contourB)
{
	if(((CONTOUR*)(contourA))->normal * ((CONTOUR*)(contourB))->normal > 0)
	{
		// Same orientation
		unset_mark(contourA);
		//((CONTOUR*)(contourA))->Write();
		unset_mark(contourB);
		((CONTOUR*)(contourB))->DontWrite();
	}
	else
	{
		// Opposite orientation
		unset_mark(contourA);
		((CONTOUR*)(contourA))->DontWrite();
		unset_mark(contourB);
		((CONTOUR*)(contourB))->DontWrite();
	}
}

// ContourA and contourB are matching contours in coplanar faces
// Keep one if they have opposite orientation.
void keep_one_opposite(BREP_CONTOUR* contourA, BREP_CONTOUR* contourB)
{
	if(((CONTOUR*)(contourA))->normal * ((CONTOUR*)(contourB))->normal > 0)
	{
		// Same orientation
		unset_mark(contourA);
		((CONTOUR*)(contourA))->DontWrite();
		unset_mark(contourB);
		((CONTOUR*)(contourB))->DontWrite();
	}
	else
	{
		// Opposite orientation
		unset_mark(contourA);
		//((CONTOUR*)(contourA))->Write();
		unset_mark(contourB);
		((CONTOUR*)(contourB))->DontWrite();
	}
}

// Partition pairs of faces belonging to shellA and shellB
void partition_shells(BREP_SHELL* shellA, BREP_SHELL* shellB)
{
	BREP_FACE_RING::iterator faceA, faceB;
	for(faceA = shellA->faces.begin(); faceA != shellA->faces.end(); faceA++)
		for(faceB = shellB->faces.begin(); faceB != shellB->faces.end(); faceB++)
		{
			FACE* faA = (FACE*)((*faceA));
			FACE* faB = (FACE*)((*faceB));
			if(!DisjunctBounds(faA->bounds, faB->bounds))
				if(PartitionFaces(faA, faB) == COPLANAR_ISECT)
					// Record a pair of coplanar faces
					coplanar_faces.push_back(COPLANAR_FACES(*faceA, *faceB));
		}
}

// inside/outside mark propagation: when a contour is cut during face partitioning,
// the cut contour parts are marked whether they are inside or outside the transversal
// solid. After partitioning, thi mark can be propagated to neighbouring contours,
// reducing the number of contours for which an explicit inside/outside test will
// be necessary
void wing_propagate_mark(BREP_WING* wing, unsigned int mark)
{
	BREP_CONTOUR* ocontour = BrepEdgeOtherWing(wing)->contour;
	if(ocontour == NULL || inoutmark(ocontour))
		return;

	mark_contour(ocontour, mark);
	contour_propagate_mark(ocontour);
}

void contour_propagate_mark(BREP_CONTOUR* contour)
{
	unsigned int mark = inoutmark(contour);

	if(mark != 0)
		contour->IterateWings1A((void (*)(BREP_WING*, void*))wing_propagate_mark, (void*)mark);
}

// Propagates inside/outside mark of contours for which it was
// determined during PartitionFaces() to neighbouring contours
void csg_propagate_mark(SOLID* solid)
{
	solid->IterateContours(contour_propagate_mark);
}

// inside/outside mark propagation only reduces the number of contours for which
// an explicit inside/outside test will be necessary. After propagation, this
// test is necessary for all contours that remained unmarked
void contour_mark_unmarked(BREP_CONTOUR* contour)
{
	if (inoutmark(contour))
		return;

	unsigned int mark;
	if((mark = ContourInSolid(contour, global_othersolid)))
	{
		mark_contour(contour, mark);
		contour_propagate_mark(contour);
	}
	else
	{
		// The contour is most probably coplanar with a contour
		// of the other solid and will be handled later
		Warning("contour_mark_unmarked", "can't determine whether contour is inside or outside other solid");
	}
}
void csg_mark_unmarked(SOLID* solid, SOLID* othersolid)
{
	global_othersolid = othersolid;
	solid->IterateContours(contour_mark_unmarked);
}

// Mark a contour that is outside the transversal solid for writing
void mark_out(BREP_CONTOUR* contour)
{
	if(!(inoutmark(contour) & OUTMASK))
		((CONTOUR*)(contour))->DontWrite();
}

// Mark a contour that is inside the transversal solid for writing
void mark_in(BREP_CONTOUR* contour)
{
	if(!(inoutmark(contour) & INMASK))
		((CONTOUR*)(contour))->DontWrite();
}

// Mark a contour that is inside the transversal solid for reverse writing
void mark_in_reverse(BREP_CONTOUR* contour)
{
	if(inoutmark(contour) & INMASK)
		((CONTOUR*)(contour))->WriteReverse();
	else
		((CONTOUR*)(contour))->DontWrite();
}

void mark_with_outmask(BREP_CONTOUR* contour)
{
	mark_contour(contour, OUTMASK);
}

// ------------------------------------------------------------------------
unsigned long GetVertexId(BREP_VERTEX *vertex)
{
	PointMap::iterator mid = pointMap.find(vertex);
	if(mid != pointMap.end())
		return mid->second;

	// Point has no ID yet
	unsigned long vid = vertexid++;
	pointMap[vertex] = vid;

	return vid;
}

void OutputVertexId(BREP_VERTEX *vertex)
{
	currentface.push_back(GetVertexId((BREP_VERTEX*)vertex));
}

void OutputReverseContour(BREP_CONTOUR* contour)
{
	if(!contour->wings)
		return;

	BREP_WING* wing = contour->wings->prev;
	do
	{
		OutputVertexId(wing->vertex);
		wing = wing->prev;
	}
	while(wing != contour->wings->prev);
}

void DoOutputContour(CONTOUR* contour)
{
	if(contour->IsDontWrite())
		return;

	currentface.clear();

	if(contour->IsWriteReverse())
		OutputReverseContour(contour);
	else
		contour->IterateVertices(OutputVertexId);

	faces.push_back(currentface);
}

void OutputFaceWithHoles(BREP_FACE *face)
{
	// Count the number of contours to be written
	int nrcontours = 0;

	BREP_CONTOUR_RING::iterator contour;
	for(contour = face->outer_contour.begin(); contour != face->outer_contour.end(); contour++)
		if(!((CONTOUR*)(*contour))->IsDontWrite())
			nrcontours++;

	// No contours to be written
	if(nrcontours == 0)
		return;

	for(contour = face->outer_contour.begin(); contour != face->outer_contour.end(); contour++)
	{
		CONTOUR* c = (CONTOUR*)(*contour);

		//if(contour->IsHole())
		DoOutputContour(c);
	}
}

int FaceWithHoles(BREP_FACE* face)
{
	BREP_CONTOUR_RING::iterator contour;
	for(contour = face->outer_contour.begin(); contour != face->outer_contour.end(); contour++)
		if(!((CONTOUR*)(*contour))->IsDontWrite() && ((CONTOUR*)(*contour))->IsHole())
			return true;

	return false;
}

void OutputFace(BREP_FACE* face)
{
	BREP_CONTOUR_RING::iterator contour;

	for(contour = face->outer_contour.begin(); contour != face->outer_contour.end(); contour++)
	{
		// For all wings in contour ...
		if(*contour != NULL && (*contour)->wings != NULL)
		{
			BREP_WING* first = (*contour)->wings;
			BREP_WING* iwing;
			BREP_WING* next;
			for(iwing = first, next = iwing->next; iwing != NULL; iwing = (next == first) ? NULL : next, next = next->next)
				if(iwing->vertex->wing_ring.size())
					GetVertexId(iwing->vertex);
		}
	}

	if(FaceWithHoles(face))
		OutputFaceWithHoles(face);
	else
	{
		for(contour = face->outer_contour.begin(); contour != face->outer_contour.end(); contour++)
			DoOutputContour((CONTOUR*)(*contour));
	}
}

void OutputShell(BREP_SHELL* shell)
{
	BREP_FACE_RING::iterator face;
	for(face = shell->faces.begin(); face != shell->faces.end(); face++)
		OutputFace(*face);
}

VECTOR GetNormal(VECTOR v1, VECTOR v2, VECTOR v3)
{
	VECTOR a = v1-v2;
	VECTOR b = v2-v3;

	return VECTOR( (a.y * b.z) - (a.z * b.y), (a.z * b.x) - (a.x * b.z), (a.x * b.y) - (a.y * b.x) );
}
