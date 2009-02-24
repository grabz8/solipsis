@echo off

rem Just drag and drop the release directory on this batch and the dedicated plugin will
rem be copied into Mozilla plugins directory

if not exist "C:\Program Files\Mozilla Firefox\plugins" goto MozillaFirefoxNotFound
copy "%1\npsolnavrun.dll" "C:\Program Files\Mozilla Firefox\plugins\." /y
copy "%1\..\SolipsisNavigatorRunner.xpt" "C:\Program Files\Mozilla Firefox\plugins\." /y
goto End

:MozillaFirefoxNotFound
:End
