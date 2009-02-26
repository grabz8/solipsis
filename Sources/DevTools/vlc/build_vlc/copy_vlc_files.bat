@rem ===== source/destination dirs that may change depending where you build vlc =====
@set SRC_DIR=".\vlc-0.8.6e"
@set SRC_PKG_DIR=".\vlc-0.8.6e\vlc-0.8.6e"
@set DEST_DIR="..\libraries\i686-win32"

@rem =============== instructions ===============
@echo This batch file copies the files required to use the libvlc library 
@echo.
@echo It will copy the required files from a VLC build in the specified 
@echo directories to the right location for Dependencies.
@echo VLC build is the resulting directory of make package-win32-base !!
@echo.
@echo The build will be copied from:
@echo  %SRC_DIR%
@echo  %SRC_PKG_DIR%
@echo.
@echo to the %DEST_DIR% directory under the current directory.
@echo.
@echo IMPORTANT: delete the contents of the destination directory first.
@echo.
@pause

@echo =========== copying header files ==========
xcopy %SRC_DIR%\include\*.*										%DEST_DIR%\include\ /s

@echo ============ copying libraries ============
xcopy %SRC_PKG_DIR%\libvlc.lib									%DEST_DIR%\lib\

@echo ========== copying runtime files ==========
xcopy %SRC_PKG_DIR%\*.*											%DEST_DIR%\runtime\ /s
del %DEST_DIR%\runtime\libvlc.lib
del %DEST_DIR%\runtime\libvlc.exp
del %DEST_DIR%\runtime\libvlc.def

@echo ========== finished ==========
@pause