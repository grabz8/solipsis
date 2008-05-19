#ifndef __AbstractNodeMethod_h__
#define __AbstractNodeMethod_h__

#include <iostream>
#include <string>

#include <XmlRpc.h>

//forward declaration
class NodeServer;

/** Abstract Class
 */
class AbstractNodeMethod : public XmlRpc::XmlRpcServerMethod {
private:
	std::string mHelp;

protected:
	AbstractNodeMethod(const std::string& name, const std::string& help, NodeServer* s);

public:
	virtual ~AbstractNodeMethod();
	
	virtual void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result) = 0;
	
	std::string help(void);

};

#endif // #ifndef __AbstractNodeMethod_h__