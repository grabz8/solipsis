Check list to create the setup :

1 - Execute the cleanup.bat on the root repository

2 - Compile Solipsis in ReleaseNCS mode

3 - Don't forget to execute the "copyNCSPhysXFiles.bat" under the "navigatorModuleOnlyNav" repository

4 - Edit the "Media\lua\boot.lua" to set the good World Server IP

5 - Edit the "Media\NaviLocal\NaviLocal\uimdlrprop.html" to set for example the VNC server IP

6 - Edit the "Media\NaviLocal\NaviLocal\uidebug.html" to set the Voice server IP

7 - In "Media", remove the server cache, and launch the "clean_cache.bat" in the cache repository

8 - Launch the setup\nsis\makensisNCS.bat

9 - Copy the result in a tmp repository, and add the dependencies (VCRedist, PhysX, Pyhton, DirectX 9.0c) and the guides (Build & Install)

10 - Burn this tmp repository, or zip it to upload it