set dst="..\Common\bin\nodeSimulator\Release"
set vc="%1"

if not exist "%dst%" mkdir "%dst%"
copy Release\nodeSimulator.exe "%dst%" /y
if exist "%dst%\OgreMain.dll" goto OgreMain_exist
 if exist "%OGRE_HOME%\lib\OgreMain.dll" xcopy "%OGRE_HOME%\lib\OgreMain.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\OgreMain.dll" xcopy "%OGRE_HOME%\bin\release\OgreMain.dll" "%dst%" /s /y
:OgreMain_exist
if not exist "%dst%\pthreadVC.dll" xcopy "..\Dependencies\pthreads\lib\pthreadVC.dll" "%dst%" /s /y

copy "readme.txt" "%dst%" /y
