#ifndef __DebugHelpers_h__
#define __DebugHelpers_h__

#ifdef UIDEBUG

#include "Ogre.h"
#include <stack>

class DebugHelpers
{
public:
    static std::map<Ogre::String,Ogre::String> debugCommands;
};

#endif

#endif // #ifndef __DebugHelpers_h__