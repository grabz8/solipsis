@echo This will clean up all intermediate/user-specific files for NaviLibrary. 
@echo ** You must close Visual C++ before running this file! **
@pause

@echo ========== Cleaning up... ==========

@del Navi*.ncb

@del Navi\*.ncb
@del Navi\*.user
@rmdir Navi\Objects_vc8\ /S /Q
@rmdir Navi\lib_vc8\ /S /Q

@del NaviDemo\*.ncb
@del NaviDemo\*.user
@rmdir NaviDemo\Objects_vc8\ /S /Q
@rmdir NaviDemo\Bin\release_vc8\ /S /Q
@rmdir NaviDemo\Bin\debug_vc8\ /S /Q

@echo ============== Done! ===============
@pause