@echo off

echo ========== Cleaning up... ==========

del navigatorModule\html /S /Q
rmdir navigatorModule /S /Q

del navigatorPhysicsODE\html /S /Q
rmdir navigatorPhysicsODE /S /Q
del navigatorPhysicsPhysX\html /S /Q
rmdir navigatorPhysicsPhysX /S /Q
del navigatorPhysicsTokamak\html /S /Q
rmdir navigatorPhysicsTokamak /S /Q

echo ============== Done! ===============
pause
