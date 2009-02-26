#ifndef _PROTOCOL_VERSION_HEADER_FOR_RAYNET__
#define _PROTOCOL_VERSION_HEADER_FOR_RAYNET__


#include "P2PTools.h"

namespace P2P{
	typedef unsigned short RaynetVersion;
	const RaynetVersion RAYNET_VERSION = 1;
	DEFINE_EXCEPTION(ProtocolVersionException)
}

#endif