if not exist "%1\OgreMain.dll" xcopy "%OGRE_HOME%\lib\OgreMain.dll" "%1" /s /y
if not exist "%1\Plugin_BSPSceneManager.dll" xcopy "%OGRE_HOME%\lib\Plugin_BSPSceneManager.dll" "%1" /s /y
if not exist "%1\Plugin_OctreeSceneManager.dll" xcopy "%OGRE_HOME%\lib\Plugin_OctreeSceneManager.dll" "%1" /s /y
if not exist "%1\Plugin_ParticleFX.dll" xcopy "%OGRE_HOME%\lib\Plugin_ParticleFX.dll" "%1" /s /y
if not exist "%1\Plugin_CgProgramManager.dll" xcopy "%OGRE_HOME%\lib\Plugin_CgProgramManager.dll" "%1" /s /y
if not exist "%1\RenderSystem_Direct3D9.dll" xcopy "%OGRE_HOME%\lib\RenderSystem_Direct3D9.dll" "%1" /s /y
if not exist "%1\RenderSystem_GL.dll" xcopy "%OGRE_HOME%\lib\RenderSystem_GL.dll" "%1" /s /y
if not exist "%1\cg.dll" xcopy "%OGRE_HOME%\Samples\Common\bin\Release\cg.dll" "%1" /s /y
if not exist "%1\OIS.dll" xcopy "%OGRE_HOME%\Samples\Common\bin\Release\OIS.dll" "%1" /s /y
if not exist "%1\pthreadVC.dll" xcopy "..\Dependencies\pthreads\lib\pthreadVC.dll" "%1" /s /y
if not exist "%1\xul.dll" xcopy "..\Dependencies\llmozlib\win32_vc8\runtime\Release\*.*" "%1" /s /y
if not exist "%1\NaviLocal\" xcopy "..\Media\NaviLocal\*.*" "%1\NaviLocal" /s /y /i
if not exist "%1\NaviLocal\Navi.js" xcopy "..\Dependencies\navi\Javascript\*.*" "%1\NaviLocal" /y
if not exist "%1\OgreOde_Core.dll" xcopy "..\Dependencies\OgreOde\lib\OgreOde_Core.dll" "%1" /s /y
if not exist "%1\Plugins.cfg" xcopy "%OGRE_HOME%\Samples\Common\bin\Release\Plugins.cfg" "%1" /s /y
if not exist "%1\quake3settings.cfg" xcopy "%OGRE_HOME%\Samples\Common\bin\Release\quake3settings.cfg" "%1" /s /y
if not exist "%1\resources.cfg" xcopy "..\Media\resources.cfg" "%1" /s /y
if not exist "%1\SolipsisFakeTerrain.cfg" xcopy "..\Media\SolipsisFakeTerrain.cfg" "%1" /s /y
if not exist "%1\readme.txt" copy "readme.txt" "%1readme.txt" /y