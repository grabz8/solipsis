@echo off

echo This will clean up all dependencies, binaries, objects and libraries.
echo ** You must close Visual C++ before running this file! **
pause

echo ========== Cleaning up... ==========

rmdir Common\bin\navigator\debug /S /Q
rmdir Common\bin\navigator\release /S /Q

rmdir Common\bin\node\debug /S /Q
rmdir Common\bin\node\release /S /Q

rmdir Common\bin\nodeSimulator\debug /S /Q
rmdir Common\bin\nodeSimulator\release /S /Q

rmdir Dependencies\llmozlib /S /Q
rmdir Dependencies\lua /S /Q
rmdir Dependencies\navi /S /Q
rmdir Dependencies\navigatorModule /S /Q
rmdir Dependencies\navigatorPlugin /S /Q
rmdir Dependencies\ode /S /Q
rmdir Dependencies\OgreOde /S /Q
rmdir Dependencies\pthreads /S /Q
rmdir Dependencies\tinyxml /S /Q
rmdir Dependencies\tokamak /S /Q
rmdir Dependencies\xmlrpc++ /S /Q
rmdir Dependencies\brep /S /Q
rmdir Dependencies\csg /S /Q
rmdir Dependencies\MdlrTools /S /Q
rmdir Dependencies\expat /S /Q
rmdir Dependencies\ulxmlrpcpp /S /Q
rmdir Dependencies\zlib123 /S /Q
rmdir Dependencies\protocols /S /Q
rmdir Dependencies\peerModule /S /Q


rmdir DevTools\llmozlib\Debug_vc7 /S /Q
rmdir DevTools\llmozlib\Debug_vc8 /S /Q
rmdir DevTools\llmozlib\Debug_vc9 /S /Q
rmdir DevTools\llmozlib\Release_vc7 /S /Q
rmdir DevTools\llmozlib\Release_vc8 /S /Q
rmdir DevTools\llmozlib\Release_vc9 /S /Q
rmdir DevTools\lua\lib /S /Q
rmdir DevTools\navi\Navi\Objects_vc7 /S /Q
rmdir DevTools\navi\Navi\Objects_vc8 /S /Q
rmdir DevTools\navi\Navi\Objects_vc9 /S /Q
rmdir DevTools\navi\Navi\lib_vc7 /S /Q
rmdir DevTools\navi\Navi\lib_vc8 /S /Q
rmdir DevTools\navi\Navi\lib_vc9 /S /Q
rmdir DevTools\navi\NaviDemo\Objects_vc7 /S /Q
rmdir DevTools\navi\NaviDemo\Objects_vc8 /S /Q
rmdir DevTools\navi\NaviDemo\Objects_vc9 /S /Q
rmdir DevTools\navi\NaviDemo\Bin\debug_vc7 /S /Q
rmdir DevTools\navi\NaviDemo\Bin\debug_vc8 /S /Q
rmdir DevTools\navi\NaviDemo\Bin\debug_vc9 /S /Q
rmdir DevTools\navi\NaviDemo\Bin\release_vc7 /S /Q
rmdir DevTools\navi\NaviDemo\Bin\release_vc8 /S /Q
rmdir DevTools\navi\NaviDemo\Bin\release_vc9 /S /Q
rmdir DevTools\OgreOde\lib\debug /S /Q
rmdir DevTools\OgreOde\lib\release /S /Q
rmdir DevTools\OgreOde\obj\Debug /S /Q
rmdir DevTools\OgreOde\obj\Release /S /Q
rmdir DevTools\OgreOde\ode\lib\debuglib /S /Q
rmdir DevTools\OgreOde\ode\lib\releaselib /S /Q
rmdir DevTools\OgreOde\ode\build\vs2003\obj /S /Q
rmdir DevTools\OgreOde\ode\build\vs2005\obj /S /Q
rmdir DevTools\OgreOde\ode\build\vs2008\obj /S /Q
del DevTools\pthreads\pthreadVC* /S /Q
rmdir DevTools\tinyxml\Debug /S /Q
rmdir DevTools\tinyxml\Release /S /Q
rmdir DevTools\tokamak\lib\debugdll /S /Q
rmdir DevTools\tokamak\lib\releasedll /S /Q
rmdir DevTools\tokamak\tokamaksrc\tokamakdll /S /Q
del DevTools\xmlrpc++\lib\*.lib /S /Q
rmdir DevTools\xmlrpc++\Debug /S /Q
rmdir DevTools\xmlrpc++\Release /S /Q
rmdir DevTools\brep\Debug /S /Q
rmdir DevTools\brep\Release /S /Q
rmdir DevTools\brep\lib /S /Q
rmdir DevTools\CSG\Debug /S /Q
rmdir DevTools\CSG\Release /S /Q
rmdir DevTools\CSG\lib /S /Q
rmdir DevTools\expat\win32\bin /S /Q
rmdir DevTools\expat\win32\tmp /S /Q
rmdir DevTools\ulxmlrpcpp\msvc71\ulxmlrpcpp\Debug /S /Q
rmdir DevTools\ulxmlrpcpp\msvc71\ulxmlrpcpp\Release /S /Q
rmdir DevTools\ulxmlrpcpp\msvc8\ulxmlrpcpp\Debug /S /Q
rmdir DevTools\ulxmlrpcpp\msvc9\ulxmlrpcpp\Debug /S /Q
rmdir DevTools\ulxmlrpcpp\msvc8\ulxmlrpcpp\Release /S /Q
rmdir DevTools\ulxmlrpcpp\msvc9\ulxmlrpcpp\Release /S /Q
rmdir DevTools\zlib123\projects\visualc6\Win32_LIB_Debug /S /Q
rmdir DevTools\zlib123\projects\visualc6\Win32_LIB_Release /S /Q

rmdir Docs\doxygen\html /S /Q

rmdir MdlrTools\obj /S /Q

rmdir navigator\Debug /S /Q
rmdir navigator\Release /S /Q

rmdir navigatorModule\Debug /S /Q
rmdir navigatorModule\Release /S /Q

rmdir navigatorPhysics\navigatorODE\Debug /S /Q
rmdir navigatorPhysics\navigatorODE\Release /S /Q
rmdir navigatorPhysics\navigatorPhysX\Debug /S /Q
rmdir navigatorPhysics\navigatorPhysX\Release /S /Q
rmdir navigatorPhysics\navigatorTokamak\Debug /S /Q
rmdir navigatorPhysics\navigatorTokamak\Release /S /Q

rmdir navigatorPlugin\navigatorActiveX\Debug_vc7 /S /Q
rmdir navigatorPlugin\navigatorActiveX\Debug_vc8 /S /Q
rmdir navigatorPlugin\navigatorActiveX\Debug_vc9 /S /Q
rmdir navigatorPlugin\navigatorActiveX\Release_vc7 /S /Q
rmdir navigatorPlugin\navigatorActiveX\Release_vc8 /S /Q
rmdir navigatorPlugin\navigatorActiveX\Release_vc9 /S /Q
rmdir navigatorPlugin\navigatorPlugin\Debug_vc7 /S /Q
rmdir navigatorPlugin\navigatorPlugin\Debug_vc8 /S /Q
rmdir navigatorPlugin\navigatorPlugin\Debug_vc9 /S /Q
rmdir navigatorPlugin\navigatorPlugin\Release_vc7 /S /Q
rmdir navigatorPlugin\navigatorPlugin\Release_vc8 /S /Q
rmdir navigatorPlugin\navigatorPlugin\Release_vc9 /S /Q

rmdir navigatorRunnerPlugin\navigatorRunnerAx\Debug_vc7 /S /Q
rmdir navigatorRunnerPlugin\navigatorRunnerAx\Debug_vc8 /S /Q
rmdir navigatorRunnerPlugin\navigatorRunnerAx\Debug_vc9 /S /Q
rmdir navigatorRunnerPlugin\navigatorRunnerAx\Release_vc7 /S /Q
rmdir navigatorRunnerPlugin\navigatorRunnerAx\Release_vc8 /S /Q
rmdir navigatorRunnerPlugin\navigatorRunnerAx\Release_vc9 /S /Q
rmdir navigatorRunnerPlugin\navigatorRunnerMoz\Debug_vc7 /S /Q
rmdir navigatorRunnerPlugin\navigatorRunnerMoz\Debug_vc8 /S /Q
rmdir navigatorRunnerPlugin\navigatorRunnerMoz\Debug_vc9 /S /Q
rmdir navigatorRunnerPlugin\navigatorRunnerMoz\Release_vc7 /S /Q
rmdir navigatorRunnerPlugin\navigatorRunnerMoz\Release_vc8 /S /Q
rmdir navigatorRunnerPlugin\navigatorRunnerMoz\Release_vc9 /S /Q

rmdir node\bin /S /Q

rmdir protocols\XmlDatas\Debug /S /Q
rmdir protocols\XmlDatas\Release /S /Q
rmdir protocols\P2NClient\xmlrpc++\Debug /S /Q
rmdir protocols\P2NClient\xmlrpc++\Release /S /Q
rmdir protocols\P2NServer\xmlrpc++\Debug /S /Q
rmdir protocols\P2NServer\xmlrpc++\Release /S /Q
rmdir protocols\P2NServer\ulxmlrpcpp\Debug /S /Q
rmdir protocols\P2NServer\ulxmlrpcpp\Release /S /Q

rmdir nodeSimulator\Debug /S /Q
rmdir nodeSimulator\Release /S /Q

rmdir peerPhysics\peerPhysX\Debug /S /Q
rmdir peerPhysics\peerPhysX\Release /S /Q

rmdir navigatorModuleOnlyNav\Debug /S /Q
rmdir navigatorModuleOnlyNav\Release /S /Q

echo ============== Done! ===============
pause
