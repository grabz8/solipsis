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


--------------------------------------------------------------------------------
HELP
--------------------------------------------------------------------------------

nodeSimulator simply simulate the node behaviour as server behind the Navigator,
XMLRPC (XML/HTTP) is used (either XMLRPC++ or ULXMLRPCPP), server created on : localhost:8550
Server accepts login/logout, give 1 different nodeId to each cnx and handle those events
specifically for nodeId="00000001" :
- 10s later : send the User avatar object
- 10s later : send the Deltastation1 scene object
- 10s later : send the z6-salamandra avatar object


--------------------------------------------------------------------------------
HOW TO BUILD
--------------------------------------------------------------------------------

vcX stands for Visual config (vc7 or vc8)

Install Visual Studio 2005 + SP1 package

Download Ogre source package (branch 1.4)
Download Ogre Dependencies for VS8.1 (http://www.ogre3d.org/index.php?option=com_content&task=view&id=412&Itemid=132)
and unzip them into ogrenew subdirectory

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

COMPILATION DEFINES (add 1 starting 'N' char to unset it):
ULXR           Using ULXMLRPCPP MT-library instead of XMLRPC++ ST-library
