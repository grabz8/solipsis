set MOZ_TOOLS=c:\Work\llmozlib\build_mozilla\moztools
set GLIB_PREFIX=c:\Work\llmozlib\build_mozilla\moztools\vc71
set LIBIDL_PREFIX=c:\Work\llmozlib\build_mozilla\moztools\vc71
set MOZ_CO_PROJECT=browser
PATH=%PATH%;%GLIB_PREFIX%\bin;%LIBIDL_PREFIX%/bin;%MOZ_TOOLS%\bin;c:\cygwin\bin
call "C:\Program Files\Microsoft Visual Studio .NET 2003\Vc7\bin\vcvars32.bat"
set CVSROOT=:pserver:anonymous@cvs-mirror.mozilla.org:/cvsroot