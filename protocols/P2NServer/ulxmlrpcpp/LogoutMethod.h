#ifndef __LogoutMethod_h__
#define __LogoutMethod_h__

#include "AbstractP2NMethod.h"

namespace Solipsis {

class LogoutMethod : public AbstractP2NMethod
{
public :
    LogoutMethod(P2NServer* s);
    virtual ulxr::MethodResponse execute(const ulxr::MethodCall& calldata);
};

} // namespace Solipsis

#endif // #ifndef __LogoutMethod_h__