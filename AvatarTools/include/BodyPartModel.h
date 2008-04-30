#ifndef __BodyPartModel_h__
#define __BodyPartModel_h__


#include <OgreNoMemoryMacros.h>
#include <OgreMemoryMacros.h>
#include <Ogre.h>

#include "ModifiableMaterialObject.h"

using namespace Ogre;
namespace Solipsis {

class BodyPart;

/// brief This is the class of a model of a body part of an Avatar. It is associated to one or more SubMesh of the avatar's SubMesh. It could be for example a clown noze if te BodyPart is a noze. It is a modifiable material object so it derives from the class ModifiableMaterialObject.
/// file BodyPartModel.h
/// author François FOURNEL
/// date 2007.07.17

class BodyPartModel : public ModifiableMaterialObject
{
public:
	///brief Constructor
	///param name Name of the BodyPartModel
	///param subEntity One of the Ogre SubEntity representing the BodyPartModel
	///param owner BodyPart form which the BodyPartModel is a model
	BodyPartModel(const String& name,SubEntity* subEntity,BodyPart* owner);



	///brief Method which gives the name of the BodyPartModel
	///return name of the BodyPartModel
	const String& getName();

	///brief Method which return the SubEntity representing the BodyPartModel
	///return SubEntity representing the BodyPartModel
	SubEntity* getSubEntity();



private:
	BodyPart* mOwner;									///brief BodyPart whose this BodyPartModel is a model. For example, the BodyPart could be the noze and the BodyPartModel a Clown's noze

	String mName;										///brief Name of the BodyPartModel
	SubEntity* mSubEntity;								///brief Ogre SubEntity representing the BodyPartModel
};
	
}

#endif
