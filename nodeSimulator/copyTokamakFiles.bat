@echo off

if exist ..\Common\bin\nodeSimulator\Debug copy ..\Dependencies\tokamak\lib\debugdll\tokamakdll.dll ..\Common\bin\nodeSimulator\Debug /y

if exist ..\Common\bin\nodeSimulator\Release copy ..\Dependencies\tokamak\lib\releasedll\tokamakdll.dll ..\Common\bin\nodeSimulator\Release /y

pause
