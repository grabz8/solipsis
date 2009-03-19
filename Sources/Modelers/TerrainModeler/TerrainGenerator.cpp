#include "TerrainGenerator.h"

#include "PerlinNoise.h"

#include <cmath>
#include <iostream>
#include <algorithm>

#include <float.h>

using namespace Ogre;
using namespace std;

namespace tege {

	#define SUBDIVISIONLIMIT 12 // 2^12 = 4096
	#define PRECISION 100 
	#define VALLEYSMOOTHING 0.6



	/***********************************************************************************
	 * Object construction and destruction : Ctors, Dtor, and initialisation functions *
	 **********************************************************************************/

	//default constructor, just allocate memory for a 3*3 grid and the mask
	TerrainGenerator::TerrainGenerator() : 
		_nbPointsPerSide(3), _minAltitude(0), _maxAltitude(1), _randx(rand()), _hurst(1.0), 
		_triangulated(false), _xDistanceBetweenPoints(1.0), _zDistanceBetweenPoints(1.0) {
		initMask();
		_grid = (float**)malloc(_nbPointsPerSide*sizeof(float*));
		_grid[0] = (float*)malloc(_nbPointsPerSide*sizeof(float));
		_grid[1] = (float*)malloc(_nbPointsPerSide*sizeof(float));
		_grid[2] = (float*)malloc(_nbPointsPerSide*sizeof(float));
	}
	
	//Ctor giving control over the parameters of the grid
	TerrainGenerator::TerrainGenerator(unsigned int subdivisionPower, float xDist, float zDist) 
	:  _randx(rand()), _triangulated(false) {
		//ask for at least a 9*9 grid
		if(subdivisionPower < 3){
			subdivisionPower = 3;
		}
		//but less points than the limit defined at the top of the file
		if(subdivisionPower <= SUBDIVISIONLIMIT){
			_nbPointsPerSide = pow((float)2.0,(int)subdivisionPower) + 1;
		} else {
			cout << "number of subdivision superior to upper limit (" << SUBDIVISIONLIMIT << "), reducing to limit" << endl;
			_nbPointsPerSide = pow((float)2.0,SUBDIVISIONLIMIT) + 1;
		}
		//check if the asked distance is positive before assigning it
		_xDistanceBetweenPoints = (xDist>0)?xDist:1;
		_zDistanceBetweenPoints = (zDist>0)?zDist:1;
		//init memory of the mask
		initMask();
		//init memory of the grid
		_grid = (float**)malloc(_nbPointsPerSide*sizeof(float*));
		for(unsigned int i = 0 ; i < _nbPointsPerSide ; ++i){
			_grid[i] = (float*)malloc(_nbPointsPerSide*sizeof(float));
		}
		//seed the corners of the grid to zero
		_grid[0][0] = 0;//myRand()%(_maxAltitude-_minAltitude)+_minAltitude;
		_grid[_nbPointsPerSide-1][0] = _grid[0][0];
		_grid[0][_nbPointsPerSide-1] = _grid[0][0];
		_grid[_nbPointsPerSide-1][_nbPointsPerSide-1] = _grid[0][0];
	}
	
	//Dtor, free grid and mask
	TerrainGenerator::~TerrainGenerator(){
		for(unsigned int i = 0 ; i < _nbPointsPerSide ; ++i){
			free(_grid[i]);
			free(_mask[i]);
		}
		free(_grid);
		free(_mask);
	}
	
	//init memory of the mask but doesn't compute the mask
	void TerrainGenerator::initMask(){
		_mask = (bool**)malloc(_nbPointsPerSide*sizeof(bool*));
		for(unsigned int i = 0 ; i < _nbPointsPerSide ; ++i){
			_mask[i]= (bool*)malloc(_nbPointsPerSide*sizeof(bool));
			//set the whole mask to 1
			memset(_mask[i], 1, _nbPointsPerSide*sizeof(bool));
		}
	}
	
	
	
	/**********************************************
	 * Altitude computation : RMD, perlin methods *
	 *********************************************/
	
	//set the whole grid to a zero altitude
	void TerrainGenerator::generateZeroAltitude(){
		for(unsigned int i = 0 ; i < _nbPointsPerSide ; ++i){
			for(unsigned int j = 0 ; j < _nbPointsPerSide ; ++j){
				_grid[i][j] = 0.0;
			}
		}
	}
	
	void TerrainGenerator::generateRandomMidpointDisplacement(int maxAltitude, int minAltitude, float hurstFactor){
		//get the sharpness factor in a class member so it can be used in other functions
		//without passing it in parameter
		_hurst = hurstFactor;
		//assign altitude members to half of parameters so it won't 
		//go above the value when adding random amount
		if(maxAltitude > minAltitude){
			_maxAltitude = maxAltitude/2;
			_minAltitude = minAltitude/2;			
		} else {
			_maxAltitude = minAltitude/2;
			_minAltitude = maxAltitude/2;					
		}
		//seed the corners of the grid to the same value
		_grid[0][0] = 2*(myRand()%(_maxAltitude-_minAltitude)+_minAltitude);
		_grid[_nbPointsPerSide-1][0] = _grid[0][0];
		_grid[0][_nbPointsPerSide-1] = _grid[0][0];
		_grid[_nbPointsPerSide-1][_nbPointsPerSide-1] = _grid[0][0];
		_triangulated = false;
		//since end point of each line is the first point 
		//and last line is first line, the four corners
		//of the master square are the first point
		
		//determine the random amount to add to this point
		//we are assuming random number generator has already been seeded
		//use a precision to the 1/100
		unsigned int halfGrid = (_nbPointsPerSide-1) >> 1;
		_grid[halfGrid][halfGrid] = generateRandomAmountMD(PRECISION, 0);
		
		//now do the diamond step : compute the center of the upper and left diamonds
		//as we are on the first level, use same technique than for the square center rather than averaging
		//to prevent corners to totally influence terrain
		_grid[0][halfGrid] = generateRandomAmountMD(PRECISION, 1);
		_grid[_nbPointsPerSide-1][halfGrid] = _grid[0][halfGrid];
		_grid[halfGrid][0] = generateRandomAmountMD(PRECISION, 1);
		_grid[halfGrid][_nbPointsPerSide-1] = _grid[halfGrid][0];
		
		if(_nbPointsPerSide > 3){
			//launch recursive process
			squareStep(1);
		}
	}
	
	void TerrainGenerator::squareStep(unsigned int subdivisionLevel){
		//cout << "step " << subdivisionLevel << " square" << endl;
		//coordinates of the centers of the squares
		unsigned int squareMinX, squareMinY;
		//increment is the number of points of a side of each square minus one
		unsigned int increment = (_nbPointsPerSide-1) >> subdivisionLevel;
		//half of the increment will be useful to calculate coordinates
		unsigned int halfInc = increment >> 1;
		//center of the last square
		unsigned int lastCenter = _nbPointsPerSide - 1 - halfInc;
		for(squareMinX = halfInc ; squareMinX <= lastCenter ; squareMinX += increment){
			for(squareMinY = halfInc ; squareMinY <= lastCenter ; squareMinY += increment){
				//compute the value of this point by averaging the four corners of the square
				//then adding a random amount
				_grid[squareMinX][squareMinY] =
					(_grid[squareMinX-halfInc][squareMinY-halfInc] + //upper left corner
					 _grid[squareMinX+halfInc][squareMinY-halfInc] + //upper right
					 _grid[squareMinX-halfInc][squareMinY+halfInc] + //lower left
					 _grid[squareMinX+halfInc][squareMinY+halfInc]) / 4 // lower right
					+ generateRandomAmountMD(PRECISION, subdivisionLevel);
			}
		}
		
		//now the diamond step
		diamondStep(subdivisionLevel);
	}
	
	void TerrainGenerator::diamondStep(unsigned int subdivisionLevel){
		//cout << "step " << subdivisionLevel << " diamond" << endl;
		//when computing diamonds centers, only the centers of the of the upper
		//diamond and of the left one are computed of each square, as lower
		//diamond is upper diamond for square one the next line, and right diamond
		//is left diamond for square on the next column
		//cordinates of the centers of the squares
		unsigned int squareX, squareY;
		//increment is the number of points of a side of each square minus one
		unsigned int increment = _nbPointsPerSide - 1  >> subdivisionLevel;
		//half of the increment will be useful to calculate coordinates
		unsigned int halfInc = increment >> 1;
		unsigned int lastCenter = _nbPointsPerSide - 1 - halfInc;
		
		//first compute the diamonds of the upper left square to copy points computed
		//upper diamond
		_grid[halfInc][0] = 
			(_grid[0][0] +
			 _grid[increment][0] +
			 _grid[halfInc][halfInc] +
			 _grid[halfInc][lastCenter]) / 4
			+ generateRandomAmountMD(PRECISION, subdivisionLevel + 1);
		//left diamond
		_grid[0][halfInc] = 
			(_grid[0][0] +
			 _grid[0][increment] +
			 _grid[halfInc][halfInc] +
			 _grid[lastCenter][halfInc]) / 4
			+ generateRandomAmountMD(PRECISION, subdivisionLevel + 1);
		//copy points on other side of the grid
		_grid[halfInc][_nbPointsPerSide-1] = _grid[halfInc][0];
		_grid[_nbPointsPerSide-1][halfInc] = _grid[0][halfInc];
		
		//then compute diamonds center of the first line as there upper diamonds are wrapped
		//and we need to copy there center for the bottom of the grid
		for(squareX = increment + halfInc ; squareX <= lastCenter ; squareX += increment){
			//upper diamond (wrapped)
			_grid[squareX][0] = 
				(_grid[squareX-halfInc][0] +
				 _grid[squareX+halfInc][0] +
				 _grid[squareX][halfInc] +
				 _grid[squareX][lastCenter]) / 4
				+ generateRandomAmountMD(PRECISION, subdivisionLevel + 1);
			//copy point where relevant
			_grid[squareX][_nbPointsPerSide-1] = _grid[squareX][0];
			//left diamond (not wrapped)
			_grid[squareX-halfInc][halfInc] = 
				(_grid[squareX-halfInc][0] +
				 _grid[squareX-halfInc][increment] +
				 _grid[squareX][halfInc] +
				 _grid[squareX-increment][halfInc]) / 4
				+ generateRandomAmountMD(PRECISION, subdivisionLevel + 1);				
		}
		//now compute diamonds of the first column squares as it's there left diamonds that
		//are wrapped and we need to copy the points on the right side this time
		for(squareY = increment + halfInc ; squareY <= lastCenter ; squareY += increment){
			//upper diamond (not wrapped)
			_grid[halfInc][squareY-halfInc] =
				(_grid[increment][squareY-halfInc] +
				 _grid[halfInc][squareY] +
				 _grid[0][squareY-halfInc] +
				 _grid[halfInc][squareY-increment]) / 4
				+ generateRandomAmountMD(PRECISION, subdivisionLevel + 1);
			//left diamond (wrapped)
			_grid[0][squareY] =
				(_grid[0][squareY-halfInc] +
				 _grid[halfInc][squareY] +
				 _grid[0][squareY+halfInc] +
				 _grid[_nbPointsPerSide-halfInc][squareY]) / 4
				+ generateRandomAmountMD(PRECISION, subdivisionLevel + 1);
			//copy point where relevant
			_grid[_nbPointsPerSide-1][squareY] = _grid[0][squareY];
		}
		//and now compute points for diamonds of all other lines
		for(squareX = increment + halfInc ; squareX <= lastCenter ; squareX += increment){
			for(squareY = increment + halfInc ; squareY  <= lastCenter ; squareY += increment){
				//upper diamond
				_grid[squareX][squareY-halfInc] =
					(_grid[squareX][squareY] +
					 _grid[squareX-halfInc][squareY-halfInc] +
					 _grid[squareX][squareY-increment] +
					 _grid[squareX+halfInc][squareY-halfInc]) / 4
					+ generateRandomAmountMD(PRECISION, subdivisionLevel + 1);
				//left diamond
				_grid[squareX-halfInc][squareY] =
					(_grid[squareX][squareY] +
					 _grid[squareX-halfInc][squareY+halfInc] +
					 _grid[squareX-increment][squareY] +
					 _grid[squareX-halfInc][squareY-halfInc]) / 4
					+ generateRandomAmountMD(PRECISION, subdivisionLevel + 1);
			}
		}
		
		//check if we are at the last recursion before relaunching
		if(pow((float)2, (int)subdivisionLevel + 1)+1 < _nbPointsPerSide){
			squareStep(subdivisionLevel+1);
		}
	}
	
	inline float TerrainGenerator::generateRandomAmountMD(int invPrecision, int subdivisionLevel) {
		return (((myRand()%(_maxAltitude-_minAltitude) //generate a random number between 0 and max-min
					*invPrecision) //scale it to increase precision
						+_minAltitude*invPrecision) //add min so it is between precision*min and precision * max 
					/ pow((float)2.0, (float)subdivisionLevel + _hurst)) //divide it by 2^(level of subdivision + a decimal number
				/(float)invPrecision; //unscale it correctly
	}
	
	void TerrainGenerator::generatePerlinNoise(float persistence, unsigned int nbOctaves, unsigned int scaling){
		//create the noise generator object
		PerlinNoise generator(_nbPointsPerSide, persistence, nbOctaves, scaling);
		//generate octaves of noise
		generator.generateNoiseTables();
		//fill the regular grid with noise
		generator.fillGrid(_grid, _nbPointsPerSide);
	}
	
	
	
	/***********************************************************
	 * Relief modification : adding plains, mountains, craters *
	 **********************************************************/
	
	void TerrainGenerator::valleyGrid(float smoothingFactor, float altPercent){
		float gridMax = DBL_MIN;
		//get max of the grid
		for(unsigned int i = 0 ; i < _nbPointsPerSide ; ++i){
			for(unsigned int j = 0 ; j < _nbPointsPerSide ; ++j){
				gridMax = (_grid[i][j] > gridMax)?_grid[i][j]:gridMax;
			}
		}
		//compute the term of the polynomial function used to compute 
		//altitude of points above altPercent
		//it's a linear interpolation between the point at altPercent 
		//and the highest point
		float firstTerm = (pow(altPercent, smoothingFactor)-1)/(altPercent-1);
		float secondTerm = 1-firstTerm;
		float cutAltitude = altPercent*gridMax;
		//have everything smoothed
		for(unsigned int i = 0 ; i < _nbPointsPerSide ; ++i){
			for(unsigned int j = 0 ; j < _nbPointsPerSide ; ++j){
				//set value between 0 and 1 so we can use property
				//of the power function in this interval
				_grid[i][j] /= gridMax;
				if(_grid[i][j] < cutAltitude){
					_grid[i][j] = pow(_grid[i][j], smoothingFactor);			
				} else {
					_grid[i][j] *= firstTerm;
					_grid[i][j] += secondTerm;
				}
				_grid[i][j] *= gridMax;		
			}
		}
	}
	
	//this function does not produce the expected result (the theory is simply false)
	void TerrainGenerator::erodeMountains(float invSmoothingFactor, float altPercent){
		float gridMax = DBL_MIN;
		//get max of the grid
		for(unsigned int i = 0 ; i < _nbPointsPerSide ; ++i){
			for(unsigned int j = 0 ; j < _nbPointsPerSide ; ++j){
				gridMax = (_grid[i][j] > gridMax)?_grid[i][j]:gridMax;
			}
		}
		float firstTerm = pow(altPercent, 1/invSmoothingFactor - 1);
		float cutAltitude = altPercent*gridMax;
		//have everything smoothed
		for(unsigned int i = 0 ; i < _nbPointsPerSide ; ++i){
			for(unsigned int j = 0 ; j < _nbPointsPerSide ; ++j){
				_grid[i][j] /= gridMax;
				if(_grid[i][j] < cutAltitude){
					_grid[i][j] *= firstTerm;
				} else {
					_grid[i][j] = pow(_grid[i][j], 1/invSmoothingFactor);
				}
				_grid[i][j] *= gridMax;
			}
		}
	}
	
	//this function does not produce the desired result, need some modifications
	void TerrainGenerator::addValleyZone(float xCoord, float zCoord, float radius){
		//first compute bounding box of the valley
		int bbxMin = floor(xCoord - radius/_xDistanceBetweenPoints);
		int bbzMin = floor(zCoord - radius/_zDistanceBetweenPoints);
		int bbxMax = ceil(xCoord + radius/_xDistanceBetweenPoints);
		int bbzMax = ceil(zCoord + radius/_zDistanceBetweenPoints);
		//if we're not totally out of the grid
		if(bbxMin<(int)_nbPointsPerSide-1 && bbzMin<(int)_nbPointsPerSide-1 && bbxMax > 0 && bbzMax > 0){
			//truncate bounding box if partially out of the grid
			bbxMin = (bbxMin<0)?0:bbxMin;
			bbzMin = (bbzMin<0)?0:bbzMin;
			bbxMax = (bbxMax>_nbPointsPerSide-1)?_nbPointsPerSide-1:bbxMax;
			bbzMax = (bbzMax>_nbPointsPerSide-1)?_nbPointsPerSide-1:bbzMax;
			//get min of the grid
			float gridMin = DBL_MAX;
			for(unsigned int i = bbzMin ; i < bbzMax ; ++i){
				for(unsigned int j = bbxMin ; j < bbxMax ; ++j){
					gridMin = (_grid[i][j] < gridMin)?_grid[i][j]:gridMin;
				}
			}
			//now compute elevation for each point
			for(unsigned int numLine = bbzMin ; numLine <= bbzMax ; ++numLine){
				for(unsigned int numCol = bbxMin ; numCol <= bbxMax ; ++numCol){
					float distance = sqrt(pow((float)(xCoord-(int)numCol),2)+pow((float)(zCoord-(int)numLine),2));
					//if the point is really in the circle
					if(distance <= radius){
						//if point is in the inner circle 
						if(distance <= VALLEYSMOOTHING*radius){
							_grid[numLine][numCol] = gridMin;
						} else {
							float factor = ((radius-distance)/radius)*(1/(1-VALLEYSMOOTHING));
							_grid[numLine][numCol] -= (_grid[numLine][numCol] - gridMin)*(pow(factor, 5));
						}
					}
				}
			}
		}
	}
	
	//add a detail using some mathematical equation
	void TerrainGenerator::addMathematicalRelief(
			ReliefType type, float xCoord, float zCoord, float altitude, float radius){
		//first compute bounding box of the base of the mountain
		int bbxMin = floor((xCoord - radius)/_xDistanceBetweenPoints);
		int bbzMin = floor((zCoord - radius)/_zDistanceBetweenPoints);
		int bbxMax = ceil((xCoord + radius)/_xDistanceBetweenPoints);
		int bbzMax = ceil((zCoord + radius)/_zDistanceBetweenPoints);
		//if we're not totally out of the grid
		if(bbxMin<(int)_nbPointsPerSide-1 && bbzMin<(int)_nbPointsPerSide-1 && bbxMax > 0 && bbzMax > 0){
			//truncate bounding box if partially out of the grid
			bbxMin = (bbxMin<0)?0:bbxMin;
			bbzMin = (bbzMin<0)?0:bbzMin;
			bbxMax = (bbxMax>_nbPointsPerSide-1)?_nbPointsPerSide-1:bbxMax;
			bbzMax = (bbzMax>_nbPointsPerSide-1)?_nbPointsPerSide-1:bbzMax;
			//now compute elevation for each point
			for(unsigned int numLine = bbzMin ; numLine <= bbzMax ; ++numLine){
				for(unsigned int numCol = bbxMin ; numCol <= bbxMax ; ++numCol){
					switch(type){
					case MOUNTAIN:
						useMathematicalMountain(xCoord, zCoord, numCol, numLine, radius, altitude);
						break;
						
					case ERODED:
						useMathematicalErodedMountain(xCoord, zCoord, numCol, numLine, radius, altitude);
						break;
						
					case CRATER:
						useMathematicalCrater(xCoord, zCoord, numCol, numLine, radius, altitude);
						break;
					}
				}
			}
		}
	}
	
	//add a mountain using equation of the cone : y = (x^2+z^2)^0.5
	//xCoord and zCoord are the center of the base circle
	//take the value given by the equation (basically the distance 
	//between the origin and the computed point), scale it using 
	//the radius of the base circle so it is in [0,1], substract it to 1
	//(so points on the circle give 0 and center give 1) and multiply it by the altitude
	//so we have an inversed cone scaled in every dimensions to fit asked base circle
	//and given altitude
	inline void TerrainGenerator::useMathematicalMountain(
			float xCoord, float zCoord,
			unsigned int numCol, unsigned int numLine,
			float radius, float altitude){
		float distance = sqrt(pow(xCoord - (_xDistanceBetweenPoints*numCol), 2) +
							  pow(zCoord - (_zDistanceBetweenPoints*numLine), 2));
		if(distance <= radius){
			_grid[numLine][numCol] += (1-distance/radius) * altitude;
		}
	}
	
	//add a hill using equation of a parabola like : y = (x^2+y^2)
	//the value is square of the distance, so scale it by the square of the radius
	//and use the same technique than the cone to have an inverse parabola with a 
	//zero value on the base circle
	inline void TerrainGenerator::useMathematicalErodedMountain(
			float xCoord, float zCoord,
			unsigned int numCol, unsigned int numLine,
			float radius, float altitude){
		float distanceSquared = pow(xCoord-(_xDistanceBetweenPoints*numCol),2) + 
								pow(zCoord-(_zDistanceBetweenPoints*numLine),2);
		if(distanceSquared <= radius*radius){
			_grid[numLine][numCol] += (1-(distanceSquared/(radius*radius))) * altitude;
		}		
	}
	
	//add a crater using a modified version of the ripples equation : y = cos(x^2+y^2)
	//modified version y = (1-(cos(x^2+y^2)+1)/2)
	//x and z are scaled to have the function defined in [-2;2] because it isolate one ripple
	//and its is vertically translated by -0.82 to have the extreme border of the crater encounter the terrain
	inline void TerrainGenerator::useMathematicalCrater(
			float xCoord, float zCoord,
			unsigned int numCol, unsigned int numLine,
			float radius, float altitude){
		float distance = sqrt(pow((float)(xCoord-(int)numCol),2)+pow((float)(zCoord-(int)numLine),2));
		//if the point is really in the circle
		if(distance <= radius){
			float scaledX = 2*(numCol-xCoord)/radius;
			float scaledZ = 2*(numLine-zCoord)/radius;
			_grid[numLine][numCol] += ((1-(cos(scaledX*scaledX+scaledZ*scaledZ)+1)/2) - 0.82) * altitude;
		}		
	}
	
	//Experimental, don't produce a great effect
	//river starts at a given point, then follow the altitude and erode on its way
	void TerrainGenerator::addRiver(float xCoord, float zCoord){
		vector<pair<int, int> > riverWay;
		//store the coordinates of the starting point
		//floor(x+0.5) <=> round(x);
		riverWay.push_back(make_pair(floor(xCoord/_xDistanceBetweenPoints + 0.5),
									 floor(zCoord/_zDistanceBetweenPoints + 0.5)));
		bool continueEroding = true;
		//if there is a lower case in the grid neighbouring the one we are on
		while(continueEroding){
			//get the coordinates of the last point we tested
			int testingX = riverWay[riverWay.size()-1].first;
			int testingZ = riverWay[riverWay.size()-1].second;
			float minAlt = _grid[testingZ][testingX];
			//direction are numbered starting from x axis positive direction and going counter clockwise :
			//+x , +x-z, -z, -z-x, -x, -x+z, +z, +x+z
			int direction = -1;
			//now test each neighbour
			//warning : modulo not implemented, may crash near the border of the grid
			if(_grid[testingZ][testingX+1] < minAlt){
				direction = 0;
				minAlt = _grid[testingZ][testingX+1];
			}
			if(_grid[testingZ-1][testingX+1] < minAlt){
				direction = 1;
				minAlt = _grid[testingZ-1][testingX+1];
			}
			if(_grid[testingZ-1][testingX] < minAlt){
				direction = 2;
				minAlt = _grid[testingZ-1][testingX];
			}
			if(_grid[testingZ-1][testingX-1] < minAlt){
				direction = 3;
				minAlt = _grid[testingZ-1][testingX-1];
			}
			if(_grid[testingZ][testingX-1] < minAlt){
				direction = 4;
				minAlt = _grid[testingZ][testingX-1];
			}
			if(_grid[testingZ+1][testingX-1] < minAlt){
				direction = 5;
				minAlt = _grid[testingZ+1][testingX-1];
			}
			if(_grid[testingZ+1][testingX] < minAlt){
				direction = 6;
				minAlt = _grid[testingZ+1][testingX];
			}
			if(_grid[testingZ+1][testingX+1] < minAlt){
				direction = 7;
			}
			//now add the good pair in the "way"
			switch(direction){
			case -1:
				continueEroding = false;
				break;
				
			case 0:
				riverWay.push_back(make_pair(testingX+1, testingZ));
				break;
				
			case 1:
				riverWay.push_back(make_pair(testingX+1, testingZ-1));
				break;
				
			case 2:
				riverWay.push_back(make_pair(testingX, testingZ-1));
				break;
				
			case 3:
				riverWay.push_back(make_pair(testingX-1, testingZ-1));
				break;
				
			case 4:
				riverWay.push_back(make_pair(testingX-1, testingZ));
				break;
				
			case 5:
				riverWay.push_back(make_pair(testingX-1, testingZ+1));
				break;
				
			case 6:
				riverWay.push_back(make_pair(testingX, testingZ+1));
				break;
				
			case 7:
				riverWay.push_back(make_pair(testingX+1, testingZ+1));
				break;
			}
		}
		//now update the grid
		for(unsigned int i = 0 ; i < riverWay.size() ; ++i){
			_grid[riverWay[i].second][riverWay[i].first] -= 20;
		}
	}
	
	
	
	/***********************************************************************************
	 * Triangulation : adding holes, and simple, using mask and using triangle methods *
	 **********************************************************************************/
	
	//add a hole
	void TerrainGenerator::addHole(float xm, float xM, float zm, float zM){
		_holes.push_back(Hole(xm, xM, zm, zM));
	}
	
	//get participation of a hole on a particular line of the grid
	bool TerrainGenerator::getHolesInLine(unsigned int numLine, vector<pair<int, int> >& holesInLine){
		bool foundHoles = false;
		//for each hole
		for(unsigned int i = 0 ; i < _holes.size() ; ++i){
			if(_holes[i].zMin/_zDistanceBetweenPoints <= numLine && _holes[i].zMax/_zDistanceBetweenPoints >= numLine){
				//the tested line pass through the hole, so add infos on the hole
				foundHoles = true;
				holesInLine.push_back(make_pair(
						(int)floor(_holes[i].xMin/_xDistanceBetweenPoints) - 1,
						(int)ceil(_holes[i].xMax/_xDistanceBetweenPoints) + 1));
			}
		}
		return foundHoles;
	}
	
	void TerrainGenerator::triangulateSimple(){
		_triangles.clear();
		for(unsigned int numLine = 0 ; numLine < _nbPointsPerSide - 1 ; ++numLine){
			vector<pair<int, int> > holesInLine;
			if(getHolesInLine(numLine, holesInLine)){
				//sort holes using function defined in the header
				sort(holesInLine.begin(), holesInLine.end(), CompareLineHoles());
				//now triangulate the part of grid between the left side and the first line hole
				//if hole is totally on the left of the map it's ok as getHolesInline put -1 in the pair
				for(unsigned int numCol = 0 ; numCol < holesInLine[0].first - 1 ; ++numCol){
					addTriangles(numCol, numLine);
				}
				//now triangulate between holes
				for(unsigned int numHole = 0 ; numHole < holesInLine.size() - 1 ; ++numHole){
					unsigned xMinSecondHole = holesInLine[numHole+1].first;
					for(unsigned int numCol = holesInLine[numHole].second ; numCol < xMinSecondHole-1 ; ++numCol){
						addTriangles(numCol, numLine);		
					}
				}
				//finally, triangulate between last hole and right side of the grid
				for(unsigned int numCol = holesInLine[holesInLine.size()-1].second ; numCol < _nbPointsPerSide-1 ; ++numCol){
					addTriangles(numCol, numLine);				
				}
			} else {//no holes detected, triangulate the entire line
				for(unsigned int numCol = 0 ; numCol < _nbPointsPerSide - 1 ; ++numCol){
					addTriangles(numCol, numLine);
				}
			}
		}
		_triangulated = true;
	}
	
	//use a mask to triangulate
	void TerrainGenerator::triangulateWithMask(){
		computeMask();
		_triangles.clear();
		for(unsigned int numLine = 0 ; numLine < _nbPointsPerSide - 1 ; ++numLine){
			for(unsigned int numCol = 0 ; numCol < _nbPointsPerSide - 1 ; ++numCol){
				//if there is no masked point for the actual square
				if(_mask[numLine][numCol] && _mask[numLine+1][numCol] && _mask[numLine][numCol+1] && _mask[numLine+1][numCol+1]){
					addTriangles(numCol, numLine);
				}
			}
		}
	}
	
	void TerrainGenerator::resetMask(){
		for(unsigned int i = 0 ; i < _nbPointsPerSide ; ++i){
			memset(_mask[i], 1, _nbPointsPerSide*sizeof(bool));			
		}
	}
	
	void TerrainGenerator::computeMask(){
		//first put the whole mask to true (true means vertex not in a hole)
		resetMask();
		//then for each hole, update the mask
		for(unsigned int i = 0 ; i < _holes.size() ; ++i){
			//as cells can't be placed on the border, no problem of overflow
			int xMax = (int)ceil(_holes[i].xMax/_xDistanceBetweenPoints + 1);
			int zMax = (int)ceil(_holes[i].zMax/_zDistanceBetweenPoints + 1);
			for(unsigned int numLine = floor(_holes[i].zMin/_zDistanceBetweenPoints - 1) ; numLine < zMax ; ++numLine){
				for(unsigned int numCol = floor(_holes[i].xMin/_xDistanceBetweenPoints - 1) ; numCol < xMax ; ++numCol){
					_mask[numLine][numCol] = false;
				}
			}
		}
	}
	
	void TerrainGenerator::triangulateTriangle(){
		//computeMask();
		_triangles.clear();

		char cmd[1024];
		sprintf(cmd,"-z -p -S");

		struct triangulateio in;
		struct triangulateio out;
		initStruct(in);
		initStruct(out);
		
		in.numberofpoints = _nbPointsPerSide*_nbPointsPerSide;
		in.numberofsegments = 4*(_nbPointsPerSide-1) + 4*_holes.size();
		
		in.pointlist = (REAL*)malloc(2*in.numberofpoints*sizeof(REAL));
		in.segmentlist = (int*)malloc(2*in.numberofsegments*sizeof(int));
		
		if(_holes.size() > 0){
			in.holelist = (REAL*)malloc(2*_holes.size()*sizeof(REAL));
			in.numberofholes = _holes.size();
		}
		
		REAL* pdata = in.pointlist;
		
		//register all points of the grid
		for(unsigned int numLine = 0 ; numLine < _nbPointsPerSide ; ++numLine){
			for(unsigned int numCol = 0 ; numCol < _nbPointsPerSide ; ++numCol){
				*pdata++ = numCol * _xDistanceBetweenPoints;
				*pdata++ = numLine * _zDistanceBetweenPoints;
			}
		}
		
		int* sdata = in.segmentlist;
		//register exterior edge segments
		//upper edge
		for(unsigned int numCol = 0 ; numCol < _nbPointsPerSide-1 ; ++numCol){
			*sdata++ = numCol;
			*sdata++ = numCol+1;
		}
		//right edge
		for(unsigned int numLine = 0 ; numLine < _nbPointsPerSide-1 ; ++numLine){
			*sdata++ = _nbPointsPerSide*numLine + _nbPointsPerSide-1;
			*sdata++ = _nbPointsPerSide*(numLine+1) + _nbPointsPerSide-1;
		}
		//lower edge
		for(unsigned int numCol = 0 ; numCol < _nbPointsPerSide-1 ; ++numCol){
			*sdata++ = (_nbPointsPerSide-1)*_nbPointsPerSide + numCol;
			*sdata++ = (_nbPointsPerSide-1)*_nbPointsPerSide + numCol + 1;
		}
		//left edge
		for(unsigned int numLine = 0 ; numLine < _nbPointsPerSide-1 ; ++numLine){
			*sdata++ = numLine*_nbPointsPerSide;
			*sdata++ = (numLine+1)*_nbPointsPerSide;
		}
		
		//now the holes
		REAL* hdata = in.holelist;
		for(unsigned int i = 0 ; i < in.numberofholes ; ++i){
			//register segments of the bounding box
			unsigned int xMin = floor(_holes[i].xMin/_xDistanceBetweenPoints-1);
			unsigned int zMin = floor(_holes[i].zMin/_zDistanceBetweenPoints-1);
			unsigned int xMax = ceil(_holes[i].xMax/_xDistanceBetweenPoints+1);
			unsigned int zMax = ceil(_holes[i].zMax/_zDistanceBetweenPoints+1);
			//upper segment
			*sdata++ = zMin*_nbPointsPerSide + xMin;
			*sdata++ = zMin*_nbPointsPerSide + xMax;
			//right segment
			*sdata++ = zMin*_nbPointsPerSide + xMax;
			*sdata++ = zMax*_nbPointsPerSide + xMax;
			//lower segment
			*sdata++ = zMax*_nbPointsPerSide + xMax;
			*sdata++ = zMax*_nbPointsPerSide + xMin;
			//left segment
			*sdata++ = zMax*_nbPointsPerSide + xMin;
			*sdata++ = zMin*_nbPointsPerSide + xMin;
			
			//register a point in the hole
			*hdata++ = (xMin+xMax)/2.0;
			*hdata++ = (zMin+zMax)/2.0;
		}
		
		//launch triangulation
		triangulate(cmd, &in, &out, NULL);
		
		//register triangles
		for(unsigned int k = 0; k < out.numberoftriangles; k++){
			/*if(out.trianglelist[k*out.numberofcorners] >= count + externPolyline.size() || out.trianglelist[k*out.numberofcorners+1] >= count + externPolyline.size() || out.trianglelist[k*out.numberofcorners+2] >= count + externPolyline.size())
				cout << "erreur d'indice interne" << endl;*/
			_triangles.push_back(Triangle(out.trianglelist[k*out.numberofcorners],
					out.trianglelist[k*out.numberofcorners+1],
					out.trianglelist[k*out.numberofcorners+2]));
		}

		freeStruct(in);
		//warning : out.holelist is simply a copy of in.holelist
		//in.holelist as been freed by the previous freeStruct and set to NULL
		//but out.holelist always point to the memory location, and the freeStruct
		//will try to free the memory two times, leading to a memory corruption
		//so set out.holelist to NULL now as we know the memory has been freed
		out.holelist = NULL;
		freeStruct(out);
	}

	void TerrainGenerator::initStruct(struct triangulateio& tio)
	{
		tio.pointlist                  = NULL;
		tio.pointattributelist         = NULL;
		tio.pointmarkerlist            = NULL;
		tio.numberofpoints             = 0;
		tio.numberofpointattributes    = 0;

		tio.trianglelist               = NULL;
		tio.triangleattributelist      = NULL;
		tio.trianglearealist           = NULL;
		tio.neighborlist               = NULL;
		tio.numberoftriangles          = 0;
		tio.numberofcorners            = 0;
		tio.numberoftriangleattributes = 0;

		tio.segmentlist                = NULL;
		tio.segmentmarkerlist          = NULL;
		tio.numberofsegments           = 0;

		tio.holelist                   = NULL;
		tio.numberofholes              = 0;

		tio.regionlist                 = NULL;
		tio.numberofregions            = 0;

		tio.edgelist                   = NULL;
		tio.edgemarkerlist             = NULL;
		tio.normlist                   = NULL;
		tio.numberofedges              = 0;
		tio.pointlist                  = NULL;
		tio.pointattributelist         = NULL;
		tio.pointmarkerlist            = NULL;
		tio.numberofpoints             = 0;
		tio.numberofpointattributes    = 0;

	}

	void TerrainGenerator::freeStruct(struct triangulateio& tio)
	{
		if(tio.pointlist != NULL){
			free(tio.pointlist);
			tio.pointlist = NULL;
		}
		if(tio.pointattributelist != NULL){
			free(tio.pointattributelist);
			tio.pointattributelist = NULL;
		}
		if(tio.pointmarkerlist != NULL){
			free(tio.pointmarkerlist);
			tio.pointmarkerlist = NULL;
		}
		if(tio.trianglelist != NULL){
			free(tio.trianglelist);
			tio.trianglelist = NULL;
		}
		if(tio.triangleattributelist != NULL){
			free(tio.triangleattributelist);
			tio.triangleattributelist = NULL;
		}
		if(tio.trianglearealist != NULL){
			free(tio.trianglearealist);
			tio.trianglearealist = NULL;
		}
		if(tio.neighborlist != NULL){
			free(tio.neighborlist);
			tio.neighborlist = NULL;
		}
		if(tio.segmentlist != NULL){
			free(tio.segmentlist);
			tio.segmentlist = NULL;
		}
		if(tio.segmentmarkerlist != NULL){
			free(tio.segmentmarkerlist);
			tio.segmentmarkerlist = NULL;
		}
		if(tio.holelist != NULL){
			free(tio.holelist);
			tio.holelist = NULL;
		}
		if(tio.regionlist != NULL){
			free(tio.regionlist);
			tio.regionlist = NULL;
		}
		if(tio.edgelist != NULL){
			free(tio.edgelist);
			tio.edgelist = NULL;
		}
		if(tio.edgemarkerlist != NULL){
			free(tio.edgemarkerlist);
			tio.edgemarkerlist = NULL;
		}
		if(tio.normlist != NULL){
			free(tio.normlist);
			tio.normlist = NULL;
		}
	}
	
	void TerrainGenerator::addTriangles(int xUpperLeftCorner, int zUpperLeftCorner){
		_triangles.push_back(Triangle(
				zUpperLeftCorner*_nbPointsPerSide+xUpperLeftCorner,
				(zUpperLeftCorner+1)*_nbPointsPerSide+xUpperLeftCorner,
				zUpperLeftCorner*_nbPointsPerSide+xUpperLeftCorner+1));
		_triangles.push_back(Triangle(
				zUpperLeftCorner*_nbPointsPerSide+xUpperLeftCorner+1,
				(zUpperLeftCorner+1)*_nbPointsPerSide+xUpperLeftCorner,
				(zUpperLeftCorner+1)*_nbPointsPerSide+xUpperLeftCorner+1));		
	}
	
	
	
	/**********************************************************************
	 * Mesh management : assigning vertices, triangles and exporting mesh *
	 *********************************************************************/
	
	void TerrainGenerator::prepareMesh(const Ogre::String& name){
		createMesh(name);
		assignVerticesToMesh();
		assignTrianglesToMesh();
		assignMaterialToMesh(name);
	}
	
	void TerrainGenerator::createMesh(const Ogre::String& name){
		//Ask Ogre to create a pointer for us
	    _meshTerrain = MeshManager::getSingleton().createManual(name, "solipsis");		
		
	    //We create a submesh associated to the parent mesh
	    _meshTerrain->createSubMesh("solipsisTerrainSubMesh");
	}
	
	void TerrainGenerator::assignMaterialToMesh(const Ogre::String& name)
	{
		//Ask Ogre to create a material
		MaterialPtr material = MaterialManager::getSingleton().create(name, "solipsis");
		
		//Start with a fresh one
		material->removeAllTechniques();
		
		//Create a technique
		Technique* tech = material->createTechnique();
		
		//Create a pass
		tech->removeAllPasses();
		Pass* pass = tech->createPass();

		//No backface culling for this pass
		pass->setCullingMode(CULL_NONE);
		pass->setManualCullingMode(MANUAL_CULL_NONE);
		
		pass->setAmbient(0,0,0);
		
	    _meshTerrain->getSubMesh(0)->setMaterialName(name);
	}

	void TerrainGenerator::assignVerticesToMesh(){
	    //A bounding box for culling
	    AxisAlignedBox aabox;
	    
		unsigned int numCol, numLine;
	    
	    //We must create the vertex data, indicating how many vertices there will be
		unsigned int nbVertices = _nbPointsPerSide*_nbPointsPerSide;
		
		//get the submesh
		SubMesh* submesh = _meshTerrain->getSubMesh(0);
		
		//Submeshes won't share informations between them
	    submesh->useSharedVertices = false;
	    
	    //Create the Vertex data
	    submesh->vertexData = new VertexData();
	    submesh->vertexData->vertexStart = 0;
	    submesh->vertexData->vertexCount = nbVertices;
	    
	    //We must now declare what the vertex data contains
	    VertexDeclaration* declaration = submesh->vertexData->vertexDeclaration;
	    static const unsigned short source = 0;
	    size_t offset = 0;
	    
	    //Tell we want to have a position for each vertex
	    offset += declaration->addElement(source,offset,VET_FLOAT3,VES_POSITION).getSize();
	    offset += declaration->addElement(source, offset, VET_FLOAT3, VES_NORMAL).getSize();
	    
	    //Create a vertex buffer
	    HardwareVertexBufferSharedPtr vbuffer =
	    HardwareBufferManager::getSingletonPtr()->createVertexBuffer(declaration->getVertexSize(source), // == offset
	                                 submesh->vertexData->vertexCount,   // == nbVertices
	                                 HardwareBuffer::HBU_STATIC_WRITE_ONLY,
	                                 true);
	    
	    //Lock the buffer for writing
	    float* vdata = static_cast<float*>(vbuffer->lock(HardwareBuffer::HBL_DISCARD));

	    float xJitterValue = (_xDistanceBetweenPoints/2)*0.9;
	    float zJitterValue = (_zDistanceBetweenPoints/2)*0.9;
	    
	    // No we get access to the buffer to fill it.  During so we record the bounding box.
	    for (numLine=0;numLine<_nbPointsPerSide;++numLine) {
	    	for(numCol=0 ; numCol < _nbPointsPerSide ; ++numCol){
		        // Position
		        Vector3 position(_xDistanceBetweenPoints*numCol + xJitterValue*(((myRand()%100)-50)/50.0), 
		        		_grid[numLine][numCol], 
		        		_zDistanceBetweenPoints*numLine + zJitterValue*(((myRand()%100)-50)/50.0));
		        *vdata++ = position.x;
		        *vdata++ = position.y;
		        *vdata++ = position.z;
		        //Update the bounding box
		        aabox.merge(position);
		        //compute normal
		        //normal between the computed vertex and the point above
		        float n0[3] = {_grid[numLine][(numCol+_nbPointsPerSide-1)%_nbPointsPerSide] - _grid[numLine][numCol],
		        		1,
		        		_grid[(numLine+_nbPointsPerSide-1)%_nbPointsPerSide][numCol]};
		        //normal between the computed vertex and the point on the left
		        float n1[3] = {_grid[numLine][(numCol+_nbPointsPerSide-1)%_nbPointsPerSide] - _grid[numLine][numCol],
		        		1,
		        		_grid[numLine][numCol] - _grid[(numLine+1)%_nbPointsPerSide][numCol]};
		        //normal between the computed vertex and the point below
		        float n2[3] = {_grid[numLine][numCol] - _grid[numLine][(numCol+1)%_nbPointsPerSide],
		        		1,
		        		_grid[numLine][numCol] - _grid[(numLine+1)%_nbPointsPerSide][numCol]};
		        //normal between the computed vertex and the point on the right
		        float n3[3] = {_grid[numLine][numCol] - _grid[numLine][(numCol+1)%_nbPointsPerSide],
		        		1,
		        		_grid[(numLine+_nbPointsPerSide-1)%_nbPointsPerSide][numCol] - _grid[numLine][numCol]};
		        //compute the length of each normal
		        float ln0 = sqrt(n0[0]*n0[0] + 1 + n0[2]*n0[2]);
		        float ln1 = sqrt(n1[0]*n1[0] + 1 + n1[2]*n1[2]);
		        float ln2 = sqrt(n2[0]*n2[0] + 1 + n2[2]*n2[2]);
		        float ln3 = sqrt(n3[0]*n3[0] + 1 + n3[2]*n3[2]);
		        //normalize each normal
		        n0[1] /= ln0; n0[2] /= ln0;
		        n1[1] /= ln1; n1[0] /= ln1;
		        n2[1] /= ln2; n2[2] /= ln2;
		        n3[1] /= ln3; n3[0] /= ln3;
		        //create a normal for the middle point
		        float normal[3] = {n0[0] + n1[0] + n2[0] + n3[0],
		        		4,
		        		n0[2] + n1[2] + n2[2] + n3[2]};
		        //compute its length
		        float lnormal = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
		        //normalize
		        normal[0] /= lnormal; normal[1] /= lnormal; normal[2] /= lnormal;
		        //add in buffer
		        *vdata++ = normal[0];
		        *vdata++ = normal[1];
		        *vdata++ = normal[2];
	    	}
	    }
	    
	    //Release the buffer
	    vbuffer->unlock();
	    
	    //Set the buffer binding
	    submesh->vertexData->vertexBufferBinding->setBinding(source,vbuffer);
	    
	    //We must indicate the bounding box
	    _meshTerrain->_setBounds(aabox);
	    _meshTerrain->_setBoundingSphereRadius((aabox.getMaximum()-aabox.getMinimum()).length()/2.0);
	}
	
	void TerrainGenerator::assignTrianglesToMesh(){
		SubMesh* submesh = _meshTerrain->getSubMesh(0);
	    
		unsigned int nbFaces = _triangles.size();
		
	    // Creates the index data
	    submesh->indexData->indexStart = 0;
	    submesh->indexData->indexCount = nbFaces*3;
	    submesh->indexData->indexBuffer =
	    	HardwareBufferManager::getSingletonPtr()->createIndexBuffer(HardwareIndexBuffer::IT_32BIT,
	                                submesh->indexData->indexCount,
	                                HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	    
	    //Lock the buffer
	    uint32* idata = static_cast<uint32*>(submesh->indexData->indexBuffer->lock(HardwareBuffer::HBL_DISCARD));
	    
	    //Fill it
	    for (unsigned int numFace = 0 ; numFace < nbFaces ; ++numFace){
	    	*idata++ = _triangles[numFace].index0;
	    	*idata++ = _triangles[numFace].index1;
	    	*idata++ = _triangles[numFace].index2;
	    }
	    
	    //And unlock it
	    submesh->indexData->indexBuffer->unlock();
	}
	
	void TerrainGenerator::exportMesh(){
	    
	    //Create a material for the mesh
		
		//Ask Ogre to create a material
		MaterialPtr material = MaterialManager::getSingleton().create("solipsisTerrainMaterial", "solipsis");
		
		//Start with a fresh one
		material->removeAllTechniques();
		
		//Create a technique
		Technique* tech = material->createTechnique();
		
		//Create a pass
		tech->removeAllPasses();
		Pass* pass = tech->createPass();

		//No backface culling for this pass
		pass->setCullingMode(CULL_NONE);
		pass->setManualCullingMode(MANUAL_CULL_NONE);
		
		pass->setAmbient(0,0,0);
		
	    _meshTerrain->getSubMesh(0)->setMaterialName("solipsisTerrainMaterial");
		
		//Prepare the exporter
		MaterialSerializer matSerializer;
		
		//Finally save the material
		matSerializer.exportMaterial(material, "solipsisTerrainMaterial.material");

		//Create an exporter
		MeshSerializer serializer;
		
		//Finally save the mesh
		serializer.exportMesh(_meshTerrain.get(), "solipsisTerrain.mesh");
	}
	
	inline int TerrainGenerator::myRand(){
		return (((_randx=_randx*1103515245 + 12345)>>16) & 077777);
	}

} //namespace TerrainGenerator
