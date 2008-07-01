/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

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

#include "Object.h"
#include "Modeler.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
#ifdef POOL
Object::Object(RefCntPoolPtr<XmlEntity>& xmlEntity, bool isLocal, const Object3DPtrList& object3DList) :
#else
Object::Object(XmlEntity* xmlEntity, bool isLocal, const Object3DPtrList& object3DList) :
#endif
    OgrePeer(xmlEntity, isLocal),
    mObject3DList(object3DList)
{
}

//-------------------------------------------------------------------------------------
Object::~Object()
{
    Modeler *modeler = Modeler::getSingletonPtr();
    if (modeler == 0)
        return;
    Selection *selection = modeler->getSelection();
    for(Object3DPtrList::const_iterator it=mObject3DList.begin();it!=mObject3DList.end();++it)
        selection->remove3DObject(*it);
}

//-------------------------------------------------------------------------------------
void Object::update(Real timeSinceLastFrame)
{
}

//-------------------------------------------------------------------------------------
#ifdef POOL
bool Object::update(RefCntPoolPtr<XmlEntity>& xmlEntity)
#else
bool Object::update(XmlEntity* xmlEntity)
#endif
{

    return true;
}

//-------------------------------------------------------------------------------------
