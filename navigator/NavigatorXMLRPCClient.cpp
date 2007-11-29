#include "NavigatorXMLRPCClient.h"
#include "OgreHelpers.h"
#include "DebugHelpers.h"
#include "Platform.h"

using namespace Solipsis;

//-------------------------------------------------------------------------------------
NavigatorXMLRPCClient::NavigatorXMLRPCClient(const char *host, int port, const char *uri) :
    XmlRpcClient(host, port, uri),
    mConnected(false),
    mCallsMutex(PTHREAD_MUTEX_INITIALIZER)
{
}

//-------------------------------------------------------------------------------------
NavigatorXMLRPCClient::~NavigatorXMLRPCClient()
{
    //Disconnect from node
    if (mConnected)
        disconnect();
}

//-------------------------------------------------------------------------------------
bool NavigatorXMLRPCClient::connect()
{
    const XmlRpc::XmlRpcValue value;
    const XmlRpc::XmlRpcValue noParam;
    XmlRpc::XmlRpcValue result;
#ifdef UNPLUG_MODE
    char csaXml[] = "<value>cid</value>";
    int offset = 0;
    result.fromXml(csaXml, &offset);
    mConnected = true;
#else
    mConnected = this->executeThreadSafe("Connect", noParam, result); //OGRE_LOG("result.toXml()");
#endif

    if (mConnected)
    {
        mConnectionId = removeFirstLevelOfXmlAnchor(String(result.toXml()));
        OGRE_LOG("Connection ID : " + mConnectionId);
    }

    return mConnected;
}

//-------------------------------------------------------------------------------------
bool NavigatorXMLRPCClient::disconnect()
{
    // Disconnect
    if ((mConnected == true) && (!mConnectionId.empty()))
    {
        const XmlRpc::XmlRpcValue param(mConnectionId);
        XmlRpc::XmlRpcValue result;
#ifdef UNPLUG_MODE
        char csaXml[] = "";
        int offset = 0;
        result.fromXml(csaXml, &offset);
        mConnected = false;
#else
        mConnected = !(this->executeThreadSafe("Disconnect", param, result));
#endif
    }
    return (!mConnected);
}

//-------------------------------------------------------------------------------------
bool NavigatorXMLRPCClient::isConnected()
{
    return mConnected;
}

//-------------------------------------------------------------------------------------
bool NavigatorXMLRPCClient::getAllPeers(std::list<Peer*> &peersList)
{
    const XmlRpc::XmlRpcValue param(mConnectionId);
    XmlRpc::XmlRpcValue result;

#ifdef UNPLUG_MODE
    //************ FAKE RESULT for demo purpose **/ 
    /* Here is and example of result from Solipsis Python Node v1 :
    <value>
        <array>
            <data>

                <value>
                    <struct>
                        <member>
                            <name>address</name>
                            <value>
                                <struct>
                                    <member>
                                        <name>host</name>
                                        <value>192.33.178.29</value>
                                    </member>
                                    <member>
                                        <name>port</name>
                                        <value><i4>5514</i4></value>
                                    </member>
                                </struct>
                            </value>
                        </member>
                        <member>
                            <name>awareness_radius</name>
                                <value>
                                    <double>41529791677676826000000000000000000000.000000</double>
                                </value>
                        </member>
                        <member>
                            <name>id_</name>
                            <value>5500_14_3f1bf4a1408a5e4462c51053a14d5b5cf1e28c7a</value>
                        </member>
                        <member>
                            <name>languages</name>
                            <value>
                                <array>
                                    <data>
                                        <value>fr</value>
                                        <value>en</value>
                                    </data>
                                </array>
                            </value>
                        </member>
                        <member>
                            <name>position</name>
                            <value>
                                <array>
                                    <data>
                                        <value>173822792765251003703866450027445485568</value>
                                        <value>152993429928224644246366999440628121600</value>
                                        <value>0</value>
                                    </data>
                                </array>
                            </value>
                        </member>
                        <member>
                            <name>pseudo</name>
                            <value>e6-otoch</value>
                        </member>
                        <member>
                            <name>services</name>
                            <value><struct></struct></value>
                        </member>
                    </struct>
                </value>
                
                <value>
                    <struct>
                        <member>
                            <name>address</name>
                            <value>
                                <struct>
                                    <member>
                                        <name>host</name>
                                        <value>192.33.178.29</value>
                                    </member>
                                    <member>
                                        <name>port</name>
                                        <value><i4>5072</i4></value>
                                    </member>
                                </struct>
                            </value>
                        </member>
                        <member>
                            <name>awareness_radius</name>
                            <value>
                                <double>106094510752704200000000000000000000000.000000</double>
                            </value>
                        </member>
                        <member>
                            <name>id_</name>
                            <value>5061_11_08312f6affa5059b69cfcc461d43c06a928a3068</value>
                        </member>
                        <member>
                            <name>languages</name>
                            <value>
                                <array>
                                    <data>
                                        <value>fr</value>
                                        <value>en</value>
                                    </data>
                                </array>
                            </value>
                        </member>
                        <member>
                            <name>position</name>
                            <value>
                                <array>
                                    <data>
                                        <value>92723195369998900382691924526011252736</value>
                                        <value>92990617197029383694826744946911870976</value>
                                        <value>0</value>
                                    </data>
                                </array>
                            </value>
                        </member>
                        <member>
                            <name>pseudo</name>
                            <value>c3-bolig</value>
                        </member>
                        <member>
                            <name>services</name>
                            <value><struct></struct></value>
                        </member>
                    </struct>
                </value>

            </data>
        </array>
    </value>
    */
    //char csaXml[] = "<value><array><data><value><struct><member><name>address</name><value><struct><member><name>host</name><value>192.33.178.29</value></member><member><name>port</name><value><i4>5514</i4></value></member></struct></value></member><member><name>awareness_radius</name><value><double>41529791677676826000000000000000000000.000000</double></value></member><member><name>id_</name><value>5500_14_3f1bf4a1408a5e4462c51053a14d5b5cf1e28c7a</value></member><member><name>languages</name><value><array><data><value>fr</value><value>en</value></data></array></value></member><member><name>position</name><value><array><data><value>173822792765251003703866450027445485568</value><value>152993429928224644246366999440628121600</value><value>0</value></data></array></value></member><member><name>pseudo</name><value>e6-otoch</value></member><member><name>services</name><value><struct></struct></value></member></struct></value><value><struct><member><name>address</name><value><struct><member><name>host</name><value>192.33.178.29</value></member><member><name>port</name><value><i4>5072</i4></value></member></struct></value></member><member><name>awareness_radius</name><value><double>106094510752704200000000000000000000000.000000</double></value></member><member><name>id_</name><value>5061_11_08312f6affa5059b69cfcc461d43c06a928a3068</value></member><member><name>languages</name><value><array><data><value>fr</value><value>en</value></data></array></value></member><member><name>position</name><value><array><data><value>92723195369998900382691924526011252736</value><value>92990617197029383694826744946911870976</value><value>0</value></data></array></value></member><member><name>pseudo</name><value>c3-bolig</value></member><member><name>services</name><value><struct></struct></value></member></struct></value><value><struct><member><name>address</name><value><struct><member><name>host</name><value>192.33.178.29</value></member><member><name>port</name><value><i4>5522</i4></value></member></struct></value></member><member><name>awareness_radius</name><value><double>57497016563131985000000000000000000000.000000</double></value></member><member><name>id_</name><value>5500_22_4e43908b0f027d4b3fed490282c0218cb29422c0</value></member><member><name>languages</name><value><array><data><value>fr</value><value>en</value></data></array></value></member><member><name>position</name><value><array><data><value>114643077511635545053898715355565522944</value><value>147745454614083710247737340241073143808</value><value>0</value></data></array></value></member><member><name>pseudo</name><value>j8-dom</value></member><member><name>services</name><value><struct></struct></value></member></struct></value><value><struct><member><name>address</name><value><struct><member><name>host</name><value>192.33.178.29</value></member><member><name>port</name><value><i4>6015</i4></value></member></struct></value></member><member><name>awareness_radius</name><value><double>113768004718250700000000000000000000000.000000</double></value></member><member><name>id_</name><value>6000_15_bca63b11f1e0380020a597c795bcf789e3024904</value></member><member><name>languages</name><value><array><data><value>fr</value><value>en</value></data></array></value></member><member><name>position</name><value><array><data><value>152990647870498603370442299242415390720</value><value>83592804703463022932728128387757899776</value><value>0</value></data></array></value></member><member><name>pseudo</name><value>e3-kas</value></member><member><name>services</name><value><struct></struct></value></member></struct></value><value><struct><member><name>address</name><value><struct><member><name>host</name><value>192.33.178.29</value></member><member><name>port</name><value><i4>6024</i4></value></member></struct></value></member><member><name>awareness_radius</name><value><double>114482020885780310000000000000000000000.000000</double></value></member><member><name>id_</name><value>6000_24_352a62dfe9f80c6a13c878fd1b0509fcf30229c5</value></member><member><name>languages</name><value><array><data><value>fr</value><value>en</value></data></array></value></member><member><name>position</name><value><array><data><value>91534492478291524504682165277183442944</value><value>52007856065097067078347429374215585792</value><value>0</value></data></array></value></member><member><name>pseudo</name><value>c2-bustaour</value></member><member><name>services</name><value><struct></struct></value></member></struct></value><value><struct><member><name>address</name><value><struct><member><name>host</name><value>192.33.178.29</value></member><member><name>port</name><value><i4>5041</i4></value></member></struct></value></member><member><name>awareness_radius</name><value><double>111169932927944450000000000000000000000.000000</double></value></member><member><name>id_</name><value>5031_10_153cfeb87b6fa97000367164d9adcc7ec2bd980b</value></member><member><name>languages</name><value><array><data><value>fr</value><value>en</value></data></array></value></member><member><name>position</name><value><array><data><value>161777618195416364175076177898689789952</value><value>11150160611606016283806178638083129344</value><value>0</value></data></array></value></member><member><name>pseudo</name><value>e1-casa</value></member><member><name>services</name><value><struct></struct></value></member></struct></value><value><struct><member><name>address</name><value><struct><member><name>host</name><value>192.33.178.29</value></member><member><name>port</name><value><i4>5010</i4></value></member></struct></value></member><member><name>awareness_radius</name><value><double>127794222606647670000000000000000000000.000000</double></value></member><member><name>id_</name><value>5001_9_baf5dbf83fb41c69796d008e684f887dd139a79a</value></member><member><name>languages</name><value><array><data><value>fr</value><value>en</value></data></array></value></member><member><name>position</name><value><array><data><value>107670376319930774539550138914211627008</value><value>129273251655584169704700238274158919680</value><value>0</value></data></array></value></member><member><name>pseudo</name><value>d4-haza</value></member><member><name>services</name><value><struct></struct></value></member></struct></value><value><struct><member><name>address</name><value><struct><member><name>host</name><value>192.33.178.29</value></member><member><name>port</name><value><i4>5065</i4></value></member></struct></value></member><member><name>awareness_radius</name><value><double>125154257694235260000000000000000000000.000000</double></value></member><member><name>id_</name><value>5061_4_6af7bd8e2de0e65070ac52bac00e30591a33061d</value></member><member><name>languages</name><value><array><data><value>fr</value><value>en</value></data></array></value></member><member><name>position</name><value><array><data><value>155492164754751172365434932101510070272</value><value>8897885066601472835710465134326448128</value><value>0</value></data></array></value></member><member><name>pseudo</name><value>e1-koti</value></member><member><name>services</name><value><struct></struct></value></member></struct></value><value><struct><member><name>address</name><value><struct><member><name>host</name><value>192.33.178.29</value></member><member><name>port</name><value><i4>5040</i4></value></member></struct></value></member><member><name>awareness_radius</name><value><double>121937961563838870000000000000000000000.000000</double></value></member><member><name>id_</name><value>5031_9_4f71a70c4bb75af0691dbc9c6eb32fd37e1b3a16</value></member><member><name>languages</name><value><array><data><value>fr</value><value>en</value></data></array></value></member><member><name>position</name><value><array><data><value>171969811357398265007864166937941508096</value><value>23714309345344550207570369625832030208</value><value>0</value></data></array></value></member><member><name>pseudo</name><value>f1-thus</value></member><member><name>services</name><value><struct></struct></value></member></struct></value><value><struct><member><name>address</name><value><struct><member><name>host</name><value>192.33.178.29</value></member><member><name>port</name><value><i4>5046</i4></value></member></struct></value></member><member><name>awareness_radius</name><value><double>84939723232037905000000000000000000000.000000</double></value></member><member><name>id_</name><value>5046_0_11a205c71f3439ff5a5f80e3f8fdb316881807db</value></member><member><name>languages</name><value><array><data><value>fr</value><value>en</value></data></array></value></member><member><name>position</name><value><array><data><value>130347786247931838429828761695012519936</value><value>164919735899381197555387609737649455104</value><value>0</value></data></array></value></member><member><name>pseudo</name><value>d5-maison</value></member><member><name>services</name><value><struct></struct></value></member></struct></value><value><struct><member><name>address</name><value><struct><member><name>host</name><value>192.33.178.29</value></member><member><name>port</name><value><i4>5061</i4></value></member></struct></value></member><member><name>awareness_radius</name><value><double>121632319968519450000000000000000000000.000000</double></value></member><member><name>id_</name><value>5061_0_092ce7961bc3f5e17c7b7d6d50df0535548e29cd</value></member><member><name>languages</name><value><array><data><value>fr</value><value>en</value></data></array></value></member><member><name>position</name><value><array><data><value>144658699946690554829591375804314419200</value><value>120395270974516236579116154136970657792</value><value>0</value></data></array></value></member><member><name>pseudo</name><value>e4-viv</value></member><member><name>services</name><value><struct></struct></value></member></struct></value><value><struct><member><name>address</name><value><struct><member><name>host</name><value>192.33.178.29</value></member><member><name>port</name><value><i4>6009</i4></value></member></struct></value></member><member><name>awareness_radius</name><value><double>113741482561274380000000000000000000000.000000</double></value></member><member><name>id_</name><value>6000_9_1d1349ce02c2314eea9314a2acf39937c318e46d</value></member><member><name>languages</name><value><array><data><value>fr</value><value>en</value></data></array></value></member><member><name>position</name><value><array><data><value>153044421787095013255755179842240249856</value><value>93558678657299572899216269684282228736</value><value>0</value></data></array></value></member><member><name>pseudo</name><value>e3-haza</value></member><member><name>services</name><value><struct></struct></value></member></struct></value><value><struct><member><name>address</name><value><struct><member><name>host</name><value>192.33.178.29</value></member><member><name>port</name><value><i4>5060</i4></value></member></struct></value></member><member><name>awareness_radius</name><value><double>166595234877758420000000000000000000000.000000</double></value></member><member><name>id_</name><value>5046_14_80777bc2b82fe79712c6490ce8828627602e2d13</value></member><member><name>languages</name><value><array><data><value>fr</value><value>en</value></data></array></value></member><member><name>position</name><value><array><data><value>161125870763379300323950601566502780928</value><value>103844638980674834478176265941750382592</value><value>0</value></data></array></value></member><member><name>pseudo</name><value>e4-bustaour</value></member><member><name>services</name><value><struct></struct></value></member></struct></value><value><struct><member><name>address</name><value><struct><member><name>host</name><value>192.33.178.29</value></member><member><name>port</name><value><i4>5003</i4></value></member></struct></value></member><member><name>awareness_radius</name><value><double>129708766754633480000000000000000000000.000000</double></value></member><member><name>id_</name><value>5001_2_49bcc5fa46582029c6b3c6cd0ae9a8bc2ed22e13</value></member><member><name>languages</name><value><array><data><value>fr</value><value>en</value></data></array></value></member><member><name>position</name><value><array><data><value>146687086842067842349369297483885182976</value><value>83281232912776020071688095235821273088</value><value>0</value></data></array></value></member><member><name>pseudo</name><value>e3-ikhaya</value></member><member><name>services</name><value><struct></struct></value></member></struct></value></data></array></value>";
    char csaXml[] = "<value><array><data><value><struct><member><name>address</name><value><struct><member><name>host</name><value>192.33.178.29</value></member><member><name>port</name><value><i4>5514</i4></value></member></struct></value></member><member><name>awareness_radius</name><value><double>41529791677676826000000000000000000000.000000</double></value></member><member><name>id_</name><value>5500_14_3f1bf4a1408a5e4462c51053a14d5b5cf1e28c7a</value></member><member><name>languages</name><value><array><data><value>fr</value><value>en</value></data></array></value></member><member><name>position</name><value><array><data><value>0</value><value>0</value><value>0</value></data></array></value></member><member><name>pseudo</name><value>z6-salamandra</value></member><member><name>services</name><value><struct></struct></value></member></struct></value></data></array></value>";
    int offset = 0;
    result.fromXml(csaXml, &offset);
    bool noError = mConnected = true;
#else
    bool noError = mConnected = this->executeThreadSafe("GetAllPeers", param, result);
#endif
    if (noError)
    {
        OGRE_LOG("GetAllPeers Result : " + result.toXml());
        int offset = 0; 
        XmlRpc::XmlRpcValue peersTable(result.toXml(), &offset);

        char txt[16]; sprintf(txt, "%d", peersTable.size());
        OGRE_LOG("peersTable.size : " + String(txt));

        if (peersTable.size()>0)
        {
            for (int i=0;i<peersTable.size();i++)
            {
                //OGRE_LOG("mPeersTable : " + peersTable[i].toXml());
                //Feed peersList
                //OGRE_LOG("getAllPeers " + login);
                peersList.push_back(createPeerFromXml(peersTable[i].toXml()));
            }
        }
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------
bool NavigatorXMLRPCClient::getEvents(std::list<NodeEvent*> &nodeEventsList)
{
    const XmlRpc::XmlRpcValue param(mConnectionId);
    XmlRpc::XmlRpcValue result;
#ifdef UNPLUG_MODE
    static int counter = 0;
    // FAKE example for no events
    char csaXmlNoEvent[] = "<value><array><data></data></array></value>";
    char *csaXml = csaXmlNoEvent;
#ifdef UIDEBUG
    // FAKE example to simulate lost of peer (c3-bolig) according to its networkId
    /* 
    <value>
        <array>
            <data>

                <value>
                    <struct>
                        <member>
                            <name>timestamp</name>
                            <value>12341234</value>
                        </member>
                        <member>
                            <name>type</name>
                            <value>LOST</value>
                        </member>
                        <member>
                            <name>datas</name>
                            <value>
                                <struct>
                                    <member>
                                        <name>id_</name>
                                        <value>5061_11_08312f6affa5059b69cfcc461d43c06a928a3068</value>
                                    </member>
                                </struct>
                            </value>
                        </member>
                    </struct>
                </value>

            </data>
        </array>
    </value>
    */
    char csaXmlLost[] = "<value><array><data><value><struct><member><name>timestamp</name><value>12341234</value></member><member><name>type</name><value>LOST</value></member><member><name>datas</name><value><struct><member><name>id_</name><value>5061_11_08312f6affa5059b69cfcc461d43c06a928a3068</value></member></struct></value></member></struct></value></data></array></value>";
    // FAKE example to simulate new peer (stationspatiale)
    /* 
    <value>
        <array>
            <data>

                <value>
                    <struct>
                        <member>
                            <name>timestamp</name>
                            <value>12341234</value>
                        </member>
                        <member>
                            <name>type</name>
                            <value>NEW</value>
                        </member>
                        <member>
                            <name>datas</name>
                            <value>
                                <struct>
                                    <member>
                                        <name>address</name>
                                        <value>
                                            <struct>
                                                <member>
                                                    <name>host</name>
                                                    <value>192.33.178.29</value>
                                                </member>
                                                <member>
                                                    <name>port</name>
                                                    <value><i4>5541</i4></value>
                                                </member>
                                            </struct>
                                        </value>
                                    </member>
                                    <member>
                                        <name>awareness_radius</name>
                                            <value>
                                                <double>41529791677676826000000000000000000000.000000</double>
                                            </value>
                                    </member>
                                    <member>
                                        <name>id_</name>
                                        <value>5500_14_3f1bf4a1408a5e4462c51053a14d5b5cf1e28c7b</value>
                                    </member>
                                    <member>
                                        <name>languages</name>
                                        <value>
                                            <array>
                                                <data>
                                                    <value>fr</value>
                                                    <value>en</value>
                                                </data>
                                            </array>
                                        </value>
                                    </member>
                                    <member>
                                        <name>position</name>
                                        <value>
                                            <array>
                                                <data>
                                                    <value>173822792765251003703866450027445485568</value>
                                                    <value>152993429928224644246366999440628121600</value>
                                                    <value>0</value>
                                                </data>
                                            </array>
                                        </value>
                                    </member>
                                    <member>
                                        <name>pseudo</name>
                                        <value>mySpaceStation</value>
                                    </member>
                                    <member>
                                        <name>services</name>
                                        <value><struct></struct></value>
                                    </member>
                                </struct>
                            </value>
                        </member>
                    </struct>
                </value>

            </data>
        </array>
    </value>
    */
    char csaXmlNew[] = "<value><array><data><value><struct><member><name>timestamp</name><value>12341234</value></member><member><name>type</name><value>NEW</value></member><member><name>datas</name><value><struct><member><name>address</name><value><struct><member><name>host</name><value>192.33.178.29</value></member><member><name>port</name><value><i4>5541</i4></value></member></struct></value></member><member><name>awareness_radius</name><value><double>41529791677676826000000000000000000000.000000</double></value></member><member><name>id_</name><value>5500_14_3f1bf4a1408a5e4462c51053a14d5b5cf1e28c7b</value></member><member><name>languages</name><value><array><data><value>fr</value><value>en</value></data></array></value></member><member><name>position</name><value><array><data><value>173822792765251003703866450027445485568</value><value>152993429928224644246366999440628121600</value><value>0</value></data></array></value></member><member><name>pseudo</name><value>Deltastation1</value></member><member><name>services</name><value><struct></struct></value></member></struct></value></member></struct></value></data></array></value>";
    std::map<String,String>::iterator dbgCmd = DebugHelpers::debugCommands.find("testEvt");
    if (dbgCmd != DebugHelpers::debugCommands.end())
    {
        static bool peerLostShot = false;
        static bool peerNewShot = false;
        if (dbgCmd->second.compare("peerLost") == 0)
        {
            if (!peerLostShot)
                csaXml = csaXmlLost;
            peerLostShot = true;
        }
        else if (dbgCmd->second.compare("peerNew") == 0)
        {
            if (!peerNewShot)
                csaXml = csaXmlNew;
            peerNewShot = true;
        }
        DebugHelpers::debugCommands.erase(dbgCmd);
    }
#endif
    int offset = 0;
    result.fromXml(csaXml, &offset);
    bool noError = mConnected = true;
    Platform::sleep(1000);
#else
    bool noError = mConnected = this->executeThreadSafe("GetEvents", param, result);
#endif
    if (noError)
    {
//        OGRE_LOG("GetEvents Result : " + result.toXml());
        int offset = 0; 
        XmlRpc::XmlRpcValue eventsTable(result.toXml(), &offset);

//        char txt[16]; sprintf(txt, "%d", eventsTable.size());
//        OGRE_LOG("eventsTable.size : " + String(txt));

        if (eventsTable.size()>0)
        {
            for (int i=0;i<eventsTable.size();i++)
            {
                offset = 0;
                XmlRpc::XmlRpcValue nodeEventStruct(eventsTable[i].toXml(), &offset);
                // Decode timestamp
                String timestampString = removeFirstLevelOfXmlAnchor(nodeEventStruct["timestamp"].toXml());
                if (!StringConverter::isNumber(timestampString))
                    Exception(Exception::ERR_INTERNAL_ERROR, "Timestamp bad format : " + timestampString, "NavigatorXMLRPCClient::getEvents");
                time_t timestamp = StringConverter::parseLong(timestampString);
                // Decode event type
                String typeString = removeFirstLevelOfXmlAnchor(nodeEventStruct["type"].toXml());
                NodeEvent::Type type;
                // Get datas
                NodeEvent::Datas* datas = 0;
                if (typeString.compare("NEW") == 0)
                {
                    type = NodeEvent::TNew;
                    // Decode event datas
                    datas = new NodeEvent::DatasPeerNew();
                    ((NodeEvent::DatasPeerNew*)datas)->mPeer = createPeerFromXml(nodeEventStruct["datas"].toXml());
                }
                else if (typeString.compare("LOST") == 0)
                {
                    type = NodeEvent::TLost;
                    // Decode event datas
                    datas = new NodeEvent::DatasPeerLost();
                    int offsetDatas = 0; 
                    XmlRpc::XmlRpcValue nodeEventDatasStruct(nodeEventStruct["datas"].toXml(), &offsetDatas);
                    String networkId = removeFirstLevelOfXmlAnchor(nodeEventDatasStruct["id_"].toXml());
                    ((NodeEvent::DatasPeerLost*)datas)->mNetworkId = String(networkId);
                }
                else if (typeString.compare("STATUS") == 0)
                {
                    type = NodeEvent::TStatusChanged;
                    // Decode event datas
                    datas = new NodeEvent::DatasStatusChanged();
                    int offsetDatas = 0; 
                    XmlRpc::XmlRpcValue nodeEventDatasStruct(nodeEventStruct["datas"].toXml(), &offsetDatas);
                    ((NodeEvent::DatasStatusChanged*)datas)->mStatus = removeFirstLevelOfXmlAnchor(nodeEventDatasStruct["status"].toXml());
                }
                else
                    Exception(Exception::ERR_INTERNAL_ERROR, "Unknown event type : " + typeString, "NavigatorXMLRPCClient::getEvents");

                // Feed nodeEventsList
                nodeEventsList.push_back(new NodeEvent(timestamp, type, datas));
            }
        }
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------
bool NavigatorXMLRPCClient::getStatus(String &status)
{
    const XmlRpc::XmlRpcValue value;
    const XmlRpc::XmlRpcValue noParam;
    XmlRpc::XmlRpcValue result;
#ifdef UNPLUG_MODE
    char csaXml[] = "<value>READY</value>";
    int offset = 0;
    result.fromXml(csaXml, &offset);
    mConnected = true;
#else
    mConnected = this->executeThreadSafe("GetStatus", noParam, result); //OGRE_LOG("result.toXml()");
#endif

    if (mConnected)
    {
        status = removeFirstLevelOfXmlAnchor(String(result.toXml()));
        OGRE_LOG("Status : " + status);
    }

    return mConnected;
}

//-------------------------------------------------------------------------------------
bool NavigatorXMLRPCClient::getDesc(const Peer &peer)
{
    const XmlRpc::XmlRpcValue param(mConnectionId);
    XmlRpc::XmlRpcValue result;

#ifdef UNPLUG_MODE
    // FAKE example to simulate peer's desc (stationspatiale)
    /*
    <value>
        <array>
            <data>

                <value>
                    <struct>
                        <member>
                            <name>type</name>
                            <value>scene</value>
                        </member>
                        <member>
                            <name>content</name>
                            <value>StationSpatiale.osm</value>
                        </member>
                    </struct>
                </value>

            </data>
        </array>
    </value>
    */
    char csaXml[] = "<value><array><data><value><struct><member><name>type</name><value>scene</value></member><member><name>content</name><value>StationSpatiale.osm</value></member></struct></value></data></array></value>";
    int offset = 0;
    result.fromXml(csaXml, &offset);
    bool noError = mConnected = true;
#else
    bool noError = mConnected = this->executeThreadSafe("GetDesc", param, result);
#endif
    if (noError)
    {
        OGRE_LOG("GetDesc Result : " + result.toXml());

        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------
bool NavigatorXMLRPCClient::move(const Peer &peer)
{
    const XmlRpc::XmlRpcValue params;
    XmlRpc::XmlRpcValue result;

#ifdef UNPLUG_MODE
    char csaXml[] = "";
    int offset = 0;
    result.fromXml(csaXml, &offset);
    bool noError = mConnected = true;
#else
    params[0] = peer.getNetworkId();
    params[1] = peer.x;
    params[2] = peer.y;
    params[3] = peer.z;
    bool noError = mConnected = this->executeThreadSafe("Move", params, result);
#endif
    if (noError)
    {
        OGRE_LOG("Move Result : " + result.toXml());
        return true;
    }

    return false;
}

//-------------------------------------------------------------------------------------
bool NavigatorXMLRPCClient::sendMessage(const String& message, std::list<Peer*> &peersList)
{
    const XmlRpc::XmlRpcValue params;
    XmlRpc::XmlRpcValue result;

    // Send the message to each peer
    for (std::list<Peer*>::iterator peer = peersList.begin();peer != peersList.end();++peer)
    {
        OGRE_LOG("Send message " + message + " to " + (*peer)->getLogin());
#ifdef UNPLUG_MODE
        char csaXml[] = "";
        int offset = 0;
        result.fromXml(csaXml, &offset);
        bool noError = mConnected = true;
#else
        params[0] = peer.getNetworkId();
        params[1] = message;
        bool noError = mConnected = this->executeThreadSafe("SendMessage", params, result);
#endif
        if (noError)
            OGRE_LOG("Send message Result : " + result.toXml());
    }

    return true;
}

//-------------------------------------------------------------------------------------
Peer* NavigatorXMLRPCClient::createPeerFromXml(std::string& peerXml)
{
    int offset = 0;

    XmlRpc::XmlRpcValue peerStruct(peerXml, &offset);
    String login = removeFirstLevelOfXmlAnchor(peerStruct["pseudo"].toXml());
    String networkId = removeFirstLevelOfXmlAnchor(peerStruct["id_"].toXml());
    CBigInt x(0);
    CBigInt y(0);
    CBigInt z(0);

    offset = 0;
    XmlRpc::XmlRpcValue positionArray(peerStruct["position"].toXml(), &offset);
    if (positionArray.size() >= 1)
    {
        //OGRE_LOG("position X : " + removeFirstLevelOfXmlAnchor(positionArray[0].toXml()));
        x = CBigInt(removeFirstLevelOfXmlAnchor(positionArray[0].toXml()).c_str());
    }
    if (positionArray.size() >= 2)
    {
        //OGRE_LOG("position Y : " + removeFirstLevelOfXmlAnchor(positionArray[1].toXml()));
        y = CBigInt(removeFirstLevelOfXmlAnchor(positionArray[1].toXml()).c_str());
    }
    if (positionArray.size() >= 3)
    {
        //OGRE_LOG("position Z : " + removeFirstLevelOfXmlAnchor(positionArray[2].toXml()));
        z = CBigInt(removeFirstLevelOfXmlAnchor(positionArray[2].toXml()).c_str());
    }

    // Create the peer
    return new Peer(networkId,login,x,y,z);
}

//-------------------------------------------------------------------------------------
String NavigatorXMLRPCClient::removeFirstLevelOfXmlAnchor(String& xml)
{
    size_t first = xml.find_first_of('>');
    size_t last = xml.find_last_of('<');
    return String(xml.substr(first+1,last-first-1));
}

//-------------------------------------------------------------------------------------
bool NavigatorXMLRPCClient::executeThreadSafe(const char* method, XmlRpc::XmlRpcValue const& params, XmlRpc::XmlRpcValue& result)
{
    if (pthread_mutex_lock(&mCallsMutex) != 0)
        Exception(Exception::ERR_INTERNAL_ERROR, "Unable to lock the XMLRPC calls mutex", "NavigatorXMLRPCClient::executeThreadSafe");
    bool succeeded = this->execute(method, params, result);
    if (pthread_mutex_unlock(&mCallsMutex) != 0)
        Exception(Exception::ERR_INTERNAL_ERROR, "Unable to unlock the XMLRPC calls mutex", "NavigatorXMLRPCClient::executeThreadSafe");

    return succeeded;
}

//-------------------------------------------------------------------------------------
