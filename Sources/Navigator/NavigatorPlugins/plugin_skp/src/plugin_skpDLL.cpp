#include "Plugin_skp.h"
#include "Plugin_skpPrerequisites.h"


using namespace Ogre;


Plugin_skp* _Plugin_skp;

//-------------------------------------------------------------------------------------
extern "C" _Plugin_skpExport void dllStartPlugin(void)
{
    // Create new plugin
    _Plugin_skp = new Plugin_skp();

    // Register
    Root::getSingleton().installPlugin(_Plugin_skp);
}

//-------------------------------------------------------------------------------------
extern "C" _Plugin_skpExport void dllStopPlugin(void)
{
    // Unregister
    Root::getSingleton().uninstallPlugin(_Plugin_skp);

    // Destroy plugin
    delete _Plugin_skp;
}
