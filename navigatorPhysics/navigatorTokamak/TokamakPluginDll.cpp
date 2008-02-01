#include "TokamakPlugin.h"
#include "TokamakPluginPrerequisites.h"
#include "OgreRoot.h"

using namespace Ogre;

namespace Solipsis {

TokamakPlugin* _TokamakPlugin;

//-----------------------------------------------------------------------
extern "C" _NavigatorTokamakPluginExport void dllStartPlugin(void)
{
    // Create new plugin
    _TokamakPlugin = new TokamakPlugin();

    // Register
    Root::getSingleton().installPlugin(_TokamakPlugin);
}

//-----------------------------------------------------------------------
extern "C" _NavigatorTokamakPluginExport void dllStopPlugin(void)
{
    // Unregister
    Root::getSingleton().uninstallPlugin(_TokamakPlugin);

    // Destroy plugin
    delete _TokamakPlugin;
}

} // namespace Solipsis