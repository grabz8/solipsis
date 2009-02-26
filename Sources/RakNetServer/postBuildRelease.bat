set dst="..\..\Common\bin\RakNetServer\Release"

if not exist "%dst%" mkdir "%dst%"
if exist "%dst%\OgreMain.dll" goto OgreMain_exist
 if exist "%OGRE_HOME%\lib\OgreMain.dll" xcopy "%OGRE_HOME%\lib\OgreMain.dll" "%dst%" /s /y
 if exist "%OGRE_HOME%\bin\release\OgreMain.dll" xcopy "%OGRE_HOME%\bin\release\OgreMain.dll" "%dst%" /s /y
:OgreMain_exist
if not exist "%dst%\pthreadVC.dll" xcopy "..\..\..\Dependencies\pthreads\lib\pthreadVC.dll" "%dst%" /s /y
xcopy "..\..\..\Dependencies\protocols\XmlDatas\lib\ReleaseDLL\XmlDatas.dll" "%dst%" /s /y
if not exist "%dst%\RakNet.dll" xcopy "..\..\..\Dependencies\RakNet\lib\RakNet.dll" "%dst%" /s /y
xcopy "..\..\..\Dependencies\libiconv\lib\libiconv2.dll" "%dst%" /s /y

copy "readme.txt" "%dst%" /y
