set dst="..\..\Common\bin\RakNetServer\Debug"

if not exist "%dst%" mkdir "%dst%"
if exist "%dst%\OgreMain_d.dll" goto OgreMain_d_exist
 if exist "%OGRE_HOME%\lib\OgreMain_d.dll" xcopy "%OGRE_HOME%\lib\OgreMain_d.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\debug\OgreMain_d.dll" xcopy "%OGRE_HOME%\bin\debug\OgreMain_d.dll" "%dst%" /s /y
:OgreMain_d_exist
if not exist "%dst%\pthreadVC_d.dll" xcopy "..\..\Dependencies\pthreads\lib\pthreadVC_d.dll" "%dst%" /s /y
xcopy "..\..\Dependencies\protocols\XmlDatas\lib\DebugDLL\XmlDatas.dll" "%dst%" /s /y
if not exist "%dst%\RakNetDebug.dll" xcopy "%RAKNET_PATH%\Lib\RakNetDebug.dll" "%dst%" /s /y
xcopy "..\..\Dependencies\libiconv\lib\libiconv2.dll" "%dst%" /s /y

copy "readme.txt" "%dst%" /y
