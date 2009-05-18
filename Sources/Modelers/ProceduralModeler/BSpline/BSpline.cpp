/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author HUIBAN Vincent

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

#include <math.h>

#include <algorithm>

#include "BSpline.h"

BSpline::BSpline()
{
  controlPointVec       = NULL;
  calculControlPointVec = new vector<Vec3<double>  >;
  allPointVec           = new vector<Vec3<double>  >;
  value                 = 0;
  smoothness            = 1.0;
  closed                = false;
}

BSpline::BSpline(const vector<Vec3<double>  >& _controlPointVec, 
				 double _smoothness, 
				 bool _closed) 
{
  controlPointVec       = new vector<Vec3<double>  >(_controlPointVec);
  calculControlPointVec = new vector<Vec3<double>  >;
  allPointVec           = new vector<Vec3<double>  >;
  value                 = 0;
  smoothness            = _smoothness;
  closed                = _closed;
}

BSpline::~BSpline() 
{
  delete controlPointVec;
  delete calculControlPointVec;
  delete allPointVec;
}

void BSpline::setControlPoints(const vector<Vec3<double>  >& _controlPointVec)
{
  if(controlPointVec)
    delete controlPointVec;
  controlPointVec = new vector<Vec3<double>  >(_controlPointVec);
}

vector<Vec3<double>  > *BSpline::getControlPoints()
{
  return controlPointVec;
}

vector<Vec3<double>  > *BSpline::getAllPoints()
{
  return allPointVec;
}

Vec3<double>  *BSpline::getAllPoint(int num)
{
  return(&(*allPointVec)[num]);
}

Vec3<double>  *BSpline::getControlPoint(int num)
{
  return(&(*controlPointVec)[(num) >= 0 ? (num) : 0]);
}

int BSpline::getControlPointNum()
{
  return controlPointVec->size();
}

int BSpline::getAllPointNum()
{
  return allPointVec->size();
}

void BSpline::addControlPoint(const Vec3<double> & controlPoint)
{
  if(!controlPointVec)
    controlPointVec = new vector<Vec3<double>  >;
  controlPointVec->push_back(controlPoint);
}

bool BSpline::removeControlPoint(const Vec3<double>  & controlPoint)
{
  vector<Vec3<double>  >::iterator it = find(controlPointVec->begin(), 
					     controlPointVec->end(), 
					     controlPoint);
  if(it != controlPointVec->end()){
    controlPointVec->erase(it);
    return true;
  }
  return false;
}

void BSpline::setSmoothness(double _smoothness)
{
  smoothness = _smoothness;
}

double BSpline::getSmoothness()
{
  return smoothness;
}

void BSpline::setClosed(bool _closed)
{
  closed = _closed;
}

bool BSpline::getClosed()
{
  return closed;
}

void BSpline::setValue(int _value)
{
  value = _value;
}

int BSpline::getValue()
{
  return value;
}

void BSpline::setColor(const Vec3<double> & _color)
{
  color.setValue(_color[0], _color[1], _color[2]);
}

Vec3<double>  BSpline::getColor()
{
  return color;
}

void BSpline::setSection(double _section)
{
  section = _section;
}

double BSpline::getSection()
{
  return section;
}

void BSpline::setThroughEndPoints(bool _throughEndPoints)
{
  throughEndPoints = _throughEndPoints;
}

bool BSpline::getThroughEndPoints()
{
  return throughEndPoints;
}

double BSpline::length()
{
  double backLength = 0.0;
  int pNum = allPointVec->size();
  for(int i = 0; i < pNum-1; i++){
    backLength += ((*allPointVec)[i+1]-(*allPointVec)[i]).length(); 
  }
  return backLength;
}

void BSpline::prepareControlPoints()
{
	if( controlPointVec->size() < 2 ) {
		return;
	}

  int pointNum = controlPointVec->size();
  for(int i = 0; i < pointNum; i++)
    calculControlPointVec->push_back((*controlPointVec)[i]);

  if(getClosed()){
    calculControlPointVec->push_back((*controlPointVec)[0]);
    calculControlPointVec->push_back((*controlPointVec)[1]);    
//     calculControlPointVec->push_back((*controlPointVec)[2]);      
//     calculControlPointVec->push_back((*controlPointVec)[3]);      
    calculControlPointVec->insert(calculControlPointVec->begin(), (*controlPointVec)[pointNum-1]);
  }
  else{
    if(!getThroughEndPoints()){
      calculControlPointVec->push_back((*controlPointVec)[controlPointVec->size()-1]);
      calculControlPointVec->insert(calculControlPointVec->begin(), (*controlPointVec)[0]);
    }
    else{
      calculControlPointVec->push_back((*controlPointVec)[controlPointVec->size()-1]);
      calculControlPointVec->push_back((*controlPointVec)[controlPointVec->size()-1]);
      calculControlPointVec->insert(calculControlPointVec->begin(), (*controlPointVec)[0]);
      calculControlPointVec->insert(calculControlPointVec->begin(), (*controlPointVec)[0]);
    } 
  }
}

///////////////////////////////////////////////////////////////////////////
// Compute the spline coefficients
// x = f(u) (, y = f(u), z = f(u))
///////////////////////////////////////////////////////////////////////////

vector<Vec3<double>  > *BSpline::compute()
{ 

  if(calculControlPointVec){
    calculControlPointVec->clear();
    delete calculControlPointVec;
  }
  calculControlPointVec = new vector<Vec3<double>  >;

  if(allPointVec){
    allPointVec->clear();
    delete allPointVec;
  }
  allPointVec = new vector<Vec3<double>  >;

  int i;
  prepareControlPoints();
  int pNum = calculControlPointVec->size();

  if( pNum < 2 )
	  return allPointVec;

  double x = 0.0, y = 0.0, z = 0.0;

  if(getClosed() || !getThroughEndPoints()){
    for(i = 1; i < pNum-2; i++){
      // because of precision: 0.999 and not 1.0
      for(double u = 0.0; u < 0.999; u += smoothness){
	x = (pow(1-u, 3)/6)*(*calculControlPointVec)[i-1][0] + 
	  (3*(pow(u, 3))-6*(pow(u, 2))+4)/6*(*calculControlPointVec)[i][0] +
	  (-3*(pow(u, 3))+3*(pow(u, 2))+3*u+1)/6*(*calculControlPointVec)[i+1][0] +
	  (pow(u, 3)/6)*(*calculControlPointVec)[i+2][0];
	y = (pow(1-u, 3)/6)*(*calculControlPointVec)[i-1][1] + 
	  (3*(pow(u, 3))-6*(pow(u, 2))+4)/6*(*calculControlPointVec)[i][1] +
	  (-3*(pow(u, 3))+3*(pow(u, 2))+3*u+1)/6*(*calculControlPointVec)[i+1][1] +
	  (pow(u, 3)/6)*(*calculControlPointVec)[i+2][1];  
	z = (pow(1-u, 3)/6)*(*calculControlPointVec)[i-1][2] + 
	  (3*(pow(u, 3))-6*(pow(u, 2))+4)/6*(*calculControlPointVec)[i][2] +
	  (-3*(pow(u, 3))+3*(pow(u, 2))+3*u+1)/6*(*calculControlPointVec)[i+1][2] +
	  (pow(u, 3)/6)*(*calculControlPointVec)[i+2][2];  
	Vec3<double>  point(x, y, z);
	allPointVec->push_back(point); 
      }
    }
  }
  else{
    allPointVec->push_back((*calculControlPointVec)[0]);
    for(i = 2; i < pNum-3; i++){
      // because of precision: 0.999 and not 1.0
      for(double u = 0.0; u < 0.999; u += smoothness){
	x = (pow(1-u, 3)/6)*(*calculControlPointVec)[i-1][0] + 
	  (3*(pow(u, 3))-6*(pow(u, 2))+4)/6*(*calculControlPointVec)[i][0] +
	  (-3*(pow(u, 3))+3*(pow(u, 2))+3*u+1)/6*(*calculControlPointVec)[i+1][0] +
	  (pow(u, 3)/6)*(*calculControlPointVec)[i+2][0];
	y = (pow(1-u, 3)/6)*(*calculControlPointVec)[i-1][1] + 
	  (3*(pow(u, 3))-6*(pow(u, 2))+4)/6*(*calculControlPointVec)[i][1] +
	  (-3*(pow(u, 3))+3*(pow(u, 2))+3*u+1)/6*(*calculControlPointVec)[i+1][1] +
	  (pow(u, 3)/6)*(*calculControlPointVec)[i+2][1];  
	z = (pow(1-u, 3)/6)*(*calculControlPointVec)[i-1][2] + 
	  (3*(pow(u, 3))-6*(pow(u, 2))+4)/6*(*calculControlPointVec)[i][2] +
	  (-3*(pow(u, 3))+3*(pow(u, 2))+3*u+1)/6*(*calculControlPointVec)[i+1][2] +
	  (pow(u, 3)/6)*(*calculControlPointVec)[i+2][2];  
	Vec3<double> point(x, y, z);
	allPointVec->push_back(point); 
      }
    }
    allPointVec->push_back((*calculControlPointVec)[calculControlPointVec->size()-1]);
  }
  return allPointVec;
} 
