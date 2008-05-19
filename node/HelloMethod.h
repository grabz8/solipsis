#ifndef __HelloMethod_h__
#define __HelloMethod_h__

#include "AbstractNodeMethod.h"

class HelloMethod : public AbstractNodeMethod {

public :
	HelloMethod(NodeServer* s) : AbstractNodeMethod("Hello", "Say Hello", s) {
	}
	
	virtual void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result){
		result = "Hello World"; 
	}

};

#endif // #ifndef __AbstractNodeMethod_h__