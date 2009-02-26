
#include <OgreErrorDialog.h>
using namespace Ogre;

#include "OgreDeclarativeModeler.h"

OgreDeclarativeModeler::OgreDeclarativeModeler( const std::string & s )
: DeclarativeModeler( s )
{
	std::vector< std::string > properties;
	properties.push_back( "C:/solipsis_20080919/solipsis/trunk/Media/models/MII/primitives/" ); // LOCATION
	properties.push_back( "Sphere.mesh" ); // MODEL + EXTENSION
	m_availableActorModels["sphere"] = properties; 
	m_availableActorModels["ball"] = properties; 
	properties.clear();
	properties.push_back( "C:/solipsis_20080919/solipsis/trunk/Media/models/MII/primitives/" ); // LOCATION
	properties.push_back( "Box.mesh" ); // MODEL + EXTENSION
	m_availableActorModels["box"] = properties; 
	m_availableActorModels["cube"] = properties; 
}

OgreDeclarativeModeler::~OgreDeclarativeModeler()
{}

void OgreDeclarativeModeler::assignModelToActor( Actor* actor ) 
{
	std::string actorName = actor->getName();
	if( !m_availableActorModels[actorName].empty() )
		actor->setModelName( m_availableActorModels[actorName][0] + m_availableActorModels[actorName][1] );
}

void OgreDeclarativeModeler::assignModelsToActors()
{
	std::vector< Actor* >::iterator it;
	for( it = m_actors.begin(); it != m_actors.end(); it++ ) {
		assignModelToActor( *it );
	}
}

