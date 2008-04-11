#include "VNCPlugin.h"
#include "VNCPluginPrerequisites.h"
#include "OgreRoot.h"

using namespace Ogre;

namespace Solipsis {

VNCPlugin* _VNCPlugin;

//-----------------------------------------------------------------------
extern "C" _VNCPluginExport void dllStartPlugin(void)
{
    // Create new plugin
    _VNCPlugin = new VNCPlugin();

    // Register
    Root::getSingleton().installPlugin(_VNCPlugin);
}

//-----------------------------------------------------------------------
extern "C" _VNCPluginExport void dllStopPlugin(void)
{
    // Unregister
    Root::getSingleton().uninstallPlugin(_VNCPlugin);

    // Destroy plugin
    delete _VNCPlugin;
}

} // namespace Solipsis