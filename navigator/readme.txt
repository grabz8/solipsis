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

F1 .. F4 SkyDome choice
F5       FakeTerrain

[PAUSE]  switch on/off DEBUG panel
button1  small test on LOST evt from Solipsis node
button2  small Navi test (1 Knot  with Google, 1 panel on gpl.txt, 1 panel on YouTube video Warcraft in Simpsons)
. . .

About HTTP proxy, you can now define it into the options panel, the resulting config
file will be updated : navigator\bin\release\NaviProfile\prefs.js like this (FT proxy example)
user_pref("network.proxy.type", 2);
user_pref("network.proxy.autoconfig_url", "http://proxyconf.rd.francetelecom.fr");

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
OGRE_HOME = C:\<your_path>\ogrenew

Open DevTools/llmozlib/llmozlib_vcX.sln
Batch build all (Debug+Release)

Run DevTools/navi/copy_llmozlib_files_vcX.bat

Open DevTools/navi/Navi_vcX.sln
Batch build all (Debug+Release)

Open DevTools/pthreads/pthread_vcX.sln
Batch build all (Debug+Release)

Open DevTools/tinyxml/tinyxml.sln
Batch build ONLY the tinyxml project (Debug+Release), (STL, test and testSTL are not used)

Open DevTools/xmlrpc++/XmlRpc.sln
Batch build ONLY the XmlRpc project (Debug+Release)

Open DevTools/navigator/navigator.sln
Batch build all (Debug+Release)

Check into the navigator project settings you set correctly the Debug/Working directory to $(TargetDir)
Press F5 to run the Release binary OR launch directly navigator/bin/Release/Navigator.exe
(avoid to launch the Debug config, you will be too often prompted by Mozilla/LLMozLib assertions,
they will be suppressed in the future)
