#include "ODEPlugin.h"
#include "ODEPluginPrerequisites.h"
#include "OgreRoot.h"

using namespace Ogre;

namespace Solipsis {

ODEPlugin* _ODEPlugin;

//-----------------------------------------------------------------------
extern "C" _NavigatorODEPluginExport void dllStartPlugin(void)
{
    // Create new plugin
    _ODEPlugin = new ODEPlugin();

    // Register
    Root::getSingleton().installPlugin(_ODEPlugin);
}

//-----------------------------------------------------------------------
extern "C" _NavigatorODEPluginExport void dllStopPlugin(void)
{
    // Unregister
    Root::getSingleton().uninstallPlugin(_ODEPlugin);

    // Destroy plugin
    delete _ODEPlugin;
}

} // end namespace