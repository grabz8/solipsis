/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Jonathan MERLET

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

#ifndef _TERRAINGENERATOR_H_
#define _TERRAINGENERATOR_H_

#include <vector>
#include <utility>

#include <Ogre.h>

#include <OgreDefaultHardwareBufferManager.h>

extern "C" {
#define REAL float
#define ANSI_DECLARATORS 1
#include <triangle.h>
}

namespace tege {

	enum ReliefType{MOUNTAIN, ERODED, CRATER};

	//WARNING : we are assuming holes are stricly disjoint
	//even if holes are overlapping it seems to work, but we don't assure that
	struct Hole{
		Hole(){}
		Hole(float xm, float xM, float zm, float zM) : xMin(xm), xMax(xM), zMin(zm), zMax(zM){}
		Hole(const Hole& right){
			xMin = right.xMin;
			xMax = right.xMax;
			zMin = right.zMin;
			zMax = right.zMax;
		}
		Hole& operator=(const Hole& right){
			xMin = right.xMin;
			xMax = right.xMax;
			zMin = right.zMin;
			zMax = right.zMax;
			return *this;
		}
		float xMin, xMax, zMin, zMax;
	};
	
	//As we are assuming holes are disjoint, part of a hole on a line is disjoint too
	//To compare hole in a line, just check if the xMax coord of the first
	//is inferior to the xMin coord of the second
	struct CompareLineHoles{
		bool operator()(const std::pair<int, int>& p0, const std::pair<int, int>& p1){
			return (p0.second < p1.first);
		}
	};
	
	//a little struct rather than using arVec3 (no need of mathematical operations)
	struct Triangle{
		Triangle(){}
		Triangle(int i0, int i1, int i2) : index0(i0), index1(i1), index2(i2){}
		Triangle(const Triangle& right){
			this->index0 = right.index0;
			this->index1 = right.index1;
			this->index2 = right.index2;
		}
		Triangle& operator=(const Triangle& right){
			this->index0 = right.index0;
			this->index1 = right.index1;
			this->index2 = right.index2;
			return *this;
		}
		int index0, index1, index2;
	};

	class TerrainGenerator {
	public:
		/*
		 * Empty Ctor, initialize grid to a 3*3
		 */
		TerrainGenerator();
		
		/*
		 * Normal constructor, intialize grid to (2^subdivisionPower)+1
		 */
		TerrainGenerator(unsigned int subdivisionPower, float xDist = 1.0, float zDist = 1.0);
		
		/*
		 * Dtor, in charge of the freeing of the grid
		 */
		~TerrainGenerator();

		//Altitude generation methods
		/*
		 * Simply put the whole grid to zero, can be used to "clean" a grid
		 * grid memory is initialized at construction but NOT its value, so they are pointing to 
		 * random values
		 */
		void generateZeroAltitude();
		/*
		 * generate grid values with RMD method : fast but not very good
		 * 
		 * \param maxAltitude the max altitude on the grid
		 * \param minAltitude the min altitude on the grid
		 * \param hurstFactor the sharpness of the grid
		 */
		void generateRandomMidpointDisplacement(int maxAltitude = 1, int minAltitude = 0, float hurstFactor = 1.0);
		/*
		 * generate grid values with perlin noise : looks greater than RMD but more expensive
		 * especially if lot of octaves are asked and if bicubic interpolation is implemented 
		 * (by now it's only bilinear)
		 */
		void generatePerlinNoise(float persistence = 0.5, unsigned int nbOctaves = 4, unsigned int scaling = 10);

		//global modifications
		/*
		 * force low altitude to smoothen
		 * 
		 * \param smoothFactor the higher it is, the smoother the valley will be (1 = no smoothing)
		 * \param altPercent the portion of the altitude to smoothen (1 = full altitude)
		 */
		void valleyGrid(float smoothFactor, float altPercent = 1.0);
		/*
		 * Experiment to try to erode mountains, but simply don't work
		 */
		void erodeMountains(float invSmoothingFactor, float altPercent = 1.0);
		
		//local modifications
		/*
		 * Experiment to try yo add a valley at a certain point, but don't work
		 */
		void addValleyZone(float xCoord, float zCoord, float radius);
		/*
		 * Add river using erosion method, that is starting from a certain point
		 * the river will follow lower altitude until it can't go further
		 * 
		 * \param xCoord where to start the river on x axis
		 * \param zCoord where to start the river on z axis
		 */
		void addRiver(float xCoord, float zCoord);
		
		/*
		 * Add a local detail using a mathematical equation (private methods)
		 * 
		 * \param type type of the relief to add using enum at the start of the header
		 * \param xCoord where to place the center of the detail on x axis
		 * \param zCoord where to place the center of the detail on z axis
		 * \param altitude amplitude of the detail
		 * \param radius radius of the base of the detail
		 */
		void addMathematicalRelief(ReliefType type, float xCoord, float zCoord, float altitude, float radius);
		
		/*
		 * add the BBox of a hole
		 */
		void addHole(float xm, float xM, float ym, float yM);
		
		//triangulation methods
		/*
		 * triangulate line by line asking each hole if it is somewhere on the line
		 */
		void triangulateSimple();
		/*
		 * triangulate using a mask
		 * a bit faster than simple triangulation but use more memory for the mask
		 */
		void triangulateWithMask();
		/*
		 * triangulate using triangle
		 * slower, but if the constraints on the grid (like using BBox and not arbitrary holes)
		 * are released, could be simplier to use
		 */
		void triangulateTriangle();
		/*
		 * get "participation" of each hole to a given line of the grid
		 */
		bool getHolesInLine(unsigned int numLine, std::vector<std::pair<int, int> >& holesInLine);
		/*
		 * to know if we need to retriangulate (if a hole is added, for example)
		 */
		bool isTriangulated(){return _triangulated;}
		
		//mesh management method
		/*
		 * call createMesh(), assignVertices and assign Triangles
		 * just to have a few lines less 
		 */
		void prepareMesh(const Ogre::String& name);
		/*
		 * ask Ogre to create a mesh
		 */
		void createMesh(const Ogre::String& name);
		//vertices assignment and triangles assignment are separated so
		//you can change altitude of points without retriangulating, 
		//or add a hole without regenerating altitude
		/*
		 * assign vertices of the grid to the mesh
		 */
		void assignVerticesToMesh();
		/*
		 * assign triangles
		 */
		void assignTrianglesToMesh();
		/*
		 * assign a material to the mesh
		 */
		void assignMaterialToMesh(const Ogre::String& name);
		/*
		 * get a smart pointer on the created mesh
		 */
		Ogre::MeshPtr getMeshPtr(){return _meshTerrain;}
		/*
		 * export the mesh to a .mesh file
		 */
		void exportMesh();
		
	private:
		/*
		 * allocate memory of the mask and set it to true
		 */
		void initMask();
		/*
		 * reset entire mask to true
		 */
		void resetMask();
		/*
		 * update mask by using holes to find where we do not need triangles
		 */
		void computeMask();
		
		/*
		 * use mathematical equation of a cone to simulate a mountain
		 */
		void useMathematicalMountain(float xCoord, float zCoord, 
				unsigned int numCol, unsigned int numLine, 
				float radius, float altitude);
		/* 
		 * use mathematical equation of a parabola to simulate a hill
		 */
		void useMathematicalErodedMountain(float xCoord, float zCoord, 
				unsigned int numCol, unsigned int numLine, 
				float radius, float altitude);
		/*
		 * use modified mathematical equation of a ripple to simulate a crater
		 */ 
		void useMathematicalCrater(float xCoord, float zCoord, 
				unsigned int numCol, unsigned int numLine, 
				float radius, float altitude);

		/*
		 * add triangles corresponding to a rectangle in the grid
		 */
		void addTriangles(int xUpperLeftCorner, int zUpperLeftCorner);
		
		/*
		 * init Triangle's I/O struct
		 */ 
		void initStruct(struct triangulateio& tio);
		/*
		 * free Triangle's I/O struct
		 */
		void freeStruct(struct triangulateio& tio);
		
		/*
		 * square step used in the RMD method
		 */
		void squareStep(unsigned int subdivisionLevel);
		/*
		 * diamond step used in the RMD method
		 */
		void diamondStep(unsigned int subDivisionLevel);
		
		/*
		 * generate the random amount we need to add in RMD method
		 */
		float generateRandomAmountMD(int invPrecision, int subdivisionLevel); //inline method but declared in the cpp file as it is private
		
		/*
		 * STG have it's own random number generator, as it can be extensively used, so
		 * embedding it here rather than using the standard generator increase performances
		 * (even though this generator is seeded using the standard generator)
		 */
		int myRand();
		
		/*
		 * number of points on the side of the grid
		 *  always 2^n+1 where n is some integer > 0
		 */
		unsigned int _nbPointsPerSide;
		/*
		 * the regular grid used to store altitude of points
		 */
		float** _grid;
		/*
		 * the mask used for triangulation
		 * same size as the grid
		 */
		bool** _mask;
		/*
		 * max altitude used in RMD method
		 */
		int _maxAltitude;
		/*
		 * min altitude used in RMD method
		 */
		int _minAltitude;
		/*
		 * distance between two points in the grid on the x axis
		 */
		float _xDistanceBetweenPoints;
		/*
		 * distance between two points in the grid on the z axis
		 */
		float _zDistanceBetweenPoints;
		
		/*
		 * term of the random number generator
		 */
		int _randx;
		
		/*
		 * sharpness factor used in the RMD method
		 */
		float _hurst;
		/*
		 * to know if the grid has already been triangulated
		 */
		bool _triangulated;
		
		/*
		 * triangles computed during the triangulation
		 */
		std::vector<Triangle> _triangles;
		/*
		 * holes added by the user representing the cells
		 */
		std::vector<Hole> _holes;
		
		/*
		 * pointer on the mesh to store the terrain
		 */
		Ogre::MeshPtr _meshTerrain;
		
	};

}// namespace TerrainGenerator

#endif // _TERRAINGENERATOR_H_
