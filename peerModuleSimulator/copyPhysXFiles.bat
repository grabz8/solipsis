@echo off

if exist ..\Common\bin\peerSimulator\Debug copy "%PHYSX_DIR%\Bin\win32\PhysXLoader.dll" ..\Common\bin\peerSimulator\Debug /y
if exist ..\Common\bin\peerSimulator\Debug copy "%PHYSX_DIR%\Bin\win32\NxCooking.dll" ..\Common\bin\peerSimulator\Debug /y
if exist ..\Common\bin\peerSimulator\Debug copy "%PHYSX_DIR%\Bin\win32\NxExtensions.dll" ..\Common\bin\peerSimulator\Debug /y
if exist ..\Common\bin\peerSimulator\Debug copy "%PHYSX_DIR%\Bin\win32\NxCharacter.dll" ..\Common\bin\peerSimulator\Debug /y

if exist ..\Common\bin\peerSimulator\Release copy "%PHYSX_DIR%\Bin\win32\PhysXLoader.dll" ..\Common\bin\peerSimulator\Release /y
if exist ..\Common\bin\peerSimulator\Release copy "%PHYSX_DIR%\Bin\win32\NxCooking.dll" ..\Common\bin\peerSimulator\Release /y
if exist ..\Common\bin\peerSimulator\Release copy "%PHYSX_DIR%\Bin\win32\NxExtensions.dll" ..\Common\bin\peerSimulator\Release /y
if exist ..\Common\bin\peerSimulator\Release copy "%PHYSX_DIR%\Bin\win32\NxCharacter.dll" ..\Common\bin\peerSimulator\Release /y

pause
