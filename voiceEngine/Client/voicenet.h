/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author RealXTend, updated by JAN Gregory

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
#if 0
#ifndef VOICENET_H
#define VOICENET_H

#define VE_INVALID_SOCKET -1

typedef long VESocketHandle;

// GREG BEGIN
void initWinSock();
// GREG END
bool ve_create_socket_tcp(VESocketHandle& hSocket);
void ve_destroy_socket_tcp(VESocketHandle& hSocket);
bool ve_connect_socket_tcp(VESocketHandle hSocket, const char* host, int port);
int ve_send_packet_tcp(VESocketHandle hSocket, const char* sendBuffer, int size);
int ve_receive_packet_tcp(VESocketHandle hSocket, char* receiveBuffer, int size);

#endif	//	VOICENET_H
#endif 0