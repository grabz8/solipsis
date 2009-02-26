@echo off

if exist ..\..\..\Common\bin\peerRakNet\Debug copy "%PHYSX_DIR%\Bin\win32\PhysXLoader.dll" ..\..\..\Common\bin\peerRakNet\Debug /y
if exist ..\..\..\Common\bin\peerRakNet\Debug copy "%PHYSX_DIR%\Bin\win32\NxCooking.dll" ..\..\..\Common\bin\peerRakNet\Debug /y
if exist ..\..\..\Common\bin\peerRakNet\Debug copy "%PHYSX_DIR%\Bin\win32\NxExtensions.dll" ..\..\..\Common\bin\peerRakNet\Debug /y
if exist ..\..\..\Common\bin\peerRakNet\Debug copy "%PHYSX_DIR%\Bin\win32\NxCharacter.dll" ..\..\..\Common\bin\peerRakNet\Debug /y

if exist ..\..\..\Common\bin\peerRakNet\Release copy "%PHYSX_DIR%\Bin\win32\PhysXLoader.dll" ..\..\..\Common\bin\peerRakNet\Release /y
if exist ..\..\..\Common\bin\peerRakNet\Release copy "%PHYSX_DIR%\Bin\win32\NxCooking.dll" ..\..\..\Common\bin\peerRakNet\Release /y
if exist ..\..\..\Common\bin\peerRakNet\Release copy "%PHYSX_DIR%\Bin\win32\NxExtensions.dll" ..\..\..\Common\bin\peerRakNet\Release /y
if exist ..\..\..\Common\bin\peerRakNet\Release copy "%PHYSX_DIR%\Bin\win32\NxCharacter.dll" ..\..\..\Common\bin\peerRakNet\Release /y

pause
