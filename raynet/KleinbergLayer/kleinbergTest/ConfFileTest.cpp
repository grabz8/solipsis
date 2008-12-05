
#include <IpNode.h>
#include <ConfFile.h>
#include <LogFile.h>



#include <iostream>
#include "CppTest.h"

/*
ConfFile( const std::string& _path);

int readPort() const;
int gossipInterval()const;
int updateViewSize()const;
int viewSize()const;
std::vector<IPNode> initView() const;
*/

void BadTest()
{
	try{
		std::string strPATH = "..\\ConfFiles\\conffileUnitTestBad.txt";
		P2P::ConfFile<P2P::IPNode> cf( strPATH  );		
		ASSURE(false);
	}
	catch( P2P::ConfFileDataError& )
	{
	}

}

void testConfFile()
{
	using namespace P2P;
	/*
	PORT=700
	GOSSIP=1
	UPDATE_SIZE=3
	Peer=100,127.0.0.1
	Peer=200,127.0.0.1
	Peer=300,127.0.0.1
	Peer=400,127.0.0.1
	Peer=500,127.0.0.1
	Peer=600,127.0.0.1
	*/
	std::string strPATH = "..\\ConfFiles\\conffileUnitTestGood.txt";

	P2P::ConfFile<P2P::IPNode> cf( strPATH  );
	ASSURE( cf.protocolPort() == 2000);	
	ASSURE( cf.updateSize() == 3);
	ASSURE( cf.kleinbergGossipInterval() == 1);
	ASSURE( cf.voronoiGossipInterval() == 2);
	ASSURE( cf.raynetPort()  ==2002 );
	ASSURE( cf.updateSize() == 3);
	ASSURE( cf.viewSize() == 6);	
	ASSURE( cf.logLevel()  == "HIGH");	
	ASSURE( cf.logFile()  == "HELLO.TXT");	
	ASSURE( cf.tcpTimeOut()  == 5);

	


	std::vector<P2P::IPNode> expectedNodes;

	
	// Points are all zero because a confFile is not supposed to KNOW about Peer positions (peers know that)
	expectedNodes.push_back( IPNode(1, 2,::inet_addr("127.0.0.1"), 	Point3D(0.0,0.0,0.0) ) );
	expectedNodes.push_back( IPNode(3, 4,::inet_addr("127.0.0.1"), 	Point3D(0.0,0.0,0.0) ) );
	expectedNodes.push_back( IPNode(5, 6,::inet_addr("127.0.0.1"), 	Point3D(0.0,0.0,0.0) ) );
	expectedNodes.push_back( IPNode(7, 8,::inet_addr("127.0.0.1"), 	Point3D(0.0,0.0,0.0) ) );
	expectedNodes.push_back( IPNode(9, 10,::inet_addr("127.0.0.1"), Point3D(0.0,0.0,0.0) ) );
	
	
	ASSURE( cf.viewSize() == 6 );
	ASSURE( cf.initView() == expectedNodes);


	ASSURE( cf.position().getXpos()== 1.0);
	ASSURE( cf.position().getYpos()== 2.0);
	ASSURE( cf.position().getZpos() == 3.0);



}


ADD_TEST(BadTest,ConfFile, "Check bad conffiles fail")
ADD_TEST(testConfFile,ConfFile, "Access data")