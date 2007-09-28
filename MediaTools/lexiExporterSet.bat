rem
rem Copy this batch into your 3DS Max directory
rem Install lexiExporter plugin and run lexiExporterUnSet.bat to create backup files
rem Then call Set to restore plugin and UnSet to remove it
rem

pause
copy lexiExporter_cg.dll cg.dll /y
copy lexiExporter_OgreMain.dll OgreMain.dll /y
copy lexiExporter_plugins.cfg plugins.cfg /y
copy lexiExporter_resources.cfg resources.cfg /y
copy plugins_moved\LEXIExport.dlu plugins\. /y
pause
