pause
if not exist plugins_moved\LEXIExport.dlu copy plugins\LEXIExport.dlu plugins_moved\. /y
if not exist lexiExporter_cg.dll copy cg.dll lexiExporter_cg.dll /y
if not exist lexiExporter_OgreMain.dll copy OgreMain.dll lexiExporter_OgreMain.dll /y
if not exist lexiExporter_plugins.cfg copy plugins.cfg lexiExporter_plugins.cfg /y
if not exist lexiExporter_resources.cfg copy resources.cfg lexiExporter_resources.cfg /y
del cg.dll
del OgreMain.dll
del plugins.cfg
del resources.cfg
del plugins\LEXIExport.dlu
pause
