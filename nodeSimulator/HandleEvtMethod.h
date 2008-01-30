#ifndef __HandleEvtMethod_h__
#define __HandleEvtMethod_h__

#include "AbstractNodeMethod.h"
#include "INodeClient.h"

class HandleEvtMethod : public AbstractNodeMethod
{
public :
	HandleEvtMethod(NodeServer* s);
#ifdef ULXR
    virtual ulxr::MethodResponse execute(const ulxr::MethodCall& calldata);
#else
	virtual void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result);
#endif

protected:
    Solipsis::INodeClient::RetCode _execute(const Solipsis::INodeClient::NodeId& nodeId, std::string& xmlRespStr);
};

#endif // #ifndef __HandleEvtMethod_h__