@echo on

if exist ..\..\..\Common\bin\navigator\Debug\NaviLocal xcopy ..\..\..\Media\NaviLocal\*.html ..\..\..\Common\bin\navigator\Debug\NaviLocal /s /y
if exist ..\..\..\Common\bin\navigator\Debug\NaviLocal xcopy ..\..\..\Media\NaviLocal\*.css ..\..\..\Common\bin\navigator\Debug\NaviLocal /s /y
if exist ..\..\..\Common\bin\navigator\Debug\NaviLocal xcopy ..\..\..\Media\NaviLocal\*.js ..\..\..\Common\bin\navigator\Debug\NaviLocal /s /y
if exist ..\..\..\Common\bin\navigator\Debug\NaviLocal xcopy ..\..\..\Media\NaviLocal\*.png ..\..\..\Common\bin\navigator\Debug\NaviLocal /s /y
if exist ..\..\..\Common\bin\navigator\Debug\NaviLocal xcopy ..\..\..\Media\NaviLocal\*.gif ..\..\..\Common\bin\navigator\Debug\NaviLocal /s /y
if exist ..\..\..\Common\bin\navigator\Debug\NaviLocal xcopy ..\..\..\Media\NaviLocal\*.txt ..\..\..\Common\bin\navigator\Debug\NaviLocal /s /y
if exist ..\..\..\Common\bin\navigator\Debug\NaviLocal xcopy ..\..\..\Media\NaviLocal\color ..\..\..\Common\bin\navigator\Debug\NaviLocal\color /s /y
if exist ..\..\..\Common\bin\navigator\Debug\NaviLocal xcopy ..\..\..\Media\NaviLocal\slider ..\..\..\Common\bin\navigator\Debug\NaviLocal\slider /s /y
if exist ..\..\..\Common\bin\navigator\Debug\NaviLocal xcopy ..\..\..\Media\NaviLocal\tabber ..\..\..\Common\bin\navigator\Debug\NaviLocal\tabber /s /y

if exist ..\..\..\Common\bin\navigator\Release\NaviLocal xcopy ..\..\..\Media\NaviLocal\*.html ..\..\..\Common\bin\navigator\Release\NaviLocal /s /y
if exist ..\..\..\Common\bin\navigator\Release\NaviLocal xcopy ..\..\..\Media\NaviLocal\*.css ..\..\..\Common\bin\navigator\Release\NaviLocal /s /y
if exist ..\..\..\Common\bin\navigator\Release\NaviLocal xcopy ..\..\..\Media\NaviLocal\*.js ..\..\..\Common\bin\navigator\Release\NaviLocal /s /y
if exist ..\..\..\Common\bin\navigator\Release\NaviLocal xcopy ..\..\..\Media\NaviLocal\*.png ..\..\..\Common\bin\navigator\Release\NaviLocal /s /y
if exist ..\..\..\Common\bin\navigator\Release\NaviLocal xcopy ..\..\..\Media\NaviLocal\*.gif ..\..\..\Common\bin\navigator\Release\NaviLocal /s /y
if exist ..\..\..\Common\bin\navigator\Release\NaviLocal xcopy ..\..\..\Media\NaviLocal\*.txt ..\..\..\Common\bin\navigator\Release\NaviLocal /s /y
if exist ..\..\..\Common\bin\navigator\Release\NaviLocal xcopy ..\..\..\Media\NaviLocal\color ..\..\..\Common\bin\navigator\Release\NaviLocal\color /s /y
if exist ..\..\..\Common\bin\navigator\Release\NaviLocal xcopy ..\..\..\Media\NaviLocal\slider ..\..\..\Common\bin\navigator\Release\NaviLocal\slider /s /y
if exist ..\..\..\Common\bin\navigator\Release\NaviLocal xcopy ..\..\..\Media\NaviLocal\tabber ..\..\..\Common\bin\navigator\Release\NaviLocal\tabber /s /y

pause
