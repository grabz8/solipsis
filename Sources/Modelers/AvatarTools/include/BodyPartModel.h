/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author ARTEFACTO - François FOURNEL

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

#ifndef __BodyPartModel_h__
#define __BodyPartModel_h__


#if (OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR < 6)
	#include <OgreNoMemoryMacros.h>
	#include <OgreMemoryMacros.h>
#endif
#include <Ogre.h>

#include "ModifiableMaterialObject.h"

using namespace Ogre;

namespace Solipsis {

class BodyPart;
class BodyPartInstance;

class BodyPartModel;
class BodyPartModelInstance : public ModifiableMaterialObject
{
public:
	///brief Constructor
	///param bodyPartModel BodyPart model of this instance.
	///param owner BodyPartInstance from which the BodyPartModelInstance is a model.
    BodyPartModelInstance(BodyPartModel* bodyPartModel, BodyPartInstance* owner);
	///brief Destructor
    ~BodyPartModelInstance();

    ///brief Method which gives the BodyPartModel.
    BodyPartModel* getBodyPartModel() { return mBodyPartModel; }

	///brief Method which return the owner.
	///return Owner.
    BodyPartInstance* getOwner() { return mOwner; }

	///brief Method which return the SubEntity representing the BodyPartModel.
	///return Entity representing the BodyPartModel.
    SubEntity* getSubEntity() { return mSubEntity; }

    /// brief Set the ghost attribute
    void setGhost(bool ghost);

private:
	BodyPartModel* mBodyPartModel;							///brief BodyPartModel associated to the BodyPartModelInstance.
	BodyPartInstance* mOwner;								///brief Owner of the BodyPartModelInstance.
	SubEntity* mSubEntity;									///brief SubEntity associated to the BodyPartModel.
    bool mGhost;											///brief Ghost flag.
};

/// brief This is the class of a model of a body part of a character. It is associated to one or more SubMesh of the character's SubMesh. It could be for example a clown noze if te BodyPart is a noze. It is a modifiable material object so it derives from the class ModifiableMaterialObject.
/// file BodyPartModel.h
/// author François FOURNEL
/// date 2007.07.17

class BodyPartModel : public ModifiableMaterialObjectBase
{
    friend class BodyPartModelInstance;

public:
	///brief Constructor
	///param name Name of the BodyPartModel
	///param subEntity One of the Ogre SubEntity representing the BodyPartModel
	///param owner BodyPart form which the BodyPartModel is a model
	BodyPartModel(const String& name,const String& subEntityName,SubEntity* subEntity,BodyPart* owner);



	///brief Method which gives the name of the BodyPartModel
	///return name of the BodyPartModel
	const String& getName();

	///brief Method which return the SubEntity representing the BodyPartModel
	///return SubEntity representing the BodyPartModel
	SubEntity* getSubEntity();

	const String& getSubEntityName();


private:
	BodyPart* mOwner;									///brief BodyPart whose this BodyPartModel is a model. For example, the BodyPart could be the noze and the BodyPartModel a Clown's noze

	String mName;										///brief Name of the BodyPartModel
	SubEntity* mSubEntity;								///brief Ogre SubEntity representing the BodyPartModel
    String mSubEntityName;
};
	
}

#endif
