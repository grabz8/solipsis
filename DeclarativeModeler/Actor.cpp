/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author Sebastien LAIGRE

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

#include "Actor.h"

Actor::Actor(const std::string & n)
:_name(n), _singular(true), _scale(100,100,100), _positionConstraint(false), _sizeConstraint(false), _colorConstraint(false), _indice(-1)
{
	// génération d'une taille aléatoire (pour simuler le chargement d'un objet)
	// on multiplie par 1000 pour passer en millimetres (car Gecode ne prend que des entiers !)
	int j1 = 2+ 20.0*rand()/(RAND_MAX+1.0) *1000;
	int j3 = 2+  20.0*rand()/(RAND_MAX+1.0) *1000;
	int j2 = 2+  20.0*rand()/(RAND_MAX+1.0)*1000;
	_size = Vec3i(j1,j2,j3);

	std::cout << name()<<" : dimensions =" << _size<< std::endl;

	// matrice de rotation
	_rotationMatrix = Matrixd(Vec3d());

	// génération d'une couleur aléatoire
	int r = rand()/(RAND_MAX+1.0) * 255;
	int g = rand()/(RAND_MAX+1.0) * 255;
	int b = rand()/(RAND_MAX+1.0) * 255;
	_color = Vec4(r,g,b,255);
}

Actor::~Actor()
{ }

std::string Actor::name()
{
	std::string name = _name +" (";
	for(unsigned int i=0 ; i<_attributes.size() ; i++)
		name += _attributes[i] + ", ";
	name += ")";
	name += " ";
	name += toString(_indice);

	return name;
}

bool Actor::containsAttribute(const std::string & s)
{
	for(unsigned int i=0 ; i<_attributes.size() ; i++)
	{
		if(s.compare(_attributes[i])==0)
			return true;
	}
	return false;
}

bool Actor::operator == (const Actor & a)
{
	Actor & aa = const_cast<Actor&>(a);
	bool nameEquals = (_name.compare(aa._name)==0);
	bool attributesEqual = true;

	for(unsigned int i=0 ; i<aa.getAttributes().size() ; i++)
		attributesEqual = attributesEqual && containsAttribute(aa.getAttributes()[i]);

	return nameEquals&&attributesEqual;
}


void Actor::setPosition(int XYouZ, int value)
{
	if(XYouZ==0)
		_position.setX(value);
	else if(XYouZ==1)
		_position.setY(value);
	else if(XYouZ==2)
		_position.setZ(value);
}

void Actor::setScale(int XYouZ, int value)
{
	if(XYouZ==0)
		_scale.setX(value);
	else if(XYouZ==1)
		_scale.setY(value);
	else if(XYouZ==2)
		_scale.setZ(value);
}

void Actor::setAttribute(const std::string & s)
{ 
	if(!isColorConstraint())
	{
		if(s.compare("red")==0)
			_color = Vec4(200,0,0,255);
		else if(s.compare("green")==0)
			_color = Vec4(0,200,0,255);
		else if(s.compare("blue")==0)
			_color = Vec4(0,0,200,255);
		else if(s.compare("yellow")==0)
			_color = Vec4(255,255,0,255);
		else if(s.compare("white")==0)
			_color = Vec4(255,255,255,255);
		else if(s.compare("grey")==0)
			_color = Vec4(127,127,127,255);
		else if(s.compare("black")==0)
			_color = Vec4(0,0,0,255);
		else if(s.compare("transparent")==0)
			_color = Vec4(0,0,0,127);
	}

	if(!isSizeConstraint())
	{
		// les dimensions
		if(s.compare("tiny")==0)
			_scale = Vec3i(25, 25, 25);
		else if(s.compare("small")==0)
			_scale = Vec3i(50,50,50);
		else if(s.compare("big")==0)
			_scale = Vec3i(150,150,150);
		else if(s.compare("huge")==0)
			_scale = Vec3i(200,200,200);
		else if(s.compare("enormous")==0)
			_scale = Vec3i(400,400,400);
		else if(s.compare("humongous")==0)
			_scale = Vec3i(800,800,800);
		else if(s.compare("tall")==0)
			_scale = Vec3i(100,200,100);
		else if(s.compare("wide")==0 || s.compare("large")==0)
			_scale = Vec3i(200,100,100);
		else if(s.find("_")!=std::string::npos)
			treatParticularAttribute(s);
		else if(s.compare("deep")==0)
			_scale = Vec3i(100,100,200);
	}

	if(!isRotationConstraint())
	{
		// les dimensions
		if(s.compare("facing_left")==0 )//|| s.compare("facing_west")==0)
			_rotationDegree = _rotationDegree + Vec3i(0, 90, 0);
		else if(s.compare("facing_right")==0 )//|| s.compare("facing_east")==0)
			_rotationDegree = _rotationDegree + Vec3i(0,270,0);
		else if(s.compare("facing_up")==0 )//|| s.compare("facing_north")==0)
			_rotationDegree = _rotationDegree + Vec3i(270,0,0);
		else if(s.compare("facing_down")==0 )//|| s.compare("facing_south")==0)
			_rotationDegree = _rotationDegree + Vec3i(90,0,0);
		else if(s.compare("facing_back")==0)
			_rotationDegree = _rotationDegree + Vec3i(0,180,0);
		else
			_rotationDegree = _rotationDegree + Vec3i(0,0,0);

		_rotation = DegreeToRadian(_rotationDegree);
		_rotationMatrix = Matrixd(_rotation);

		Vec3d res = _rotationMatrix * Vec3d(_size.x(), _size.y(), _size.z());
		_size.setX(res.x());
		_size.setY(res.y());
		_size.setZ(res.z());
	}

	_attributes.push_back(s); 
}

void Actor::setColor(int RGBouA, int value)
{
	if(RGBouA==0)
		_color.setR(value);
	else if(RGBouA==1)
		_color.setG(value);
	else if(RGBouA==2)
		_color.setB(value);
	else if(RGBouA==3)
		_color.setA(value);
}

//void Actor::affectAttribute()
//{
//	for(unsigned int i=0 ; i<_attributes.size() ; i++)
//	{
//		// les couleurs
//		if(_attributes[i].compare("red")==0)
//			_color = Vec4(128,0,0,255);
//		else if(_attributes[i].compare("green")==0)
//			_color = Vec4(128,0,0,255);
//		else if(_attributes[i].compare("blue")==0)
//			_color = Vec4(0,0,128,255);
//		else if(_attributes[i].compare("yellow")==0)
//			_color = Vec4(255,255,0,255);
//		else if(_attributes[i].compare("white")==0)
//			_color = Vec4(0,0,128,255);
//		else if(_attributes[i].compare("grey")==0)
//			_color = Vec4(128,128,128,255);
//		else if(_attributes[i].compare("black")==0)
//			_color = Vec4(0,0,0,255);
//		else if(_attributes[i].compare("transparent")==0)
//			_color = Vec4(0,0,0,128);
//
//		// les dimensions
//		else if(_attributes[i].compare("tiny")==0)
//			_scale = Vec3i(25, 25, 25);
//		else if(_attributes[i].compare("small")==0)
//			_scale = Vec3i(50,50,50);
//		else if(_attributes[i].compare("big")==0)
//			_scale = Vec3i(150,150,150);
//		else if(_attributes[i].compare("huge")==0)
//			_scale = Vec3i(200,200,200);
//		else if(_attributes[i].compare("enormous")==0)
//			_scale = Vec3i(400,400,400);
//		else if(_attributes[i].compare("humongous")==0)
//			_scale = Vec3i(800,800,800);
//		else if(_attributes[i].compare("tall")==0)
//			_scale = Vec3i(100,200,100);
//		else if(_attributes[i].compare("wide")==0 || _attributes[i].compare("large")==0)
//			_scale = Vec3i(200,100,100);
//		else if(_attributes[i].compare("deep")==0)
//			_scale = Vec3i(100,100,200);
//		else 		
//			// sinon c'est un attribut composé
//			if(_attributes[i].find("_")!=std::string::npos)
//				treatParticularAttribute(_attributes[i]);
//			else if(_attributes[i].compare("deep")==0)
//				_scale = Vec3i(100,100,100);
//	}
//}

void Actor::treatParticularAttribute(const std::string & s)
{
	// l'attribut entier : ex 2_feet_tall
	std::string attribute = s;
	std::string tmpString = attribute;

	// acces au '2'
	int indice = tmpString.find('_');
	std::string val = tmpString.substr(0,indice);
	tmpString = tmpString.substr(indice+1);

	// acces au 'feet'
	indice = tmpString.find('_');
	std::string unit = tmpString.substr(0,indice);
	tmpString = tmpString.substr(indice+1);

	// acces au 'tall'
	indice = tmpString.find('_');
	std::string attr = tmpString.substr(indice+1);

	double value=0;
	if(from_string(val, value))
	{
		double v(value);
		double coeff(0.0);
		double c(0.0);

		// calcul du facteur de conversion de mm en inches ou feet
		if(unit.compare("inch")==0 || unit.compare("inches")==0)
			c = 25.4;
		else if(unit.compare("foot")==0 || unit.compare("feet")==0)
			c = 304.8;

		if(attr.compare("tall")==0)
			coeff = c*v / _size.y();
		else if(attr.compare("wide")==0 || unit.compare("large")==0)
			coeff = c*v / _size.x();
		else if(attr.compare("deep")==0)
			coeff = c*v / _size.z();

		// un coeff de 100 signifie que la taille est la taille par défaut -> coeff = coeff*100
		coeff = coeff * 100; 

		// on prend coeff+1 car pour un coeff petit (<5), le cast en entier engendre d'énormes erreurs de precision...
		if(attr.compare("wide")==0)
			_scale = Vec3i((int)coeff+1,_scale.y(),_scale.z());
		else if(attr.compare("tall")==0)
			_scale = Vec3i(_scale.x(),(int)coeff+1,_scale.z());
		else if(attr.compare("deep")==0)
			_scale = Vec3i(_scale.x(),_scale.y(),(int)coeff+1);
	}
}

Vec3d Actor::getFinalSize()
{
	Vec3d resultat = Vec3d(_size.x()*_scale.x()/100.0 , _size.y()*_scale.y()/100.0 , _size.z()*_scale.z()/100.0 );
	return resultat;  
}

Vec3i Actor::getFinalPosition()
{  
	return Vec3i(_position.x()/100.0,_position.y()/100.0,_position.z()/100.0);  
}

int Actor::load()
{


	return 0;
}

// ----------------------------------------     operateurs     ------------------------------------------------------
bool Actor::operator != (const Actor & a)
{
	Actor & aa = const_cast<Actor&>(a);
	bool nameDiffers = (_name.compare(aa._name)!=0);
	bool attributesDifferent = false;

	for(unsigned int i=0 ; i<aa.getAttributes().size() ; i++)
		attributesDifferent = attributesDifferent || !containsAttribute(aa.getAttributes()[i]);

	return nameDiffers || attributesDifferent;
}

void Actor::operator = (const Actor & a)
{
	Actor & aa = const_cast<Actor&>(a);
	_name = aa._name;
	for(unsigned int i=0 ; i<aa.getAttributes().size() ; i++)
		_attributes.push_back(aa.getAttributes()[i]);
}

std::ostream& operator<<(std::ostream& os, Actor & a)
{
	os << a._name << " (" ;
	for(unsigned int i=0 ; i<a.getAttributes().size() ; i++)
		os << a.getAttributes()[i]<< ", ";
	os << ")";
	return os;
}
