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

Camera:
1        First-Person view mode
2        First-Person view mode with mouse to interact
3        Third-Person view mode
MOUSE    Cursor mode (3rd person) / View mode (1st person)


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
F5       FakeTerrain
F9       Toggle MODELER main panel
F10      Toggle physics contacts display
F11      Toggle physics geometries display
F12      Toggle bounding-boxes display
[PAUSE]  Toggle DEBUG panel
 Helpers
  toggleAxis     Toggle 1 axis on (0,0,0) with scale x1, position + scale can be updated to locate something
  rotateSunLight Rotate the directional light to debug normals illumination
 Demos
  demoNavi1      Launch DEMO_NAVI1 (refer below)
  demoPhysics1   Launch DEMO_PHYSICS1 (refer below)
 Tests
  peerLost       Small test on LOST evt from Solipsis node
  peerNew        Small test on NEW evt (for instance this evt is automatically launched on uichat.html page loaded evt)


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

Open solipsis_vcX.sln
Batch build Navigator project (Debug+Release)

Check into the Navigator project settings you set correctly the Debug/Working directory to $(TargetDir)
Press F5 to run the Release binary OR launch directly navigator/bin/Release/Navigator.exe
(avoid to launch the Debug config, you will be too often prompted by Mozilla/LLMozLib assertions,
they will be suppressed in the future)

If you are using Ageia PhysX:
 Download 2 packages runtime & SDK on http://devsupport.ageia.com (refer to registered email/password below)
 Download runtime : PhysX_7.06.25_SystemSoftware.exe
 Download SDK : PhysX_2.7.2_SDK_Core.exe
 Define your PHYSX_DIR env. var.

COMPILATION DEFINES (add 1 starting 'N' char to unset it):
UIDEBUG        Set/Unset the debug user interface panel
DEMO_NAVI1     Set/Unset Navi demo1 (1 panel on YouTube video, 1 panel on gpl.txt + 1 knot mesh with google homepage)
DEMO_NAVI2     Set/Unset Navi demo2 (spatialStation official demo with 1 panel on YouTube video)
NSHADOWS       Set/Unset shadows (hm hm just test, still lot of stuff)
LEXI           User avatar exported with LEXI plugin
PHYSICS        Set/Unset Physics management by Ode and OgreODE
 DEMO_PHYSICS1 Set/Unset Physics demo (1 cube will drop into the spatialStation)
 CAPSULEGEOM   Set/Unset Physics capsule on user avatar (walls)
 FEET          Set/Unset Physics feet sphere on user avatar (floor + gravity)
PHYSX          Set/Unset Physics management by Ageia PhysX
TOKAMAK        Set/Unset Physics management by Tokamak

--------------------------------------------------------------------------------
FORUMS LOGINS
--------------------------------------------------------------------------------

Ogre
url      http://www.ogre3d.org/phpBB2/
username g6r6e6g
email    gregoryjan@free.fr
password greg666

Navi
url      http://navi.agelessanime.com/forum
username g6r6e6g
email    gregoryjan@free.fr
password greg666

Mozilla
http://www.nabble.com
http://www.nabble.com/Mozilla---Embedding-f6665.html
username g6r6e6g
email    gregoryjan@free.fr
password greg666

PhysX
http://devsupport.ageia.com
email    gregoryjan@free.fr
password greg666
