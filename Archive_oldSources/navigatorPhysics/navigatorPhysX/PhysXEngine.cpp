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

#include "PhysXEngine.h"
#include "PhysXScene.h"
#include "PhysXHelpers.h"

using namespace Solipsis;

const String sPhysicsEngineName = "PhysX engine";

//-------------------------------------------------------------------------------------
const String& PhysXEngine::getName() const
{
    return sPhysicsEngineName;
}

//-------------------------------------------------------------------------------------
bool PhysXEngine::init()
{
    return PhysXHelpers::init();
}

//-------------------------------------------------------------------------------------
bool PhysXEngine::shutdown()
{
   return PhysXHelpers::shutdown();
}

//-------------------------------------------------------------------------------------
IPhysicsScene* PhysXEngine::createScene()
{
   return new PhysXScene();
}

//-------------------------------------------------------------------------------------
void PhysXEngine::destroyScene(IPhysicsScene* scene)
{
    assert(scene != 0);
    delete scene;
}

//-------------------------------------------------------------------------------------
