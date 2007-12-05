set dst="..\Common\bin\navigator\Release"
set vc="%1"

if not exist "..\Dependencies\navigatorModule\include" mkdir ..\Dependencies\navigatorModule\include
copy NavigatorModule.h ..\Dependencies\navigatorModule\include\. /y
copy Event.h ..\Dependencies\navigatorModule\include\. /y
copy IApplication.h ..\Dependencies\navigatorModule\include\. /y
copy IInstance.h ..\Dependencies\navigatorModule\include\. /y
copy IWindow.h ..\Dependencies\navigatorModule\include\. /y
if not exist ..\Dependencies\navigatorModule\lib\Release mkdir ..\Dependencies\navigatorModule\lib\Release
copy Release\Navigator.lib ..\Dependencies\navigatorModule\lib\Release\. /y
copy Release\Navigator.dll ..\Dependencies\navigatorModule\lib\Release\. /y

if not exist "%dst%" mkdir "%dst%"
copy Release\Navigator.dll "%dst%" /y
if not exist "%dst%\OgreMain.dll" xcopy "%OGRE_HOME%\lib\OgreMain.dll" "%dst%" /s /y
if not exist "%dst%\Plugin_BSPSceneManager.dll" xcopy "%OGRE_HOME%\lib\Plugin_BSPSceneManager.dll" "%dst%" /s /y
if not exist "%dst%\Plugin_OctreeSceneManager.dll" xcopy "%OGRE_HOME%\lib\Plugin_OctreeSceneManager.dll" "%dst%" /s /y
if not exist "%dst%\Plugin_ParticleFX.dll" xcopy "%OGRE_HOME%\lib\Plugin_ParticleFX.dll" "%dst%" /s /y
if not exist "%dst%\Plugin_CgProgramManager.dll" xcopy "%OGRE_HOME%\lib\Plugin_CgProgramManager.dll" "%dst%" /s /y
if not exist "%dst%\RenderSystem_Direct3D9.dll" xcopy "%OGRE_HOME%\lib\RenderSystem_Direct3D9.dll" "%dst%" /s /y
if not exist "%dst%\RenderSystem_GL.dll" xcopy "%OGRE_HOME%\lib\RenderSystem_GL.dll" "%dst%" /s /y
if not exist "%dst%\cg.dll" xcopy "%OGRE_HOME%\Samples\Common\bin\Release\cg.dll" "%dst%" /s /y
if not exist "%dst%\OIS.dll" xcopy "%OGRE_HOME%\Samples\Common\bin\Release\OIS.dll" "%dst%" /s /y
if not exist "%dst%\pthreadVC.dll" xcopy "..\Dependencies\pthreads\lib\pthreadVC.dll" "%dst%" /s /y
if not exist "%dst%\xul.dll" xcopy "..\Dependencies\llmozlib\win32_%vc%\runtime\Release\*.*" "%dst%" /s /y
if not exist "%dst%\NaviLocal\" xcopy "..\Media\NaviLocal\*.*" "%dst%\NaviLocal" /s /y /i
if not exist "%dst%\NaviLocal\Navi.js" xcopy "..\Dependencies\navi\Javascript\*.*" "%dst%\NaviLocal" /y
if not exist "%dst%\lua\" xcopy "..\Media\lua\*.*" "%dst%\lua" /s /y /i
if not exist "%dst%\OgreOde_Core.dll" xcopy "..\Dependencies\OgreOde\lib\OgreOde_Core.dll" "%dst%" /s /y
if not exist "%dst%\Plugins.cfg" xcopy "%OGRE_HOME%\Samples\Common\bin\Release\Plugins.cfg" "%dst%" /s /y
if not exist "%dst%\quake3settings.cfg" xcopy "%OGRE_HOME%\Samples\Common\bin\Release\quake3settings.cfg" "%dst%" /s /y
if not exist "%dst%\resources.cfg" xcopy "..\Media\resources.cfg" "%dst%" /s /y
if not exist "%dst%\SolipsisFakeTerrain.cfg" xcopy "..\Media\SolipsisFakeTerrain.cfg" "%dst%" /s /y
if not exist "%dst%\readme.txt" copy "..\readme.txt" "%dst%\readme.txt" /y