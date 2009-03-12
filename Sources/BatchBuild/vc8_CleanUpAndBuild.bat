cd ..
set PATH=C:\Program Files\Microsoft Visual Studio 8\Common7\IDE;C:\Program Files\Subversion\bin;%PATH%

del buildLog.log

set DEVENV=devenv
if not exist "C:\Program Files\Microsoft Visual Studio 8\Common7\IDE\devenv.exe" set DEVENV=VCExpress

pushd ..
call cleanup.bat
popd

%DEVENV% "3rdParties_vc8.sln" /build "Release|Win32" /out buildLog.log
%DEVENV% "3rdParties_vc8.sln" /build "Debug|Win32" /out buildLog.log

%DEVENV% "solipsis_vc8.sln" /build "Release|Win32" /out buildLog.log 
%DEVENV% "solipsis_vc8.sln" /build "Debug|Win32" /out buildLog.log

pause

