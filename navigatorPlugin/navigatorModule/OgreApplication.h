#ifndef __OgreApplication_h__
#define __OgreApplication_h__

#include "Instance.h"
#include "IWindow.h"
#include "Ogre.h"

class OgreApplication
{
public: 
    OgreApplication::OgreApplication();
    virtual ~OgreApplication(); 

    bool registerInstance(NavigatorModule::Instance* instance, NavigatorModule::IWindow* window);
    unsigned int unregisterInstance(NavigatorModule::Instance* instance);

    Ogre::Root* getRoot() { return mRoot; }
    unsigned int getNumInstances() { return mNumInstances; }

    bool initialize();
    bool finalize();

    virtual void createResourceListener(); 
    virtual void addResourceLocations(); 
    virtual void initResources(); 

private: 
    Ogre::Root* mRoot;
    unsigned int mNumInstances;
};

#endif // #ifndef __OgreApplication_h__