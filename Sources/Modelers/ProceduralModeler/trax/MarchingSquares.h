/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author DELMONT Romuald

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

#ifndef _MARCHING_SQUARES_H_
#define _MARCHING_SQUARES_H_ 1

#include <vector>

#include <Linear/Linear.h>

namespace pm_trax {

/**
 * This class implements the marching squares algorithm in order to fetch curves of equal values in a bitmap field.
 */

class MarchingSquares {

public:

  /**
   * This structure defines a point with its attributes in the marching square algorithm.
   */
  typedef struct {
	    Vec2< double > pt;
	    double value;
	    long cell1, cell2;
  } Point;

  /**
   * This structure defines a level curve.
   */
  typedef std::vector< Point > Level;

public:
  
	/**
   * Constructor
   * @param size The size (width,height) of the data.
   * @param pts The points. pts's size MUST BE size[0]*size[1]
   * @param val The points' values. val's size MUST BE size[0]*size[1]
   */
	MarchingSquares( const Vec2<long>& size, const std::vector<Vec2<double> >& pts, const std::vector<double>& val );

  /**
   * Destructor
   */
  ~MarchingSquares ();

  /**
   * Computes the level curves for one value.
   * Fetch the result after calling this method with getResult.
   * @param value The level's value you want.
   * @see getResult
   */
  void compute( double value );

  /**
   * Returns wether the data is supposed to be 'jointive' at the cut pixel.
   * @ return true if data is considered as cylindrical.
   */
  bool isCylindrical() { return( cutPixel == -1 ); };

  /**
   * Indicates that the data is cylindrical (eg. the first and last columns are neighbors).
   * @param cyl The cylindrical value.
   */
  void setCylindrical( bool cyl );

  /**
   * Allows to set the value
   * @param pixel The cut pixel's value.
   */
  void setCutPixel( long pixel ) { cutPixel = pixel; };

  /**
   * Fetches the levels from the last computation.
   * You should call compute before that.
   * @return The level curves.
   */
  std::vector< Level > getResult() { return result; };

protected:

  typedef struct {
	long pt[4];
  } SquareCell;

  void interpolate( double value1, double value2, const Vec2< double >& pt1, const Vec2< double >& pt2, double threshold, Vec2< double >& inter );

  void generatePoints( double threshold );

  void buildLines();

private:

	std::vector< Point > mpoints;

	std::vector< SquareCell > cells;

	Vec2< long > size;

	std::vector< Vec2< double > > points;

	std::vector< double > values;

	std::vector< Level > result;

	long cutPixel;
   
}; // MarchingSquares

}; // pm_trax

#endif // _MARCHING_SQUARES_H_
