@echo off

if exist bin\Debug\NaviLocal xcopy ..\Media\NaviLocal\*.html bin\Debug\NaviLocal /s /y
if exist bin\Debug\NaviLocal xcopy ..\Media\NaviLocal\*.css bin\Debug\NaviLocal /s /y
if exist bin\Debug\NaviLocal xcopy ..\Media\NaviLocal\*.js bin\Debug\NaviLocal /s /y
if exist bin\Debug\NaviLocal xcopy ..\Media\NaviLocal\*.png bin\Debug\NaviLocal /s /y
if exist bin\Debug\NaviLocal xcopy ..\Media\NaviLocal\*.txt bin\Debug\NaviLocal /s /y

if exist bin\Release\NaviLocal xcopy ..\Media\NaviLocal\*.html bin\Release\NaviLocal /s /y
if exist bin\Release\NaviLocal xcopy ..\Media\NaviLocal\*.css bin\Release\NaviLocal /s /y
if exist bin\Release\NaviLocal xcopy ..\Media\NaviLocal\*.js bin\Release\NaviLocal /s /y
if exist bin\Release\NaviLocal xcopy ..\Media\NaviLocal\*.png bin\Release\NaviLocal /s /y
if exist bin\Release\NaviLocal xcopy ..\Media\NaviLocal\*.txt bin\Release\NaviLocal /s /y

pause
