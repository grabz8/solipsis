@echo off

"%DOXYGEN_PATH%\doxygen.exe" Doxyfile4navigatorModuleOnlyNav > Doxyfile4navigatorModuleOnlyNav.log 2>&1
"%DOXYGEN_PATH%\doxygen.exe" Doxyfile4peerModuleSimulator > Doxyfile4peerModuleSimulator.log 2>&1
"%DOXYGEN_PATH%\doxygen.exe" Doxyfile4peerPlugin_PhysX > Doxyfile4peerPlugin_PhysX.log 2>&1
"%DOXYGEN_PATH%\doxygen.exe" Doxyfile4protocols > Doxyfile4protocols.log 2>&1

pause
