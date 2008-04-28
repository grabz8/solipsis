#include "FModSpeexPlugin.h"
#include "FModSpeexPluginPrerequisites.h"
#include "OgreRoot.h"

using namespace Ogre;

namespace Solipsis {

FModSpeexPlugin* _FModSpeexPlugin;

//-----------------------------------------------------------------------
extern "C" _FModSpeexPluginExport void dllStartPlugin(void)
{
    // Create new plugin
    _FModSpeexPlugin = new FModSpeexPlugin();

    // Register
    Root::getSingleton().installPlugin(_FModSpeexPlugin);
}

//-----------------------------------------------------------------------
extern "C" _FModSpeexPluginExport void dllStopPlugin(void)
{
    // Unregister
    Root::getSingleton().uninstallPlugin(_FModSpeexPlugin);

    // Destroy plugin
    delete _FModSpeexPlugin;
}

} // namespace Solipsis