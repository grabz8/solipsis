set dst="..\Common\bin\navigator\Release"
set vc="%1"

if not exist "..\Dependencies\navigatorModule\include" mkdir ..\Dependencies\navigatorModule\include
copy ..\navigatorModule\NavigatorModule.h ..\Dependencies\navigatorModule\include\. /y
copy ..\navigatorModule\Event.h ..\Dependencies\navigatorModule\include\. /y
copy ..\navigatorModule\IApplication.h ..\Dependencies\navigatorModule\include\. /y
copy ..\navigatorModule\IInstance.h ..\Dependencies\navigatorModule\include\. /y
copy ..\navigatorModule\IWindow.h ..\Dependencies\navigatorModule\include\. /y
copy ..\navigatorModule\OgreHelpers.h ..\Dependencies\navigatorModule\include\. /y
copy .\IVoiceEngine.h ..\Dependencies\navigatorModule\include\. /y
copy .\VoiceEngineManager.h ..\Dependencies\navigatorModule\include\. /y
copy .\ExternalTextureSourceEx.h ..\Dependencies\navigatorModule\include\. /y

if not exist ..\Dependencies\navigatorModule\lib\Release mkdir ..\Dependencies\navigatorModule\lib\Release
copy ReleaseNCS\Navigator.lib ..\Dependencies\navigatorModule\lib\Release\. /y
copy ReleaseNCS\Navigator.dll ..\Dependencies\navigatorModule\lib\Release\. /y

if not exist "%dst%" mkdir "%dst%"
copy ReleaseNCS\Navigator.dll "%dst%" /y
if exist "%dst%\OgreMain.dll" goto OgreMain_exist
 if exist "%OGRE_HOME%\lib\OgreMain.dll" xcopy "%OGRE_HOME%\lib\OgreMain.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\OgreMain.dll" xcopy "%OGRE_HOME%\bin\release\OgreMain.dll" "%dst%" /s /y
:OgreMain_exist
if exist "%dst%\Plugin_BSPSceneManager.dll" goto Plugin_BSPSceneManager_exist
 if exist "%OGRE_HOME%\lib\Plugin_BSPSceneManager.dll" xcopy "%OGRE_HOME%\lib\Plugin_BSPSceneManager.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\Plugin_BSPSceneManager.dll" xcopy "%OGRE_HOME%\bin\release\Plugin_BSPSceneManager.dll" "%dst%" /s /y
:Plugin_BSPSceneManager_exist
if exist "%dst%\Plugin_OctreeSceneManager.dll" goto Plugin_OctreeSceneManager_exist
 if exist "%OGRE_HOME%\lib\Plugin_OctreeSceneManager.dll" xcopy "%OGRE_HOME%\lib\Plugin_OctreeSceneManager.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\Plugin_OctreeSceneManager.dll" xcopy "%OGRE_HOME%\bin\release\Plugin_OctreeSceneManager.dll" "%dst%" /s /y
:Plugin_OctreeSceneManager_exist
if exist "%dst%\Plugin_ParticleFX.dll" goto Plugin_ParticleFX_exist
 if exist "%OGRE_HOME%\lib\Plugin_ParticleFX.dll" xcopy "%OGRE_HOME%\lib\Plugin_ParticleFX.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\Plugin_ParticleFX.dll" xcopy "%OGRE_HOME%\bin\release\Plugin_ParticleFX.dll" "%dst%" /s /y
:Plugin_ParticleFX_exist
if exist "%dst%\Plugin_CgProgramManager.dll" goto Plugin_CgProgramManager_exist
 if exist "%OGRE_HOME%\lib\Plugin_CgProgramManager.dll" xcopy "%OGRE_HOME%\lib\Plugin_CgProgramManager.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\Plugin_CgProgramManager.dll" xcopy "%OGRE_HOME%\bin\release\Plugin_CgProgramManager.dll" "%dst%" /s /y
:Plugin_CgProgramManager_exist
if exist "%dst%\RenderSystem_Direct3D9.dll" goto RenderSystem_Direct3D9_exist
 if exist "%OGRE_HOME%\lib\RenderSystem_Direct3D9.dll" xcopy "%OGRE_HOME%\lib\RenderSystem_Direct3D9.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\RenderSystem_Direct3D9.dll" xcopy "%OGRE_HOME%\bin\release\RenderSystem_Direct3D9.dll" "%dst%" /s /y
:RenderSystem_Direct3D9_exist
if exist "%dst%\RenderSystem_GL.dll" goto RenderSystem_GL_exist
 if exist "%OGRE_HOME%\lib\RenderSystem_GL.dll" xcopy "%OGRE_HOME%\lib\RenderSystem_GL.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\RenderSystem_GL.dll" xcopy "%OGRE_HOME%\bin\release\RenderSystem_GL.dll" "%dst%" /s /y
:RenderSystem_GL_exist
if exist "%dst%\cg.dll" goto cg_exist
 if exist "%OGRE_HOME%\Samples\Common\bin\Release\cg.dll" xcopy "%OGRE_HOME%\Samples\Common\bin\Release\cg.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\cg.dll" xcopy "%OGRE_HOME%\bin\release\cg.dll" "%dst%" /s /y
:cg_exist
if exist "%dst%\OIS.dll" goto OIS_exist
 if exist "%OGRE_HOME%\Samples\Common\bin\Release\OIS.dll" xcopy "%OGRE_HOME%\Samples\Common\bin\Release\OIS.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\OIS.dll" xcopy "%OGRE_HOME%\bin\release\OIS.dll" "%dst%" /s /y
:OIS_exist
if not exist "%dst%\pthreadVC.dll" xcopy "..\Dependencies\pthreads\lib\pthreadVC.dll" "%dst%" /s /y
if not exist "%dst%\xul.dll" xcopy "..\Dependencies\llmozlib\win32_%vc%\runtime\Release\*.*" "%dst%" /s /y
xcopy "..\Dependencies\protocols\XmlDatas\lib\ReleaseDLL\XmlDatas.dll" "%dst%" /s /y
xcopy "..\Dependencies\protocols\P2NNullClientServer\ReleaseDLL\P2NNullClientServer.dll" "%dst%" /s /y
xcopy "..\Dependencies\peerModule\lib\Release\Peer.dll" "%dst%" /s /y
if exist "..\Dependencies\expat\lib\ReleaseDLL\libexpat.dll" xcopy "..\Dependencies\expat\lib\ReleaseDLL\libexpat.dll" "%dst%" /s /y
if not exist "%dst%\NaviLocal\" xcopy "..\Media\NaviLocal\*.*" "%dst%\NaviLocal" /s /y /i
if not exist "%dst%\NaviLocal\Navi.js" xcopy "..\Dependencies\navi\Javascript\*.*" "%dst%\NaviLocal" /y
if not exist "%dst%\lua\" xcopy "..\Media\lua\*.*" "%dst%\lua" /s /y /i
xcopy "%FMOD_PATH%\fmodex.dll" "%dst%" /s /y
if not exist "%dst%\plugin_3ds.dll" xcopy "..\Dependencies\plugin_3ds\lib\plugin_3ds.dll" "%dst%" /s /y

if exist "%dst%\Plugins.cfg" goto Plugins_exist
 if exist "%OGRE_HOME%\Samples\Common\bin\Release\Plugins.cfg" xcopy "%OGRE_HOME%\Samples\Common\bin\Release\Plugins.cfg" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\Plugins.cfg" xcopy "%OGRE_HOME%\bin\release\Plugins.cfg" "%dst%" /s /y
:Plugins_exist
if exist "%dst%\quake3settings.cfg" goto quake3settings_exist
 if exist "%OGRE_HOME%\Samples\Common\bin\Release\quake3settings.cfg" xcopy "%OGRE_HOME%\Samples\Common\bin\Release\quake3settings.cfg" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\quake3settings.cfg" xcopy "%OGRE_HOME%\bin\release\quake3settings.cfg" "%dst%" /s /y
:quake3settings_exist
xcopy "..\Media\resources.cfg" "%dst%" /s /y
copy "..\readme.txt" "%dst%" /y
