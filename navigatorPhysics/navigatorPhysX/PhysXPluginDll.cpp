#include "PhysXPlugin.h"
#include "PhysXPluginPrerequisites.h"
#include "OgreRoot.h"

using namespace Ogre;

namespace Solipsis {

PhysXPlugin* _PhysXPlugin;

//-----------------------------------------------------------------------
extern "C" _NavigatorPhysXPluginExport void dllStartPlugin(void)
{
    // Create new plugin
    _PhysXPlugin = new PhysXPlugin();

    // Register
    Root::getSingleton().installPlugin(_PhysXPlugin);
}

//-----------------------------------------------------------------------
extern "C" _NavigatorPhysXPluginExport void dllStopPlugin(void)
{
    // Unregister
    Root::getSingleton().uninstallPlugin(_PhysXPlugin);

    // Destroy plugin
    delete _PhysXPlugin;
}

} // end namespace