@echo This will clean up all intermediate/user-specific files for Navigator.
@echo ** You must close Visual C++ before running this file! **
@pause

@echo ========== Cleaning up... ==========

@del Navigator*.ncb
@del Navigator*.user
@rmdir bin\ /S /Q

@echo ============== Done! ===============
@pause