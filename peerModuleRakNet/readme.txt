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
XMLRPC (XML/HTTP) is used (either XMLRPC++ or ULXMLRPCPP), server created on : localhost:8880 (IP@:8880 when using ULXMLRPCPP)

According to use of XMLRPC++ or ULXMLRPCPP, then HandleEvt will respond
NOEVT or will block until 1 EVT must be processed
LOGSNDRCV define is used to log some XMLRPC traces
LOGRAKNET define is used to log some RakNet traces

Peer.exe is taking 3 optional args:
-h <IPaddr:str>         : IP address (default: localhost)
-p <port:int>           : port (default: 8880)
-v <verbosity:int>      : verbosity level (0..2)
-m <mediaCachePath:str> : Pathname to Media/cache directory (usefull to share same network directory between the Navigator and its Peer)


--------------------------------------------------------------------------------
HOW TO BUILD
--------------------------------------------------------------------------------

Refer to the Solipsis readme.txt
