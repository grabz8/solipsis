#ifndef __AbstractP2NMethod_h__
#define __AbstractP2NMethod_h__

#include <string>
#include <XmlRpc.h>

namespace Solipsis {

// Forward declaration
class P2NServer;

/** Abstract Class
 */
class AbstractP2NMethod : public XmlRpc::XmlRpcServerMethod
{
protected:
    std::string mHelp;
    P2NServer* mServer;

protected:
    AbstractP2NMethod(const std::string& name, const std::string& help, P2NServer* s);

public:
    virtual ~AbstractP2NMethod();

    virtual void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result) = 0;

    std::string help();
};

} // namespace Solipsis

#endif // #ifndef __AbstractP2NMethod_h__