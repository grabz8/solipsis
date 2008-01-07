@echo off

%DOXYGEN_PATH%\doxygen.exe Doxyfile4navigatorModule > Doxyfile4navigatorModule.log 2>&1
%DOXYGEN_PATH%\doxygen.exe Doxyfile4navigatorPlugin_ODE > Doxyfile4navigatorPlugin_ODE.log 2>&1
%DOXYGEN_PATH%\doxygen.exe Doxyfile4navigatorPlugin_PhysX > Doxyfile4navigatorPlugin_PhysX.log 2>&1
%DOXYGEN_PATH%\doxygen.exe Doxyfile4navigatorPlugin_Tokamak > Doxyfile4navigatorPlugin_Tokamak.log 2>&1

pause
