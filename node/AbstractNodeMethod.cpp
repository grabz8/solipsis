#include "AbstractNodeMethod.h"

AbstractNodeMethod::AbstractNodeMethod(
	const std::string& name,
	const std::string& help,
	NodeServer* s) : 
		XmlRpc::XmlRpcServerMethod(name, (XmlRpc::XmlRpcServer* )s), 
		mHelp(help) {
	
}

AbstractNodeMethod::~AbstractNodeMethod(){
	//std::cout<<"Desctructeur de AbstractNodeMethod"<<std::endl;
}

std::string AbstractNodeMethod::help(void){
	return mHelp;
}
