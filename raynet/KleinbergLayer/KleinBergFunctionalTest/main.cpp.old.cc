#include "KleinBergFacade.h"

	
		// NODE CHANGEABILITY:
// Design Note: The node implementation can be statically modified 
//              simply by including a new node implementation and 
//              modifying the typedef
#include "IPNode.h"

#include <map>

int main ( int _argc, char ** _argv)
{
	
	using namespace P2P;

	std::string strConfFile;

	std::cout << "Bind Port:" << P2P::ConfFile( _argv[1] ).recvPort();

	typedef std::map< IPNode, int> NodeCoefMap;
	NodeCoefMap NodeMap;
/*

Protocol<KleinBergAlgo<Node>,Node>(
				KleinBergAlgo<Node>(	Node( m_o_confFile.recvPort(), Tools::getHostAddr() ) , 
										m_o_confFile.updateViewSize()			) 
				, m_o_confFile.initView() 
			) , 
			m_o_confFile
*/
	P2P::ConfFile o_confFile( _argv[1] );
	P2P::LogFile::setPath(  o_confFile.logPath() );

	ProtocolController<  KleinBergAlgo<IPNode>, IPNode > facade	( 
			Protocol< KleinBergAlgo<IPNode>, IPNode>(
				KleinBergAlgo<IPNode>(	IPNode( o_confFile.recvPort(), Tools::getHostAddr() ) , 
										o_confFile.updateViewSize()		
				),
				o_confFile.initView()
			),
			o_confFile
	);



	for ( int i = 0; i < 20; i++)
	{

		std::vector<IPNode> Nodes = facade.getView();
	
		for ( std::vector<IPNode>::iterator it = Nodes.begin(); it != Nodes.end() ; it++ )
		{
			NodeMap[*it] ++;
		}		
		::Sleep(1000);
	}

	std::cout	<< "Output\n";
	for ( NodeCoefMap::iterator it = NodeMap.begin(); it != NodeMap.end(); it++)
	{	
		IN_ADDR addr;
		addr.S_un.S_addr = it->first.getAddr();
		std::cout	<< "\n Addr:" << ::inet_ntoa( addr ) <<",port:" << it->first.getPort()
					<< " : " << it->second  <<" times";
	}

	
	
}