#ifndef __SendEvtMethod_h__
#define __SendEvtMethod_h__

#include "AbstractP2NMethod.h"

namespace Solipsis {

class SendEvtMethod : public AbstractP2NMethod
{
public :
    SendEvtMethod(P2NServer* s);
    virtual void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result);
};

} // namespace Solipsis

#endif // #ifndef __SendEvtMethod_h__