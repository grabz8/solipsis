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

#include "Peer.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
Peer::Peer(String& networkId, String& login, CBigInt x_, CBigInt y_, CBigInt z_) :
    mNetworkId(networkId),
    mLogin(login),
    x(x_),
    y(y_),
    z(z_)
{
}

//-------------------------------------------------------------------------------------
Peer::Peer(String& login, CBigInt x_, CBigInt y_, CBigInt z_) :
    mNetworkId(PEER_LOCAL),
    mLogin(login),
    x(x_),
    y(y_),
    z(z_)
{
}

//-------------------------------------------------------------------------------------
Peer::~Peer()
{
}

//-------------------------------------------------------------------------------------
String Peer::getNetworkId()
{
    return mNetworkId;
}

//-------------------------------------------------------------------------------------
String Peer::getLogin()
{
    return mLogin;
}

//-------------------------------------------------------------------------------------
int Peer::getFakeX()
{ 
    //return (int)(x / CBigInt("100000000000000000000000000000000")); Div don't work well..
    /* 
    const CBigInt magic = CBigInt("10000");
    OGRE_LOG((String)x);
    OGRE_LOG((String)magic);
    OGRE_LOG((String)((x/magic)));
    */
    return (int)(x % CBigInt("10000"))-5000;
}

//-------------------------------------------------------------------------------------
int Peer::getFakeY()
{
    return (int)(y % CBigInt("10000"))-5000;
}

//-------------------------------------------------------------------------------------
int Peer::getFakeZ()
{
    return (int)(z % CBigInt("10000"));
}

//-------------------------------------------------------------------------------------
bool Peer::isLocal()
{
    return (mNetworkId.compare(PEER_LOCAL) == 0);
}

//-------------------------------------------------------------------------------------
