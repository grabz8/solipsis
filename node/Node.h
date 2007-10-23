#ifndef __Node_h__
#define __Node_h__

#include "NodeServer.h"

/**
 *
 */
class Node {
private:
	NodeServer mXMLPRCServer;

public:
	/** Default Constructor
	 */
	Node();

	/** Destructor
	 */
	~Node();

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
