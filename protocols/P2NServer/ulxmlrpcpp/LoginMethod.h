#ifndef __LoginMethod_h__
#define __LoginMethod_h__

#include "AbstractP2NMethod.h"

namespace Solipsis {

class LoginMethod : public AbstractP2NMethod
{
public :
    LoginMethod(P2NServer* s);
    virtual ulxr::MethodResponse execute(const ulxr::MethodCall& calldata);
};

} // namespace Solipsis

#endif // #ifndef __LoginMethod_h__