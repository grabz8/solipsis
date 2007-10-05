#ifndef __OgreHelpers_h__
#define __OgreHelpers_h__

#include "Ogre.h"

using namespace Ogre;

class OgreHelpers
{
private:
    // No instanciation
    OgreHelpers() {};

public:
    // Retrieve recursivelly from 1 scene node all movable objects of 1 type
    static void getMovableObjectsList(SceneNode* node, const String movableType, std::list<MovableObject*> &movableObjectsList);

    // Convert 1 string representation of Real into 1 Real
    static bool convertString2Real(const String& real, Real& r);
    // Convert 1 string representation of Vector3 (ie. x, y, z) into 1 Vector3
    static bool convertString2Vector3(const String& vector, Vector3& v);
};

#endif // #ifndef __OgreHelpers_h__