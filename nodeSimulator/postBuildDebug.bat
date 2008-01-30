set dst="..\Common\bin\nodeSimulator\Debug"
set vc="%1"

if not exist "%dst%" mkdir "%dst%"
copy Debug\nodeSimulator.exe "%dst%" /y
if exist "%dst%\OgreMain_d.dll" goto OgreMain_d_exist
 if exist "%OGRE_HOME%\lib\OgreMain_d.dll" xcopy "%OGRE_HOME%\lib\OgreMain_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\OgreMain_d.dll" xcopy "%OGRE_HOME%\bin\debug\OgreMain_d.dll" "%dst%" /s /y
:OgreMain_d_exist
if not exist "%dst%\pthreadVC_d.dll" xcopy "..\Dependencies\pthreads\lib\pthreadVC_d.dll" "%dst%" /s /y

copy "readme.txt" "%dst%" /y
