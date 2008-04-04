#include <sstream>
#include "PeerModule.h"
#include "IPeer.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

using namespace Solipsis;

// Client part
#include "IP2NClient.h"
#include "XmlDatas.h"

class P2NClientLogger : public IP2NClientLogger
{
public:
    virtual void logMessage(const std::string& message)
    {
        std::cout << message;
    }
};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
int main(int argc, char** argv)
#else
int main(int argc, char *argv[])
#endif
{
    setlocale(LC_NUMERIC, "English");
    try
    {
        // Create the Peer application
        IPeer* peer = IPeer::createPeer("", argc, argv);
        assert(peer != 0);
        bool initialized = peer->initialize();
        assert(initialized);

        // Client part
        bool client = ((argc > 5) && (strstr(argv[5], "-c") != 0));
        if (!client)
        {
            while (true)
            {
                char c[256];
                fscanf(stdin, "%s", c);
                if (*c == 'q') break;
            }
        }
        else
        {
            IP2NClient* P2NClient = 0;
            P2NClientLogger P2NClientLogger;
            P2NClient = IP2NClient::createClient("localhost", 8550, 2, "");
            P2NClient->setLogger(&P2NClientLogger);
            IP2NClient::RetCode retCode;

            XmlLogin xmlLogin("user", "demo");
            std::string xmlParams;
            xmlParams.append("<solipsis>").append(xmlLogin.toXmlString()).append("</solipsis>");
            std::string xmlResp;
            std::cout << "login() xmlParams=\n" << xmlParams << "\n";
            retCode = P2NClient->login(xmlParams, xmlResp);
            std::cout << "-> login() retCode=" << retCode << " xmlResp=\n" << xmlResp << "\n";

            while (true)
            {
                char c[256];
                fscanf(stdin, "%s", c);
                if (*c == 'q') break;
                switch (*c)
                {
                case 'h':
                    std::cout << "handleEvt()\n";
                    retCode = P2NClient->handleEvt(xmlResp);
                    std::cout << "-> handleEvt() retCode=" << retCode << " xmlResp=\n" << xmlResp << "\n";
                    break;
                }
            }

            std::cout << "logout()\n";
            retCode = P2NClient->logout();
            std::cout << "-> logout() retCode=" << retCode << "\n";

            delete P2NClient;
        }

        // Destroy the Peer application
        peer->destroy();
    }
    catch (...)
    {
#ifdef WIN32
        MessageBox(0, "Unable to launch the Peer application ...", "An exception has occured !", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
        std::cerr << "Unable to launch the Peer application ... An exception has occured !" << std::endl;
#endif
    }

    return 0;
}

#ifdef __cplusplus
}
#endif
