//Plugin_skp.h

#ifndef __Plugin_skp_h__
#define __Plugin_skp_h__

#include "Plugin_skpPrerequisites.h"
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
#include <list>
#include <map>
#include "GeomUtilities.h"

#define VERSION "1.1"

using namespace std;
using namespace Ogre;



class _Plugin_skpExport Plugin_skp : public Ogre::Plugin
{
public:
    Plugin_skp();
    ~Plugin_skp();

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

    static void WriteTextureFiles(SketchUp::ISkpDocumentPtr pDoc,String szMeshPrefix, 
        SketchUp::ISkpTextureWriter2Ptr m_pTextureWriter);

    static void LoadTexturesFromEntities(SketchUp::ISkpEntityProviderPtr pEntProvider,
        SketchUp::ISkpTextureWriter2Ptr m_pTextureWriter);

    static void WriteMaterials(SketchUp::ISkpDocumentPtr pDoc,
        String szMeshPrefix);

    static void createSubMesh(Ogre::MeshPtr ogreMesh,
        SketchUp::ISkpEntityProviderPtr pEntProvider,
        CTransform mTransform,
        String sMaterialName,
        SketchUp::ISkpTextureWriter2Ptr m_pTextureWriter,
        Ogre::Vector3 &min,
        Ogre::Vector3 &max,
        Ogre::Real &maxSquaredRadius);

    static bool convertskpToMesh(const Ogre::String & szskpFileName,
        const Ogre::String & szMeshPrefix);

    static Ogre::Entity* createEntityFromskp(const Ogre::String & entityName, 
        const Ogre::String & szskpFileName,
        Ogre::SceneManager* mSceneMgr);
};

#endif // #ifndef __Plugin_skp_h__