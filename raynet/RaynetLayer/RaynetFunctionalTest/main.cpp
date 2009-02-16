
#include <RaynetFacade.h>
#include <IPNode.h>
#include <ConfFile.h>
#include <RaynetMessage.h>





class Myhandler : public P2P::RaynetHandler
{
	
	public:
	Myhandler(){}

	
	virtual void evRaynetMessage( P2P::RaynetMessagePtr _pPackage )	
	{

	}	
	virtual void evRaynetMessage( const P2P::RaynetMessage& _INpackage )	
	{

	}	
};




//GREG BEGIN
//int main ( int, void **)
int main ( int argc, char **argv)
//GREG END
{
	using namespace P2P;

//GREG BEGIN
    std::string confFilename = "  ";
    if (argc > 1)
        confFilename = std::string(argv[1]);
//	ConfFile<IPNode> confFile("  ");	
	ConfFile<IPNode> confFile(confFilename);	
//GREG END

	Myhandler  handler;

	RaynetFacade<IPNode> raynet( handler , confFile  );

	char* pData="hello I am on mars";

	RaynetMessagePtr pPacket( new RaynetMessage(IPNode::Point (1,1,1),  pData, ::strlen(pData) +1  )  );	
	raynet.asyncRoute( pPacket );


	RaynetMessage syncPacket(  IPNode::Point (1,1,1),  pData, ::strlen(pData) +1  );	
	raynet.syncRoute( syncPacket );	

	return 0;
}