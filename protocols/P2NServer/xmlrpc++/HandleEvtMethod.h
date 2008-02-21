#ifndef __HandleEvtMethod_h__
#define __HandleEvtMethod_h__

#include "AbstractP2NMethod.h"

namespace Solipsis {

class HandleEvtMethod : public AbstractP2NMethod
{
public :
    HandleEvtMethod(P2NServer* s);
    virtual void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result);
};

} // namespace Solipsis

#endif // #ifndef __HandleEvtMethod_h__