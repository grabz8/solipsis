#ifndef __INodeClient_h__
#define __INodeClient_h__

#include <string>

namespace Solipsis {

/** This class represents a generic Node client interface.
*/
class INodeClient
{
public:
    /** Enumeration of possible return codes */
    enum RetCode {
        RCOk,
        RCError,
        RCNoEvt
    };
    /** Description of 1 node identifier */
    typedef std::string NodeId;

public:
    INodeClient() {}
    virtual ~INodeClient() {}

    /** login request.
    @remarks An implementation must be supplied for this method.
    @param xmlParams The xml-formatted string containing login parameters
    @param xmlResp The string containing the xml-formatted response
    @returns OK if call suceeded
    */
    virtual RetCode login(const std::string& xmlParams, std::string& xmlResp) = 0;
    /** logout request.
    @remarks An implementation must be supplied for this method.
    @returns OK if call suceeded
    */
    virtual RetCode logout() = 0;
    /** isConnected.
    @remarks An implementation must be supplied for this method.
    @returns True if client is connected
    */
    virtual bool isConnected() = 0;
    /** getNodeId.
    @remarks An implementation must be supplied for this method.
    @returns The nodeId if client is connected
    */
    virtual const NodeId& getNodeId() = 0;

    /** handleEvt request.
    @remarks An implementation must be supplied for this method.
    @param xmlResp The string containing the xml-formatted response
    @returns OK if call suceeded, NOEVT if no event to handle
    */
    virtual RetCode handleEvt(std::string& xmlResp) = 0;
    /** sendEvt request.
    @remarks An implementation must be supplied for this method.
    @param xmlEvt The string containing the xml-formatted event
    @param xmlResp The string containing the xml-formatted response
    @returns OK if call suceeded
    */
    virtual RetCode sendEvt(const std::string& xmlEvt, std::string& xmlResp) = 0;
};

} // end namespace

#endif // #ifndef __INodeClient_h__