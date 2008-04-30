/*
		COPYRIGHTS (c) ARTEFACTO 
		
			!! Licence GPL !!	
*/

#ifndef __GoodyModel_h__
#define __GoodyModel_h__

#include "ModifiableMaterialObject.h"


using namespace Ogre;
namespace Solipsis {

class Goody;

/// brief This is the class representing a GoodyModel of a Goody of avatar. It could be for instance a "Rolex" if the Goody is a "watch". We can change his colour (more precisely the colour of the submesh 0 of the mesh representing the GoodyModel) if it's allowed. Same thing for the texture.
/// author François FOURNEL
/// date 2007.06.25

class GoodyModel : public ModifiableMaterialObject
{
public:
	///brief Constructor
	///param name Name of the GoodyModel.
	///param entity Entity representing the GoodyModel.
	///param owner Goody from which the GoodyModel is a model.
	GoodyModel(const String& name, Entity* entity, Goody* owner);


	
	///brief Method which gives the name of the GoodyModel.
	///return name of the GoodyModel.
	const String& getName();

	///brief Method which return the SubEntity representing the GoodyModel.
	///return Entity representing the GoodyModel.
	Entity* getEntity();


private:
	Goody* mOwner;										///brief Owner of the GoodyModel. Goody to which the GoodyModel is a model.

	String mName;										///brief Complete name of the GoodyModel.
	Entity* mEntity;									///brief Entity associated to the GoodyModel.
};

}

#endif
