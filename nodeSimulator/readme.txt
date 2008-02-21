--------------------------------------------------------------------------------
CONFIG
--------------------------------------------------------------------------------

Ogre built 1.4.3 from SVN repository
DXSDK (August 2007)
Visual C++ 2005 release runtime


--------------------------------------------------------------------------------
PREREQUISITES
--------------------------------------------------------------------------------

DXSDK (August 2007) => C:\WINDOWS\system32\d3dx9*_35.dll
otherwise install at least August 2007 DX Redist package from http://www.microsoft.com/downloads

Ageia PhysX drivers
If you are using PhysX plugin then check you have a recent release of PhysX drivers
supporting Engine release 2.7.2 from http://www.ageia.com/drivers/drivers.html


--------------------------------------------------------------------------------
HELP
--------------------------------------------------------------------------------

nodeSimulator simply simulate the node behaviour as server behind the Navigator,
XMLRPC (XML/HTTP) is used (either XMLRPC++ or ULXMLRPCPP), server created on : localhost:8550 (IP@:8550 when using ULXMLRPCPP)
Server accepts login/logout, give 1 different nodeId to each cnx and handle those events
specifically for nodeId="00000001" :
- 10s later : send the User avatar entity
- 10s later : send the Deltastation1 site entity
- 10s later : send the z6-salamandra avatar entity

According to use of XMLRPC++ or ULXMLRPCPP, then HandleEvt will respond
NOEVT or will block until 1 EVT must be processed
LOGSNDRCV define is used to log some traces

Peer.exe is taking 3 optional args:
-h <server host>
-p <port number>
-v <verbosity 0..2>


--------------------------------------------------------------------------------
HOW TO BUILD
--------------------------------------------------------------------------------

vcX stands for Visual config (vc7 or vc8)

Install Visual Studio 2005 + SP1 package

Download Ogre source package (branch 1.4)
Download Ogre Dependencies for VS8.1 (http://www.ogre3d.org/index.php?option=com_content&task=view&id=412&Itemid=132)
and unzip them into ogrenew subdirectory

WARNING: If you are using 1 prebuilt release of Ogre SDK, take care this release was built with same
release of Visual Studio you will use, examples:
 OGRE 1.4.x SDK for Visual C++ .Net 2003 (7.1) with Visual Studio .Net 2003
 OGRE 1.4.x SDK for Visual C++ .Net 2005 (8.0) SP1 with Visual Studio Express 2005 or VS2005Pro
 OGRE 1.4.x SDK for Visual C++ .Net 2008 (9.0) with Visual Studio Express 2008

ZLib: If you are using 1 prebuilt release of Ogre SDK, ZLib is not embedded, download last version on http://www.zlib.net,
open visualc6/zlib.dsw, upgrade it, Edit the LIB_Debug and LIB_Release project settings and in the Resources|General section, remove the resource name,
build LIB_Debug and LIB_Release configs, copy zlib.h and zconf.h into ogrenew/Dependencies/include, copy zlib.lib and zlibd.lib from visualc6/Win32_LIB_<config>
into ogrenew/Dependencies/lib/<config>

Check you installed correctly the DirectX SDK August 2007 (at least ;-) )
- C:\Program Files\Microsoft DirectX SDK (August 2007) present
- the env. var. DXSDK is defined

Get SVN repository for Solipsis

Open ogrenew/Ogre_vc8.sln
Set up RenderSystem_Direct3D9 properties :
Add Additional Include path "$(DXSDK_DIR)\include"
Add Additional Lib path "$(DXSDK_DIR)\lib\x86" 
Batch build all (Debug+Release)

Define the OGRE_HOME env. var. with [WINDOWS]+[PAUSE] / Advanced / Env. Var.
If you are compiling Ogre from source code
 OGRE_HOME = C:\<your_path>\ogrenew
If you are using Ogre prebuilt-SDK
 OGRE_HOME = C:\OgreSDK

Open solipsis_vcX.sln
Batch build Navigator project (Debug+Release)

Check into the Navigator project settings you set correctly the Debug/Working directory to $(TargetDir)
Press F5 to run the Release binary OR launch directly navigator/bin/Release/Navigator.exe
(avoid to launch the Debug config, you will be too often prompted by Mozilla/LLMozLib assertions,
they will be suppressed in the future)

If you are using Ageia PhysX:
 Download 2 packages runtime & SDK on http://devsupport.ageia.com
 Download runtime : PhysX_7.06.25_SystemSoftware.exe
 Download SDK : PhysX_2.7.2_SDK_Core.exe
 Define your PHYSX_DIR env. var.
 Run CopyPhysXFiles.bat to copy dedicated DLLs into your Common\bin

If you are using Tokamak:
 Compile project Tokamak under DevTools (config TokamakDLL)
 Run CopyTokamakFiles.bat to copy dedicated DLLs into your Common\bin

To generate HTML documentation:
 Download Doxygen Tool (doxygen.exe)
 Define your DOXYGEN_PATH env. var.
 Run Docs\doxygen\doxygen.bat
