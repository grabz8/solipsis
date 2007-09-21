@echo This will clean up all intermediate/user-specific files for LLMozLib. 
@echo ** You must close Visual C++ before running this file! **
@pause

@echo ========== Cleaning up... ==========

@del llmozlib_*.ncb
@del llmozlib_*.user
@rmdir Debug_vc7\ /S /Q
@rmdir Release_vc7\ /S /Q
@rmdir Debug_vc8\ /S /Q
@rmdir Release_vc8\ /S /Q

@echo ============== Done! ===============
@pause