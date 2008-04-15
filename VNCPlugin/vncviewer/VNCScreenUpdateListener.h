#ifndef _VNCSCREENUPDATELISTENER_H_
#define _VNCSCREENUPDATELISTENER_H_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class ClientConnection;
class VNCScreenUpdateListener
{
public:
// GREG BEGIN
//    virtual void screenUpdated(HDC dc, HBITMAP bitmap) = 0;
    virtual void screenUpdated(ClientConnection* clientConnection) = 0;
// GREG END

}; //   class VNCScreenUpdateListener

#undef WIN32_LEAN_AND_MEAN

#endif  //  _SCREENUPDATELISTENER_H_
