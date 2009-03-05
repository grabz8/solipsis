@echo off
goto end
if exist ..\..\..\Common\bin\navigator\Debug copy "%PHYSX_DIR%\Bin\win32\PhysXLoader.dll" ..\..\..\Common\bin\navigator\Debug /y
if exist ..\..\..\Common\bin\navigator\Debug copy "%PHYSX_DIR%\Bin\win32\NxCooking.dll" ..\..\..\Common\bin\navigator\Debug /y
if exist ..\..\..\Common\bin\navigator\Debug copy "%PHYSX_DIR%\Bin\win32\NxExtensions.dll" ..\..\..\Common\bin\navigator\Debug /y
if exist ..\..\..\Common\bin\navigator\Debug copy "%PHYSX_DIR%\Bin\win32\NxCharacter.dll" ..\..\..\Common\bin\navigator\Debug /y

if exist ..\..\..\Common\bin\navigator\Release copy "%PHYSX_DIR%\Bin\win32\PhysXLoader.dll" ..\..\..\Common\bin\navigator\Release /y
if exist ..\..\..\Common\bin\navigator\Release copy "%PHYSX_DIR%\Bin\win32\NxCooking.dll" ..\..\..\Common\bin\navigator\Release /y
if exist ..\..\..\Common\bin\navigator\Release copy "%PHYSX_DIR%\Bin\win32\NxExtensions.dll" ..\..\..\Common\bin\navigator\Release /y
if exist ..\..\..\Common\bin\navigator\Release copy "%PHYSX_DIR%\Bin\win32\NxCharacter.dll" ..\..\..\Common\bin\navigator\Release /y
:end
pause
