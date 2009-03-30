echo off
copy /y boot.lua ..\Common\bin\navigator\Release\lua\boot.lua
copy /y boot.lua ..\Common\bin\navigator\Debug\lua\boot.lua

copy /y uilogin.html ..\Common\bin\navigator\Release\NaviLocal\uilogin.html
copy /y uilogin.html ..\Common\bin\navigator\Debug\NaviLocal\uilogin.html
pause