@rem ===== source/destination dirs that may change depending where you build LLMozLib =====
@set SRC_DIR_LLMOZLIB="..\llmozlib"
@set DEST_DIR_LLMOZLIB="..\..\..\Dependencies"

@rem =============== instructions ===============
@echo This batch file copies the files required to build the 
@echo Navi Library (based on LLMozLib and Mozilla).
@echo.
@echo It will copy the required files from a LLMozLib build in the specified 
@echo directories to the right location for Navi.
@echo.
@echo The build will be copied from:
@echo  %SRC_DIR_LLMOZLIB%
@echo.
@echo to the %DEST_DIR_LLMOZLIB% directory.
@echo.
@echo IMPORTANT: delete the contents of the destination directory first.
@echo.
@pause

@echo ========== creating directories ==========
@mkdir %DEST_DIR_LLMOZLIB%\llmozlib\include
@mkdir %DEST_DIR_LLMOZLIB%\llmozlib\win32_vc7\lib
@mkdir %DEST_DIR_LLMOZLIB%\llmozlib\win32_vc7\runtime\Debug
@mkdir %DEST_DIR_LLMOZLIB%\llmozlib\win32_vc7\runtime\Release

@echo ========== copying header files ==========
@xcopy %SRC_DIR_LLMOZLIB%\llmozlib.h %DEST_DIR_LLMOZLIB%\llmozlib\include\ /y

@echo ========== copying debug and release libraries ==========
@copy %SRC_DIR_LLMOZLIB%\Debug_vc7\llmozlib_vc7.lib %DEST_DIR_LLMOZLIB%\llmozlib\win32_vc7\lib\llmozlib_d.lib /y
@copy %SRC_DIR_LLMOZLIB%\Release_vc7\llmozlib_vc7.lib %DEST_DIR_LLMOZLIB%\llmozlib\win32_vc7\lib\llmozlib.lib /y

@echo ========== copying debug and release runtime files ==========
@xcopy %SRC_DIR_LLMOZLIB%\libraries\i686-win32\runtime_debug\*.* %DEST_DIR_LLMOZLIB%\llmozlib\win32_vc7\runtime\Debug\ /s /y
@xcopy %SRC_DIR_LLMOZLIB%\libraries\i686-win32\runtime_release\*.* %DEST_DIR_LLMOZLIB%\llmozlib\win32_vc7\runtime\Release\ /s /y

@echo ========== finished ==========
@pause