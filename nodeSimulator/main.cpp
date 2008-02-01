#include <iostream>
#include "Node.h"
#include "XMLRPCNodeClient.h"
#include "XmlDatas.h"

using namespace XmlRpc;
using namespace Solipsis;

class NodeClientLogger : public Solipsis::XMLRPCNodeClientLogger
{
public:
    virtual void logMessage(const std::string& message)
    {
        fprintf(stdout, "%s\n", message.c_str());
    }
};

int main(int argc, char** argv)
{
    bool client = ((argc > 1) && (strstr(argv[1], "client") != 0));
    bool server = ((argc > 1) && (strstr(argv[1], "server") != 0));

	Node* myNode = 0;
    Solipsis::XMLRPCNodeClient* nodeClient = 0;
    NodeClientLogger nodeClientLogger;

    if (server)
    {
    	// Launch of a Node server with a XML-RPC inside
	    myNode = new Node(8550, 2);
        if (!client)
        {
            char c[256];
            fscanf(stdin, "%s", c);
            delete myNode;
            return 0;
        }
    }

    if (client)
    {
        nodeClient = new Solipsis::XMLRPCNodeClient("localhost", 8550, "");
        nodeClient->setLogger(&nodeClientLogger);
        Solipsis::INodeClient::RetCode retCode;

        XmlLogin xmlLogin("user", "demo");
        std::string xmlParams;
        xmlParams.append("<solipsis>").append(xmlLogin.toXmlString()).append("</solipsis>");
        std::string xmlResp;
        std::cout << "login() xmlParams=\n" << xmlParams << "\n";
        retCode = nodeClient->login(xmlParams, xmlResp);
        std::cout << "-> login() retCode=" << retCode << " xmlResp=\n" << xmlResp << "\n";

        while (true)
        {
            char c[256];
            fscanf(stdin, "%s", c);
            if (*c == 'q') break;
            if (client)
            {
                switch (*c)
                {
                case 'h':
                    std::cout << "handleEvt()\n";
                    retCode = nodeClient->handleEvt(xmlResp);
                    std::cout << "-> handleEvt() retCode=" << retCode << " xmlResp=\n" << xmlResp << "\n";
                    break;
                }
            }
        }

        std::cout << "logout()\n";
        retCode = nodeClient->logout();
        std::cout << "-> logout() retCode=" << retCode << "\n";
    }

    delete nodeClient;
    delete myNode;

	return 0;
}
