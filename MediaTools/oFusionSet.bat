rem
rem Copy this batch into your 3DS Max directory
rem Install oFusion plugin and run oFusionUnSet.bat to create backup files
rem Then call Set to restore plugin and UnSet to remove it
rem

pause
copy oFusion_cg.dll cg.dll /y
copy oFusion_OgreMain.dll OgreMain.dll /y
copy oFusion_OgrePlatform.dll OgrePlatform.dll /y
copy oFusion_plugins.cfg plugins.cfg /y
copy oFusion_resources.cfg resources.cfg /y
copy plugins_moved\oFusion.dle plugins\. /y
copy plugins_moved\oFusionCore.gup plugins\. /y
copy plugins_moved\oMaterial.dlt plugins\. /y
pause
