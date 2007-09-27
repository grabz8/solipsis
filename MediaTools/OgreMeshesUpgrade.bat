@echo off
rem OGRE_HOME must be defined (eg. c:\Work\ogre144src\ogrenew)
rem OgreMeshUpgrade.exe must be built and present into %OGRE_HOME%\Tools\MeshUpgrader\bin\debug\OgreMeshUpgrade.exe

PATH=%PATH%;%OGRE_HOME%\lib
cd %1
for %%f in (*.mesh) do %OGRE_HOME%\Tools\MeshUpgrader\bin\debug\OgreMeshUpgrade.exe %%f
pause
