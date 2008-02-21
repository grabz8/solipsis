#ifndef __NavigatorXMLRPCClient_h__
#define __NavigatorXMLRPCClient_h__

#include <list>
#include "IP2NClient.h"
#include "XmlDatas.h"
#include "Ogre.h"

using namespace Ogre;

namespace Solipsis {

/** This class manages XMLRPC requests and responses processed by the Navigator.
 */
class NavigatorXMLRPCClient : public IP2NClientLogger
{
protected:
    IP2NClient* mP2NClient;
    IP2NClient* mSharedCnx;

public:
    NavigatorXMLRPCClient(const std::string& host, int port, const std::string& extras);
    NavigatorXMLRPCClient(NavigatorXMLRPCClient& sharedCnx);
    ~NavigatorXMLRPCClient();

    /** log on Solipsis host.
    @param xmlLogin Login parameters
    @param myEntities List of owned entities
    @returns True if login succeeded
    */
    bool login(const XmlLogin& xmlLogin, std::list<EntityUID>& myXmlEntities);
    /** log off Solipsis host.
    @returns True if logout succeeded
    */
    bool logout();
    /** test if client is connected to Solipsis host.
    @returns True if client is connected
    */
    bool isConnected();

    /** handle an event.
    @param xmlEvt If exists, the allocated event to handle
    @returns True if handle operation succeded (even if no event returned)
    */
    bool handleEvt(XmlEvt** xmlEvt);
    /** send an event.
    @param xmlEvt The event to send
    @param xmlResp The response
    @returns True if the event was correctly sent
    */
    bool sendEvt(const XmlEvt& xmlEvt, std::string& xmlResp);

protected:
    /// @copydoc IP2NClientLogger::logMessage
    virtual void logMessage(const std::string& message);
};

} // namespace Solipsis

#endif // #ifndef __NavigatorXMLRPCClient_h__