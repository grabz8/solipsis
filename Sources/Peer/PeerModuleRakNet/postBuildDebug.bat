set dst="..\..\..\Common\bin\peerRakNet\Debug"
set vc="%1"

if not exist "..\..\..\Dependencies\peerModule\include" mkdir ..\..\..\Dependencies\peerModule\include
copy PeerModule.h ..\..\..\Dependencies\peerModule\include\. /y
copy IPeer.h ..\..\..\Dependencies\peerModule\include\. /y
copy OgreHelpers.h ..\..\..\Dependencies\peerModule\include\. /y
copy IPhysics*.h ..\..\..\Dependencies\peerModule\include\. /y
copy PhysicsEngineManager.h ..\..\..\Dependencies\peerModule\include\. /y

if not exist ..\..\..\Dependencies\peerModule\lib\Debug mkdir ..\..\..\Dependencies\peerModule\lib\Debug
copy Debug\Peer.lib ..\..\..\Dependencies\peerModule\lib\Debug\. /y
copy Debug\Peer.dll ..\..\..\Dependencies\peerModule\lib\Debug\. /y

if not exist "%dst%" mkdir "%dst%"
copy Debug\Peer.dll "%dst%" /y
if exist "%dst%\OgreMain_d.dll" goto OgreMain_d_exist
 if exist "%OGRE_HOME%\lib\OgreMain_d.dll" xcopy "%OGRE_HOME%\lib\OgreMain_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\OgreMain_d.dll" xcopy "%OGRE_HOME%\bin\debug\OgreMain_d.dll" "%dst%" /s /y
:OgreMain_d_exist
if not exist "%dst%\pthreadVC_d.dll" xcopy "..\..\..\Dependencies\pthreads\lib\pthreadVC_d.dll" "%dst%" /s /y
xcopy "..\..\..\Dependencies\protocols\P2NServer\xmlrpc++\DebugDLL\XMLRPCP2NServer.dll" "%dst%" /s /y
xcopy "..\..\..\Dependencies\protocols\P2NServer\ulxmlrpcpp\DebugDLL\ULXMLRPCPPP2NServer.dll" "%dst%" /s /y
if exist "..\..\..\Dependencies\ulxmlrpcpp\lib\DebugDLL\ulxmlrpcpp_23.dll" xcopy "..\..\..\Dependencies\ulxmlrpcpp\lib\DebugDLL\ulxmlrpcpp_23.dll" "%dst%" /s /y
if exist "..\..\..\Dependencies\expat\lib\DebugDLL\libexpat.dll" xcopy "..\..\..\Dependencies\expat\lib\DebugDLL\libexpat.dll" "%dst%" /s /y
xcopy "..\..\..\Dependencies\protocols\XmlDatas\lib\DebugDLL\XmlDatas.dll" "%dst%" /s /y
xcopy "..\..\..\Dependencies\protocols\P2NClient\xmlrpc++\DebugDLL\XMLRPCP2NClient.dll" "%dst%" /s /y
if not exist "%dst%\RakNetDebug.dll" xcopy "%RAKNET_PATH%\Lib\RakNetDebug.dll" "%dst%" /s /y
xcopy "..\..\..\Dependencies\libiconv\lib\libiconv2.dll" "%dst%" /s /y

if exist "%dst%\Plugins.cfg" goto Plugins_exist
 xcopy "Plugins.cfg" "%dst%" /s /y
:Plugins_exist
xcopy "..\..\..\Media\resources.cfg" "%dst%" /s /y
copy "readme.txt" "%dst%" /y

call copyPhysXFiles.bat
