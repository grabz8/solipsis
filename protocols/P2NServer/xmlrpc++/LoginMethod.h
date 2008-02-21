#ifndef __LoginMethod_h__
#define __LoginMethod_h__

#include "AbstractP2NMethod.h"

namespace Solipsis {

class LoginMethod : public AbstractP2NMethod
{
public :
    LoginMethod(P2NServer* s);
    virtual void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result);
};

} // namespace Solipsis

#endif // #ifndef __LoginMethod_h__