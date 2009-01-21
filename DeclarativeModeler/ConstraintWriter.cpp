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

#include "ConstraintWriter.h"

ConstraintWriter::ConstraintWriter(std::vector<Actor *>& actors, std::vector<Link *>& ensRelation,  int variables)
:_ensRelation(ensRelation), _ensActor(actors)
{ 
  setConstraintedActors();

  if(variables==0)
    establishSizeConstraint();
  else if(variables==1)
    establishPositionConstraint();
  else if(variables==2)
    establishColorConstraint();
}

ConstraintWriter::~ConstraintWriter()
{ }

void ConstraintWriter::establishPositionConstraint()
{
  for(unsigned int k=0 ; k<_ensActor.size() ; k++)
    {
      // traitement des proriétés de chaque acteur
      Actor * a1 = _ensActor[k];

      std::string name1 = a1->name();

      // 9 variables de position  pour chaque acteur (position, scale, taille)
      std::string posi1X = name1+"poX";
      std::string posi1Y = name1+"poY";
      std::string posi1Z = name1+"poZ";

      std::string taille1X = name1+"siX";
      std::string taille1Y = name1+"siY";
      std::string taille1Z = name1+"siZ";

      // on empile les noms des variables
      _names.push_back(posi1X); _names.push_back(posi1Y); _names.push_back(posi1Z);
      _names.push_back(taille1X); _names.push_back(taille1Y); _names.push_back(taille1Z);

      // la taille est toujours constante
      Constraint * t1X = new Equal(taille1X, a1->getSize().x() * a1->getScale().x()/2);
      Constraint * t1Y = new Equal(taille1Y, a1->getSize().y() * a1->getScale().y()/2);
      Constraint * t1Z = new Equal(taille1Z, a1->getSize().z() * a1->getScale().z()/2);
      _constraintVector.push_back(t1X);_constraintVector.push_back(t1Y);_constraintVector.push_back(t1Z);

      for(unsigned int k=0 ; k<_ensRelation.size() ; k++)
        {
          Link* link = _ensRelation[k];
          if(a1== link->getFirst())
            {
              std::string relation = link->getLink();
              Actor * a2 = link->getSecond();

              std::string name2 = a2->name();

              // 12 variables pour chaque acteur (position, scale, taille, couleur)
              std::string posi2X = name2+"poX";
              std::string posi2Y = name2+"poY";
              std::string posi2Z = name2+"poZ";

              std::string taille2X = name2+"siX";
              std::string taille2Y = name2+"siY";
              std::string taille2Z = name2+"siZ";

              Vec3<std::string> t1(taille1X,taille1Y,taille1Z);
              Vec3<std::string> p1(posi1X,posi1Y,posi1Z);
              Vec3<std::string> t2(taille2X,taille2Y,taille2Z);
              Vec3<std::string> p2(posi2X,posi2Y,posi2Z);

              // objets ayant une contrainte de taille entre eux
              if(relation.compare("to_the_right_of")==0 || relation.compare("on_the_right_of")==0 || relation.compare("next_to")==0 ||
                  relation.compare("on_the_left_of")==0 || relation.compare("to_the_left_of")==0 ||
                  relation.compare("behind")==0 || relation.compare("in_front_of")==0 ||
                  relation.compare("under")==0 || relation.compare("on")==0 || relation.compare("over")==0)  // objets ayant une contrainte de positionnement entre eux
                {
                  Constraint * constraint = new CompPosition(p1,p2,t1,t2 , relation);
                  _constraintVector.push_back(constraint);
                }
              else  // pas de collision des objets non contraints entre eux
                {
                  Constraint * nocollide = new NoCollision(p1,p2,t1,t2);
                  _constraintVector.push_back(nocollide);
                }
            }
        }
    }
}

void ConstraintWriter::establishSizeConstraint()
{
  for(unsigned int k=0 ; k<_ensActor.size() ; k++)
    {
      // traitement des proriétés de chaque acteur
      Actor * a1 = _ensActor[k];

      std::string name1 = a1->name();

      // 6 variables de position  pour chaque acteur (position, scale, taille)
      std::string scale1X = name1+"scX";
      std::string scale1Y = name1+"scY";
      std::string scale1Z = name1+"scZ";

      std::string taille1X = name1+"siX";
      std::string taille1Y = name1+"siY";
      std::string taille1Z = name1+"siZ";

      // on empile les noms des variables
      _names.push_back(taille1X); _names.push_back(taille1Y); _names.push_back(taille1Z);
      _names.push_back(scale1X); _names.push_back(scale1Y); _names.push_back(scale1Z);

      // la taille est toujours constante
      Constraint * t1X = new Equal(taille1X, a1->getSize().x()/2);
      Constraint * t1Y = new Equal(taille1Y, a1->getSize().y()/2);
      Constraint * t1Z = new Equal(taille1Z, a1->getSize().z()/2);
      _constraintVector.push_back(t1X);_constraintVector.push_back(t1Y);_constraintVector.push_back(t1Z);

      // on regarde si le scale de A1 est contraint. sinon on fixe son scale a sa valeur qu'il avait avant
      if(!a1->isSizeConstraint())
        {
          Constraint * scaleX = new Equal(scale1X, a1->getScale().x());
          Constraint * scaleY = new Equal(scale1Y, a1->getScale().y());
          Constraint * scaleZ = new Equal(scale1Z, a1->getScale().z());
          _constraintVector.push_back(scaleX); _constraintVector.push_back(scaleY); _constraintVector.push_back(scaleZ);
        }

      for(unsigned int k=0 ; k<_ensRelation.size() ; k++)
        {
          Link* link = _ensRelation[k];
          if(a1== link->getFirst())
            {
              std::string relation = link->getLink();
              Actor * a2 = link->getSecond();

              std::string name2 = a2->name();

              // 6 variables pour chaque acteur (position, scale, taille, couleur)
              std::string scale2X = name2+"scX";
              std::string scale2Y = name2+"scY";
              std::string scale2Z = name2+"scZ";

              std::string taille2X = name2+"siX";
              std::string taille2Y = name2+"siY";
              std::string taille2Z = name2+"siZ";

              Vec3<std::string> t1(taille1X,taille1Y,taille1Z);
              Vec3<std::string> s1(scale1X,scale1Y,scale1Z);
              Vec3<std::string> t2(taille2X,taille2Y,taille2Z);
              Vec3<std::string> s2(scale2X,scale2Y,scale2Z);

              // objets ayant une contrainte de taille entre eux
              if(relation.compare("bigger")==0 || relation.compare("smaller")==0 || relation.compare("taller")==0 || 
                  relation.compare("wider")==0 || relation.compare("larger")==0|| relation.compare("deeper")==0)
                {
                  Constraint * ct = new CompTailles(t1,t2,s1,s2, relation);
                  _constraintVector.push_back(ct);
                }
            }
        }
    }
}

void ConstraintWriter::establishColorConstraint()
{
  for(unsigned int i=0 ; i<_ensActor.size() ; i++)
    {
      // traitement des proriétés de chaque acteur
      Actor * a1 = _ensActor[i];

      std::string name1 = a1->name();

      // 4 variables pour chaque acteur (R,G,B et A)
      std::string R1 = name1+"R";
      std::string G1 = name1+"G";
      std::string B1 = name1+"B";
      std::string A1 = name1+"A";

      // on empile les noms des variables
      _names.push_back(R1); _names.push_back(G1); _names.push_back(B1);_names.push_back(A1);

      // on regarde si A1 est colorimétriquement contraint. s'il ne l'est pas, on fixe sa couleur
      if(!a1->isColorConstraint())
        {
          Constraint * R = new Equal(R1, a1->getColor().r());
          Constraint * G = new Equal(G1,a1->getColor().g());
          Constraint * B = new Equal(B1, a1->getColor().b());
          Constraint * A = new Equal(A1, a1->getColor().a());
          _constraintVector.push_back(R); _constraintVector.push_back(G); _constraintVector.push_back(B); _constraintVector.push_back(A);
        }

      for(unsigned int j=0 ; j<_ensRelation.size() ; j++)
        {
          Link* link = _ensRelation[j];
          if(a1== link->getFirst())
            {
              std::string relation = link->getLink();
              Actor * a2 = link->getSecond();

              std::string name2 = a2->name();

              std::string R2 = name2+"R";
              std::string G2 = name2+"G";
              std::string B2 = name2+"B";
              std::string A2 = name2+"A";

              if(relation.compare("lighter")==0 || relation.compare("darker")==0 || relation.compare("redder")==0 
                  || relation.compare("greener")==0 || relation.compare("bluer")==0)
                {
                  Constraint * constraint = new CompColor(R1,G1,B1,A1,R2,G2,B2,A2,relation);
                  _constraintVector.push_back(constraint);
                }
            }
        }
    }
}

void ConstraintWriter::setConstraintedActors()
{
  for(unsigned int i=0 ; i<_ensRelation.size() ; i++)
    {
      Actor * a1 = _ensRelation[i]->getFirst();
      Actor * a2 = _ensRelation[i]->getSecond();

      std::string r = _ensRelation[i]->getLink();

      if(r.compare("bigger")==0 || r.compare("smaller")==0 || r.compare("taller")==0 || r.compare("wider")==0 || r.compare("larger")==0 || r.compare("deeper")==0)
        a1->setSizeConstraint(true);

      if(r.compare("lighter")==0 || r.compare("darker")==0 || r.compare("redder")==0  || r.compare("bluer")==0  || r.compare("greener")==0)
        a1->setColorConstraint(true);

      if(r.compare("to_the_right_of")==0 || r.compare("on_the_right_of")==0 ||
          r.compare("on_the_left_of")==0 || r.compare("to_the_left_of")==0 ||
          r.compare("behind")==0 || r.compare("in_front_of")==0 ||
          r.compare("under")==0 || r.compare("on")==0 || r.compare("over")==0)
        {
          a1->setPositionConstraint(true);
        }

      if(r.compare("nocollision")==0)
        {
          a1->setPositionConstraint(true);
          a2->setPositionConstraint(true);
        }

      if(r.compare("facing")==0)
        a1->setRotationConstraint(true);

      // recherche si a2 intervient dans une formule qui contient une information de taille...
      for(unsigned int j=0 ; j<_ensRelation.size() ; j++)
        {
          if(*a2==*_ensRelation[i]->getFirst() && (_ensRelation[i]->getLink().compare("bigger")==0 || _ensRelation[i]->getLink().compare("smaller")==0 
              || _ensRelation[i]->getLink().compare("taller")==0 || _ensRelation[i]->getLink().compare("wider")==0 || _ensRelation[i]->getLink().compare("larger")==0 || _ensRelation[i]->getLink().compare("deeper")==0) )
            {
              a2->setSizeConstraint(true);
            }
        }
    }
}

bool ConstraintWriter::contains(std::string & name)
{
  for(unsigned int i=0 ; i<_names.size() ; i++)
    {
      if(name.compare(_names[i])==0)
        return true;
    }
  return false;
}
