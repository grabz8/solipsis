/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author ARTEFACTO

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

#include "CharacterInstance.h"
#include "BodyPart.h"
#include "Character.h"
#include "CharacterInstance.h"

using namespace Solipsis;

//--------------------------------------------------------------------------------------------------------------------------------------------------
BodyPartModelInstance::BodyPartModelInstance(BodyPartModel* bodyPartModel, BodyPartInstance* owner) :
ModifiableMaterialObject(bodyPartModel),
mBodyPartModel(bodyPartModel), mOwner(owner), mGhost(false)
{
    mSubEntity = mOwner->getOwner()->getEntity()->getSubEntity(mBodyPartModel->mSubEntityName);
	String s = "Material" + mOwner->getOwner()->getUid() + mBodyPartModel->mOwner->getName() + mBodyPartModel->mName;
    MaterialPtr materialSubEntity = MaterialManager::getSingleton().getByName(s);
    if (materialSubEntity.isNull())
        materialSubEntity = mBodyPartModel->mSubEntity->getMaterial()->clone(s, true, mOwner->getOwner()->getResourceGroup());
	mSubEntity->setMaterialName(materialSubEntity->getName());
	initialise(materialSubEntity);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
BodyPartModelInstance::~BodyPartModelInstance()
{
//    mSubEntity->setMaterialName(mBodyPartModel->mSubEntity->getMaterial()->getName());
    // Cloned material should be freed when no more referenced
/*    const MaterialPtr& clonedMaterial = mBodyPartModel->mSubEntity->getMaterial();
    MaterialManager::getSingleton().remove((ResourcePtr&)clonedMaterial);*/
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
void BodyPartModelInstance::setGhost(bool ghost)
{
    mGhost = ghost;
    ModifiableMaterialObject::setGhost(ghost);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
BodyPartModel::BodyPartModel(const String& name,const String& subEntityName,SubEntity* subEntity,BodyPart* owner) :
mOwner(owner), mName(name), mSubEntity(subEntity), mSubEntityName(subEntityName)
{
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
