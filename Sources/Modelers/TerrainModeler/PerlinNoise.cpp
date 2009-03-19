#include "PerlinNoise.h"

#include <cstdlib>
#include <cmath>
#include <iostream>

#include <Ogre.h>

using namespace Ogre;

namespace tege {

PerlinNoise::PerlinNoise() 
		: _randx(rand()), _persistence(0.5), _nbOfOctaves(4), _noiseTables(NULL), _noiseTablesNeedReallocation(false), _amplitudeScale(1.0){
	allocateNoiseTables();
}

PerlinNoise::PerlinNoise(int generatedGridSize, double persistence, unsigned int nbOfOctaves, double amplitudeScale)
		: _randx(rand()), _persistence(persistence), _noiseTables(NULL), _noiseTablesNeedReallocation(false), _amplitudeScale(amplitudeScale){
	//on the first noise table, we choose a length of the function of 8, meaning 
	//with a frequency of 2^0 = 1 there will be 9*9 points in the table (first and last points being equals)
	//now check if the asked number of octaves isn't too high
	//(meaning some later octaves will need more points that they actually are int the generated grid)
	//first if number of octaves is zero...
	if (nbOfOctaves == 0){
		nbOfOctaves = 1;
	} else if(8*pow(2.0, (int)(nbOfOctaves-1)) + 1 > generatedGridSize) {
		//useless octaves are asked, reduce the number
		generatedGridSize = (generatedGridSize-1)/8;
		_nbOfOctaves = 1;
		while(generatedGridSize != 1){
			generatedGridSize >>= 1;
			++_nbOfOctaves;
		}
	} else {
		_nbOfOctaves = nbOfOctaves;
	}
	allocateNoiseTables();
}

PerlinNoise::~PerlinNoise() 
{
	freeNoiseTables();
}

void PerlinNoise::freeNoiseTables(){
	if(_noiseTables != NULL){
		for(unsigned int octaveNumber = 0 ; octaveNumber < _nbOfOctaves ; ++octaveNumber){
			int numberOfPoints = 8*pow(2.0,(int)octaveNumber);
			for(unsigned int numLine = 0 ; numLine < numberOfPoints ; ++numLine){
				free(_noiseTables[octaveNumber][numLine]);
			}
			free(_noiseTables[octaveNumber]);
		}
		free(_noiseTables);
	}
}

void PerlinNoise::allocateNoiseTables(){
	_noiseTables = (double***)malloc(_nbOfOctaves*sizeof(double**));
	for(unsigned int octaveNumber = 0 ; octaveNumber < _nbOfOctaves ; ++octaveNumber){
		int numberOfPoints = 8*pow(2.0,(int)octaveNumber)+1;
		_noiseTables[octaveNumber] = (double**)malloc(numberOfPoints*sizeof(double*));
		for(unsigned int numLine = 0 ; numLine < numberOfPoints-1 ; ++numLine){
			_noiseTables[octaveNumber][numLine] = (double*)malloc(numberOfPoints*sizeof(double));
		}
		//some dirty trick for a wrapping table : last line pointing on first
		//it handles vertical wrapping, horizontal wrapping will be handled
		//when filling the table by recopying the value
		_noiseTables[octaveNumber][numberOfPoints-1] = _noiseTables[octaveNumber][0];
	}	
}

void PerlinNoise::generateNoiseTables(){
	if(_noiseTablesNeedReallocation){
		freeNoiseTables();
		allocateNoiseTables();
		_noiseTablesNeedReallocation = false;
	}
	//now fill the tables
	for(unsigned int octaveNumber = 0 ; octaveNumber < _nbOfOctaves ; ++octaveNumber){
		//fill the table for the octaveNumber-th octave
		std::cout << "filling noise table for octave " << octaveNumber << std::endl;
		int numberOfPoints = 8*pow(2.0,(int)octaveNumber);
		for(unsigned int numLine = 0 ; numLine < numberOfPoints ; ++numLine){
			for(unsigned int numCol = 0 ; numCol  < numberOfPoints ; ++numCol){
				_noiseTables[octaveNumber][numLine][numCol] = generateNoise(myRand());
			}
			//copy the first point of the line at the end
			_noiseTables[octaveNumber][numLine][numberOfPoints] = _noiseTables[octaveNumber][numLine][0];

			//don't fill the last line as it is pointing on the first line already filled
		}
	}
}

void PerlinNoise::fillGrid(float** grid, unsigned int numberOfPoints){
	//we are assuming the grid is set to zero
	//for each octave
	for(unsigned int octaveNumber = 0 ; octaveNumber < _nbOfOctaves ; ++octaveNumber){
		//std::cout << "computing grid for octave " << octaveNumber << std::endl;
		float octaveAmplitude = pow(_persistence,(int)octaveNumber);
		//for each grid point
		for(unsigned int numLine = 0 ; numLine < numberOfPoints ; ++numLine){
			for(unsigned int numCol = 0 ; numCol < numberOfPoints ; ++numCol){
				grid[numLine][numCol] += (float)(_amplitudeScale * 
						octaveAmplitude * 
						interpolateBilinearNoise(octaveNumber, numLine, numCol, numberOfPoints));
			}
		}
	}
}

double PerlinNoise::interpolateBilinearNoise(unsigned int octaveNumber, unsigned int numLine, unsigned int numCol, unsigned int numberOfPoints){
	//formula of bilinear interpolation with 0<=x<=1 and 0<=y<=1
	//f(x,y) ~ f(0,0)(1-x)(1-y) + f(1,0)x(1-y) + f(0,1)(1-x)y + f(1,1)xy 
	float noiseValue;
	unsigned int wavelength = (numberOfPoints-1)/(8*(1<<octaveNumber));
	//true if the point is on a line of the grid noise
	bool pointOnLineOfNoise = (numLine%wavelength == 0);
	//true if the point is on a vertical of the grid noise
	bool pointOnVertOfNoise = (numCol%wavelength == 0);
	if(!(pointOnLineOfNoise || pointOnVertOfNoise)){//we must interpolate in both directions
		float xShift = numCol/(float)wavelength;
		float zShift = numLine/(float)wavelength;
		int xCoord = floor(xShift);
		int zCoord = floor(zShift);
		xShift -= floor(xShift);
		zShift -= floor(zShift);
		noiseValue = _noiseTables[octaveNumber][zCoord][xCoord]*(1-xShift)*(1-zShift) +
		 			 _noiseTables[octaveNumber][zCoord][xCoord+1]*xShift*(1-zShift) +
		 			 _noiseTables[octaveNumber][zCoord+1][xCoord]*(1-xShift)*zShift +
		 			 _noiseTables[octaveNumber][zCoord+1][xCoord+1]*xShift*zShift;
	} else if(pointOnLineOfNoise && pointOnVertOfNoise){//point on grid of noise
		//simply return the value for the noise at this point
		noiseValue = _noiseTables[octaveNumber][numLine/wavelength][numCol/wavelength];
	} else if(pointOnLineOfNoise){//point on horizontal line, interpolation in one dimension only (horizontal)
		float xShift = numCol/(float)wavelength;
		int xCoord = floor(xShift);
		xShift -= floor(xShift);
		int zCoord = numLine/wavelength;
		noiseValue = _noiseTables[octaveNumber][zCoord][xCoord]*(1-xShift) +
		 			 _noiseTables[octaveNumber][zCoord][xCoord+1]*xShift;
	} else if(pointOnVertOfNoise){//point on vertical line, interpolation in one dimension only (vertical)
		float zShift = numLine/(float)wavelength;
		int zCoord = floor(zShift);
		zShift -= floor(zShift);
		int xCoord = numCol/wavelength;
		noiseValue = _noiseTables[octaveNumber][zCoord][xCoord]*(1-zShift) +
		 			 _noiseTables[octaveNumber][zCoord+1][xCoord]*zShift;
	}
	return noiseValue;
}

double PerlinNoise::interpolateBicubicNoise(
		unsigned int octaveNumber, unsigned int numLine, unsigned int numCol, unsigned int numberOfPoints){
	//formula for bicubic convolution algorithm
	//                            [ 0  2  0  0][a-1]
	//                            [-1  0  1  0][ a0]
	// p(t) = 1/2*[1  t  t^2  t^3][ 2 -5  4 -1][ a1]
	//                            [-1  3 -3  1][ a2]
	float noiseValue;
	unsigned int wavelength = (numberOfPoints-1)/(8*(1<<octaveNumber));
	//true if the point is on a line of the grid noise
	bool pointOnLineOfNoise = (numLine%wavelength == 0);
	//true if the point is on a vertical of the grid noise
	bool pointOnVertOfNoise = (numCol%wavelength == 0);
	//if(!(pointOnLineOfNoise || pointOnVertOfNoise)){//we must interpolate in both directions
		//std::cout << "normal point" << std::endl;
		float xShift = numCol/(float)wavelength;
		float zShift = numLine/(float)wavelength;
		int xCoord = floor(xShift);
		int zCoord = floor(zShift);
		xShift -= floor(xShift);
		zShift -= floor(zShift);
		//the constant matrix
		Matrix4 coeff( 0,  2,  0,  0,
					  -1,  0,  1,  0,
					   2, -5,  4, -1,
					  -1,  3, -3,  1);
		
		Vector4 xPow(1, 
				xShift,
				xShift*xShift,
				xShift*xShift*xShift);
		Vector4 xCoeff(0, 
				_noiseTables[octaveNumber][zCoord][xCoord],
				_noiseTables[octaveNumber][zCoord][xCoord+1],
				0);

		Vector4 zPow(1, 
				zShift,
				zShift*zShift,
				zShift*zShift*zShift);
		Vector4 zCoeff(0,
				_noiseTables[octaveNumber][zCoord][xCoord],
				_noiseTables[octaveNumber][zCoord+1][xCoord],
				0);
		//check if we are not near the bondaries of the table
		int xCoordMinusOne = xCoord-1;
		int xCoordPlusTwo  = xCoord+2;
		int zCoordMinusOne = zCoord-1;
		int zCoordPlusTwo  = zCoord+2;
		if(xCoord == 0){
			xCoordMinusOne = 8*(1<<octaveNumber);
		} else if(xCoord + 2 == 8*(1<<octaveNumber) + 1){
			xCoordPlusTwo  = 0;
		}
		if(zCoord == 0){
			zCoordMinusOne = 8*(1<<octaveNumber);
		} else if(zCoord + 2 == 8*(1<<octaveNumber) + 1){
			zCoordPlusTwo = 0;
		}
		
		//std::cout << xCoord << " " << zCoord << " " << xCoordMinusOne << " " << xCoordPlusTwo << " " << zCoordMinusOne << " " << zCoordPlusTwo << std::endl;
		
		xCoeff.x = _noiseTables[octaveNumber][zCoord][xCoordMinusOne];
		xCoeff.w = _noiseTables[octaveNumber][zCoord][xCoordPlusTwo];
		
		zCoeff.x = _noiseTables[octaveNumber][zCoordMinusOne][xCoord];
		zCoeff.w = _noiseTables[octaveNumber][zCoordPlusTwo][xCoord];
		
		float pX = 0.5*xPow.dotProduct(coeff*xCoeff);
		float pZ = 0.5*zPow.dotProduct(coeff*zCoeff);
		//the constant 
		noiseValue = (pX+pZ)/2;
	/*} else if(pointOnLineOfNoise && pointOnVertOfNoise){//point on grid of noise
		//simply return the value for the noise at this point
		noiseValue = _noiseTables[octaveNumber][numLine/wavelength][numCol/wavelength];
	} else if(pointOnLineOfNoise){//point on horizontal line, interpolation in one dimension only (horizontal)
		float xShift = numCol/(float)wavelength;
		int xCoord = floor(xShift);
		xShift -= floor(xShift);
		int zCoord = numLine/wavelength;
		//the constant matrix
		Matrix4 coeff( 0,  2,  0,  0,
					  -1,  0,  1,  0,
					   2, -5,  4, -1,
					  -1,  3, -3,  1);
		
		Vector4 xPow(1, 
				xShift,
				xShift*xShift,
				xShift*xShift*xShift);
		Vector4 xCoeff(0, 
				_noiseTables[octaveNumber][zCoord][xCoord],
				_noiseTables[octaveNumber][zCoord][xCoord+1],
				0);
		//check if we are not near the bondaries of the table
		int xCoordMinusOne = xCoord-1;
		int xCoordPlusTwo  = xCoord+2;
		
		if(xCoord == 0){
			xCoordMinusOne = 8*(1<<octaveNumber);
		} else if(xCoord + 2 == 8*(1<<octaveNumber) + 1){
			xCoordPlusTwo  = 0;
		}
		xCoeff.x = _noiseTables[octaveNumber][zCoord][xCoordMinusOne];
		xCoeff.w = _noiseTables[octaveNumber][zCoord][xCoordPlusTwo];

		float pX = 0.5*xPow.dotProduct(coeff*xCoeff);
		//std::cout << "horizontal point" << xShift << " " << 
		//_noiseTables[octaveNumber][zCoord][xCoord] << " " << _noiseTables[octaveNumber][zCoord][xCoord+1] << std::endl;
		noiseValue = pX;
	} else if(pointOnVertOfNoise){//point on vertical line, interpolation in one dimension only (vertical)
		//std::cout << "vertical point" << std::endl;
		float zShift = numLine/(float)wavelength;
		int zCoord = floor(zShift);
		zShift -= floor(zShift);
		int xCoord = numCol/wavelength;
		//the constant matrix
		Matrix4 coeff( 0,  2,  0,  0,
					  -1,  0,  1,  0,
					   2, -5,  4, -1,
					  -1,  3, -3,  1);

		Vector4 zPow(1, 
				zShift,
				zShift*zShift,
				zShift*zShift*zShift);
		Vector4 zCoeff(0,
				_noiseTables[octaveNumber][zCoord][xCoord],
				_noiseTables[octaveNumber][zCoord+1][xCoord],
				0);
		int zCoordMinusOne = zCoord-1;
		int zCoordPlusTwo  = zCoord+2;
		if(zCoord == 0){
			zCoordMinusOne = 8*(1<<octaveNumber);
		} else if(zCoord + 2 == 8*(1<<octaveNumber) + 1){
			zCoordPlusTwo = 0;
		}
		
		zCoeff.x = _noiseTables[octaveNumber][zCoordMinusOne][xCoord];
		zCoeff.w = _noiseTables[octaveNumber][zCoordPlusTwo][xCoord];
		float pZ = 0.5*zPow.dotProduct(coeff*zCoeff);
		noiseValue = pZ;
	}*/
	//std::cout << noiseValue << std::endl;
	return noiseValue;	
}

inline int PerlinNoise::myRand(){
	return (((_randx=_randx*1103515245 + 12345)>>16) & 077777);
}

double PerlinNoise::generateNoise(int noiseSeed){
	noiseSeed = (noiseSeed << 13) ^ noiseSeed;
	return ((2.0 - ((noiseSeed * (noiseSeed * noiseSeed * 15731 + 789221) + 1376312589) & 0x7FFFFFFF) / 1073741824.0) / 2.0);
}

};