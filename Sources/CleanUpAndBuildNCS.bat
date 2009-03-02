set PATH=C:\Program Files\Microsoft Visual Studio 8\Common7\IDE;C:\Program Files\Subversion\bin;%PATH%

pushd ..
call cleanup.bat
popd

devenv "3rdParties_vc8.sln" /build "Release|Win32" /out buildLog.log
devenv "solipsis_vc8.sln" /build "ReleaseNCS|Win32" /out buildLog.log 

devenv "3rdParties_vc8.sln" /build "Debug|Win32" /out buildLog.log
devenv "solipsis_vc8.sln" /build "DebugNCS|Win32" /out buildLog.log

pause

