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

RakNetServer simulates the behaviour of 1 site node by managing network nodes/entities with the RakNet library.

RakNetServer.exe is taking 4 optional args:
If you launch it from the same system than navigator then you have to change the Media/cache directory (eg. copy the default Media/cache directory into Media/cacheServer)
-p <port:int>                   : port (default: 8660)
-c <maxIncomingConnections:int> : maximum incoming connections (default: 32)
-s <sceneNodeId:str>            : Scene nodeId loaded by the simulator (eg. 11112222 for Deltastation1, 11112223 Ile, ...), look into the .xml file to update some presets
-m <mediaCachePath:str>         : Pathname to Media/cache directory


--------------------------------------------------------------------------------
HOW TO BUILD
--------------------------------------------------------------------------------

Refer to the Solipsis readme.txt
