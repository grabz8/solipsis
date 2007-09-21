@rem =============== instructions ===============
@echo This batch file removes the temporary files
@echo generated during a build of the Linden Lab 
@echo Mozilla Library (LLMozLib).
@echo.
@echo It's useful when you change the Mozilla code
@echo and wish to regenerate LLMOZLIB and the runtime files.
@echo.
@echo IMPORTANT: You must close Visual Studio before running this file.
@pause

@echo ========== deleting intermediate files from main directory ==========
@del ..\llmozlib.ncb
@del ..\llmozlib.suo /AH
@del ..\llmozlib.Tags.WW
@rmdir ..\Release\ /S /Q
@rmdir ..\Debug\ /S /Q

@echo ========== deleting intermediate files from test directory ==========

@del ..\tests\screengrab\screengrab.Tags.WW
@rmdir ..\tests\screengrab\Debug\ /S /Q
@rmdir ..\tests\screengrab\Release\ /S /Q

@del ..\tests\testgl\testgl.Tags.WW
@rmdir ..\tests\testgl\Debug\ /S /Q
@rmdir ..\tests\testgl\Release\ /S /Q

@del ..\tests\ubrowser\ubrowser.Tags.WW
@rmdir ..\tests\ubrowser\Debug\ /S /Q
@rmdir ..\tests\ubrowser\Release\ /S /Q

@echo ========== finished ==========
@pause