@echo off

echo ========== Cleaning up... ==========

del *.log

del navigatorModule\html /S /Q
rmdir navigatorModule /S /Q

del navigatorPhysicsODE\html /S /Q
rmdir navigatorPhysicsODE /S /Q
del navigatorPhysicsPhysX\html /S /Q
rmdir navigatorPhysicsPhysX /S /Q
del navigatorPhysicsTokamak\html /S /Q
rmdir navigatorPhysicsTokamak /S /Q

del navigatorModule\html /S /Q
rmdir navigatorModule /S /Q

del peerModuleSimulator\html /S /Q
rmdir peerModuleSimulator /S /Q

del peerPhysicsPhysX\html /S /Q
rmdir peerPhysicsPhysX /S /Q

del protocols\html /S /Q
rmdir protocols /S /Q

echo ============== Done! ===============
pause
