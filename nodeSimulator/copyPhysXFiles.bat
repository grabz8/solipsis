@echo off

if exist ..\Common\bin\nodeSimulator\Debug copy "%PHYSX_DIR%\Bin\win32\PhysXLoader.dll" ..\Common\bin\nodeSimulator\Debug /y
if exist ..\Common\bin\nodeSimulator\Debug copy "%PHYSX_DIR%\Bin\win32\NxCooking.dll" ..\Common\bin\nodeSimulator\Debug /y
if exist ..\Common\bin\nodeSimulator\Debug copy "%PHYSX_DIR%\Bin\win32\NxExtensions.dll" ..\Common\bin\nodeSimulator\Debug /y
if exist ..\Common\bin\nodeSimulator\Debug copy "%PHYSX_DIR%\Bin\win32\NxCharacter.dll" ..\Common\bin\nodeSimulator\Debug /y

if exist ..\Common\bin\nodeSimulator\Release copy "%PHYSX_DIR%\Bin\win32\PhysXLoader.dll" ..\Common\bin\nodeSimulator\Release /y
if exist ..\Common\bin\nodeSimulator\Release copy "%PHYSX_DIR%\Bin\win32\NxCooking.dll" ..\Common\bin\nodeSimulator\Release /y
if exist ..\Common\bin\nodeSimulator\Release copy "%PHYSX_DIR%\Bin\win32\NxExtensions.dll" ..\Common\bin\nodeSimulator\Release /y
if exist ..\Common\bin\nodeSimulator\Release copy "%PHYSX_DIR%\Bin\win32\NxCharacter.dll" ..\Common\bin\nodeSimulator\Release /y

pause
