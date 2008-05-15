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

Camera:
1        First-Person view mode
2        First-Person view mode with mouse to interact
3        Third-Person view mode
4        Third-Person view mode with camera orbit
MOUSE    Cursor mode (3rd person) / View mode (1st person)
DBLCLICK In plugins mode, you have to double-click left mouse to enter/leave FirstPerson mouse mode


Move (french keyboard letters):
UP/Z     Move forward
DOWN/S   Move backward
LEFT/Q   Turn left (3rd person) / Straff left (1st person)
RIGHT/D  Turn right (3rd person) / Straff right (1st person)
PGUP/E   Fly up (+ deactivate gravity)
PGDOWN/C Fly down
END      Activate gravity


Debug commands:
F1 .. F4 SkyDome choice
F9       Toggle MODELER main panel
F10      Toggle physics contacts display (if supported by the physics plugin) (NavDemo only)
F11      Toggle physics geometries display (if supported by the physics plugin) (NavDemo only)
F12      Toggle bounding-boxes display
[PAUSE]  Toggle DEBUG panel
 Helpers
  toggleAxis     Toggle 1 axis on (0,0,0) with scale x1, position + scale can be updated to locate something
  rotateSunLight Rotate the directional light to debug normals illumination
 Demos
  demoNavi1      Launch DEMO_NAVI1 (refer below)
  demoPhysics1   Launch DEMO_PHYSICS1 (refer below) (NavDemo only)
 Tests
  peerLost       Small test on LOST evt from Solipsis node (NavDemo only)
  peerNew        Small test on NEW evt (for instance this evt is automatically launched on uichat.html page loaded evt) (NavDemo only)


Demos Tips/Tricks:
into Common\bin\navigator\XXXX\NaviLocal\uilogin.html, you can uncomment 2 lines under // AUTOMATIC LOGIN TO SPEED UP UNITARY TESTS
into Common\bin\navigator\XXXX\NaviLocal\uilogin.html, you can the position of the demoNavi2 panel
into Media\lua\boot.lua, you can tune default connection params into the line navigator:setConnectionParams("localhost", 8550, "me")


About HTTP proxy, you can now define it into the options panel, the resulting config
file will be updated : navigator\bin\release\NaviProfile\prefs.js like this (FT proxy example)
user_pref("network.proxy.type", 2);
user_pref("network.proxy.autoconfig_url", "http://proxyconf.rd.francetelecom.fr");


You can build:
- the Peer and Navigator applications separately (Debug/Release configs)
- the Navigator application with the Peer embedded (DebugNCS/ReleaseNCS configs, NCS stands for NullClientServer)


To run the Peer application you currently have to specify host, port and verbosity of the Peer/Navigator XMLRPC server:
-h <IPaddr:str>    : IP address (default: localhost)
-p <port:int>      : port (default: 8550)
-v <verbosity:int> : verbosity level (0..2)
-s <sceneName:str> : Scene name loaded by the peer simulator (eg. Deltastation1, Ile, ...)


--------------------------------------------------------------------------------
HOW TO BUILD
--------------------------------------------------------------------------------

vcX stands for Visual config (vc7 or vc8)

Install Visual Studio 2005 + SP1 package
If you are using Visual Studio Express 2005, upgrade it with the SP1 package
Update the default Visual Studio COREWIN properties XML file to add system libraries:
 VSExpress2005 C:\Program Files\Microsoft Visual Studio 8\VC\VCProjectDefaults\corewin_express.vsprops
 VSExpress2008 C:\Program Files\Microsoft Visual Studio 9.0\VC\VCProjectDefaults\CoreWin.vsprops
 replace this line
  AdditionalDependencies="kernel32.lib"/>
 by this line
  AdditionalDependencies="kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib"/>

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

VoiceEngine:
Download the BOOST package release 1.34.1 (http://sourceforge.net, search boost c++ libraries, download 1.34.1)
 (because some bugs found into pool<> in 1.35), unzip it and define your BOOST_ROOT env. var.
Download the FMOD EX package (http://www.fmod.org/index.php/download),
 install it and define your FMOD_PATH env. var. (eg. C:\Program Files\FMOD SoundSystem\FMOD Programmers API Win32\api)

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

To generate HTML documentation:
 Download Doxygen Tool (doxygen.exe)
 Define your DOXYGEN_PATH env. var. on directory containing doxygen.exe
 Run Docs\doxygen\doxygen.bat

To generate setup from NSIS
 Download NSIS on nsis.sourceforge.net
 HM NIS Edit can help you on http://hmne.sourceforge.net
 Define your NSIS_PATH env. var. on directory containing makensis.exe
 Run setup\nsis\makensis.bat

COMPILATION DEFINES (add 1 starting 'N' char to unset it):
UIDEBUG        Set/Unset the debug user interface panel
DEMO_NAVI1     Set/Unset Navi demo1 (1 panel on YouTube video, 1 panel on gpl.txt + 1 knot mesh with google homepage)
DEMO_NAVI2     Set/Unset Navi demo2 (demo with 1 panel on YouTube video)
DEMO_VNC       Set/Unset VNC demo (demo with 1 panel on VNC viewer)
DEMO_VOICE     Set/Unset Voice chat demo
DEMO_PHYSICS1  Set/Unset Physics demo (1 cube will drop into the spatialStation)
NSHADOWS       Set/Unset shadows (hm hm just test, still lot of stuff)
LOGSNDRCV      Set/Unset traces on sent/received XML msgs

About Navi/LLMozLib Debug assertion, to avoid them:
Define the XPCOM_DEBUG_BREAK syst. env. var. with [WINDOWS]+[PAUSE] / Advanced / Env. Var.
 XPCOM_DEBUG_BREAK = warn

LLMozLib was originally compiled/linked on VC7.1 so msvcp71.dll, msvcp71d.dll, msvcr71.dll, msvcr71d.dll
are necessary when Solipsis is compiled/linked on VC > 7.1, unfortunately those DLLs were not packaged in
a redistributable, you can find them into DevTools\llmozlib\libraries\i686-win32\vc71redist

In Network configs, before launching, don t forget to run:
navigatorModuleOnlyNav\copyFirefoxPlugins.bat
peerModuleSimulator\copyPhysXFiles.bat

In NCS configs, before launching, don t forget to run:
navigatorModuleOnlyNav\copyNCSPhysXFiles.bat

