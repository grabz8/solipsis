#include "OgreHelpers.h"

void OgreHelpers::getMovableObjectsList(SceneNode* node, const String movableType, std::list<MovableObject*> &movableObjectsList)
{
    SceneNode::ObjectIterator objectIterator = node->getAttachedObjectIterator();
    while (objectIterator.hasMoreElements())
    {
        MovableObject* movableObject = objectIterator.getNext();
        if (movableObject->getMovableType().compare(movableType) == 0)
            movableObjectsList.push_back(movableObject);
    }
    Node::ChildNodeIterator childNodeIterator = node->getChildIterator();
    while (childNodeIterator.hasMoreElements())
    {
        Node* childNode = childNodeIterator.getNext();
        getMovableObjectsList((SceneNode*)childNode, movableType, movableObjectsList);
    }
}

bool OgreHelpers::convertString2Real(const String& real, Real& r)
{
    Real result = 0;

    if (1 != sscanf(real.c_str(), "%f", &result))
        return false;
    r = result;

    return true;
}

bool OgreHelpers::convertString2Vector3(const String& vector, Vector3& v)
{
    Vector3 result = Vector3::ZERO;
    size_t prev, comma;

    comma = vector.find_first_of("(", 0);
    if (comma == String::npos)
    {
        comma = -1;
        prev = 0;
    }
    else
    {
        comma += 1;
        prev = comma;
    }
    comma = vector.find_first_of(",", comma + 1);
    if (comma == String::npos) return false;
    if (1 != sscanf(vector.substr(prev, comma).c_str(), "%f", &result.x))
        return false;
    prev = comma + 1;
    comma = vector.find_first_of(",", comma + 1);
    if (comma == String::npos) return false;
    if (1 != sscanf(vector.substr(prev, comma).c_str(), "%f", &result.y))
        return false;
    prev = comma + 1;
    comma = vector.find_first_of(")", comma + 1);
    if (comma != String::npos)
        comma -= 1;
    if (1 != sscanf(vector.substr(prev, comma).c_str(), "%f", &result.z))
        return false;
    v = result;

    return true;
}
