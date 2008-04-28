#ifndef __AbstractP2NMethod_h__
#define __AbstractP2NMethod_h__

#include <string>
#include <ulxmlrpcpp/ulxmlrpcpp.h>
#include <ulxmlrpcpp/ulxr_call.h>
#include <ulxmlrpcpp/ulxr_response.h>

namespace Solipsis {

// Forward declaration
class P2NServer;

/** Abstract Class
 */
class AbstractP2NMethod
{
protected:
    std::string mHelp;
    std::string mName;
    P2NServer* mServer;

protected:
    AbstractP2NMethod(const std::string& name, const std::string& help, P2NServer* s);

public:
    virtual ~AbstractP2NMethod();

    virtual ulxr::MethodResponse execute(const ulxr::MethodCall& calldata) = 0;

    std::string help();
};

} // namespace Solipsis

#endif // #ifndef __AbstractP2NMethod_h__