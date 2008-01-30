#ifndef __AbstractNodeMethod_h__
#define __AbstractNodeMethod_h__

#include <string>
#ifdef ULXR
#include "ulxmlrpcpp/ulxmlrpcpp.h"
#include "ulxmlrpcpp/ulxr_call.h"
#include "ulxmlrpcpp/ulxr_response.h"
#else
#include <XmlRpc.h>
#endif

// Forward declaration
class NodeServer;

/** Abstract Class
 */
#ifdef ULXR
class AbstractNodeMethod
#else
class AbstractNodeMethod : public XmlRpc::XmlRpcServerMethod
#endif
{
protected:
	std::string mHelp;
#ifdef ULXR
	std::string mName;
    NodeServer* mServer;
#endif

protected:
	AbstractNodeMethod(const std::string& name, const std::string& help, NodeServer* s);

public:
	virtual ~AbstractNodeMethod();
	
#ifdef ULXR
    virtual ulxr::MethodResponse execute(const ulxr::MethodCall& calldata) = 0;
#else
	virtual void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result) = 0;
#endif
	
	std::string help(void);

};

#endif // #ifndef __AbstractNodeMethod_h__