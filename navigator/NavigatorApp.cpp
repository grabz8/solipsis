#include <sstream>
#include "NavigatorModule.h"
#include "IApplication.h"
#include "IInstance.h"
#include "IWindow.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

using namespace Solipsis;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char *argv[])
#endif
{
    try
    {
        // Create application
        IApplication* application = IApplication::createApplication("", true, "Solipsis - StandAlone Navigator");
        assert(application != 0);

        std::string envVar;
        envVar = CONTAINER_NAME_ENV"=standalone";
        _putenv(envVar.c_str());
        envVar = NAVI_SUPPORT_ENV"=yes";
        _putenv(envVar.c_str());

        // Create instance
        IInstance* instance = application->createInstance();
        assert(instance != 0);

        // Create the internal automatic window
        instance->setWindow(0);

        // Run instance
        instance->run();

        // Destroy instance
        application->destroyInstance(instance);

        // Destroy application
        application->destroy();
    }
    catch (...)
    {
#ifdef WIN32
        MessageBox(0, "Unable to launch the Navigator ...", "An exception has occured !", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        std::cerr << "Unable to launch the Navigator ... An exception has occured !" << std::endl;
#endif
    }

    return 0;
}

#ifdef __cplusplus
}
#endif
