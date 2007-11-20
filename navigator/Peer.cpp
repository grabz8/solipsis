#include "Peer.h"

using namespace Solipsis;

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
