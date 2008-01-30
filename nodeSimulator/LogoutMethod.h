#ifndef __LogoutMethod_h__
#define __LogoutMethod_h__

#include "AbstractNodeMethod.h"
#include "INodeClient.h"

class LogoutMethod : public AbstractNodeMethod
{
public :
	LogoutMethod(NodeServer* s);
#ifdef ULXR
    virtual ulxr::MethodResponse execute(const ulxr::MethodCall& calldata);
#else
	virtual void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result);
#endif
};

#endif // #ifndef __LogoutMethod_h__