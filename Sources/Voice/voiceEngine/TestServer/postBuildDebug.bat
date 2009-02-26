set dst="..\..\..\..\Common\bin\VoiceEngineServer\Debug"

if not exist "%dst%" mkdir "%dst%"
if not exist "%dst%\pthreadVC_d.dll" xcopy "..\..\..\..\Dependencies\pthreads\lib\pthreadVC_d.dll" "%dst%" /s /y

copy "readme.txt" "%dst%" /y
