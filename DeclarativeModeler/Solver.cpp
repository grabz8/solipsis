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

#include "Solver.h"

int  Solver::nl= 10;

Solver::Solver(unsigned int nbVariables, std::vector<std::string> & variableNames, std::vector<Constraint*> & contraintes, int BRANCHING_MODE, int mode)
: _le(this,nbVariables,0,999999999), _sizeConstraint(mode==1)
{
  Gecode::IntVarArgs values(0);
  if(_sizeConstraint)
    {
      _distance = Gecode::IntVar(this,0, 999999999);
      int nbconstraint = nbConstraintToOptimize(contraintes);
      values = Gecode::IntVarArgs(nbconstraint);
    }

  int cpt=0;
  std::map<std::string, Gecode::IntVar> variables;
  for(unsigned int i=0 ; i<variableNames.size() ; i++)
    {
      // si la variable est une taille on la recherche dans l'intervalle [1,100000]
      if(variableNames[i].find("scX")!=std::string::npos || variableNames[i].find("scY")!=std::string::npos || variableNames[i].find("scZ")!=std::string::npos)
        _le[i] = Gecode::IntVar(this, 1, 100000);

      // si la variable est une couleur on la recherche dans l'intervalle [0,255]
      if(variableNames[i].find("R")!=std::string::npos || variableNames[i].find("G")!=std::string::npos || 
          variableNames[i].find("B")!=std::string::npos || variableNames[i].find("A")!=std::string::npos)
        _le[i] = Gecode::IntVar(this, 0, 255);

      std::cout << variableNames[i]<< std::endl;
      variables.insert( std::pair<std::string,Gecode::IntVar>(variableNames[i],_le[i]) );
    }

  for(unsigned int i=0 ; i<contraintes.size() ; i++)
    {
      // plusieurs cas : 1.Egalité  /  2.Comparaison des tailles  /  3.Comparaison des positions / 4. Comparaison des couleurs

      // contrainte d'égalité
      if(dynamic_cast<Equal*>(contraintes[i])!=NULL)
        {
          Equal * eq = dynamic_cast<Equal*>(contraintes[i]);
          std::string var = eq->getVar();

          int val = eq->getVal();
          std::cout << "CONTRAINTE : " << var << " == " << val<< std::endl;
          post(this, variables[var] == val );
        }

      // contrainte des positions : 'sur', 'devant', 'derriere', ...
      else if(dynamic_cast<CompPosition*>(contraintes[i])!=NULL)
        {
          CompPosition * posi = dynamic_cast<CompPosition*>(contraintes[i]);
          std::string p1x = posi->getP1X(); std::string p2x = posi->getP2X();
          std::string p1y = posi->getP1Y(); std::string p2y = posi->getP2Y();
          std::string p1z = posi->getP1Z(); std::string p2z = posi->getP2Z();

          std::string t1x = posi->getT1X(); std::string t2x = posi->getT2X();
          std::string t1y = posi->getT1Y(); std::string t2y = posi->getT2Y();
          std::string t1z = posi->getT1Z(); std::string t2z = posi->getT2Z();

          std::cout << "posi->getRelation() = "<< posi->getRelation()<< std::endl;
          if(posi->getRelation().compare("over")==0)
            {
              // le Y du 1er est > au Y du 2eme
              post(this,   variables[p1y] >  Gecode::plus(this, variables[p2y] , Gecode::plus(this, variables[t1y] , variables[t2y]) ) );

              // a coté de X
              post(this,  Gecode::abs(this, Gecode::minus(this, variables[p1x] , variables[p2x]) ) < variables[t2x] );

              // a coté de Z
              post(this,  Gecode::abs(this, Gecode::minus(this, variables[p1z] , variables[p2z]) ) < variables[t2z] );
            }

          else if(posi->getRelation().compare("under")==0)
            {
              // le Y du 1er est < au Y du 2eme
              post(this,   variables[p1y]  < Gecode::minus(this, variables[p2y], Gecode::plus(this, variables[t1y],variables[t2y] ))  );

              // en face de X
              post(this,  Gecode::abs(this, Gecode::minus(this, variables[p1x] , variables[p2x]) ) < variables[t2x] );

              // en face de Z
              post(this,  Gecode::abs(this, Gecode::minus(this, variables[p1z] , variables[p2z]) ) < variables[t2z] );
            }

          if(posi->getRelation().compare("on")==0)
            {
              // le Y du 1er est = au Y du 2eme
              post(this,   variables[p1y] ==  Gecode::plus(this, variables[p2y] , Gecode::plus(this, variables[t1y] , variables[t2y]) ) );

              // a coté de X
              post(this,  Gecode::abs(this, Gecode::minus(this, variables[p1x] , variables[p2x]) ) < variables[t2x] );

              // a coté de Z
              post(this,  Gecode::abs(this, Gecode::minus(this, variables[p1z] , variables[p2z]) ) < variables[t2z] );
            }

          else if(posi->getRelation().compare("in_front_of")==0)
            {
              // le Z
              post(this,   variables[p1z]  > Gecode::plus(this, variables[p2z] , Gecode::plus(this, variables[t1z],variables[t2z] ))  );

              // les X et Y sont devant l'objet
              post(this,  Gecode::abs(this, Gecode::minus(this, variables[p1x] , variables[p2x]) ) < variables[t2x] );

              post(this,  Gecode::abs(this, Gecode::minus(this, variables[p1y] , variables[p2y]) ) < variables[t2y] );
            }
          if(posi->getRelation().compare("behind")==0)
            {
              // le Z
              post(this,   variables[p2z]  > Gecode::plus(this, variables[p1z] , Gecode::plus(this, variables[t1z],variables[t2z] ))  );

              // les X et Y sont derriere l'objet
              post(this,  Gecode::abs(this, Gecode::minus(this, variables[p1x] , variables[p2x]) ) < variables[t2x] );

              post(this,  Gecode::abs(this, Gecode::minus(this, variables[p1y] , variables[p2y]) ) < variables[t2y] );
            }
          else if(posi->getRelation().compare("on_the_left_of")==0 || posi->getRelation().compare("to_the_left_of")==0)
            {
              // le X
              post(this,   variables[p2x] > Gecode::plus(this, variables[p1x] , Gecode::plus(this, variables[t1x],variables[t2x] )) );

              // les Y et Z sont devant l'objet
              post(this,  Gecode::abs(this, Gecode::minus(this, variables[p1y] , variables[p2y]) ) < variables[t2y] );

              post(this,  Gecode::abs(this, Gecode::minus(this, variables[p1z] , variables[p2z]) ) < variables[t2z] );
            }

          else if(posi->getRelation().compare("on_the_right_of")==0 || posi->getRelation().compare("to_the_right_of")==0)
            {
              // le X
              post(this,   variables[p1x] > Gecode::plus(this, variables[p2x] , Gecode::plus(this, variables[t1x],variables[t2x] )) );

              // les Y et Z sont devant l'objet
              post(this,  Gecode::abs(this, Gecode::minus(this, variables[p1y] , variables[p2y]) ) < variables[t2y] );

              post(this,  Gecode::abs(this, Gecode::minus(this, variables[p1z] , variables[p2z])  ) < variables[t2z] );
            }
          else if(posi->getRelation().compare("next_to")==0)
            {
              // création d'un vecteur de 2 booléens
              Gecode::BoolVarArray b(this, 2, 0, 1);

              // b[0] = a1 a droite de a2, b[1] = a1 a gauche de a2  
              rel(this,  variables[p1x] , Gecode::IRT_GR, Gecode::plus(this, variables[p2x] , Gecode::plus(this, variables[t1x],variables[t2x] )), b[0] );
              rel(this,   variables[p2x] , Gecode::IRT_GR, Gecode::plus(this, variables[p1x] , Gecode::plus(this, variables[t1x],variables[t2x] )), b[1] );
              rel(this, Gecode::BOT_OR, b, 1);

              // les Y et Z sont devant l'objet
              post(this,  Gecode::abs(this, Gecode::minus(this, variables[p1y] , variables[p2y]) ) < variables[t2y] );
              post(this,  Gecode::abs(this, Gecode::minus(this, variables[p1z] , variables[p2z])  ) < variables[t2z] );
            }
        }

      // contrainte de taillle : 'plus grand que', 'plus petit que', etc.
      else if(dynamic_cast<CompTailles*>(contraintes[i])!=NULL)
        {
          CompTailles * posi = dynamic_cast<CompTailles*>(contraintes[i]);
          std::cout << "posi->getRelation() = "<< posi->getRelation()<< std::endl;
          std::string t1x = posi->getT1X(); std::string t2x = posi->getT2X();
          std::string t1y = posi->getT1Y(); std::string t2y = posi->getT2Y();
          std::string t1z = posi->getT1Z(); std::string t2z = posi->getT2Z();

          std::string s1x = posi->getS1X(); std::string s2x = posi->getS2X();
          std::string s1y = posi->getS1Y(); std::string s2y = posi->getS2Y();
          std::string s1z = posi->getS1Z(); std::string s2z = posi->getS2Z();

          // differentes options : 
          // on peut soit ajouter un offset pour dire "tel objet est 1m plus grand qu'un autre"
          // ou alors "tel objet1 est plus grand qu'un autre objet2" ssi objet1.size > 2*objet2.size()

          if(posi->getRelation().compare("bigger")==0)
            {
              post(this,   Gecode::mult(this,variables[t1x], variables[s1x]) > (Gecode::mult(this,variables[t2x], variables[s2x]) +Gecode::mult(this,variables[t2x], variables[s2x]))); 
              values[cpt] = post(this, Gecode::abs(this,Gecode::minus(this,Gecode::plus(this, variables[s1x],variables[s2x]),Gecode::IntVar(this,200,200)) ) ); cpt++;
              post(this,   Gecode::mult(this,variables[t1y], variables[s1y]) > (Gecode::mult(this,variables[t2y], variables[s2y]) +Gecode::mult(this,variables[t2y], variables[s2y])));
              values[cpt] = post(this, Gecode::abs(this,Gecode::minus(this,Gecode::plus(this, variables[s1y],variables[s2y]),Gecode::IntVar(this,200,200)) ) ); cpt++;
              post(this,   Gecode::mult(this,variables[t1z], variables[s1z]) > (Gecode::mult(this,variables[t2z], variables[s2z]) +Gecode::mult(this,variables[t2z], variables[s2z])));
              values[cpt] = post(this, Gecode::abs(this,Gecode::minus(this,Gecode::plus(this, variables[s1z],variables[s2z]),Gecode::IntVar(this,200,200)) ) ); cpt++;
            }
          else if(posi->getRelation().compare("smaller")==0)
            {
              post(this,   Gecode::mult(this,variables[t1x], variables[s1x]) < Gecode::mult(this,variables[t2x], variables[s2x]) );
              values[cpt] = post(this, Gecode::abs(this,Gecode::minus(this,Gecode::plus(this, variables[s1x],variables[s2x]),Gecode::IntVar(this,200,200)) ) ); cpt++;
              post(this,   Gecode::mult(this,variables[t1y], variables[s1y]) < Gecode::mult(this,variables[t2y], variables[s2y]) );
              values[cpt] = post(this, Gecode::abs(this,Gecode::minus(this,Gecode::plus(this, variables[s1y],variables[s2y]),Gecode::IntVar(this,200,200)) ) ); cpt++;
              post(this,   Gecode::mult(this,variables[t1z], variables[s1z]) < Gecode::mult(this,variables[t2z], variables[s2z]) );
              values[cpt] = post(this, Gecode::abs(this,Gecode::minus(this,Gecode::plus(this, variables[s1z],variables[s2z]),Gecode::IntVar(this,200,200)) ) ); cpt++;
            }

          else if(posi->getRelation().compare("wider")==0 || posi->getRelation().compare("larger")==0)
            {
              post(this,   Gecode::mult(this,variables[t1x], variables[s1x]) > Gecode::mult(this,variables[t2x], variables[s2x]) +Gecode::mult(this,variables[t2x], variables[s2x]));
              values[cpt] = post(this, Gecode::abs(this,Gecode::minus(this,Gecode::plus(this, variables[s1x],variables[s2x]),Gecode::IntVar(this,200,200)) ) ); cpt++;
              post(this,   variables[s1y] == variables[s2y]);
              post(this,   variables[s1z] == variables[s2z]);
            }
          else if(posi->getRelation().compare("taller")==0)
            {
              post(this,   variables[s1x] == variables[s2x]);
              post(this,   Gecode::mult(this,variables[t1y], variables[s1y]) > Gecode::mult(this,variables[t2y], variables[s2y]) +Gecode::mult(this,variables[t2y], variables[s2y]));
              values[cpt] = post(this, Gecode::abs(this,Gecode::minus(this,Gecode::plus(this, variables[s1y],variables[s2y]),Gecode::IntVar(this,200,200)) ) ); cpt++;
              post(this,   variables[s1z] == variables[s2z]);
            }
          else if(posi->getRelation().compare("deeper")==0)
            {
              post(this,   variables[s1x] == variables[s2x]);
              post(this,   variables[s1y] == variables[s2y]);
              post(this,   Gecode::mult(this,variables[t1z], variables[s1z]) > Gecode::mult(this,variables[t2z], variables[s2z]) +Gecode::mult(this,variables[t2z], variables[s2z]));
              values[cpt] = post(this, Gecode::abs(this,Gecode::minus(this,Gecode::plus(this, variables[s1z],variables[s2z]),Gecode::IntVar(this,200,200)) ) ); cpt++;
            }
        }

      // contrainte de couleur : 'plus rouge que', 'plus vert que', etc.
      else if(dynamic_cast<CompColor*>(contraintes[i])!=NULL)
        {
          CompColor * posi = dynamic_cast<CompColor*>(contraintes[i]);
          std::cout << "posi->getRelation() = "<< posi->getRelation()<< std::endl;

          std::string r1 = posi->getR1(); std::string r2 = posi->getR2();
          std::string g1 = posi->getG1(); std::string g2 = posi->getG2();
          std::string b1 = posi->getB1(); std::string b2 = posi->getB2();
          std::string a1 = posi->getA1(); std::string a2 = posi->getA2();

          if(posi->getRelation().compare("lighter")==0)
            {
              post(this,   variables[r1] >= variables[r2]);
              post(this,   variables[g1] >= variables[g2]);
              post(this,   variables[b1] >= variables[b2]);
              post(this,   variables[a1] == variables[a2]);
            }
          else if(posi->getRelation().compare("darker")==0)
            {
              post(this,   variables[r1] <= variables[r2]);
              post(this,   variables[g1] <= variables[g2]);
              post(this,   variables[b1] <= variables[b2]);
              post(this,   variables[a1] == variables[a2]);
            }
          else if(posi->getRelation().compare("redder")==0)
            {
              post(this,   variables[r1] >= variables[r2]);
              post(this,   variables[g1] == variables[g2]);
              post(this,   variables[b1] == variables[b2]);
              post(this,   variables[a1] == variables[a2]);
            }
          else if(posi->getRelation().compare("greener")==0)
            {
              post(this,   variables[r1] == variables[r2]);
              post(this,   variables[g1] >= variables[g2]);
              post(this,   variables[b1] == variables[b2]);
              post(this,   variables[a1] == variables[a2]);				
            }
          else if(posi->getRelation().compare("bluer")==0)
            {
              post(this,   variables[r1] == variables[r2]);
              post(this,   variables[g1] == variables[g2]);
              post(this,   variables[b1] >= variables[b2]);
              post(this,   variables[a1] == variables[a2]);				
            }
        }

      else if(dynamic_cast<NoCollision*>(contraintes[i])!=NULL)
        {
          NoCollision * posi = dynamic_cast<NoCollision*>(contraintes[i]);
          std::cout << "No collision"<< std::endl;
          std::string p1x = posi->getP1X(); std::string p2x = posi->getP2X();
          std::string p1y = posi->getP1Y(); std::string p2y = posi->getP2Y();
          std::string p1z = posi->getP1Z(); std::string p2z = posi->getP2Z();

          std::string t1x = posi->getT1X(); std::string t2x = posi->getT2X();
          std::string t1y = posi->getT1Y(); std::string t2y = posi->getT2Y();
          std::string t1z = posi->getT1Z(); std::string t2z = posi->getT2Z();

          // la distance entre les deux centre en X doit etre supérieure à la taille des 2 elements
          //			Gecode::BoolVarArray b(this, 1, 0, 1);
          //			rel(this, Gecode::abs(this, Gecode::minus(this, variables[p1x] , variables[p2x]) ), Gecode::IRT_GR, 
          //					Gecode::plus(this, Gecode::mult(this, variables[t1x], variables[s1x]), Gecode::mult(this, variables[t2x], variables[s2x]) ), b[0]);			

          Gecode::BoolVarArray b(this, 3, 0, 1);
          rel(this, Gecode::abs(this, Gecode::minus(this, variables[p1x] , variables[p2x]) ), Gecode::IRT_GR, 
              Gecode::plus(this, variables[t1x], variables[t2x] ), b[2]);			
          rel(this, Gecode::abs(this, Gecode::minus(this, variables[p1y] , variables[p2y]) ), Gecode::IRT_GR, 
              Gecode::plus(this, variables[t1y], variables[t2y] ), b[1]);			
          rel(this, Gecode::abs(this, Gecode::minus(this, variables[p1z] , variables[p2z]) ), Gecode::IRT_GR, 
              Gecode::plus(this, variables[t1z], variables[t2z] ), b[0]);

          rel(this, Gecode::BOT_OR, b, 1);
        }
    }

  if(_sizeConstraint)
    {
      // la somme des distances est minimale (pour minimiser l'écart avec 100, la valeur de référence...)
      Gecode::linear(this, values, Gecode::IRT_EQ, _distance);
    }

  // le mode de recherche (0=minimum, 1=médian)
  if(BRANCHING_MODE==0)
    branch(this, _le, Gecode::INT_VAR_NONE, Gecode::INT_VAL_MIN);
  else if(BRANCHING_MODE==1)
    branch(this, _le, Gecode::INT_VAR_NONE, Gecode::INT_VAL_MED);
}

/// Add constraint for next better solution
void Solver::constrain(Space* s) {
  rel(this, _distance, Gecode::IRT_LE, static_cast<Solver*>(s)->_distance.val());
}

Solver::Solver(bool share, Solver& s)
: Space(share,s) 
{
  _le.update(this, share, s._le);
  _sizeConstraint = s._sizeConstraint;
  if(_sizeConstraint)
    _distance.update(this, share, s._distance);
}

int Solver::run()
{
  // si la contrainte traitée est une contrainte de taille, on doit effectuer plusieurs recherches pour avoir le facteur scale le plus proche de 100 possible
  if(_sizeConstraint)
    {
      // calcul de tailles
      Gecode::BAB<Solver> e(this);
      Solver* ex = e.next();
      int res = (ex==NULL) ? -1 : 1;
      do 
        {
          ex = e.next();
          if (ex == NULL)
            break;
          *this = *ex;
        } 
      while (true);

      return res;
    }
  else // sinon recherche classique (pas d'optimisation nécessaire)
    {
      Gecode::DFS<Solver> e(this);

      Solver* ex = e.next();
      if (ex != NULL)
        {
          //ex->print(std::cout);
          *this = *ex;
          return 1;
        }
      else
        return -1;
    }
}

int Solver::nbConstraintToOptimize(std::vector<Constraint*> & contraintes)
{
  int res = 0;

  for(unsigned int i=0 ; i<contraintes.size() ; i++)
    {
      if(dynamic_cast<CompTailles*>(contraintes[i])!=NULL)
        {
          CompTailles * posi = dynamic_cast<CompTailles*>(contraintes[i]);
          // si c'est 'bigger' ou 'smaller', on a 1 contrainte sur chaque composante (X,Y,Z)
          if(posi->getRelation().compare("bigger")==0 || posi->getRelation().compare("smaller")==0)
            res+=3;
          else // sinon 1 contrainte sur 1 seule composante X xor Y xor Z
            res++;
        }
    }
  return res;
}
