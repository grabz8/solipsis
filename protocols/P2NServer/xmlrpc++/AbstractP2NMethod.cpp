#include "AbstractP2NMethod.h"
#include "P2NServer.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
AbstractP2NMethod::AbstractP2NMethod(const std::string& name, const std::string& help, P2NServer* s) :
    mServer(s),
    XmlRpc::XmlRpcServerMethod(name, s->getInternalServer()), 
    mHelp(help)
{
}

//-------------------------------------------------------------------------------------
AbstractP2NMethod::~AbstractP2NMethod()
{
}

//-------------------------------------------------------------------------------------
std::string AbstractP2NMethod::help()
{
	return mHelp;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
