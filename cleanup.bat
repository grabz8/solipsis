@echo off

echo,
echo This will clean up all dependencies, binaries, objects and libraries.
echo,
echo *********************************************************
echo ** You must close Visual C++ before running this file! **
echo *********************************************************
echo,
echo You will see some deletion warnings but don t worry about them ...
echo,
pause

echo ========== Cleaning up... ==========

del /s *.ncb
del /s vc*.idb
del /s vc*.pdb
del /s *.vcproj.*.user
del /s /A:H *.suo

rmdir Common\bin\navigator\debug /S /Q
rmdir Common\bin\navigator\release /S /Q
rmdir Common\bin\navigator

rmdir Common\bin\node\debug /S /Q
rmdir Common\bin\node\release /S /Q

rmdir Common\bin\peerRakNet\debug /S /Q
rmdir Common\bin\peerRakNet\release /S /Q
rmdir Common\bin\peerRakNet

rmdir Common\bin\peerSimulator\debug /S /Q
rmdir Common\bin\peerSimulator\release /S /Q
rmdir Common\bin\peerSimulator

rmdir Common\bin\RakNetServer\debug /S /Q
rmdir Common\bin\RakNetServer\release /S /Q
rmdir Common\bin\RakNetServer

rmdir Common\bin\VoiceEngineServer\debug /S /Q
rmdir Common\bin\VoiceEngineServer\release /S /Q
rmdir Common\bin\VoiceEngineServer

rmdir WorldsServer\stats /S /Q
del Statistics\*.csv
del Statistics\*.svg
del Statistics\*.pyc

rmdir Dependencies\llmozlib /S /Q
rmdir Dependencies\lua /S /Q
rmdir Dependencies\navi /S /Q
rmdir Dependencies\navigatorModule /S /Q
rmdir Dependencies\navigatorPlugin /S /Q
rmdir Dependencies\ode /S /Q
rmdir Dependencies\OgreMax /S /Q
rmdir Dependencies\OgreOde /S /Q
rmdir Dependencies\pthreads /S /Q
rmdir Dependencies\tinyxml /S /Q
rmdir Dependencies\tokamak /S /Q
rmdir Dependencies\xmlrpc++ /S /Q
rmdir Dependencies\brep /S /Q
rmdir Dependencies\csg /S /Q
rmdir Dependencies\MdlrTools /S /Q
rmdir Dependencies\AvatarTools /S /Q
rmdir Dependencies\expat /S /Q
rmdir Dependencies\ulxmlrpcpp /S /Q
rmdir Dependencies\zlib123 /S /Q
rmdir Dependencies\libiconv /S /Q
rmdir Dependencies\protocols /S /Q
rmdir Dependencies\peerModule /S /Q
rmdir Dependencies\CommonTools /S /Q
rmdir Dependencies\speex /S /Q
rmdir Dependencies\VoiceEngineClient /S /Q
rmdir Dependencies\VoiceEngineSimpleServer /S /Q
rmdir Dependencies\vlc /S /Q
rmdir Dependencies\plugin_3ds /S /Q
rmdir Dependencies\plugin_skp /S /Q
rmdir Dependencies\RakNet /S /Q
rmdir Dependencies\curl /S /Q
rmdir Dependencies\md5wrapper /S /Q
rmdir Dependencies\authentFB /S /Q
rmdir Dependencies\RakNetObjects /S /Q
rmdir Dependencies\navigatorConfigDialog /S /Q
rmdir Dependencies\VoiceEngine /S /Q
rmdir Dependencies\SimpleVoiceEngineProtocol /S /Q
rmdir Dependencies\Phonetizer /S /Q
rmdir Dependencies\DummyPhonetizer /S /Q
rmdir Dependencies\Antlr2.7.6 /S /Q
rmdir Dependencies\gecode-2.2.0 /S /Q
rmdir Dependencies\FaceController /S /Q
rmdir Dependencies\ogreMax /S /Q
rmdir Dependencies\hikari /S /Q
rmdir Dependencies\DeclarativeModeler /S /Q

rmdir Sources\DevTools\llmozlib\Debug_vc7 /S /Q
rmdir Sources\DevTools\llmozlib\Debug_vc8 /S /Q
rmdir Sources\DevTools\llmozlib\Debug_vc9 /S /Q
rmdir Sources\DevTools\llmozlib\Release_vc7 /S /Q
rmdir Sources\DevTools\llmozlib\Release_vc8 /S /Q
rmdir Sources\DevTools\llmozlib\Release_vc9 /S /Q
rmdir Sources\DevTools\lua\lib /S /Q
rmdir Sources\DevTools\navi\Navi\Objects_vc7 /S /Q
rmdir Sources\DevTools\navi\Navi\Objects_vc8 /S /Q
rmdir Sources\DevTools\navi\Navi\Objects_vc9 /S /Q
rmdir Sources\DevTools\navi\Navi\lib_vc7 /S /Q
rmdir Sources\DevTools\navi\Navi\lib_vc8 /S /Q
rmdir Sources\DevTools\navi\Navi\lib_vc9 /S /Q
rmdir Sources\DevTools\navi\NaviDemo\Objects_vc7 /S /Q
rmdir Sources\DevTools\navi\NaviDemo\Objects_vc8 /S /Q
rmdir Sources\DevTools\navi\NaviDemo\Objects_vc9 /S /Q
rmdir Sources\DevTools\navi\NaviDemo\Bin\debug_vc7 /S /Q
rmdir Sources\DevTools\navi\NaviDemo\Bin\debug_vc8 /S /Q
rmdir Sources\DevTools\navi\NaviDemo\Bin\debug_vc9 /S /Q
rmdir Sources\DevTools\navi\NaviDemo\Bin\release_vc7 /S /Q
rmdir Sources\DevTools\navi\NaviDemo\Bin\release_vc8 /S /Q
rmdir Sources\DevTools\navi\NaviDemo\Bin\release_vc9 /S /Q
rmdir Sources\DevTools\OgreMax\script\lib /S /Q
rmdir Sources\DevTools\OgreMax\script\Debug /S /Q
rmdir Sources\DevTools\OgreMax\script\Release /S /Q
rmdir Sources\DevTools\OgreOde\lib\debug /S /Q
rmdir Sources\DevTools\OgreOde\lib\release /S /Q
rmdir Sources\DevTools\OgreOde\obj\Debug /S /Q
rmdir Sources\DevTools\OgreOde\obj\Release /S /Q
rmdir Sources\DevTools\OgreOde\ode\lib\debuglib /S /Q
rmdir Sources\DevTools\OgreOde\ode\lib\releaselib /S /Q
rmdir Sources\DevTools\OgreOde\ode\build\vs2003\obj /S /Q
rmdir Sources\DevTools\OgreOde\ode\build\vs2005\obj /S /Q
rmdir Sources\DevTools\OgreOde\ode\build\vs2008\obj /S /Q
del Sources\DevTools\pthreads\pthreadVC* /S /Q
del Sources\DevTools\pthreads\BuildLog.htm /Q
del Sources\DevTools\pthreads\mt.dep /Q
del Sources\DevTools\pthreads\pthread.obj /Q
del Sources\DevTools\pthreads\version.res /Q
rmdir Sources\DevTools\tinyxml\Debug /S /Q
rmdir Sources\DevTools\tinyxml\Release /S /Q
rmdir Sources\DevTools\tokamak\lib\debugdll /S /Q
rmdir Sources\DevTools\tokamak\lib\releasedll /S /Q
rmdir Sources\DevTools\tokamak\tokamaksrc\tokamakdll /S /Q
del Sources\DevTools\xmlrpc++\lib\*.lib /S /Q
rmdir Sources\DevTools\xmlrpc++\Debug /S /Q
rmdir Sources\DevTools\xmlrpc++\Release /S /Q
rmdir Sources\DevTools\brep\Debug /S /Q
rmdir Sources\DevTools\brep\Release /S /Q
rmdir Sources\DevTools\brep\lib /S /Q
rmdir Sources\DevTools\CSG\Debug /S /Q
rmdir Sources\DevTools\CSG\Release /S /Q
rmdir Sources\DevTools\CSG\lib /S /Q
rmdir Sources\DevTools\expat\win32\bin /S /Q
rmdir Sources\DevTools\expat\win32\tmp /S /Q
rmdir Sources\DevTools\ulxmlrpcpp\msvc71\ulxmlrpcpp\Debug /S /Q
rmdir Sources\DevTools\ulxmlrpcpp\msvc71\ulxmlrpcpp\Release /S /Q
rmdir Sources\DevTools\ulxmlrpcpp\msvc8\ulxmlrpcpp\Debug /S /Q
rmdir Sources\DevTools\ulxmlrpcpp\msvc9\ulxmlrpcpp\Debug /S /Q
rmdir Sources\DevTools\ulxmlrpcpp\msvc8\ulxmlrpcpp\Release /S /Q
rmdir Sources\DevTools\ulxmlrpcpp\msvc9\ulxmlrpcpp\Release /S /Q
rmdir Sources\DevTools\zlib123\projects\visualc6\Win32_LIB_Debug /S /Q
rmdir Sources\DevTools\zlib123\projects\visualc6\Win32_LIB_Release /S /Q
rmdir Sources\DevTools\libiconv\Debug /S /Q
rmdir Sources\DevTools\libiconv\Release /S /Q
rmdir Sources\DevTools\vnc_winsrc\winvnc\Debug /S /Q
rmdir Sources\DevTools\vnc_winsrc\winvnc\Release /S /Q
rmdir Sources\DevTools\vnc_winsrc\vncviewer\Debug /S /Q
rmdir Sources\DevTools\vnc_winsrc\vncviewer\Release /S /Q
rmdir Sources\DevTools\speex\win32\libspeex\Debug /S /Q
rmdir Sources\DevTools\speex\win32\libspeex\Release /S /Q
rmdir Sources\DevTools\RakNet\Lib\DLL\Debug /S /Q
rmdir Sources\DevTools\RakNet\Lib\DLL\Release /S /Q
del Sources\DevTools\RakNet\Lib\*.* /Q
rmdir Sources\DevTools\curl\lib\Debug /S /Q
rmdir Sources\DevTools\curl\lib\Release /S /Q
rmdir Sources\DevTools\md5wrapper\Debug /S /Q
rmdir Sources\DevTools\md5wrapper\Release /S /Q
rmdir Sources\DevTools\hikari\lib /S /Q
rmdir Sources\DevTools\hikari\objects /S /Q
rmdir Sources\DevTools\Antlr2.7.6\Debug /S /Q
rmdir Sources\DevTools\Antlr2.7.6\Release /S /Q
rmdir Sources\DevTools\Gecode-2.2.0\Debug /S /Q
rmdir Sources\DevTools\Gecode-2.2.0\Release /S /Q

pushd Docs\doxygen
call cleanup.bat
popd

rmdir Sources\CommonTools\Debug /S /Q
rmdir Sources\CommonTools\Release /S /Q

rmdir Sources\Modelers\MdlrTools\obj /S /Q
rmdir Sources\Modelers\AvatarTools\obj /S /Q

rmdir Sources\Navigator\NavigatorLauncher\Debug /S /Q
rmdir Sources\Navigator\NavigatorLauncher\Release /S /Q
rmdir Sources\Navigator\NavigatorLauncher\DebugNCS /S /Q
rmdir Sources\Navigator\NavigatorLauncher\ReleaseNCS /S /Q

rmdir Sources\Navigator\navigatorModule\Debug /S /Q
rmdir Sources\Navigator\navigatorModule\Release /S /Q

rmdir Sources\Navigator\WebPlugins\navigatorPlugin\navigatorActiveX\Debug_vc7 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorPlugin\navigatorActiveX\Debug_vc8 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorPlugin\navigatorActiveX\Debug_vc9 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorPlugin\navigatorActiveX\Release_vc7 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorPlugin\navigatorActiveX\Release_vc8 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorPlugin\navigatorActiveX\Release_vc9 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorPlugin\navigatorPlugin\Debug_vc7 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorPlugin\navigatorPlugin\Debug_vc8 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorPlugin\navigatorPlugin\Debug_vc9 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorPlugin\navigatorPlugin\Release_vc7 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorPlugin\navigatorPlugin\Release_vc8 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorPlugin\navigatorPlugin\Release_vc9 /S /Q
del Sources\Navigator\WebPlugins\navigatorPlugin\navigatorPlugin\nsINavigatorPlugin.xpt /Q
del Sources\Navigator\WebPlugins\navigatorPlugin\navigatorPlugin\SolipsisNavigator.xpt /Q

rmdir Sources\Navigator\WebPlugins\navigatorRunnerPlugin\navigatorRunnerAx\Debug_vc7 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorRunnerPlugin\navigatorRunnerAx\Debug_vc8 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorRunnerPlugin\navigatorRunnerAx\Debug_vc9 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorRunnerPlugin\navigatorRunnerAx\Release_vc7 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorRunnerPlugin\navigatorRunnerAx\Release_vc8 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorRunnerPlugin\navigatorRunnerAx\Release_vc9 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorRunnerPlugin\navigatorRunnerMoz\Debug_vc7 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorRunnerPlugin\navigatorRunnerMoz\Debug_vc8 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorRunnerPlugin\navigatorRunnerMoz\Debug_vc9 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorRunnerPlugin\navigatorRunnerMoz\Release_vc7 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorRunnerPlugin\navigatorRunnerMoz\Release_vc8 /S /Q
rmdir Sources\Navigator\WebPlugins\navigatorRunnerPlugin\navigatorRunnerMoz\Release_vc9 /S /Q

rmdir Sources\Communication\XmlDatas\Debug /S /Q
rmdir Sources\Communication\XmlDatas\Release /S /Q
rmdir Sources\Communication\P2N\P2NClient\xmlrpc++\Debug /S /Q
rmdir Sources\Communication\P2N\P2NClient\xmlrpc++\Release /S /Q
rmdir Sources\Communication\P2N\P2NClient\ulxmlrpcpp\Debug /S /Q
rmdir Sources\Communication\P2N\P2NClient\ulxmlrpcpp\Release /S /Q
rmdir Sources\Communication\P2N\P2NServer\xmlrpc++\Debug /S /Q
rmdir Sources\Communication\P2N\P2NServer\xmlrpc++\Release /S /Q
rmdir Sources\Communication\P2N\P2NServer\ulxmlrpcpp\Debug /S /Q
rmdir Sources\Communication\P2N\P2NServer\ulxmlrpcpp\Release /S /Q
rmdir Sources\Communication\P2N\P2NNullClientServer\Debug /S /Q
rmdir Sources\Communication\P2N\P2NNullClientServer\Release /S /Q

rmdir Sources\Communication\RakNetObjects\Debug /S /Q
rmdir Sources\Communication\RakNetObjects\Release /S /Q

rmdir Sources\Peer\PeerModuleRakNet\Debug /S /Q
rmdir Sources\Peer\PeerModuleRakNet\Release /S /Q
rmdir Sources\Peer\PeerModuleRakNet\DebugNCS /S /Q
rmdir Sources\Peer\PeerModuleRakNet\ReleaseNCS /S /Q

rmdir Sources\Peer\PeerPhysX\peerPhysX\Debug /S /Q
rmdir Sources\Peer\PeerPhysX\peerPhysX\Release /S /Q
rmdir Sources\Peer\PeerPhysX\peerPhysX\DebugNCS /S /Q
rmdir Sources\Peer\PeerPhysX\peerPhysX\ReleaseNCS /S /Q

rmdir Sources\Navigator\navigatorConfigDialog\Debug /S /Q
rmdir Sources\Navigator\navigatorConfigDialog\Release /S /Q

rmdir Peer\PeerLauncher\Debug /S /Q
rmdir Peer\PeerLauncher\Release /S /Q

rmdir Sources\Navigator\NavigatorPlugins\VNCPlugin\Debug /S /Q
rmdir Sources\Navigator\NavigatorPlugins\VNCPlugin\Release /S /Q
rmdir Sources\Navigator\NavigatorPlugins\VNCPlugin\vncviewer\omnithread\Debug /S /Q
rmdir Sources\Navigator\NavigatorPlugins\VNCPlugin\vncviewer\omnithread\Release /S /Q
rmdir Sources\Navigator\NavigatorPlugins\VNCPlugin\vncviewer\libjpeg\Debug /S /Q
rmdir Sources\Navigator\NavigatorPlugins\VNCPlugin\vncviewer\libjpeg\Release /S /Q
rmdir Sources\Navigator\NavigatorPlugins\VNCPlugin\vncviewer\zlib\Debug /S /Q
rmdir Sources\Navigator\NavigatorPlugins\VNCPlugin\vncviewer\zlib\Release /S /Q
rmdir Sources\Navigator\NavigatorPlugins\VNCPlugin\vncviewer\Debug /S /Q
rmdir Sources\Navigator\NavigatorPlugins\VNCPlugin\vncviewer\Release /S /Q

rmdir Sources\Voice\voiceEngine\Debug /S /Q
rmdir Sources\Voice\voiceEngine\Release /S /Q
rmdir Sources\Voice\voiceEngine\SimpleVoiceEngineProtocol\Debug /S /Q
rmdir Sources\Voice\voiceEngine\SimpleVoiceEngineProtocol\Release /S /Q
rmdir Sources\Voice\voiceEngine\Client\Debug /S /Q
rmdir Sources\Voice\voiceEngine\Client\Release /S /Q
rmdir Sources\Voice\voiceEngine\SimpleServer\Debug /S /Q
rmdir Sources\Voice\voiceEngine\SimpleServer\Release /S /Q
rmdir Sources\Voice\voiceEngine\TestClient\Debug /S /Q
rmdir Sources\Voice\voiceEngine\TestClient\Release /S /Q
rmdir Sources\Voice\voiceEngine\TestServer\Debug /S /Q
rmdir Sources\Voice\voiceEngine\TestServer\Release /S /Q
rmdir Sources\Voice\voiceEngine\voiceEngineFModSpeex\Debug /S /Q
rmdir Sources\Voice\voiceEngine\voiceEngineFModSpeex\Release /S /Q

rmdir Navigator\NavigatorPlugins\plugin_3ds\3dsftk3\SOURCE\Debug /S /Q
rmdir Navigator\NavigatorPlugins\plugin_3ds\3dsftk3\SOURCE\Release /S /Q
rmdir Navigator\NavigatorPlugins\plugin_3ds\Debug /S /Q
rmdir Navigator\NavigatorPlugins\plugin_3ds\Release /S /Q

rmdir Navigator\NavigatorPlugins\plugin_skp\Debug /S /Q
rmdir Navigator\NavigatorPlugins\plugin_skp\Release /S /Q

rmdir Sources\RakNetServer\Debug /S /Q
rmdir Sources\RakNetServer\Release /S /Q

rmdir Sources\Navigator\NavigatorPlugins\VLCPlugin\Debug /S /Q
rmdir Sources\Navigator\NavigatorPlugins\VLCPlugin\Release /S /Q

rmdir Sources\Navigator\authentFB\Debug /S /Q
rmdir Sources\Navigator\authentFB\Release /S /Q

rmdir Sources\Voice\Phonetizer\Debug /S /Q
rmdir Sources\Voice\Phonetizer\Release /S /Q
rmdir Sources\Voice\DummyPhonetizer\Debug /S /Q
rmdir Sources\Voice\DummyPhonetizer\Release /S /Q
rmdir Sources\Voice\FaceController\Debug /S /Q
rmdir Sources\Voice\FaceController\Release /S /Q
rmdir Sources\Modelers\DeclarativeModeler\Debug /S /Q
rmdir Sources\Modelers\DeclarativeModeler\Release /S /Q

rmdir Sources\Navigator\NavigatorPlugins\plugin_swf\Debug /S /Q
rmdir Sources\Navigator\NavigatorPlugins\plugin_swf\Release /S /Q

rmdir Sources\Navigator\NavigatorPlugins\StereoPlugin\Debug /S /Q
rmdir Sources\Navigator\NavigatorPlugins\StereoPlugin\Release /S /Q

del setup\nsis\Solipsis-*-win32.exe

echo ============== Done! ===============

