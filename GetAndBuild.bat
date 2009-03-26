echo off
cd Sources

if exist "buildLog.log" (
echo deleting buildLog
del buildLog.log
)

set SVNPATH=https://scm.gforge.inria.fr/svn/solipsis/trunk
if EXIST "C:\Program Files\Subversion\bin" set updateSVN=ask

rem detect visual version
IF EXIST "C:\Program Files\Microsoft Visual Studio 9.0" set DEVENVPATH=C:\Program Files\Microsoft Visual Studio 9.0
IF EXIST "C:\Program Files\Microsoft Visual Studio 8" set DEVENVPATH=C:\Program Files\Microsoft Visual Studio 8

IF EXIST "%DEVENVPATH%\Common7\IDE\VCExpress.exe" set DEVENV=VCExpress
IF EXIST "%DEVENVPATH%\Common7\IDE\devenv.exe" set DEVENV=devenv

set PATH=%DEVENVPATH%\Common7\IDE;%PATH%

IF "%DEVENV%" == "" goto errorNoDevenp

IF "%updateSVN%"=="ask" (
	echo -------------------------------------------
	set /p updateSVN=update from SVN repository [y/n] ? 
)
REM Ask questions
echo -------------------------------------------
set /p cleanup=cleanUp [y/n] ? 
echo -------------------------------------------
echo y: build NCS (Null Client Server : Peer and navi are melted)
echo n: build separate Peer and Navi
set /p type=build NCS [y/n] ? 
echo -------------------------------------------
set /p rebuildAll=rebuild all [y/n] ? 

if "%type%"=="y" (
	set ReleaseConfig=ReleaseNCS
	set DebugConfig=DebugNCS
) ELSE (
	set ReleaseConfig=Release
	set DebugConfig=Debug
)

if "%rebuildAll%"=="y" (
	set BuildType=rebuild
) ELSE (
	set BuildType=build
)

if not "%VS90COMNTOOLS%"=="" set SUFFIX=_vc9
if not "%VS80COMNTOOLS%"=="" set SUFFIX=_vc8

echo -------------------------------------------
echo DEVENVPATH:%DEVENVPATH%
echo DEVENV=%DEVENV%
echo BuildType=%BuildType%
echo DebugConfig=%DebugConfig%
echo ReleaseConfig=%ReleaseConfig%
echo SUFFIX=%SUFFIX%
echo -------------------------------------------

if "%cleanup%"=="y" (
pushd ..
call cleanup.bat
popd
)

if "%updateSVN%"=="y" (
echo ************ update from svn ***********
pushd ..
svn checkout %SVNPATH% . 
popd
echo ************ end update from svn ***********
IF ERRORLEVEL 1 goto errorSVN
)

set BUILDERROR=noerror

echo *************************************************
echo ********** building 3rdParties Release **********
echo *************************************************

%DEVENV% "3rdParties_vc8.sln" /%BuildType% "Release|Win32" /out buildLog.log
IF ERRORLEVEL 1 goto errorBuilding

echo *************************************************
echo *********** building 3rdParties Debug ***********
echo *************************************************
%DEVENV% "3rdParties_vc8.sln" /%BuildType% "Debug|Win32" /out buildLog.log
IF ERRORLEVEL 1 goto errorBuilding

echo *************************************************
echo *********** building solipsis Release ***********
echo *************************************************
%DEVENV% "solipsis_vc8.sln" /%BuildType% "%ReleaseConfig%|Win32" /out buildLog.log 
IF ERRORLEVEL 1 goto errorBuilding

echo *************************************************
echo ************ building solipsis Debug ************
echo *************************************************
%DEVENV% "solipsis_vc8.sln" /%BuildType% "%DebugConfig%|Win32" /out buildLog.log
IF ERRORLEVEL 1 goto errorBuilding

goto ok

:errorNoDevenp
echo ************************************************************************************************************
echo error no visual studio detected, install it using the default path please
echo or set the "DEVENVPATH" in env variable to correct path ex : C:\Program Files\Microsoft Visual Studio 9.0
echo ************************************************************************************************************
goto end

:errorSVN
echo ************************************************************************************************************
echo error with SVN Update, please correct it 
echo ************************************************************************************************************
goto end

:errorBuilding
echo ************************************************************************************************************
echo error while building solution
echo please check the buildlog for more details
echo ************************************************************************************************************
goto end

:ok
echo ************************************************************************************************************
echo ************************************************************************************************************
echo ************************************************************************************************************
echo build ok !!!!!!!!!!!!!!!!!!!!!!!!!
echo ************************************************************************************************************
echo ************************************************************************************************************
echo ************************************************************************************************************

:end
pause