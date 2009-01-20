set dst="..\..\Common\bin\VoiceEngineServer\Release"

if not exist "%dst%" mkdir "%dst%"
if not exist "%dst%\pthreadVC.dll" xcopy "..\..\Dependencies\pthreads\lib\pthreadVC.dll" "%dst%" /s /y

copy "readme.txt" "%dst%" /y
