import maya.cmds
import maya.mel
import zipfile
import os
import re
import subprocess

class StaticMethod:
	""" used to be able to call static methods """
	def __init__(self, anycallable):
		self.__call__ = anycallable

def exportAvatar( strPath, strAvatarName ):
	maya.cmds.sysFile( strPath, makeDir=True )
	strMeshFilename = strAvatarName+'_edition.mesh'
	strSkeletonFilename = strAvatarName+'_mesh.skeleton'
	strMaterialFilename = strAvatarName+'_edition.material'
	# it's important to set the current path to the directory containing the exported data because the filenames exported inside the exported data are relative to this directory. More precisely, the skeletonlink filename is the one that is give as the one provided as the -skel argument.
	os.chdir(strPath)
	# it's important to export all data in a single command, otherwise the skeletonlink information is not exported in the mesh file
	strExportCommand = 'ogreExport'
	strExportCommand += ' -sel'		#export the selected mesh
	bCharacterIs100TimesTooSmall = True # I didn't manage to get the character at the right scale in Maya (problems with FBX importer that scales the root bone, which causes trouble in solipsis (bounding box) ). So, instead of having the size of a character in the order of 1 meter, it was in the order of 1 cm.
	if bCharacterIs100TimesTooSmall:
		strExportCommand += ' -lu cm'
	else:
		strExportCommand += ' -lu m' # it is important to set length units as meter otherwise it exports units as cm (why ?)
	strMeshExportOptions = ' -v' # export vertex bone assignment
	strMeshExportOptions += ' -n' # export vertex normals
	strExportCommand += ' -mesh "' + strMeshFilename + '"' + strMeshExportOptions
	strExportCommand += ' -skel "'+strSkeletonFilename+'"'
	# warning : the argument sampleByFrames is not documented but nonetheless necessary
	# By looking at the code of the exporter, the user has to specify the sampling rate either in frames (sampleByFrames) or in seconds (sampleBySeconds?)
	strIdleClipOptions = ' -skeletonClip Idle startEnd 0 128 frames sampleByFrames 1'
	strRunClipOptions = ' -skeletonClip Run startEnd 133 147 frames sampleByFrames 1'
	strWalkClipOptions = ' -skeletonClip Walk startEnd 148 172 frames sampleByFrames 1'
	strOpenMouthClipOptions = ' -skeletonClip OpenMouth startEnd 200 200 frames sampleByFrames 1'
	strCloseMouthClipOptions = ' -skeletonClip CloseMouth startEnd 201 201 frames sampleByFrames 1'
	strExportCommand += ' -skeletonAnims'
	strExportCommand += ' -skelBB'	# include skeleton anims in bounding box computation
	strExportCommand += strIdleClipOptions + strRunClipOptions + strWalkClipOptions + strOpenMouthClipOptions + strCloseMouthClipOptions
	strExportCommand += ' -mat "'+strMaterialFilename+'" -copyTex "'+strPath+'"'
	strExportCommand += ';'
	#maya.mel.eval('ogreExport -all -lu m -mesh "'+strMeshFilename+'" -v -skel "'+strSkeletonFilename+'" -skeletonAnims -skeletonClip Idle -startEnd 0 128 "frames" -skeletonClip Run -startEnd 133 147 "frames" -skeletonClip Walk -startEnd 148 172 "frames" -mat "'+strMaterialFilename+'" -copyTex "'+strPath+'";') 
	maya.cmds.select('Kevin') #just export the main mesh (ignore the Diva hierarchy)
	maya.mel.eval( strExportCommand );
	createHumanReadableFilesForDebugging = True
	if createHumanReadableFilesForDebugging:
		print('creating human readable version for the exported data')
		subprocess.check_call('"C:/Program Files/Ogre1.4.9/ogre/Tools/XMLConverter/bin/debug/OgreXMLConverter.exe" '+strMeshFilename, cwd=strPath, shell = True)
		subprocess.check_call('"C:/Program Files/Ogre1.4.9/ogre/Tools/XMLConverter/bin/debug/OgreXMLConverter.exe" '+strSkeletonFilename, cwd=strPath, shell = True)
	# create a saf (Solipsis Avatar File) file containing the exported files
	strArchiveFilename = strAvatarName+'.saf'
	zipFile = zipfile.ZipFile('c:/dev/Solipsis/trunk/media/cache/'+strArchiveFilename, 'w')
	zipFile.write( strMeshFilename )
	zipFile.write( strSkeletonFilename )
	zipFile.write( strMaterialFilename )
	# put the textures in the archive
	for root, dirs, files in os.walk( strPath, topdown=False ):
		for fileName in files:
			isATextureFile = (re.search('.jpg$', fileName) != None )
			if isATextureFile:
				print 'Solpisis.exportAvatar : adding the texture '+os.path.join(root, fileName)+' to '+strArchiveFilename
				fileNameInArchive = fileName
				zipFile.write( os.path.join(root, fileName), fileNameInArchive )
	zipFile.close()
	#maya.cmds.system('zip -r "c:/dev/Solipsis/trunk/media/cache/'+strArchiveFilename+'" "'+strPath+'"')
	print(strAvatarName)

if __name__ == "__main__":
	exportAvatar('c:/dev/KevinDiva', 'KevinDiva')
