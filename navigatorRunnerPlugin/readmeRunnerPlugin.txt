To install the NavigatorRunner ActiveX:

open 1 DOS command window
go into the Navigator directory containing navigatorRunnerAx.dll
and launch : regsvr32 /s /c "navigatorRunnerAx.dll"



To install the NavigatorRunner Mozilla plugin:

Run command : regedit
Go into HKEY_CURRENT_USER\Software
Add 1 Key "Solipsis"
Go into HKEY_CURRENT_USER\Software\Solipsis
Add 1 String Value "Install Directory"
Set its value to the Navigator directory,
for example: C:\tmp\Common\bin\navigator\release
