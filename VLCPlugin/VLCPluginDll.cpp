#include "VLCPlugin.h"
#include "VLCPluginPrerequisites.h"
#include "OgreRoot.h"

using namespace Ogre;

namespace Solipsis {

VLCPlugin* _VLCPlugin;

//-------------------------------------------------------------------------------------
extern "C" _VLCPluginExport void dllStartPlugin(void)
{
    // Create new plugin
    _VLCPlugin = new VLCPlugin();

    // Register
    Root::getSingleton().installPlugin(_VLCPlugin);
}

//-------------------------------------------------------------------------------------
extern "C" _VLCPluginExport void dllStopPlugin(void)
{
    // Unregister
    Root::getSingleton().uninstallPlugin(_VLCPlugin);

    // Destroy plugin
    delete _VLCPlugin;
}

} // namespace Solipsis