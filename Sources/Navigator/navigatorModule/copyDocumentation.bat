
copy "..\..\..\readmeUser.htm" "%dst%\readmeUser.htm" /y
copy "..\..\..\readme.htm" "%dst%\readme.htm" /y
xcopy "..\..\..\Docs\Html\images\*.jpg" "%dst%\Docs\Html\images\" /s /y
xcopy "..\..\..\Docs\Html\*.htm" "%dst%\Docs\Html\" /s /y