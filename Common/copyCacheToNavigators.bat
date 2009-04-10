mkdir bin\navigator\Debug\Media
mkdir bin\navigator\Debug\Media\cache
mkdir bin\navigator\Release\Media
mkdir bin\navigator\Release\Media\cache
call ..\Media\cache\clean_cache.bat
copy ..\Media\cache\*.* bin\navigator\Debug\Media\cache
copy ..\Media\cache\*.* bin\navigator\Release\Media\cache
