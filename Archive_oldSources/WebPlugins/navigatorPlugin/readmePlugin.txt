To install the Navigator ActiveX:

open 1 DOS command window
go into the Navigator directory containing navigatorActiveX.dll
and launch : regsvr32 /s /c "navigatorActiveX.dll"



To install the Navigator Mozilla plugin:

Run command : regedit
Go into HKEY_CURRENT_USER\Software
Add 1 Key "Solipsis"
Go into HKEY_CURRENT_USER\Software\Solipsis
Add 1 Key "Navigator"
Go into HKEY_CURRENT_USER\Software\Solipsis\Navigator
Add 1 String Value "Install Directory"
Set its value to the Navigator directory (eg. C:\tmp\Common\bin\navigator\release)
Copy plugin files :
Common\bin\navigator\release\npsolnav.dll
Common\bin\navigator\release\SolipsisNavigator.xpt
into your mozilla browser (eg. firefox) directory (eg. C:\Program Files\Mozilla Firefox\plugins)



For instance, Navi is not supported when embedded into 1 plugin, so Navi based objects will be
mapped with the default_texture.jpg and 1 automatic login will be performed on the fixedNodeId
specified in the boot.lua script, so check in this file you well described your login + nodeId.
Before running this plugin check you launched the RakNetServer and the peer !!!
