Plugin_sdk - A Ogre3D plugin for importer the 3D models from .sdk files

Ogre is an object oriented graphics rendering engine. See http://www.ogre3d.org/ for more information.

As a do not have enough .sdk files to fully test, if you encounter any problem please contact
me.

The DLL export a function:

Ogre::Entity* Plugin_sdk::createEntityFromsdk(const Ogre::String & entityName, 
				const Ogre::String & szsdkFileName,
				Ogre::SceneManager* mSceneMgr);

entityName: the name you want the entity called, like in the function 
		Ogre::SceneManager::createEntity(const Ogre::String & entityName,
						const Ogre::String & meshName)
szsdkFileName: the absolute path of the .sdk file


To compile it under windows :

1) if you have the OgreSDK installed
- set the OGRE_HOME environment variable to the path of your OgreSDK (c:\OgreSDK)
- add %OGRE_HOME%\bin\release;%OGRE_HOME%\bin\debug to your PATH

2) if you want to use the OGRE sources to compile sdk2mesh
- set the OGRE_SRC environment variable to the path of your source installation
- add (at the beginning) to your PATH : 
%OGRE_SRC%\samples\common\bin\debug;%OGRE_SRC%\samples\common\bin\release


Supported platform: at the current time, only tested with VC 2008 under Windows. 

--
CHAI dong <dong.chai@orange-ftgroup.com>
