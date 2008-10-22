--------------------------------------------------------------------------------
                               S O L I P S I S
--------------------------------------------------------------------------------
Solipsis is an opensource decentralized Metaverse platform
For the latest info, see http://www.solipsis.org/


--------------------------------------------------------------------------------
CONFIG
--------------------------------------------------------------------------------

Visual C++ 2005 Redist x86 libraries package
Python 2.5.2 or later package to run the Worlds Server
DirectX 9.0 or later package
Ageia PhysX drivers 7.11.13 or later


--------------------------------------------------------------------------------
HELP
--------------------------------------------------------------------------------

Camera:
1                First-Person view mode
2                First-Person view mode with mouse to interact
3                Third-Person view mode
4                Third-Person view mode with camera orbit
MOUSE            Cursor mode (3rd person) / View mode (1st person)
MOUSE DBL LCLICK In plugins mode, you have to double-click left mouse to enter/leave FirstPerson mouse mode


Move (french keyboard letters):
UP/Z             Move forward
DOWN/S           Move backward
LEFT/Q           Turn left (3rd person) / Straff left (1st person)
RIGHT/D          Turn right (3rd person) / Straff right (1st person)
PGUP/E           Fly up (+ deactivate gravity)
PGDOWN/C         Fly down
END              Activate gravity


Panels:
MOUSE RCLICK     Display context panel (avatar, WWW navi, VLC, ...)
ESC              Hit ESCAPE 2 times to defocus current panel (WWW Navi, VNC, ...)
F7               Toggle CHAT panel
F8               Toggle AVATAR main panel
F9               Toggle MODELER main panel


--------------------------------------------------------------------------------
APPLICATIONS/ARGUMENTS
--------------------------------------------------------------------------------

To run the Peer application you can specify host, port and verbosity of the Peer/Navigator XMLRPC server,
additionally you have to specify host, port of the RakNet server, in NCS configs those parameters should be passed to the Navigator application:
-h <IPaddr:str>         : IP address (default: localhost)
-p <port:int>           : port (default: 8880)
-v <verbosity:int>      : verbosity level (0..2)
-m <mediaCachePath:str> : Pathname to Media/cache directory (usefull to share same network directory between the Navigator and its Peer)


To run the RakNet Server you can specify the port, max connections and simulated scene
If you launch it from the same system than navigator then you have to change the Media/cache directory (eg. copy the default Media/cache directory into Media/cacheServer)
-p <port:int>                   : port (default: 8660)
-c <maxIncomingConnections:int> : maximum incoming connections (default: 32)
-s <sceneNodeId:str>            : Scene nodeId loaded by the simulator (eg. 11112222 for Delta Station, 11112223 Children Island, ...), look into the .xml file to update some presets
-m <mediaCachePath:str>         : Pathname to Media/cache directory


To run the Worlds Server, check you got python installed and launch WorldsServer.py
Update internal host:port if you want to change default values localhost:8550
