@echo This will clean up all navi and Mozilla LLMozLib dependencies for NaviLibrary.
@echo ** You must close Visual C++ before running this file! **
@pause

@echo ========== Cleaning up... ==========

@rmdir ..\..\..\Dependencies\llmozlib /S /Q
@rmdir ..\..\..\Dependencies\navi /S /Q

@echo ============== Done! ===============
@pause