set dst="..\..\Common\bin\navigator\Debug"
set vc="%1"

if not exist "..\..\Dependencies\navigatorModule\include" mkdir ..\..\Dependencies\navigatorModule\include
copy .\NavigatorModule.h ..\..\Dependencies\navigatorModule\include\. /y
copy .\Event.h ..\..\Dependencies\navigatorModule\include\. /y
copy .\IApplication.h ..\..\Dependencies\navigatorModule\include\. /y
copy .\IInstance.h ..\..\Dependencies\navigatorModule\include\. /y
copy .\IWindow.h ..\..\Dependencies\navigatorModule\include\. /y
copy .\OgreHelpers.h ..\..\Dependencies\navigatorModule\include\. /y
copy .\ExternalTextureSourceEx.h ..\..\Dependencies\navigatorModule\include\. /y

if not exist ..\..\Dependencies\navigatorModule\lib\Debug mkdir ..\..\Dependencies\navigatorModule\lib\Debug
copy Debug\Navigator.lib ..\..\Dependencies\navigatorModule\lib\Debug\. /y
copy Debug\Navigator.dll ..\..\Dependencies\navigatorModule\lib\Debug\. /y

if not exist "%dst%" mkdir "%dst%"
copy Debug\Navigator.dll "%dst%" /y
if exist "%dst%\OgreMain_d.dll" goto OgreMain_d_exist
 if exist "%OGRE_HOME%\lib\OgreMain_d.dll" xcopy "%OGRE_HOME%\lib\OgreMain_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\OgreMain_d.dll" xcopy "%OGRE_HOME%\bin\debug\OgreMain_d.dll" "%dst%" /s /y
:OgreMain_d_exist
if exist "%dst%\Plugin_BSPSceneManager_d.dll" goto Plugin_BSPSceneManager_d_exist
 if exist "%OGRE_HOME%\lib\Plugin_BSPSceneManager_d.dll" xcopy "%OGRE_HOME%\lib\Plugin_BSPSceneManager_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\Plugin_BSPSceneManager_d.dll" xcopy "%OGRE_HOME%\bin\debug\Plugin_BSPSceneManager_d.dll" "%dst%" /s /y
:Plugin_BSPSceneManager_d_exist
if exist "%dst%\Plugin_OctreeSceneManager_d.dll" goto Plugin_OctreeSceneManager_d_exist
 if exist "%OGRE_HOME%\lib\Plugin_OctreeSceneManager_d.dll" xcopy "%OGRE_HOME%\lib\Plugin_OctreeSceneManager_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\Plugin_OctreeSceneManager_d.dll" xcopy "%OGRE_HOME%\bin\debug\Plugin_OctreeSceneManager_d.dll" "%dst%" /s /y
:Plugin_OctreeSceneManager_d_exist
if exist "%dst%\Plugin_OctreeZone_d.dll" goto Plugin_OctreeZone_d_exist
 if exist "%OGRE_HOME%\lib\Plugin_OctreeZone_d.dll" xcopy "%OGRE_HOME%\lib\Plugin_OctreeZone_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\Plugin_OctreeZone_d.dll" xcopy "%OGRE_HOME%\bin\debug\Plugin_OctreeZone_d.dll" "%dst%" /s /y
:Plugin_OctreeZone_d_exist
if exist "%dst%\Plugin_PCZSceneManager_d.dll" goto Plugin_PCZSceneManager_d_exist
 if exist "%OGRE_HOME%\lib\Plugin_PCZSceneManager_d.dll" xcopy "%OGRE_HOME%\lib\Plugin_PCZSceneManager_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\Plugin_PCZSceneManager_d.dll" xcopy "%OGRE_HOME%\bin\debug\Plugin_PCZSceneManager_d.dll" "%dst%" /s /y
:Plugin_PCZSceneManager_d_exist
if exist "%dst%\Plugin_ParticleFX_d.dll" goto Plugin_ParticleFX_d_exist
 if exist "%OGRE_HOME%\lib\Plugin_ParticleFX_d.dll" xcopy "%OGRE_HOME%\lib\Plugin_ParticleFX_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\Plugin_ParticleFX_d.dll" xcopy "%OGRE_HOME%\bin\debug\Plugin_ParticleFX_d.dll" "%dst%" /s /y
:Plugin_ParticleFX_d_exist
if exist "%dst%\Plugin_CgProgramManager_d.dll" goto Plugin_CgProgramManager_d_exist
 if exist "%OGRE_HOME%\lib\Plugin_CgProgramManager_d.dll" xcopy "%OGRE_HOME%\lib\Plugin_CgProgramManager_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\Plugin_CgProgramManager_d.dll" xcopy "%OGRE_HOME%\bin\debug\Plugin_CgProgramManager_d.dll" "%dst%" /s /y
:Plugin_CgProgramManager_d_exist
if exist "%dst%\RenderSystem_Direct3D9_d.dll" goto RenderSystem_Direct3D9_d_exist
 if exist "%OGRE_HOME%\lib\RenderSystem_Direct3D9_d.dll" xcopy "%OGRE_HOME%\lib\RenderSystem_Direct3D9_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\RenderSystem_Direct3D9_d.dll" xcopy "%OGRE_HOME%\bin\debug\RenderSystem_Direct3D9_d.dll" "%dst%" /s /y
:RenderSystem_Direct3D9_d_exist
if exist "%dst%\RenderSystem_GL_d.dll" goto RenderSystem_GL_d_exist
 if exist "%OGRE_HOME%\lib\RenderSystem_GL_d.dll" xcopy "%OGRE_HOME%\lib\RenderSystem_GL_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\RenderSystem_GL_d.dll" xcopy "%OGRE_HOME%\bin\debug\RenderSystem_GL_d.dll" "%dst%" /s /y
:RenderSystem_GL_d_exist
if exist "%dst%\cg.dll" goto cg_exist
 if exist "%OGRE_HOME%\Samples\Common\bin\Debug\cg.dll" xcopy "%OGRE_HOME%\Samples\Common\bin\Debug\cg.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\cg.dll" xcopy "%OGRE_HOME%\bin\debug\cg.dll" "%dst%" /s /y
:cg_exist
if exist "%dst%\OIS_d.dll" goto OIS_d_exist
 if exist "%OGRE_HOME%\Samples\Common\bin\Debug\OIS_d.dll" xcopy "%OGRE_HOME%\Samples\Common\bin\Debug\OIS_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\OIS_d.dll" xcopy "%OGRE_HOME%\bin\debug\OIS_d.dll" "%dst%" /s /y
:OIS_d_exist
if not exist "%dst%\pthreadVC_d.dll" xcopy "..\..\Dependencies\pthreads\lib\pthreadVC_d.dll" "%dst%" /s /y
if not exist "%dst%\xul.dll" xcopy "..\..\Dependencies\llmozlib\win32_%vc%\runtime\Debug\*.*" "%dst%" /s /y
xcopy "..\..\Dependencies\protocols\XmlDatas\lib\DebugDLL\XmlDatas.dll" "%dst%" /s /y
xcopy "..\..\Dependencies\protocols\P2NClient\xmlrpc++\DebugDLL\XMLRPCP2NClient.dll" "%dst%" /s /y
xcopy "..\..\Dependencies\protocols\P2NClient\ulxmlrpcpp\DebugDLL\ULXMLRPCPPP2NClient.dll" "%dst%" /s /y
if exist "..\..\Dependencies\ulxmlrpcpp\lib\DebugDLL\ulxmlrpcpp_23.dll" xcopy "..\..\Dependencies\ulxmlrpcpp\lib\DebugDLL\ulxmlrpcpp_23.dll" "%dst%" /s /y
if exist "..\..\Dependencies\expat\lib\DebugDLL\libexpat.dll" xcopy "..\..\Dependencies\expat\lib\DebugDLL\libexpat.dll" "%dst%" /s /y
if not exist "%dst%\NaviLocal\" xcopy "..\..\Media\NaviLocal\*.*" "%dst%\NaviLocal" /s /y /i
if not exist "%dst%\lua\" xcopy "..\..\Media\lua\*.*" "%dst%\lua" /s /y /i
if not exist "%dst%\fmodexL.dll" xcopy "%FMOD_PATH%\fmodexL.dll" "%dst%" /s /y
if not exist "%dst%\plugin_3ds_d.dll" xcopy "..\..\Dependencies\plugin_3ds\lib\plugin_3ds_d.dll" "%dst%" /s /y
if not exist "%dst%\plugin_skp_d.dll" xcopy "..\..\Dependencies\plugin_skp\include\Debug\*.dll" "%dst%" /s /y
if not exist "%dst%\libcurld.dll" xcopy "..\..\Dependencies\curl\lib\DebugDLL\libcurld.dll" "%dst%" /s /y
xcopy "..\..\Dependencies\navigatorConfigDialog\lib\WIN32\Debug\navigatorConfigDialog.dll" "%dst%" /s /y
xcopy "..\..\Dependencies\VoiceEngine\lib\Debug\voiceEngine.dll" "%dst%" /s /y
xcopy "..\..\Dependencies\Phonetizer\lib\Debug\Phonetizer.dll" "%dst%" /s /y
xcopy "..\..\Dependencies\FaceController\lib\Debug\FaceController.dll" "%dst%" /s /y
xcopy "..\..\Dependencies\libiconv\lib\libiconv2.dll" "%dst%" /s /y

if exist "%dst%\Plugins.cfg" goto Plugins_exist
 if exist "%OGRE_HOME%\Samples\Common\bin\Debug\Plugins.cfg" xcopy "%OGRE_HOME%\Samples\Common\bin\Debug\Plugins.cfg" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\Plugins.cfg" xcopy "%OGRE_HOME%\bin\debug\Plugins.cfg" "%dst%" /s /y
:Plugins_exist
if exist "%dst%\quake3settings.cfg" goto quake3settings_exist
 if exist "%OGRE_HOME%\Samples\Common\bin\Debug\quake3settings.cfg" xcopy "%OGRE_HOME%\Samples\Common\bin\Debug\quake3settings.cfg" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\quake3settings.cfg" xcopy "%OGRE_HOME%\bin\debug\quake3settings.cfg" "%dst%" /s /y
:quake3settings_exist
xcopy "..\..\Media\resources.cfg" "%dst%" /s /y
copy "..\..\readmeUser.txt" "%dst%\readme.txt" /y

pause