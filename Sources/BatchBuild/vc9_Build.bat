cd ..
set PATH=C:\Program Files\Microsoft Visual Studio 9.0\Common7\IDE;C:\Program Files\Subversion\bin;%PATH%

devenv "3rdParties_vc9.sln" /build "Release|Win32" /out buildLog.log
devenv "3rdParties_vc9.sln" /build "Debug|Win32" /out buildLog.log

devenv "solipsis_vc9.sln" /build "Release|Win32" /out buildLog.log 
devenv "solipsis_vc9.sln" /build "Debug|Win32" /out buildLog.log

pause

