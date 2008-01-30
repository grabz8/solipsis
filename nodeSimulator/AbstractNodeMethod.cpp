#include "AbstractNodeMethod.h"

AbstractNodeMethod::AbstractNodeMethod(
	const std::string& name,
	const std::string& help,
	NodeServer* s) : 
#ifdef ULXR
        mName(name),
        mServer(s),
#else
		XmlRpc::XmlRpcServerMethod(name, (XmlRpc::XmlRpcServer* )s), 
#endif
		mHelp(help)
{
}

AbstractNodeMethod::~AbstractNodeMethod()
{
}

std::string AbstractNodeMethod::help(void)
{
	return mHelp;
}
