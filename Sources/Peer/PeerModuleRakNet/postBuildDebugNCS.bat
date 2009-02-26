set vc="%1"

if not exist "..\..\..\Dependencies\peerModule\include" mkdir ..\..\..\Dependencies\peerModule\include
copy PeerModule.h ..\..\..\Dependencies\peerModule\include\. /y
copy IPeer.h ..\..\..\Dependencies\peerModule\include\. /y
copy OgreHelpers.h ..\..\..\Dependencies\peerModule\include\. /y
copy IPhysics*.h ..\..\..\Dependencies\peerModule\include\. /y
copy PhysicsEngineManager.h ..\..\..\Dependencies\peerModule\include\. /y

if not exist ..\..\..\Dependencies\peerModule\lib\Debug mkdir ..\..\..\Dependencies\peerModule\lib\Debug
copy DebugNCS\Peer.lib ..\..\..\Dependencies\peerModule\lib\Debug\. /y
copy DebugNCS\Peer.dll ..\..\..\Dependencies\peerModule\lib\Debug\. /y
