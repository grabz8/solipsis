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

#include "GoodyModel.h"
#include "Goody.h"
#include "Character.h"
#include "CharacterInstance.h"

using namespace Solipsis;

//--------------------------------------------------------------------------------------------------------------------------------------------------
GoodyModelInstance::GoodyModelInstance(GoodyModel* goodyModel, GoodyInstance* owner) :
ModifiableMaterialObject(goodyModel),
mGoodyModel(goodyModel), mOwner(owner)
{
    mEntity = mGoodyModel->mEntity->clone(mOwner->getOwner()->getUidString() + mGoodyModel->getName());
    String s = "Material" + mOwner->getOwner()->getUidString() + mGoodyModel->mOwner->getName() + mGoodyModel->mName;
    MaterialPtr materialGoody = MaterialManager::getSingleton().getByName(s);
    if (materialGoody.isNull())
    	materialGoody = mGoodyModel->mEntity->getSubEntity(0)->getMaterial()->clone(s, true, mOwner->getOwner()->getResourceGroup());
    mEntity->getSubEntity(0)->setMaterialName(materialGoody->getName());
	initialise(materialGoody);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
GoodyModelInstance::~GoodyModelInstance()
{
//    mEntity->getSubEntity(0)->setMaterialName(mGoodyModel->mEntity->getSubEntity(0)->getMaterial()->getName());
    // Cloned material should be freed when no more referenced
/*    const MaterialPtr& clonedMaterial = mGoodyModel->mEntity->getSubEntity(0)->getMaterial();
    MaterialManager::getSingleton().remove((ResourcePtr&)clonedMaterial);*/
    mOwner->getOwner()->getSceneMgr()->destroyEntity(mEntity);
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
GoodyModel::GoodyModel(const String& name, Entity* entity, Goody* owner) :
mOwner(owner), mName(name), mEntity(entity)
{
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