@echo off
rem 
rem This tool is usefull when you want to upgrade all mesh files of 1 directory
rem just drag and drop the directory you want to convert mesh files on this .BAT command batch
rem
rem OGRE_HOME must be defined (eg. c:\Work\ogre144src\ogrenew)
rem OgreMeshUpgrade.exe must be built and present into %OGRE_HOME%\Tools\Common\bin\release\OgreMeshUpgrade.exe

PATH=%PATH%;%OGRE_HOME%\lib
cd %1
for %%f in (*.mesh) do %OGRE_HOME%\Tools\Common\bin\release\OgreMeshUpgrade.exe %%f
pause
