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

#ifndef _PERLINNOISE_H_
#define _PERLINNOISE_H_

namespace tege {

//class used to generate Perlin Noise and fill the regular grid
class PerlinNoise {
public:
	PerlinNoise();
	
	/*
	 * \param generatedGridSize number of points on the side of the regular grid
	 * \param persistence persistence to use on the noise, see tutorial on wiki
	 * \param nbOfOctaves number of level of noise
	 * \param amplitudeScale amplitude of the noise
	 */
	PerlinNoise(int generatedGridSize, double persistence, unsigned int nbOfOctaves, double amplitudeScale);
	~PerlinNoise();
	
	/*
	 * Fill octaves of noise
	 */
	void generateNoiseTables();
	
	/*
	 * fill the regular grid with noise
	 */
	void fillGrid(float** grid, unsigned int numberOfPoints);
	
private:
	/*
	 * private random number generator
	 */
	int myRand();
	
	/*
	 * generate one point for noise table
	 */
	double generateNoise(int noiseSeed);
	
	/*
	 * free memory used by the noise tables
	 */
	void freeNoiseTables();
	
	/*
	 * allocate memory for the noise tables
	 */
	void allocateNoiseTables();
	
	/* 
	 * use bilinear interpolation to compute value of noise
	 */ 
	double interpolateBilinearNoise(unsigned int octaveNumber, unsigned int numLine, unsigned int numCol, unsigned int numberOfPoints);
	
	/* 
	 * use bicubic interpolation to compute value of noise
	 * don't work right now
	 */
	double interpolateBicubicNoise(unsigned int octaveNumber, unsigned int numLine, unsigned int numCol, unsigned int numberOfPoints);
	
	int _randx;
	float _persistence;
	int _nbOfOctaves;
	double*** _noiseTables;
	bool _noiseTablesNeedReallocation;
	double _amplitudeScale;
};

};

#endif /* _PERLINNOISE_H_*/
