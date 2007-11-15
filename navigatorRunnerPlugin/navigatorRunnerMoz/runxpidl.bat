@echo off

echo Generate the C++ header and type library (XPT) file

rem Add moztools VC71 binaries into PATH to get libIDL and glib DLLs
PATH=%PATH%;..\..\DevTools\llmozlib\build_mozilla\moztools\vc71\bin

rem Generate IDL + XPT
..\..\DevTools\llmozlib\libraries\i686-win32\include\mozilla\sdk\bin\xpidl -m header -I..\..\DevTools\llmozlib\libraries\i686-win32\include\mozilla\sdk\idl nsINavigatorRunnerMoz.idl
..\..\DevTools\llmozlib\libraries\i686-win32\include\mozilla\sdk\bin\xpidl -m typelib -I..\..\DevTools\llmozlib\libraries\i686-win32\include\mozilla\sdk\idl nsINavigatorRunnerMoz.idl
..\..\DevTools\llmozlib\libraries\i686-win32\include\mozilla\sdk\bin\xpt_link SolipsisNavigatorRunner.xpt nsINavigatorRunnerMoz.xpt

