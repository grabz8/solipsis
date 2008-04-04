#include "ULXMLRPCPPP2NClient.h"
#include "tinyxml.h"
#include <sstream>

#define LOG(message) if (mLogger != 0) mLogger->logMessage(message);

namespace Solipsis {

const char ULXMLRPCPPP2NClient::RETCODE_TAG[] = "retCode";
const char ULXMLRPCPPP2NClient::RESPONSE_TAG[] = "response";
const char ULXMLRPCPPP2NClient::NODEID_TAG[] = "nodeId";

//-------------------------------------------------------------------------------------
IP2NClient* IP2NClient::createClient(const std::string& host, int port, int verbosity, const std::string& extras)
{
    return new ULXMLRPCPPP2NClient(host, port, verbosity, extras);
}

//-------------------------------------------------------------------------------------
bool IP2NClient::destroyClient(IP2NClient* client)
{
    delete client;

    return true;
}

//-------------------------------------------------------------------------------------
ULXMLRPCPPP2NClient::ULXMLRPCPPP2NClient(const std::string& host, int port, int verbosity, const std::string& extras) :
    mHost(host),
    mPort(port),
    mVerbosity(verbosity),
    mExtras(extras),
    mNbAttempts(1),
    mNodeId(""),
    mConnected(false),
    mTcpIpConnection(0),
    mHttpProtocol(0),
    mHttpClient(0),
    mRpcClient(0),
    mCallsMutex(PTHREAD_MUTEX_INITIALIZER),
    mLogger(0)
{
    try {
        bool wbxml = false;
        bool useProxy = false;
        std::string proxyHost("localhost");
        unsigned proxyPort = 8080;
        std::string proxyUser("");
        std::string proxyPwd("");
        bool useConnect = false;
        bool chunked = false;
        ulxr::CppString host = ULXR_PCHAR(mHost);
        unsigned port = mPort;

        // process extra informations
        std::string value;
        if (getExtraInformation(mExtras, "wbxml=", value))
            wbxml = atoi(value.c_str()) ? true : false;
        if (getExtraInformation(mExtras, "useProxy=", value))
            useProxy = atoi(value.c_str()) ? true : false;
        if (!getExtraInformation(mExtras, "proxyHost=", proxyHost))
            useProxy = false;
        if (getExtraInformation(mExtras, "proxyPort=", value))
            proxyPort = atoi(value.c_str());
        else
            useProxy = false;
        if (!getExtraInformation(mExtras, "proxyUser=", proxyUser))
            useProxy = false;
        if (!getExtraInformation(mExtras, "proxyPwd=", proxyPwd))
            useProxy = false;
        if (getExtraInformation(mExtras, "useConnect=", value))
            useConnect = atoi(value.c_str()) ? true : false;
        if (getExtraInformation(mExtras, "chunked=", value))
            chunked = atoi(value.c_str()) ? true : false;
        if (getExtraInformation(mExtras, "nattempts=", value))
            mNbAttempts = atoi(value.c_str());

        std::cout << ULXR_PCHAR("Initializing rpc client to ") << host << ULXR_PCHAR(":") << port << std::endl;
        std::cout << ULXR_PCHAR("WBXML: ") << wbxml << std::endl
                  << ULXR_PCHAR("Proxy: ") << useProxy << std::endl
                  << ULXR_PCHAR("Proxy-Connect: ") << useConnect << std::endl
                  << ULXR_PCHAR("Chunked transfer: ") << chunked << std::endl;

        mTcpIpConnection = new ulxr::TcpIpConnection(false, host, port);
        if (useProxy)
            mTcpIpConnection->setProxy(ULXR_PCHAR(proxyHost.c_str()), proxyPort);

        mHttpProtocol = new ulxr::HttpProtocol(mTcpIpConnection);
        if (useProxy)
            mHttpProtocol->setProxyAuthentication(ULXR_PCHAR(proxyUser.c_str()), ULXR_PCHAR(proxyPwd.c_str()));
        if (useConnect)
            mHttpProtocol->enableConnect();
        mHttpProtocol->setChunkedTransfer(chunked);

#ifdef _DEBUG
        mTcpIpConnection->setTimeout(30);  // debugging output needs lots of time
#endif
        mHttpClient = new ulxr::HttpClient(mHttpProtocol);

        mRpcClient = new ulxr::Requester(mHttpProtocol, wbxml);

        mTcpIpConnection->open();

        mHttpProtocol->setPersistent(true);
    }
    catch (ulxr::Exception& ex)
    {
        std::cout << ULXR_PCHAR("Error occured: ") << ULXR_GET_STRING(ex.why()) << std::endl;
        throw;
    }
    catch(...)
    {
        std::cout << ULXR_PCHAR("Unknown error occured\n");
        throw;
    }
}

//-------------------------------------------------------------------------------------
ULXMLRPCPPP2NClient::~ULXMLRPCPPP2NClient()
{
    delete mRpcClient;
    delete mHttpClient;
    delete mHttpProtocol;
    delete mTcpIpConnection;
}

//-------------------------------------------------------------------------------------
void ULXMLRPCPPP2NClient::shareCnx(IP2NClient* P2NClient)
{
    mNodeId = ((ULXMLRPCPPP2NClient*)P2NClient)->mNodeId;
    mConnected = ((ULXMLRPCPPP2NClient*)P2NClient)->mConnected;
}

//-------------------------------------------------------------------------------------
IP2NClient::RetCode ULXMLRPCPPP2NClient::login(const std::string& xmlParams, std::string& xmlResp)
{
    IP2NClient::RetCode retCode = IP2NClient::RCError;

    xmlResp.clear();
    ulxr::MethodCall loginMethodCall("Login");
    loginMethodCall.setParam(ulxr::RpcString(ulxr::Cpp8BitString(xmlParams)));
    ulxr::MethodResponse result;
    mConnected = false;
    bool success = this->executeThreadSafe(loginMethodCall, result);
    if (success)
    {
        // Parse response
        ulxr::Struct resultStruct = result.getResult();
        retCode = (IP2NClient::RetCode)(int)((ulxr::Integer)resultStruct.getMember(ULXR_PCHAR(RETCODE_TAG))).getInteger();
        if (retCode == IP2NClient::RCError)
        {
            LOG("ULXMLRPCPPP2NClient::login() retCode=" + convert2string(retCode) + ", Error:" + ((ulxr::RpcString)resultStruct.getMember(ULXR_PCHAR(RESPONSE_TAG))).getString());
            return retCode;
        }
        mNodeId = ((ulxr::RpcString)resultStruct.getMember(ULXR_PCHAR(NODEID_TAG))).getString();
        xmlResp = ((ulxr::RpcString)resultStruct.getMember(ULXR_PCHAR(RESPONSE_TAG))).getString();
        TiXmlDocument xmlDoc;
        xmlDoc.Parse(xmlResp.c_str());
        // check for errors
        if (xmlDoc.Error())
        {
            LOG("ULXMLRPCPPP2NClient::login() Unable to parse response !");
            return IP2NClient::RCError;
        }
        mConnected = true;
        LOG("ULXMLRPCPPP2NClient::login()\nxmlParams=\n" + xmlParams + "\nxmlResp=\n" + xmlResp + "\nretCode=" + convert2string(retCode) + "\nmNodeId=" + mNodeId);
    }

    return retCode;
}

//-------------------------------------------------------------------------------------
IP2NClient::RetCode ULXMLRPCPPP2NClient::logout()
{
    IP2NClient::RetCode retCode = IP2NClient::RCError;

    if (!mConnected)
        return IP2NClient::RCOk;

    ulxr::MethodCall logoutMethodCall("Logout");
    logoutMethodCall.setParam(ulxr::RpcString(ulxr::Cpp8BitString(mNodeId)));
    ulxr::MethodResponse result;
    bool success = this->executeThreadSafe(logoutMethodCall, result);
    if (success)
    {
        // Parse response
        ulxr::Struct resultStruct = result.getResult();
        retCode = (IP2NClient::RetCode)(int)((ulxr::Integer)resultStruct.getMember(ULXR_PCHAR(RETCODE_TAG))).getInteger();
        if (retCode == IP2NClient::RCError)
        {
            LOG("ULXMLRPCPPP2NClient::logout() retCode=" + convert2string(retCode) + ", Error:" + ((ulxr::RpcString)resultStruct.getMember(ULXR_PCHAR(RESPONSE_TAG))).getString());
            return retCode;
        }
        mConnected = false;
        LOG("ULXMLRPCPPP2NClient::logout() retCode=" + convert2string(retCode));
    }

    return retCode;
}

//-------------------------------------------------------------------------------------
bool ULXMLRPCPPP2NClient::isConnected()
{
    return mConnected;
}

//-------------------------------------------------------------------------------------
const NodeId& ULXMLRPCPPP2NClient::getNodeId()
{
    return mNodeId;
}

//-------------------------------------------------------------------------------------
IP2NClient::RetCode ULXMLRPCPPP2NClient::handleEvt(std::string& xmlResp)
{
    IP2NClient::RetCode retCode = IP2NClient::RCError;

    xmlResp.clear();
    ulxr::MethodCall handleEvtMethodCall("HandleEvt");
    handleEvtMethodCall.setParam(ulxr::RpcString(ulxr::Cpp8BitString(mNodeId)));
    ulxr::MethodResponse result;
    bool success = this->executeThreadSafe(handleEvtMethodCall, result);
    if (success)
    {
        // Parse response
        ulxr::Struct resultStruct = result.getResult();
        retCode = (IP2NClient::RetCode)(int)((ulxr::Integer)resultStruct.getMember(ULXR_PCHAR(RETCODE_TAG))).getInteger();
        if (retCode == IP2NClient::RCError)
        {
            LOG("ULXMLRPCPPP2NClient::handleEvt() retCode=" + convert2string(retCode) + ", Error:" + ((ulxr::RpcString)resultStruct.getMember(ULXR_PCHAR(RESPONSE_TAG))).getString());
            return retCode;
        }
        xmlResp = ((ulxr::RpcString)resultStruct.getMember(ULXR_PCHAR(RESPONSE_TAG))).getString();
//        LOG("ULXMLRPCPPP2NClient::handleEvt()\nxmlResp=\n" + xmlResp + "\nretCode=" + convert2string(retCode));
    }

    return retCode;
}

//-------------------------------------------------------------------------------------
IP2NClient::RetCode ULXMLRPCPPP2NClient::sendEvt(const std::string& xmlEvt, std::string& xmlResp)
{
    IP2NClient::RetCode retCode = IP2NClient::RCError;

    xmlResp.clear();
    ulxr::MethodCall sendEvtMethodCall("SendEvt");
    sendEvtMethodCall.setParam(ulxr::RpcString(ulxr::Cpp8BitString(mNodeId)))
        .addParam(ulxr::RpcString(ulxr::Cpp8BitString(xmlEvt)));
    ulxr::MethodResponse result;
    bool success = this->executeThreadSafe(sendEvtMethodCall, result);
    if (success)
    {
        // Parse response
        ulxr::Struct resultStruct = result.getResult();
        retCode = (IP2NClient::RetCode)(int)((ulxr::Integer)resultStruct.getMember(ULXR_PCHAR(RETCODE_TAG))).getInteger();
        if (retCode == IP2NClient::RCError)
        {
            LOG("ULXMLRPCPPP2NClient::sendEvt() retCode=" + convert2string(retCode) + ", Error:" + ((ulxr::RpcString)resultStruct.getMember(ULXR_PCHAR(RESPONSE_TAG))).getString());
            return retCode;
        }
        xmlResp = ((ulxr::RpcString)resultStruct.getMember(ULXR_PCHAR(RESPONSE_TAG))).getString();
//        LOG("ULXMLRPCPPP2NClient::sendEvt()\nxmlEvt=\n" + xmlEvt + "\nxmlResp=\n" + xmlResp + "\nretCode=" + convert2string(retCode));
    }

    return retCode;
}

//-------------------------------------------------------------------------------------
bool ULXMLRPCPPP2NClient::executeThreadSafe(ulxr::MethodCall const& method, ulxr::MethodResponse& result)
{
    bool success = false;

    if (pthread_mutex_lock(&mCallsMutex) != 0)
        return false;
    for(int attempts=mNbAttempts;attempts>0;attempts--)
    {
        result = mRpcClient->call(method, ULXR_PCHAR("/RPC2"), "", "");
        success = result.isOK();

        // Fault ?
        if (!result.isOK())
        {
            ulxr::Struct faultStruct = result.getResult();
            int faultCode = ((ulxr::Integer)faultStruct.getMember(ULXR_PCHAR("faultCode"))).getInteger();
            std::string faultString = ((ulxr::RpcString)faultStruct.getMember(ULXR_PCHAR("faultString"))).getString();
            LOG("ULXMLRPCPPP2NClient::executeThreadSafe() Method=" + method.getMethodName() + ", Fault:" + convert2string(faultCode) + ", " + faultString);
            success = false;
            // Timeout ? next attempt ...
            if (faultCode == 500)
                continue;
        }
        break;
    }
    if (pthread_mutex_unlock(&mCallsMutex) != 0)
        return false;

    return success;
}

//-------------------------------------------------------------------------------------
std::string ULXMLRPCPPP2NClient::convert2string(int value)
{
    std::stringstream sstr;
    sstr << value;
    return (std::string)sstr.str();
}

//-------------------------------------------------------------------------------------
bool ULXMLRPCPPP2NClient::getExtraInformation(const std::string& extras, const std::string& information, std::string& value)
{
    std::string::size_type idx, spc;
    idx = extras.find(information);
    if (idx == std::string::npos)
        return false;
    idx += information.length();
    if (extras.length() < idx)
        return false;
    spc = extras.find(' ', idx);
    if (spc == idx)
        return false;
    if (spc == std::string::npos)
        spc = extras.length();
    value = extras.substr(idx, spc - idx + 1);
    return true;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
