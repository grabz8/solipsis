pause
if not exist plugins_moved\oFusion.dle copy plugins\oFusion.dle plugins_moved\. /y
if not exist plugins_moved\oFusionCore.gup copy plugins\oFusionCore.gup plugins_moved\. /y
if not exist plugins_moved\oMaterial.dlt copy plugins\oMaterial.dlt plugins_moved\. /y
if not exist oFusion_cg.dll copy cg.dll oFusion_cg.dll /y
if not exist oFusion_OgreMain.dll copy OgreMain.dll oFusion_OgreMain.dll /y
if not exist oFusion_OgrePlatform.dll copy OgrePlatform.dll oFusion_OgrePlatform.dll /y
if not exist oFusion_plugins.cfg copy plugins.cfg oFusion_plugins.cfg /y
if not exist oFusion_resources.cfg copy resources.cfg oFusion_resources.cfg /y
del cg.dll
del OgreMain.dll
del OgrePlatform.dll
del plugins.cfg
del resources.cfg
del plugins\oFusion.dle
del plugins\oFusionCore.gup
del plugins\oMaterial.dlt
pause
