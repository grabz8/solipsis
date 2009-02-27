@echo off

echo ========== Cleaning up... ==========

del *.log

del CommonTools\html /S /Q
rmdir CommonTools /S /Q

del Communication\html /S /Q
rmdir Communication /S /Q

del Modelers\html /S /Q
rmdir Modelers /S /Q

del NavigatorModules\html /S /Q
rmdir NavigatorModules /S /Q

del navigatorModule\html /S /Q
rmdir navigatorModule /S /Q

del PeerModules\html /S /Q
rmdir PeerModules /S /Q

del RackNetServer\html /S /Q
rmdir RackNetServer /S /Q

del Voice\html /S /Q
rmdir Voice /S /Q

echo ============== Done! ===============

