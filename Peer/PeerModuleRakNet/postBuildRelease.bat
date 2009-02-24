set dst="..\..\Common\bin\peerRakNet\Release"
set vc="%1"

if not exist "..\..\Dependencies\peerModule\include" mkdir ..\..\Dependencies\peerModule\include
copy PeerModule.h ..\..\Dependencies\peerModule\include\. /y
copy IPeer.h ..\..\Dependencies\peerModule\include\. /y
copy OgreHelpers.h ..\..\Dependencies\peerModule\include\. /y
copy IPhysics*.h ..\..\Dependencies\peerModule\include\. /y
copy PhysicsEngineManager.h ..\..\Dependencies\peerModule\include\. /y

if not exist ..\..\Dependencies\peerModule\lib\Release mkdir ..\..\Dependencies\peerModule\lib\Release
copy Release\Peer.lib ..\..\Dependencies\peerModule\lib\Release\. /y
copy Release\Peer.dll ..\..\Dependencies\peerModule\lib\Release\. /y

if not exist "%dst%" mkdir "%dst%"
copy Release\Peer.dll "%dst%" /y
if exist "%dst%\OgreMain.dll" goto OgreMain_exist
 if exist "%OGRE_HOME%\lib\OgreMain.dll" xcopy "%OGRE_HOME%\lib\OgreMain.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\OgreMain.dll" xcopy "%OGRE_HOME%\bin\release\OgreMain.dll" "%dst%" /s /y
:OgreMain_exist
if not exist "%dst%\pthreadVC.dll" xcopy "..\..\Dependencies\pthreads\lib\pthreadVC.dll" "%dst%" /s /y
xcopy "..\..\Dependencies\protocols\P2NServer\xmlrpc++\ReleaseDLL\XMLRPCP2NServer.dll" "%dst%" /s /y
xcopy "..\..\Dependencies\protocols\P2NServer\ulxmlrpcpp\ReleaseDLL\ULXMLRPCPPP2NServer.dll" "%dst%" /s /y
if exist "..\..\Dependencies\ulxmlrpcpp\lib\ReleaseDLL\ulxmlrpcpp_23.dll" xcopy "..\..\Dependencies\ulxmlrpcpp\lib\ReleaseDLL\ulxmlrpcpp_23.dll" "%dst%" /s /y
if exist "..\..\Dependencies\expat\lib\ReleaseDLL\libexpat.dll" xcopy "..\..\Dependencies\expat\lib\ReleaseDLL\libexpat.dll" "%dst%" /s /y
xcopy "..\..\Dependencies\protocols\XmlDatas\lib\ReleaseDLL\XmlDatas.dll" "%dst%" /s /y
xcopy "..\..\Dependencies\protocols\P2NClient\xmlrpc++\ReleaseDLL\XMLRPCP2NClient.dll" "%dst%" /s /y
if not exist "%dst%\RakNet.dll" xcopy "..\..\Dependencies\RakNet\lib\RakNet.dll" "%dst%" /s /y
xcopy "..\..\Dependencies\libiconv\lib\libiconv2.dll" "%dst%" /s /y

if exist "%dst%\Plugins.cfg" goto Plugins_exist
 xcopy "Plugins.cfg" "%dst%" /s /y
:Plugins_exist
xcopy "..\..\Media\resources.cfg" "%dst%" /s /y
copy "readme.txt" "%dst%" /y
