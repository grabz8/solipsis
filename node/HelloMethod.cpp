#ifndef __HelloMethod_h__
#define __HelloMethod_h__

#include "AbstractNodeMethod.h"

class HelloMethod : public AbstractNodeMethod {

	HelloMethod(NodeServer* s, const std::string& help) 
		: AbstractNodeMethod("Hello", "Say Hello", s) {
	}
	
	virtual void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result){
		return = "Hello World"; 
	}

};

#endif // #ifndef __AbstractNodeMethod_h__