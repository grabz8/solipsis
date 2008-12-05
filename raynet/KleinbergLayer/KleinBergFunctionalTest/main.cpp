#include <KleinbergProtocol.h>
#include <IPNode.h>




// NODE CHANGEABILITY:
// Design Note: The node implementation can be statically modified 
//              simply by including a new node implementation and 
//              modifying the typedef
#include "IPNode.h"

#include <map>

int main ( int _argc, char ** _argv)
{
	try{

		using namespace P2P;

		std::string strConfFile;



		typedef std::map< IPNode, int> NodeCoefMap;
		NodeCoefMap NodeMap;



		P2P::ConfFile<IPNode> o_confFile( _argv[1] );
		P2P::LogFile::init(  o_confFile.logFile(), o_confFile.logLevel() );

		P2P::IPNode thisNode(	o_confFile.protocolPort(),
								o_confFile.raynetPort(),
								IP::getHostAddr(),
								o_confFile.position()
		) ;

		

		KleinbergProtocol<P2P::IPNode> facade	( 
				o_confFile.initView(),
				o_confFile.updateSize(),
				o_confFile.viewSize(),
				thisNode
		);
		


		while ( true )
		{

			std::vector<IPNode> Nodes = facade.getView();

			const P2P::IPNode::Point pos = P2P::ConfFile<IPNode>( _argv[1] ).position();
			const unsigned short port = P2P::ConfFile<IPNode>( _argv[1] ).protocolPort();

			::system("cls");
			std::cout	<< "\nBind Port:" << port
				<< "\tx=" << pos.getXpos()<< "\ty=" << pos.getYpos()<<"\tz=" << pos.getZpos();


			for ( std::vector<IPNode>::iterator it = Nodes.begin(); it != Nodes.end() ; it++ )
			{

				IN_ADDR addr;
				addr.S_un.S_addr = it->getAddr();
				std::cout	<< "\n Addr:" << ::inet_ntoa( addr ) <<",port:" << it->getProtocolPort() <<"\t"
					<<"x:" << it->getPoint().getXpos()
					<<"\ty:" << it->getPoint().getYpos()
					<<"\tz:" << it->getPoint().getZpos();

			}		

			::Sleep(1000);
		}

		std::cout	<< "Output\n";
		for ( NodeCoefMap::iterator it = NodeMap.begin(); it != NodeMap.end(); it++)
		{	
			IN_ADDR addr;
			addr.S_un.S_addr = it->first.getAddr();
			std::cout	<< "\n Addr:" << ::inet_ntoa( addr ) <<",port:" << it->first.getProtocolPort()
				<< " : " << it->second  <<" times";
		}

	}
	catch( std::exception& e)
	{
		std::cout << e.what();
	}

}