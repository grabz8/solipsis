
#include <IpNode.h>
#include <ConfFile.h>


#include <iostream>
#include "CppTest.h"


static void TestDistanceOp()
{
	//
	P2P::IPNode node1(0,0,0, P2P::IPNode::Point( 1.0f, 1.0f, 1.0f) );
	P2P::IPNode node2(0,0,0, P2P::IPNode::Point( 10.0f, 10.0f, 10.0f) );
	P2P::IPNode node3(0,0,0, P2P::IPNode::Point( 3.2f, 3.2f, 3.2f) );

	ASSURE( node1.getDistance( node1) == 0);	
	ASSURE( node1.getDistance( node2) == 15);

	ASSURE( node1.getDistance(node3 ) == 3);//3.8105

	
}


ADD_TEST(TestDistanceOp,IPNode, "TestDistanceOp")