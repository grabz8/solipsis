--------------------------------------------------------------------------------
CONFIG
--------------------------------------------------------------------------------

Refer to the Solipsis readme.txt


--------------------------------------------------------------------------------
PREREQUISITES
--------------------------------------------------------------------------------

Refer to the Solipsis readme.txt


--------------------------------------------------------------------------------
HELP
--------------------------------------------------------------------------------

peerModuleRakNet simply simulate the peer module behaviour as server behind the Navigator and client in front of the RakNet server,
XMLRPC (XML/HTTP) is used (either XMLRPC++ or ULXMLRPCPP), server created on : localhost:8550 (IP@:8550 when using ULXMLRPCPP)

According to use of XMLRPC++ or ULXMLRPCPP, then HandleEvt will respond
NOEVT or will block until 1 EVT must be processed
LOGSNDRCV define is used to log some XMLRPC traces
LOGRAKNET define is used to log some RakNet traces

Peer.exe is taking 3 optional args:
-h <IPaddr:str>         : IP address (default: localhost)
-p <port:int>           : port (default: 8550)
-v <verbosity:int>      : verbosity level (0..2)
-m <mediaCachePath:str> : Pathname to Media/cache directory (usefull to share same network directory between the Navigator, its Peer and the RakNet Server)
-rh <IPaddr:str>        : IP address of the RakNet server (default: localhost)
-rp <port:int>          : port of the RakNet server (default: 8660)
-id <avatarNodeId:str>  : Unique avatar node identifier, hex string of 8 digits (default: 00000001), check each peer has 1 different id !


--------------------------------------------------------------------------------
HOW TO BUILD
--------------------------------------------------------------------------------

Refer to the Solipsis readme.txt
