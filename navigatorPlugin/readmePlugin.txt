To install the Navigator ActiveX:

open 1 DOS command window
go into the Navigator directory containing navigatorActiveX.dll
and launch : regsvr32 /s /c "navigatorActiveX.dll"



To install the Navigator Mozilla plugin:

Run command : regedit
Go into HKEY_CURRENT_USER\Software
Add 1 Key "Solipsis"
Go into HKEY_CURRENT_USER\Software\Solipsis
Add 1 String Value "Install Directory"
Set its value to the Navigator directory,
for example: C:\tmp\Common\bin\navigator\release
