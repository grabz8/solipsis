@echo off

echo CommonTools
"%DOXYGEN_PATH%\doxygen.exe" Doxyfile4CommonTools.dox > Doxyfile4CommonTools.log 2>&1
echo Communication
"%DOXYGEN_PATH%\doxygen.exe" Doxyfile4Communication.dox > Doxyfile4Communication.log 2>&1
echo Navigator
"%DOXYGEN_PATH%\doxygen.exe" Doxyfile4Navigator.dox > Doxyfile4Navigator.log 2>&1
echo Peer
"%DOXYGEN_PATH%\doxygen.exe" Doxyfile4Peer.dox > Doxyfile4Peer.log 2>&1
echo Voice
"%DOXYGEN_PATH%\doxygen.exe" Doxyfile4Voice.dox > Doxyfile4Voice.log 2>&1
echo Modelers
"%DOXYGEN_PATH%\doxygen.exe" Doxyfile4Modelers.dox > Doxyfile4Modelers.log 2>&1
echo RakNetServer
"%DOXYGEN_PATH%\doxygen.exe" Doxyfile4RakNetServer.dox > Doxyfile4RakNetServer.log 2>&1

pause
