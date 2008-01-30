#ifndef __SendEvtMethod_h__
#define __SendEvtMethod_h__

#include "AbstractNodeMethod.h"
#include "INodeClient.h"

class SendEvtMethod : public AbstractNodeMethod
{
public :
	SendEvtMethod(NodeServer* s);
#ifdef ULXR
    virtual ulxr::MethodResponse execute(const ulxr::MethodCall& calldata);
#else
	virtual void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result);
#endif

protected:
    Solipsis::INodeClient::RetCode _execute(const Solipsis::INodeClient::NodeId& nodeId, const std::string& xmlEvtStr, std::string& xmlRespStr);
};

#endif // #ifndef __SendEvtMethod_h__