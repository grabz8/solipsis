if not exist "%1\OgreMain_d.dll" xcopy "%OGRE_HOME%\lib\OgreMain_d.dll" "%1" /s /y
if not exist "%1\Plugin_BSPSceneManager_d.dll" xcopy "%OGRE_HOME%\lib\Plugin_BSPSceneManager_d.dll" "%1" /s /y
if not exist "%1\Plugin_OctreeSceneManager_d.dll" xcopy "%OGRE_HOME%\lib\Plugin_OctreeSceneManager_d.dll" "%1" /s /y
if not exist "%1\Plugin_ParticleFX_d.dll" xcopy "%OGRE_HOME%\lib\Plugin_ParticleFX_d.dll" "%1" /s /y
if not exist "%1\Plugin_CgProgramManager_d.dll" xcopy "%OGRE_HOME%\lib\Plugin_CgProgramManager_d.dll" "%1" /s /y
if not exist "%1\RenderSystem_Direct3D9_d.dll" xcopy "%OGRE_HOME%\lib\RenderSystem_Direct3D9_d.dll" "%1" /s /y
if not exist "%1\RenderSystem_GL_d.dll" xcopy "%OGRE_HOME%\lib\RenderSystem_GL_d.dll" "%1" /s /y
if not exist "%1\cg.dll" xcopy "%OGRE_HOME%\Samples\Common\bin\Debug\cg.dll" "%1" /s /y
if not exist "%1\OIS_d.dll" xcopy "%OGRE_HOME%\Samples\Common\bin\Debug\OIS_d.dll" "%1" /s /y
if not exist "%1\pthreadVC_d.dll" xcopy "..\Dependencies\pthreads\lib\pthreadVC_d.dll" "%1" /s /y
if not exist "%1\xul.dll" xcopy "..\Dependencies\llmozlib\win32_vc7\runtime\Debug\*.*" "%1" /s /y
if not exist "%1\NaviLocal\" xcopy "..\Media\NaviLocal\*.*" "%1\NaviLocal" /s /y /i
if not exist "%1\NaviLocal\Navi.js" xcopy "..\Dependencies\navi\Javascript\*.*" "%1\NaviLocal" /y
if not exist "%1\ode.dll" xcopy "..\Dependencies\ode\lib\debugdll\ode.dll" "%1" /s /y
if not exist "%1\OgreOde_Core_d.dll" xcopy "..\Dependencies\OgreOde\lib\OgreOde_Core_d.dll" "%1" /s /y
if not exist "%1\Plugins.cfg" xcopy "%OGRE_HOME%\Samples\Common\bin\Debug\Plugins.cfg" "%1" /s /y
if not exist "%1\quake3settings.cfg" xcopy "%OGRE_HOME%\Samples\Common\bin\Debug\quake3settings.cfg" "%1" /s /y
if not exist "%1\resources.cfg" xcopy "..\Media\resources.cfg" "%1" /s /y
if not exist "%1\SolipsisFakeTerrain.cfg" xcopy "..\Media\SolipsisFakeTerrain.cfg" "%1" /s /y
if not exist "%1\readme.txt" copy "readme.txt" "%1readme.txt" /y