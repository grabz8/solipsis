@echo off

if exist ..\Common\bin\navigator\Debug\NaviLocal xcopy ..\Media\NaviLocal\*.html ..\Common\bin\navigator\Debug\NaviLocal /s /y
if exist ..\Common\bin\navigator\Debug\NaviLocal xcopy ..\Media\NaviLocal\*.css ..\Common\bin\navigator\Debug\NaviLocal /s /y
if exist ..\Common\bin\navigator\Debug\NaviLocal xcopy ..\Media\NaviLocal\*.js ..\Common\bin\navigator\Debug\NaviLocal /s /y
if exist ..\Common\bin\navigator\Debug\NaviLocal xcopy ..\Media\NaviLocal\*.png ..\Common\bin\navigator\Debug\NaviLocal /s /y
if exist ..\Common\bin\navigator\Debug\NaviLocal xcopy ..\Media\NaviLocal\*.gif ..\Common\bin\navigator\Debug\NaviLocal /s /y
if exist ..\Common\bin\navigator\Debug\NaviLocal xcopy ..\Media\NaviLocal\*.txt ..\Common\bin\navigator\Debug\NaviLocal /s /y

if exist ..\Common\bin\navigator\Release\NaviLocal xcopy ..\Media\NaviLocal\*.html ..\Common\bin\navigator\Release\NaviLocal /s /y
if exist ..\Common\bin\navigator\Release\NaviLocal xcopy ..\Media\NaviLocal\*.css ..\Common\bin\navigator\Release\NaviLocal /s /y
if exist ..\Common\bin\navigator\Release\NaviLocal xcopy ..\Media\NaviLocal\*.js ..\Common\bin\navigator\Release\NaviLocal /s /y
if exist ..\Common\bin\navigator\Release\NaviLocal xcopy ..\Media\NaviLocal\*.png ..\Common\bin\navigator\Release\NaviLocal /s /y
if exist ..\Common\bin\navigator\Release\NaviLocal xcopy ..\Media\NaviLocal\*.gif ..\Common\bin\navigator\Release\NaviLocal /s /y
if exist ..\Common\bin\navigator\Release\NaviLocal xcopy ..\Media\NaviLocal\*.txt ..\Common\bin\navigator\Release\NaviLocal /s /y

pause
