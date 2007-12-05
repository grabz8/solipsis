#ifndef __OgreApplication_h__
#define __OgreApplication_h__

#include "IApplication.h"
#include "Ogre.h"

using namespace Ogre;

namespace Solipsis {

/** This class represents an Ogre application composed of several running instances.
 */
class OgreApplication : public IApplication
{
private: 
    Root* mRoot;

protected: 
    OgreApplication::OgreApplication();
    virtual ~OgreApplication(); 

public: 
    Root* getRoot() { return mRoot; }

    virtual bool initialize(bool configManagedByOgre = false, String windowTitle = "");
    virtual bool finalize();

    virtual void createResourceListener(); 
    virtual void addResourceLocations(); 
    virtual void initResources(); 
};

} // namespace Solipsis

#endif // #ifndef __OgreApplication_h__