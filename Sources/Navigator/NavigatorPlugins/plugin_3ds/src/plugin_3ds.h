//Plugin_3ds.h

#ifndef __Plugin_3ds_h__
#define __Plugin_3ds_h__

#include "Plugin_3dsPrerequisites.h"
#include <Ogre.h>
#include <OgrePlugin.h>
#include <OgreRoot.h>
#include <OgreSharedPtr.h>
#include <OgreSingleton.h>
#include <OgreStringConverter.h>
#include <OgreDefaultHardwareBufferManager.h>
#include <OgreHardwareVertexBuffer.h>
#include <OgreVertexIndexData.h>
#include <OgreResourceGroupManager.h>
#include <memory>
#include <set>
#include <string>
#include <vector>

#define VERSION "1.1"

#ifdef WIN32
extern "C"{
#include "getopt.h"
}
#else
#include <getopt.h>
#endif

using namespace std;
using namespace Ogre;

extern "C"{
#include <3dsftk.h>
}


class Face {
public:
	Face() {}
	Face(ushort3ds i1, ushort3ds i2, ushort3ds i3) : v1(i1), v2(i2), v3(i3) {}
	ushort3ds v1, v2, v3;
};

class _Plugin_3dsExport Plugin_3ds : public Ogre::Plugin
{
public:
    Plugin_3ds();

    /// @copydoc Plugin::getName
    const Ogre::String& getName() const;

    /// @copydoc Plugin::install
    void install();

    /// @copydoc Plugin::initialise
    void initialise();

    /// @copydoc Plugin::shutdown
    void shutdown();

    /// @copydoc Plugin::uninstall
    void uninstall();
	
	static Ogre::String ReplaceSpaces(const Ogre::String& s);
	static bool FolderExist(std::string strPath);
	static bool convert3dsToMesh(const Ogre::String & sz3dsFileName,
						const Ogre::String & szMeshPrefix,
						bool bCreateSkeleton = false,
						bool bTextureVerticalFlip = false);
	static Ogre::Entity* createEntityFrom3ds(const Ogre::String & entityName, 
												const Ogre::String & sz3dsFileName,
												Ogre::SceneManager* mSceneMgr);
};

#endif // #ifndef __Plugin_3ds_h__