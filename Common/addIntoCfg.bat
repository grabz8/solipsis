@echo off

set configLine=%~1
set configFilename=%2

if exist %configFilename% goto configFileExist
echo BATCH WARNING, BATCH WARNING, BATCH WARNING
echo %configFilename% not found !!!
echo This plugin was not added ...
exit 1

:configFileExist
 find "%configLine%" %configFilename% >>NUL
 if %errorlevel% == 0 goto End
  echo %configLine% >> %configFilename%

:End
