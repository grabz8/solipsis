#ifndef _P2P_TOOLS_BASIC_TYPES_FILE_HEADER_
#define _P2P_TOOLS_BASIC_TYPES_FILE_HEADER_



#include "OSDependentTools.h"

namespace P2P
{
	typedef unsigned char uchar;
	typedef unsigned short ushort;
	typedef unsigned int uint;

	enum ProtocolType{ eKLEINBERG, eVORONOI};


	// Ensure that int4 is 4 bytes whatever the OS
	template <bool>class Static_assert;
	template<>	class Static_assert<true>{};		
#   define STATIC_ASSERT(BOOLEAN,DESC) static const Static_assert< BOOLEAN> DESC;
	
	STATIC_ASSERT( sizeof(int) == 4 ,INT_MUST_BE_4_CHARS)
	typedef uint uint4;
	




}








namespace Tools
{
	typedef unsigned char uchar;
	typedef unsigned short ushort;
	typedef unsigned int uint;


	typedef uint uint4;

}




namespace IP
{

// Unix Build: Needs SOCKET defined
#ifndef _WIN32
	typedef socket SOCKET;
#else
	typedef ::SOCKET SOCKET;
#endif

}

#endif