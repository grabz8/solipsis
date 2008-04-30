#include "GoodyModel.h"
#include "Goody.h"
#include "Character.h"

using namespace Solipsis;

//--------------------------------------------------------------------------------------------------------------------------------------------------
GoodyModel::GoodyModel(const String& name, Entity* entity, Goody* owner) :
mOwner(owner), mName(name), mEntity(entity)
{
	const MaterialPtr& materialGoody = mEntity->getSubEntity(0)->getMaterial()->clone("Material"+mOwner->getOwner()->getName()+mOwner->getName()+name);
	mEntity->getSubEntity(0)->setMaterialName(materialGoody->getName());
	initialise(materialGoody);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
const String& GoodyModel::getName()
{
	return mName;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
Entity* GoodyModel::getEntity()
{
	return mEntity;
}