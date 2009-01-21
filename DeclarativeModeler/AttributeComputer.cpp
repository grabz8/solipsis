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

#include "AttributeComputer.h"

AttributeComputer::AttributeComputer(std::vector<Actor*> & actors, std::vector<Link*> & links)
: _actors(actors) , _links(links)
{ }

AttributeComputer::~AttributeComputer()
{ }

int AttributeComputer::compute()
{
  // creation de lien factices pour eviter les intersections entre les objets non contraints... / ajout d'un lien factice s'il n'existe aucun lien entre les 2
  for(unsigned int i=0 ; i<_actors.size() ; i++)
    for(unsigned int j=i+1 ; j<_actors.size() ; j++)
      if(!existeLink(_actors[i], _actors[j]))
        _links.push_back(new Link("nocollision",_actors[i], _actors[j]));

  // TODO affichage
  for(unsigned int i=0 ; i<_links.size() ; i++)
    std::cout << *_links[i]<< std::endl;

  if(_links.size()!=0)
    {
      // on décompose les contraintes selon 3 types : 
      std::vector<Link *> sizeConstraints; // 1. Contraintes de tailles 
      std::vector<Link *> positionConstraints; // 2. Contraintes de position
      std::vector<Link *> colorConstraints; // 3. Contraintes sur les couleurs

      sortConstraints(colorConstraints, positionConstraints, sizeConstraints);

      // option du solveur : Recherche l'ecart type minimum / ou médian
      int MODE_MIN = 0;
      int MODE_MED = 1;

      if(sizeConstraints.size()!=0)
        {
          std::cout <<  "-----------------------   CONTRAINTES SUR LES TAILLES  ---------------------------" << std::endl;

          // Ecriture des contraintes
          ConstraintWriter cw(_actors, sizeConstraints, 0);

          // le solveur a besoin du nombre de variables...
          unsigned int nbSizeVariables = cw.getVariableNames().size();

          // l'ensemble des contraintes et des noms de variables
          std::vector<Constraint *> contraintes = cw.getConstraintVector();
          std::vector<std::string> variableNames= cw.getVariableNames();

          // le solveur
          Solver s = Solver(nbSizeVariables, variableNames, contraintes, MODE_MED, 1);

          // retourne -1 si pas de solution -> les contraintes de taille ne peuvent etre résolues
          if(s.run()<0)
            return -3;

          // parcours de l'ensemble des acteurs pour mettre a jour les valeurs calculées
          for(unsigned int j=0 ; j<variableNames.size() ; j++)
            {
              int value = s.getValue(j);
              std::string variableName = variableNames[j];

              for(unsigned int k=0 ; k<_actors.size() ; k++)
                {	
                  if(variableName.find(_actors[k]->name())!=std::string::npos)
                    {
                      if(variableName.find("scX")!=std::string::npos)
                        _actors[k]->setScale(0,value);
                      else if(variableName.find("scY")!=std::string::npos)
                        _actors[k]->setScale(1,value);
                      else if(variableName.find("scZ")!=std::string::npos)
                        _actors[k]->setScale(2,value);
                    }
                }
            }
        }

      if(positionConstraints.size()!=0)
        {
          std::cout <<  "-----------------------   CONTRAINTES SUR LES POSITIONS  ---------------------------" << std::endl;
          // Ecriture des contraintes
          ConstraintWriter cw = ConstraintWriter(_actors, positionConstraints, 1);

          // le solveur a besoin du nombre de variables...
          unsigned int nbGeometricVariables = cw.getVariableNames().size();

          // l'ensemble des contraintes et des noms de variables
          std::vector<Constraint *> contraintes = cw.getConstraintVector();
          std::vector<std::string> variableNames= cw.getVariableNames();

          // le solveur
          Solver s = Solver(nbGeometricVariables, variableNames, contraintes, MODE_MIN, 2);

          // retourne -1 si pas de solution
          if(s.run()<0)
            return -2;

          // parcours de l'ensemble des acteurs pour mettre a jour les valeurs calculées
          for(unsigned int j=0 ; j<variableNames.size() ; j++)
            {
              int value = s.getValue(j);
              std::string variableName = variableNames[j];

              for(unsigned int k=0 ; k<_actors.size() ; k++)
                {	
                  if(variableName.find(_actors[k]->name())!=std::string::npos)
                    {
                      if(variableName.find("poX")!=std::string::npos)
                        _actors[k]->setPosition(0,value);
                      else if(variableName.find("poY")!=std::string::npos)
                        _actors[k]->setPosition(1,value);
                      else if(variableName.find("poZ")!=std::string::npos)
                        _actors[k]->setPosition(2,value);
                    }
                }
            }
        }

      // parcours pour orienter les objets qui interviennent dans les relations du type A "is facing" B
      for(unsigned int i=0 ; i<_links.size() ; i++)
        {
          // si la relation est 'facing'
          if(_links[i]->getLink().compare("facing")==0)
            {
              Actor * a1(_links[i]->getFirst());
              Actor * a2(_links[i]->getSecond());

              Vec3i posi1(a1->getPosition());
              Vec3i posi2(a2->getPosition());
              Vec3i direction(posi2-posi1);

              Vec3d dir(direction.x() , direction.y() , direction.z() );
              dir.normalize();

              // Z est l'avant de l'objet -> s'il y a "is facing, on annule toutes les autres rotations calculées jusqu'alors...
              Vec3d dirZ(Vec3d(0.0,0.0,1.0));
              dirZ.normalize();

              // le vecteur de la rotation et les angles
              Vec3d rotationVector = (dir^dirZ) ;
              // on regarde si les vecteurs dir et dirZ ne sont pas colinéaires (rotation d'angle PI)
              if(rotationVector==Vec3d() )
                {
                  if(dirZ!=Vec3d(0,1,0))
                    rotationVector = Vec3d(0,1,0);
                  else
                    rotationVector = Vec3d(1,0,0);
                }

              double cosangle = cos(acos(dir * dirZ)/2);
              double sinangle = sin(acos(dir * dirZ)/2);

              // création d'un 'quaternion' pour définir la rotation
              double qx = rotationVector.x() * sinangle;
              double qy = rotationVector.y() * sinangle;
              double qz = rotationVector.z() * sinangle;
              double qw = cosangle;

              double quatNorme = sqrt(qx*qx + qy*qy + qz*qz + qw*qw);
              // normalisation du quaternion
              qx /= quatNorme; qy /= quatNorme; qz /= quatNorme; qw /= quatNorme;

              Matrixd rotationMatrix(qx,qy,qz,qw);

              Matrixd finalMatrix =  rotationMatrix;
              a1->setRotation(finalMatrix);
              Vec3d finalRotation = finalMatrix * Vec3d(1,1,1);
              a1->setRotation(finalRotation);

              // TODO mettre le vecteur rotation a jour... (est-ce important !?)
            }
        }

      // si il y a des contraintes sur les couleurs : on les traite...
      if(colorConstraints.size()!=0)
        {
          std::cout <<  "-----------------------   CONTRAINTES SUR LES COULEURS  ---------------------------" << std::endl;
          ConstraintWriter colorCW(_actors, colorConstraints, 2);
          std::vector<std::string> colorVariableNames = colorCW.getVariableNames();
          std::vector<Constraint*> colorVariableConstraints = colorCW.getConstraintVector();
          unsigned int nbColorVariables = colorCW.getVariableNames().size();


          Solver s = Solver(nbColorVariables, colorVariableNames, colorVariableConstraints, MODE_MED, 3);

          if(s.run()<0)
            return -1;

          // mise a jour des résultats obtenus pour les couleurs
          for(unsigned int i=0 ; i<colorVariableNames.size() ; i++)
            {
              int value = s.getValue(i);
              std::string variableName = colorVariableNames[i];

              for(unsigned int j=0 ; j<_actors.size() ; j++)
                {
                  if(variableName.find(_actors[j]->name())!=std::string::npos)
                    {
                      if(variableName.find("R")!=std::string::npos)
                        _actors[j]->setColor(0,value);
                      else if(variableName.find("G")!=std::string::npos)
                        _actors[j]->setColor(1,value);
                      else if(variableName.find("B")!=std::string::npos)
                        _actors[j]->setColor(2,value);
                      else if(variableName.find("A")!=std::string::npos)
                        _actors[j]->setColor(3,value);
                    }
                }
            }
        }
    }

  // affichage des résultats
  std::cout << "Les résultats --------------------------------------------------" << std::endl;
  for(unsigned int i=0 ; i<_actors.size() ; i++)
    {
      std::cout << "- "<<_actors[i]->name()<< std::endl;

      Vec3<int> posi(_actors[i]->getFinalPosition());
      std::cout << "   Position = "<< posi << std::endl;

      Vec3d v(_actors[i]->getFinalSize());
      std::cout << "   Taille = "<<  v<< std::endl;

      Matrixd rot( _actors[i]->getRotationMatrix());
      std::cout << "   Matrice de rotation = \n"<<  rot<< std::endl;

      std::cout << "   Couleur = "<< _actors[i]->getColor() << std::endl;
    }

  return 0;
}

bool AttributeComputer::existeLink(const Actor * a1, const Actor * a2)
{
  for(unsigned int i=0 ; i<_links.size() ; i++)
    {
      Link * l = _links[i];
      if( ((l->getFirst()==a1 && l->getSecond()==a2) || (l->getFirst()==a2 && l->getSecond()==a1) )&& 
          (l->getLink().compare("to_the_right_of")==0 || l->getLink().compare("on_the_right_of")==0 || l->getLink().compare("next_to")==0 ||
              l->getLink().compare("on_the_left_of")==0 || l->getLink().compare("to_the_left_of")==0 ||
              l->getLink().compare("behind")==0 || l->getLink().compare("in_front_of")==0 ||
              l->getLink().compare("under")==0 || l->getLink().compare("on")==0 || l->getLink().compare("over")==0) )
        return true;
    }
  return false;
}

void AttributeComputer::sortConstraints(std::vector<Link*> & colorLinks, std::vector<Link*> & positionLinks, std::vector<Link*> & sizeLinks)
{
  for(unsigned int i=0 ; i<_links.size() ; i++)
    {
      Link * l = _links[i];
      if(l->getLink().compare("lighter")==0 || l->getLink().compare("darker")==0 || 
          l->getLink().compare("redder")==0 || l->getLink().compare("bluer")==0 || l->getLink().compare("greener")==0)
        colorLinks.push_back(l);
      else if(l->getLink().compare("to_the_right_of")==0 || l->getLink().compare("on_the_right_of")==0 || l->getLink().compare("next_to")==0 ||
          l->getLink().compare("on_the_left_of")==0 || l->getLink().compare("to_the_left_of")==0 ||
          l->getLink().compare("behind")==0 || l->getLink().compare("in_front_of")==0 ||
          l->getLink().compare("under")==0 || l->getLink().compare("on")==0 || 
          l->getLink().compare("over")==0 || l->getLink().compare("nocollision")==0)
        positionLinks.push_back(l);
      else 
        sizeLinks.push_back(l);
    }
}
