echo off

if not exist BuildLogs mkdir BuildLogs

REM set date and time
for /F "tokens=1-4 delims=/- " %%a in ('date/T') do set CDATE=%%a%%b%%c
set CDATE=%CDATE:~-4%_%CDATE:~2,2%_%CDATE:~0,2%
for /F "tokens=1-4 delims=:., " %%a in ("%TIME%") do set CTIME=%%a%%b%%c
set CTIME=%CTIME:~0,2%h%CTIME:~2,2%mn%CTIME:~4,2%s


cd Sources

if exist "buildLog.log" (
echo ------------------ deleting buildLog ------------------------
del buildLog.log
)

rem print date
echo ******************************************************* >> buildLog.log
echo ************************* %CDATE% ****************** >> buildLog.log
echo ******************************************************* >> buildLog.log

set updateSVN=none
set SVNPATH=https://scm.gforge.inria.fr/svn/solipsis/trunk
if EXIST "C:\Program Files\Subversion\bin" set updateSVN=ask

rem detect visual version
IF EXIST "C:\Program Files\Microsoft Visual Studio 8" set DEVENVPATH=C:\Program Files\Microsoft Visual Studio 8
IF EXIST "C:\Program Files\Microsoft Visual Studio 9.0" set DEVENVPATH=C:\Program Files\Microsoft Visual Studio 9.0

IF EXIST "%DEVENVPATH%\Common7\IDE\VCExpress.exe" set DEVENV=VCExpress
IF EXIST "%DEVENVPATH%\Common7\IDE\devenv.exe" set DEVENV=devenv

set PATH=%DEVENVPATH%\Common7\IDE;%PATH%

IF "%DEVENV%" == "" goto errorNoDevenp

IF "%updateSVN%"=="ask" (
	echo -------------------------------------------
	set /p updateSVN=update from SVN repository [y/n] ? 
) ELSE (
	echo -------------------------------------------
	echo No Svn command line tool found !
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
echo -------------------------------------------
set /p builddebug=build debug version [y/n] ? 
echo -------------------------------------------

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
echo Parameters : 
echo * DEVENVPATH:%DEVENVPATH%
echo * DEVENV=%DEVENV%
echo * BuildType=%BuildType%
echo * DebugConfig=%DebugConfig%
echo * ReleaseConfig=%ReleaseConfig%
echo * SUFFIX=%SUFFIX%
echo * builddebug=%builddebug%
echo -------------------------------------------

if "%cleanup%"=="y" (
pushd ..
call cleanup.bat
popd
)

if "%updateSVN%"=="y" (
echo ************ get The Svn log ***********
echo ************ Sent to the build Log******

echo ************************************************************************ >> buildLog.log
echo ************************************ Svn log *************************** >> buildLog.log
echo ************************************************************************ >> buildLog.log
svn log -l 20 %SVNPATH% >> buildLog.log
echo ************************************************************************ >> buildLog.log
echo ************************************ END SVN log *********************** >> buildLog.log
echo ************************************************************************ >> buildLog.log

echo ************ end Svn log ***********

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
echo ************************************************** >> buildLog.log
echo ********** building 3rdParties Release *********** >> buildLog.log
echo ************************************************** >> buildLog.log

%DEVENV% "3rdParties%SUFFIX%.sln" /%BuildType% "Release|Win32" /out buildLog.log
IF ERRORLEVEL 1 goto errorBuilding

echo *************************************************
echo *********** building solipsis Release ***********
echo *************************************************
echo ************************************************* >> buildLog.log
echo *********** building solipsis Release *********** >> buildLog.log
echo ************************************************* >> buildLog.log
%DEVENV% "solipsis%SUFFIX%.sln" /%BuildType% "%ReleaseConfig%|Win32" /out buildLog.log 
IF ERRORLEVEL 1 goto errorBuilding


if "%builddebug%"=="y" (
echo *************************************************
echo *********** building 3rdParties Debug ***********
echo *************************************************
echo ************************************************* >> buildLog.log
echo *********** building 3rdParties Debug *********** >> buildLog.log
echo ************************************************* >> buildLog.log
%DEVENV% "3rdParties%SUFFIX%.sln" /%BuildType% "Debug|Win32" /out buildLog.log
IF ERRORLEVEL 1 goto errorBuilding
)

if "%builddebug%"=="y" (
echo *************************************************
echo ************ building solipsis Debug ************
echo *************************************************
echo ************************************************* >> buildLog.log
echo ************ building solipsis Debug ************ >> buildLog.log
echo ************************************************* >> buildLog.log
%DEVENV% "solipsis%SUFFIX%.sln" /%BuildType% "%DebugConfig%|Win32" /out buildLog.log
IF ERRORLEVEL 1 goto errorBuilding
)

goto ok

:errorNoDevenp
echo ************************************************************************************************************
echo error no visual studio detected, install it using the default path please
echo or set the "DEVENVPATH" in env variable to correct path ex : C:\Program Files\Microsoft Visual Studio 9.0
echo ************************************************************************************************************
goto end

:errorSVN
echo #############################################################
echo ##########  error with SVN Update, please correct it ########
echo #############################################################

echo ############################################################# >> buildLog.log
echo ##########  error with SVN Update, please correct it ######## >> buildLog.log
echo ############################################################# >> buildLog.log

goto end

:errorBuilding
echo #############################################################
echo ############### error while building solution ############### 
echo ############### please check the buildlog for more details ##  
echo #############################################################

echo ############################################################# >> buildLog.log
echo ############### error while building solution ###############  >> buildLog.log
echo ############################################################# >> buildLog.log
goto end

:ok
echo ******************************************************************** >> buildLog.log
echo ************* build complete successs !! *************************** >> buildLog.log
echo ******************************************************************** >> buildLog.log
echo ********************************************************************
echo ************* build complete successs !! ***************************
echo ********************************************************************

:end
copy buildLog.log ..\BuildLogs\%CDATE%_buildLog.log

pause