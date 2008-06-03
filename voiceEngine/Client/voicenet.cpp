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

#include "voicenet.h"
#include <windows.h>

// GREG BEGIN
void initWinSock()
{
  static bool wsInit = false;
  if (! wsInit)
  {
    WORD wVersionRequested = MAKEWORD( 2, 0 );
    WSADATA wsaData;
    WSAStartup(wVersionRequested, &wsaData);
    wsInit = true;
  }
}
// GREG END

bool ve_create_socket_tcp(VESocketHandle& hSocket)
{
    hSocket = (VESocketHandle)socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (hSocket == INVALID_SOCKET) hSocket = VE_INVALID_SOCKET;
    return (hSocket != VE_INVALID_SOCKET);
}

void ve_destroy_socket_tcp(VESocketHandle& hSocket)
{
    if (hSocket == VE_INVALID_SOCKET)
        return;

    closesocket((SOCKET)hSocket);
    hSocket = VE_INVALID_SOCKET;
}

bool ve_connect_socket_tcp(VESocketHandle hSocket, const char* host, int port)
{
    sockaddr_in saddr; 
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr(host);
    saddr.sin_port = htons(port);
    return (connect((SOCKET)hSocket, (SOCKADDR*)&saddr, sizeof(saddr)) == 0);
}

int ve_send_packet_tcp(VESocketHandle hSocket, const char* sendBuffer, int size)
{
    return send((SOCKET)hSocket, sendBuffer, size, 0);
}

int ve_receive_packet_tcp(VESocketHandle hSocket, char* receiveBuffer, int size)
{
    return recv((SOCKET)hSocket, receiveBuffer, size, 0);
}
