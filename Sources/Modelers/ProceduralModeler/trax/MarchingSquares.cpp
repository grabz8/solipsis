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

#include <algorithm>

#include "MarchingSquares.h"

pm_trax::MarchingSquares::MarchingSquares( const Vec2< long >& size, const std::vector< Vec2< double > >& pts, const std::vector< double >& val ) 
{
  this->size = size;
  points = pts;
  values = val;
  cutPixel = -1;
  cells.insert( cells.end(), size[0]*size[1], SquareCell() );
}

pm_trax::MarchingSquares::~MarchingSquares()
{}

void pm_trax::MarchingSquares::setCylindrical( bool cyl ) 
{
  cutPixel = cyl ? -1 : size[0] - 1;
}

void pm_trax::MarchingSquares::compute( double value )
{
  mpoints.clear();

  // Initialize Cell Data
  for (unsigned int i = 0; i < cells.size(); i++) {
    cells[i].pt[0] = -1;
    cells[i].pt[1] = -1;
    cells[i].pt[2] = -1;
    cells[i].pt[3] = -1;
  }

  generatePoints( value );
  buildLines();
}  

void pm_trax::MarchingSquares::interpolate (double value1, double value2, const Vec2< double >& pt1, const Vec2< double >& pt2, double threshold, Vec2< double >& inter ) 
{
  double t = (threshold - value1) / (value2 - value1);
  inter = pt1 + t*(pt2-pt1);
}

void pm_trax::MarchingSquares::generatePoints( double threshold ) 
{
  // For each segment between 2 points, check values & create isoline point if necessary
  long nextXCell, prevYCell, currentCell = 0;
  long i;
  Vec2< double > nextPt;

  long nbCell = size[0] * size[1];

  // Avoid equality to avoid problems
  for (i = 0; i < nbCell; i++)
    if (values[i] == threshold)
      values[i] -= 0.01;

  long lastLine = size[1] - 1;
  long lastPixel = size[0] - 1;
  long firstCut;
  
  if (cutPixel == -1)
    firstCut = -1;
  else
    firstCut = (cutPixel + 1) % size[0];

  // For each row
  for (long y = 0; y < size[1]; y++, currentCell = size[0]*y) {
    // For each pixel
    for (long x = 0; x < size[0]; x++, currentCell++) {
      // Segment between x & x+1
      if (x != cutPixel) { // Forget pixel if non cylindrical
	if (x == lastPixel) {
	  nextXCell = size[0]*y;
	  nextPt = points[currentCell];
	  nextPt[0] += points[currentCell][0] - points[currentCell-1][0];
	} else {
	  nextXCell = currentCell+1;
	  nextPt = points[nextXCell];
	}

	if (
	    !((values[currentCell] < threshold && values[nextXCell] < threshold) ||
	      (values[currentCell] > threshold && values[nextXCell] > threshold))
) {
	  // Create Point	    
	  Point newPt;
	  interpolate(values[currentCell], values[nextXCell], points[currentCell], nextPt, threshold, newPt.pt);
	  
	  if (y == 0) {
	    cells[currentCell].pt[0] = mpoints.size();
	    newPt.cell1 = currentCell;
	    newPt.cell2 = -1;
	  } else {
	    if (y == lastLine) {
	      cells[currentCell-size[0]].pt[2] = mpoints.size();
	      newPt.cell1 = currentCell-size[0];
	      newPt.cell2 = -1;
	    } else {
	      cells[currentCell].pt[0] = mpoints.size();
	      cells[currentCell-size[0]].pt[2] = mpoints.size();
	      newPt.cell1 = currentCell;
	      newPt.cell2 = currentCell - size[0];
	    }
	  }
	  
	  mpoints.push_back(newPt);

	}
      }
      
      // Segment between y && y+1
      if (y != lastLine) {    

	if (!((values[currentCell] < threshold && values[currentCell+size[0]] < threshold) ||
	      (values[currentCell] > threshold && values[currentCell+size[0]] > threshold))
	   ) {
	  // Create Point
	  Point newPt;
	  interpolate(values[currentCell], values[currentCell+size[0]], points[currentCell], points[currentCell+size[0]], threshold, newPt.pt);
	  
	  if (x == 0)
	    prevYCell = ((y+1) * size[0]) - 1;
	  else
	    prevYCell = currentCell - 1;
	 
	  if (x == firstCut) {
	    cells[currentCell].pt[3] = mpoints.size();
	    newPt.cell1 = currentCell;
	    newPt.cell2 = -1;
	  } else {
	    if (x == cutPixel) {
	      cells[prevYCell].pt[1] = mpoints.size();
	      newPt.cell1 = prevYCell;
	      newPt.cell2 = -1;
	    } else {
	      cells[currentCell].pt[3] = mpoints.size();
	      cells[prevYCell].pt[1] = mpoints.size();
	      newPt.cell1 = currentCell;
	      newPt.cell2 = prevYCell;
	    }
	  }
	  
	  mpoints.push_back(newPt);
	}
      }
    }
  }
}

void pm_trax::MarchingSquares::buildLines () 
{
  
  long currentCell = -1;
  long currentPoint = -1; 
  long startingPoint = -1, startingCell = -1;
  
  long nbPt = mpoints.size();

  std::vector< bool > used( nbPt, false ); 

  result.clear();

  for (int i = 0; i < nbPt; i++) {
    if (!used[i]) {
      // Starting a new Line
      Level line;

      // to be able to restart when getting end of line
      startingPoint = i;      
      startingCell = mpoints[i].cell1;      

      currentCell = -1;
      currentPoint = i;

      do {
// 	cout << "\rPoints #" << line.size() << flush;
	if (currentPoint != -1) { // Fin de la ligne, le test suivant va s'occuper de ce cas
	  line.push_back(mpoints[currentPoint]);
	  used[currentPoint] = true;
	
	  // Look in cell for the next point
	  if (currentCell != mpoints[currentPoint].cell1)
	    currentCell = mpoints[currentPoint].cell1;
	  else
	    currentCell = mpoints[currentPoint].cell2;
	}

	if (currentPoint == -1 || currentCell == -1) { // End of line on this side
	  currentPoint = -1;
	  
	  // Try reverse
	  if (startingPoint != -1) {
	    currentPoint = startingPoint;
	    currentCell = startingCell;
	    startingPoint = -1;
	    
	    std::reverse(line.begin(), line.end());
	  }
	} else {
	  // Continue line
	  int j = 0;
	  while (j < 4 && cells[currentCell].pt[j] != currentPoint)
	    j++;
	  
	  if (j == 4)
	    // End of line (border of image)
	    currentPoint = -1;
	  else {	  
	    int k; 
	    // Cherche le point suivant
	    // Si la cellule a 4 points, j'associe arbitrairement 0-1 et 2-3
	    // Si il n'y en a pas 4 je cherche le suivant 
	    if (cells[currentCell].pt[0] != -1 && 
		cells[currentCell].pt[1] != -1 &&	
		cells[currentCell].pt[2] != -1 &&
		cells[currentCell].pt[3] != -1) {
	      // Le point courant est un nombre impair (1 ou 3) je prends donc 0 ou 2 ( -1)
	      if (j%2)
		k = j-1;
	      else
		k = j+1;
	    } else {
	      k = (j+1) % 4;
	      while (k != j && cells[currentCell].pt[k] == -1) 
		k = (k+1) % 4;
	    }

	    // Si k == j, je suis dans le cas ou je n'ai qu'un point dans cette cellule 
	    // c'est donc une cellule du bord -> fin de la ligne (ca ne doit pas exister);
	    if (k == j)
	      currentPoint = -1;
	    else
	      currentPoint = cells[currentCell].pt[k];
	  }
	}
      } while (currentPoint != startingPoint && line.size()<mpoints.size());
      
      // La fin de la ligne n'es tpas un bord de l'image donc c'est une boucle
      if (currentPoint != -1) 
	line.push_back(mpoints[currentPoint]);

      result.push_back(line);
    }
  }
}
