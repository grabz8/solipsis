#ifndef __Node_h__
#define __Node_h__

#include "NodeServer.h"
#include "Ogre.h"

/**
 *
 */
class Node
{
private:
    static Node* mSingleton;
	NodeServer mXMLPRCServer;

public:
    int mConnectionsCount;
    Ogre::Vector3 mAvatarPosition;
    Ogre::Quaternion mAvatarOrientation;
    time_t mFirstEvtDate;
    int mState;
    int mAvatarDirty;

public:
	/** Default Constructor
	 */
	Node(int port = 8550, int verbosity = 2);

	/** Destructor
	 */
	~Node();

    static Node* getSingletonPtr() { return mSingleton; }
    static Node& getSingleton() { return *mSingleton; }

private:
	/** Copie Constructor (not allowed)
	 */
	Node(const Node& node);

	/** Copy assignement operator (not allowed)
	 */
	Node& operator=(const Node& node);

public:

};

#endif // #ifndef __Node_h__