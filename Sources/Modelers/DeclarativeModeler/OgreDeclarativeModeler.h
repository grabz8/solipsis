#ifndef _OGRE_DECLARATIVE_MODELER_H_
#define _OGRE_DECLARATIVE_MODELER_H_

#include <Ogre.h>

#include "DeclarativeModeler.h"

/**
 * \file DeclarativeModeler.h
 * \brief Le modeleur, il parse le texte, récupère les acteurs et calcule leur propriétés
 * \date Octobre 2008
 */
class OgreDeclarativeModeler : public DeclarativeModeler
{
public:

	/**
	 * \brief Constructeur : initialise un modeleur à partir d'une chaine de caractère
	 * \param s : La chaine de caractère
	 */
	OgreDeclarativeModeler( const std::string & s );

	/**
	 * \bried Destructeur
	 */
	virtual ~OgreDeclarativeModeler();
	
	virtual void assignModelToActor( Actor* );

	virtual void assignModelsToActors();

protected:

};

#endif /*_OGRE_DECLARATIVE_MODELER_H_*/
