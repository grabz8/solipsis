/*
-----------------------------------------------------------------------------
This source is a Stereoscopy manager for OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/
-----------------------------------------------------------------------------
* Copyright (c) 2008, Mathieu Le Ber, AXYZ-IMAGES
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the AXYZ-IMAGES nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY Mathieu Le Ber ''AS IS'' AND ANY
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL Mathieu Le Ber BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
-----------------------------------------------------------------------------
*/

#include "StereoPlugin.h"

using namespace Ogre;

StereoPlugin::StereoPlugin()
{
}
StereoPlugin::~StereoPlugin()
{
}

const String& StereoPlugin::getName() const
{
	static String name("StereoManager");
	return name;
}

void StereoPlugin::install()
{
	// init enable plug-in option
	mOptEnableStereo.name = "Enable stereo rendering";
	mOptEnableStereo.currentValue.clear();
	mOptEnableStereo.possibleValues.clear();
	mOptEnableStereo.immutable = false;
    mOptEnableStereo.possibleValues.push_back( "No" );   
    mOptEnableStereo.possibleValues.push_back( "Vertical Interlaced" );
    mOptEnableStereo.possibleValues.push_back( "Horizontal Interlaced" );
    mOptEnableStereo.possibleValues.push_back( "CheckBoard Interlaced" );
    mOptEnableStereo.possibleValues.push_back( "Vertical Split Screen" );
    mOptEnableStereo.possibleValues.push_back( "Horizontal Split Screen" );
    mOptEnableStereo.possibleValues.push_back( "Anaglyph" );
    mOptEnableStereo.possibleValues.push_back( "Dual Output" );
	mOptEnableStereo.currentValue = "No";

	RenderSystemList& renderSystemList = *Root::getSingleton().getAvailableRenderers();
	for (size_t i = 0 ; i < renderSystemList.size() ; i++)
	{
		RenderSystem * renderSystem = renderSystemList[i];
		ConfigOptionMap& configOptionMap = renderSystem->getConfigOptions();
		configOptionMap[mOptEnableStereo.name] = mOptEnableStereo;
	}
}

void StereoPlugin::initialise()
{
	// check if stereo must be enabled
	ConfigOptionMap& configOptionMap = Root::getSingleton().getRenderSystem()->getConfigOptions();
	if (configOptionMap[mOptEnableStereo.name].currentValue == "No")
	{
		return;
	}

	// the stereo manager needs a camera and a viewport but the have not been created yet in the application
	// create a frame manager to delay the initialisation to the first frame    
    Root::getSingleton().addFrameListener(this);
}

void StereoPlugin::shutdown()
{
	ConfigOptionMap& configOptionMap = Root::getSingleton().getRenderSystem()->getConfigOptions();
	if (configOptionMap[mOptEnableStereo.name].currentValue == "No")
	{
		return;
	}

	mStereoManager.shutdown();
}

void StereoPlugin::uninstall()
{

}

bool StereoPlugin::frameStarted(const FrameEvent& evt)
{
	// initialize the StereoManager at the first frame when the application is initialised

	RenderWindow *window = Root::getSingleton().getAutoCreatedWindow();
	Viewport *viewport = window->getViewport(0);

	std::list<RenderTarget*> renderTextureList;
	RenderSystem::RenderTargetIterator it = Root::getSingleton().getRenderSystem()->getRenderTargetIterator();
	while(it.hasMoreElements())
	{
		RenderTarget *rt = it.getNext();
		// store all the auto updated render textures that depend on the main camera in a list
		// (in fact any rendertarget execpt the main window since I did not found a way to tell if a rendertarget is a rendertexture or not)
		if(!rt->isPrimary() && rt->isAutoUpdated() && rt != window)
		{
			bool viewDependent = false;
			for(int i = 0; i < rt->getNumViewports(); i++)
			{
				if(rt->getViewport(i)->getCamera() == viewport->getCamera())
					viewDependent = true;
			}
			if(viewDependent)
				renderTextureList.push_back(rt);
		}
	}
    
    bool isConfigExist=true;
    // try to load the config file
	try
	{
		mStereoManager.loadConfig("stereo.cfg");
    }
	catch(Exception e)
	{
		// if the config file is not found, it throws an exception
		if(e.getNumber() == Exception::ERR_FILE_NOT_FOUND)
		{
			// Save config file after initialization
            isConfigExist=false;
		}
		else
			throw e;
	}		   
    
    ConfigOptionMap& configOptionMap = Root::getSingleton().getRenderSystem()->getConfigOptions();
    String stereoMode=configOptionMap[mOptEnableStereo.name].currentValue;
    if (stereoMode=="No")
        mStereoManager.init(viewport, NULL, StereoManager::SM_NONE);
    else if (stereoMode=="Anaglyph")
        mStereoManager.init(viewport, NULL, StereoManager::SM_ANAGLYPH);
    else if (stereoMode=="Horizontal Interlaced")
        mStereoManager.init(viewport, NULL, StereoManager::SM_INTERLACED_H);
    else if (stereoMode=="Vertical Interlaced")
        mStereoManager.init(viewport, NULL, StereoManager::SM_INTERLACED_V);
    else if (stereoMode=="CheckBoard Interlaced")
        mStereoManager.init(viewport, NULL, StereoManager::SM_INTERLACED_CB);
    else if (stereoMode=="Vertical Split Screen")
        mStereoManager.init(viewport, NULL, StereoManager::SM_SPLITSCREEN_V);
    else if (stereoMode=="Horizontal Split Screen")
        mStereoManager.init(viewport, NULL, StereoManager::SM_SPLITSCREEN_H);
    else if (stereoMode=="Dual Ouptut")
        mStereoManager.init(viewport, NULL, StereoManager::SM_DUALOUTPUT);
    else  
        mStereoManager.init(viewport, NULL, StereoManager::SM_NONE);
    
    if (!isConfigExist)
        mStereoManager.saveConfig("stereo.cfg");

	while(!renderTextureList.empty())
	{
		// tell the stereo manager to render these rendertextures one time for each eye because
		// their contents depend on the main camera position
		// (for example the rendertexture used for water reflexions)
		mStereoManager.addRenderTargetDependency(renderTextureList.front());
		renderTextureList.pop_front();
	}

	// only need to run this once, remove the listener
	Root::getSingleton().removeFrameListener(this);

	return true;
}
