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

peerModuleSimulator simply simulate the peer module behaviour as server behind the Navigator,
XMLRPC (XML/HTTP) is used (either XMLRPC++ or ULXMLRPCPP), server created on : localhost:8880 (IP@:8880 when using ULXMLRPCPP)
Server accepts login/logout, give 1 different nodeId to each cnx and handle those events

According to use of XMLRPC++ or ULXMLRPCPP, then HandleEvt will respond
NOEVT or will block until 1 EVT must be processed
LOGSNDRCV define is used to log some traces

Peer.exe is taking 5 optional args:
-h <IPaddr:str>         : IP address (default: localhost)
-p <port:int>           : port (default: 8880)
-v <verbosity:int>      : verbosity level (0..2)
-s <sceneName:str>      : Scene name loaded by the peer simulator (eg. Deltastation1, Ile, ...), look into the .xml file to update some presets
-m <mediaCachePath:str> : Pathname to Media/cache directory (usefull to share same network directory between 1 Peer and N Navigators)


--------------------------------------------------------------------------------
HOW TO BUILD
--------------------------------------------------------------------------------

Refer to the Solipsis readme.txt
