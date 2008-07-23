#include "Plugin_3ds.h"
#include "Plugin_3dsPrerequisites.h"


using namespace Ogre;


Plugin_3ds* _Plugin_3ds;

//-------------------------------------------------------------------------------------
extern "C" _Plugin_3dsExport void dllStartPlugin(void)
{
    // Create new plugin
    _Plugin_3ds = new Plugin_3ds();

    // Register
    Root::getSingleton().installPlugin(_Plugin_3ds);
}

//-------------------------------------------------------------------------------------
extern "C" _Plugin_3dsExport void dllStopPlugin(void)
{
    // Unregister
    Root::getSingleton().uninstallPlugin(_Plugin_3ds);

    // Destroy plugin
    delete _Plugin_3ds;
}
