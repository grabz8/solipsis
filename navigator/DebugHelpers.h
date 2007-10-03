#ifndef __DebugHelpers_h__
#define __DebugHelpers_h__

#ifdef UIDEBUG

#include <stack>
#include "Ogre.h"

using namespace Ogre;

class Navigator;

class DebugHelpers
{
public:
    // Map of debug commands with parameters
    static std::map<String,String> debugCommands;

    // Debug callbacks
    static bool frameStarted(const FrameEvent& evt, Navigator* navigator, SceneManager* sceneMgr);
};

#endif

#endif // #ifndef __DebugHelpers_h__