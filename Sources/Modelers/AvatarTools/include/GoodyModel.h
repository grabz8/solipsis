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
class GoodyInstance;

class GoodyModel;
class GoodyModelInstance : public ModifiableMaterialObject
{
public:
	///brief Constructor
	///param goodyModel Goody model of this instance.
	///param owner GoodyInstance from which the GoodyModelInstance is a model.
    GoodyModelInstance(GoodyModel* goodyModel, GoodyInstance* owner);
	~GoodyModelInstance();

	///brief Method which gives the GoodyModel.
    GoodyModel* getGoodyModel() { return mGoodyModel; }

	///brief Method which return the owner.
	///return Owner.
    GoodyInstance* getOwner() { return mOwner; }

	///brief Method which return the SubEntity representing the GoodyModel.
	///return Entity representing the GoodyModel.
    Entity* getEntity() { return mEntity; }

private:
	GoodyModel* mGoodyModel;							///brief GoodyModel associated to the GoodyModelInstance.
	GoodyInstance* mOwner;								///brief Owner of the GoodyModelInstance.
	Entity* mEntity;									///brief Entity associated to the GoodyModel.
};


/// brief This is the class representing a GoodyModel of a Goody of character. It could be for instance a "Rolex" if the Goody is a "watch". We can change his colour (more precisely the colour of the submesh 0 of the mesh representing the GoodyModel) if it's allowed. Same thing for the texture.
/// author François FOURNEL
/// date 2007.06.25

class GoodyModel : public ModifiableMaterialObjectBase
{
    friend class GoodyModelInstance;

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
