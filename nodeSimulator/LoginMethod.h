#ifndef __LoginMethod_h__
#define __LoginMethod_h__

#include "AbstractNodeMethod.h"
#include "INodeClient.h"

class LoginMethod : public AbstractNodeMethod
{
public :
	LoginMethod(NodeServer* s);
#ifdef ULXR
    virtual ulxr::MethodResponse execute(const ulxr::MethodCall& calldata);
#else
	virtual void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result);
#endif

protected:
    Solipsis::INodeClient::RetCode _execute(const std::string& xmlParamsStr, Solipsis::INodeClient::NodeId& nodeId, std::string& xmlRespStr);
};

#endif // #ifndef __LoginMethod_h__