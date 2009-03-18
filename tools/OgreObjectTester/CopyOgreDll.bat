set dst="debug"

if not exist "%dst%" mkdir "%dst%"

 if exist "%OGRE_HOME%\lib\OgreMain_d.dll" xcopy "%OGRE_HOME%\lib\OgreMain_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\OgreMain_d.dll" xcopy "%OGRE_HOME%\bin\debug\OgreMain_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\lib\Plugin_BSPSceneManager_d.dll" xcopy "%OGRE_HOME%\lib\Plugin_BSPSceneManager_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\Plugin_BSPSceneManager_d.dll" xcopy "%OGRE_HOME%\bin\debug\Plugin_BSPSceneManager_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\lib\Plugin_OctreeSceneManager_d.dll" xcopy "%OGRE_HOME%\lib\Plugin_OctreeSceneManager_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\Plugin_OctreeSceneManager_d.dll" xcopy "%OGRE_HOME%\bin\debug\Plugin_OctreeSceneManager_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\lib\Plugin_OctreeZone_d.dll" xcopy "%OGRE_HOME%\lib\Plugin_OctreeZone_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\Plugin_OctreeZone_d.dll" xcopy "%OGRE_HOME%\bin\debug\Plugin_OctreeZone_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\lib\Plugin_PCZSceneManager_d.dll" xcopy "%OGRE_HOME%\lib\Plugin_PCZSceneManager_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\Plugin_PCZSceneManager_d.dll" xcopy "%OGRE_HOME%\bin\debug\Plugin_PCZSceneManager_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\lib\Plugin_ParticleFX_d.dll" xcopy "%OGRE_HOME%\lib\Plugin_ParticleFX_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\Plugin_ParticleFX_d.dll" xcopy "%OGRE_HOME%\bin\debug\Plugin_ParticleFX_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\lib\Plugin_CgProgramManager_d.dll" xcopy "%OGRE_HOME%\lib\Plugin_CgProgramManager_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\Plugin_CgProgramManager_d.dll" xcopy "%OGRE_HOME%\bin\debug\Plugin_CgProgramManager_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\lib\RenderSystem_Direct3D9_d.dll" xcopy "%OGRE_HOME%\lib\RenderSystem_Direct3D9_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\RenderSystem_Direct3D9_d.dll" xcopy "%OGRE_HOME%\bin\debug\RenderSystem_Direct3D9_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\lib\RenderSystem_GL_d.dll" xcopy "%OGRE_HOME%\lib\RenderSystem_GL_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\RenderSystem_GL_d.dll" xcopy "%OGRE_HOME%\bin\debug\RenderSystem_GL_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\Samples\Common\bin\Debug\cg.dll" xcopy "%OGRE_HOME%\Samples\Common\bin\Debug\cg.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\cg.dll" xcopy "%OGRE_HOME%\bin\debug\cg.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\Samples\Common\bin\Debug\OIS_d.dll" xcopy "%OGRE_HOME%\Samples\Common\bin\Debug\OIS_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\OIS_d.dll" xcopy "%OGRE_HOME%\bin\debug\OIS_d.dll" "%dst%" /s /y

set dst="release"


if not exist "%dst%" mkdir "%dst%"
 if exist "%OGRE_HOME%\lib\OgreMain.dll" xcopy "%OGRE_HOME%\lib\OgreMain.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\OgreMain.dll" xcopy "%OGRE_HOME%\bin\release\OgreMain.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\lib\Plugin_BSPSceneManager.dll" xcopy "%OGRE_HOME%\lib\Plugin_BSPSceneManager.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\Plugin_BSPSceneManager.dll" xcopy "%OGRE_HOME%\bin\release\Plugin_BSPSceneManager.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\lib\Plugin_OctreeSceneManager.dll" xcopy "%OGRE_HOME%\lib\Plugin_OctreeSceneManager.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\Plugin_OctreeSceneManager.dll" xcopy "%OGRE_HOME%\bin\release\Plugin_OctreeSceneManager.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\lib\Plugin_OctreeZone.dll" xcopy "%OGRE_HOME%\lib\Plugin_OctreeZone.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\Plugin_OctreeZone.dll" xcopy "%OGRE_HOME%\bin\debug\Plugin_OctreeZone.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\lib\Plugin_PCZSceneManager.dll" xcopy "%OGRE_HOME%\lib\Plugin_PCZSceneManager.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\Plugin_PCZSceneManager.dll" xcopy "%OGRE_HOME%\bin\debug\Plugin_PCZSceneManager.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\lib\Plugin_ParticleFX.dll" xcopy "%OGRE_HOME%\lib\Plugin_ParticleFX.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\Plugin_ParticleFX.dll" xcopy "%OGRE_HOME%\bin\release\Plugin_ParticleFX.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\lib\Plugin_CgProgramManager.dll" xcopy "%OGRE_HOME%\lib\Plugin_CgProgramManager.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\Plugin_CgProgramManager.dll" xcopy "%OGRE_HOME%\bin\release\Plugin_CgProgramManager.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\lib\RenderSystem_Direct3D9.dll" xcopy "%OGRE_HOME%\lib\RenderSystem_Direct3D9.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\RenderSystem_Direct3D9.dll" xcopy "%OGRE_HOME%\bin\release\RenderSystem_Direct3D9.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\lib\RenderSystem_GL.dll" xcopy "%OGRE_HOME%\lib\RenderSystem_GL.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\RenderSystem_GL.dll" xcopy "%OGRE_HOME%\bin\release\RenderSystem_GL.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\Samples\Common\bin\Release\cg.dll" xcopy "%OGRE_HOME%\Samples\Common\bin\Release\cg.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\cg.dll" xcopy "%OGRE_HOME%\bin\release\cg.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\Samples\Common\bin\Release\OIS.dll" xcopy "%OGRE_HOME%\Samples\Common\bin\Release\OIS.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\OIS.dll" xcopy "%OGRE_HOME%\bin\release\OIS.dll" "%dst%" /s /y
