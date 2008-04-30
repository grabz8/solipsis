#include "Character.h"

using namespace Solipsis;

//--------------------------------------------------------------------------------------------------------------------------------------------------
BodyPartModel::BodyPartModel(const String& name,SubEntity* subEntity,BodyPart* owner) : ModifiableMaterialObject()
{
	mName = name;
	mSubEntity = subEntity;
	mOwner = owner;
	String s = "Material"+mOwner->getOwner()->getName()+mOwner->getName()+name;
	MaterialPtr materialSubEntity = mSubEntity->getMaterial()->clone(s);
	mSubEntity->setMaterialName(materialSubEntity->getName());
	initialise(materialSubEntity);
}



//--------------------------------------------------------------------------------------------------------------------------------------------------
const String& BodyPartModel::getName()
{
	return mName;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
SubEntity* BodyPartModel::getSubEntity()
{
	return mSubEntity;
}
