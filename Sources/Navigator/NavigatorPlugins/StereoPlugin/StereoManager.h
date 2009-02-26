/*
-----------------------------------------------------------------------------
This source is part of the Stereoscopy manager for OGRE
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

// Last update 11/25/2008

#ifndef _STEREOMANAGER_H_
#define _STEREOMANAGER_H_

#include "Ogre.h"
#include <map>

using namespace Ogre;
/** Stereo vision manager class.
	This manager supports two stereo vision mode : 
	 - An anaglyph mode for red/blue glasses
	 - An interlaced mode for autostereoscopic screens
	 - A dual output mode to use with a polarized screen and two projectors or mounted head display

	Note :
	Ogre does not support natively the output in fullscreen on two screens. If you want to use the 
	dual output mode with two fullscreen displays, you may want to apply this patch to OGRE :
	http://sourceforge.net/tracker2/?func=detail&aid=2474878&group_id=2997&atid=302997
	However, this manager is completely indepedent from this patch, the dual output mode only needs two viewports,
	for example two non-fullscreen windows or a windows with two viewports inside.
*/

class StereoManager
{
public:
	enum StereoMode
	{
		SM_NONE,
		SM_ANAGLYPH,
		SM_DUALOUTPUT,
		SM_INTERLACED_V,
		SM_INTERLACED_H,
		SM_INTERLACED_CB,
        SM_SPLITSCREEN_V,
        SM_SPLITSCREEN_H
		//SM_SHUTTERGLASSES
	};

	struct StereoModeDescription
	{
		StereoModeDescription() {}
		StereoModeDescription(const String &name) :
			mName(name), mUsesCompositor(false), mMaterialName("") {}
		StereoModeDescription(const String &name, const String &materialName) :
			mName(name), mUsesCompositor(true), mMaterialName(materialName) {}
		StereoModeDescription &operator=(const StereoModeDescription &desc)
		{
			mName = desc.mName;
			mMaterialName = desc.mMaterialName;
			mUsesCompositor = desc.mUsesCompositor;
			return *this;
		}
		String mName;
		String mMaterialName;
		bool mUsesCompositor;
	};

	typedef std::map<StereoMode, StereoModeDescription> StereoModeList;

protected:
	class StereoCameraListener : public RenderTargetListener
	{
		Vector3 mOldPos;
		StereoManager *mStereoMgr;
		Camera *mCamera;
		Viewport *mViewport;
	public:
		bool mIsLeftEye;
		void init(StereoManager *stereoMgr, Viewport *viewport, bool isLeftCamera);
//		virtual void preRenderTargetUpdate (const RenderTargetEvent& evt);
//		virtual void postRenderTargetUpdate  (const RenderTargetEvent& evt);
		virtual void preViewportUpdate (const RenderTargetViewportEvent &evt);
		virtual void postViewportUpdate (const RenderTargetViewportEvent &evt);
	};
	friend class StereoCameraListener;

	class DeviceLostListener : public RenderSystem::Listener
	{
		StereoManager *mStereoMgr;
	public:
		void init(StereoManager *stereoMgr);
		virtual void eventOccurred (const String &eventName, const NameValuePairList *parameters);
	};
	friend class DeviceLostListener;

	Camera *mCamera;
	StereoCameraListener mLeftCameraListener, mRightCameraListener;
	DeviceLostListener mDeviceLostListener;
	Viewport *mLeftViewport, *mRightViewport;
	uint32 mLeftMask, mRightMask;
	CompositorInstance *mCompositorInstance;

	/* config */
	StereoModeList mAvailableModes;
	StereoMode mStereoMode;
	Real mEyesSpacing;
	Real mFocalLength;
	bool mIsFocalPlaneFixed;
	Real mScreenWidth;
	bool mIsInversed;
	//String mConfigFileName;

	/* members for anaglyph only  */
	bool mAreOverlaysEnabled;
	Viewport *mCompositorViewport;

	/* dependencies */
	typedef std::map<RenderTarget *, bool> RenderTargetList;
	//typedef std::list<RenderTarget *> RenderTargetList;
	RenderTargetList mRenderTargetList;

	/* debug plane */
	SceneManager *mSceneMgr;
	Entity *mDebugPlane;
	SceneNode *mDebugPlaneNode;
	String mLeftMaterialName, mRightMaterialName;

	
	void initCompositor(Viewport *viewport, const String &materialName, Viewport *&out_left, Viewport *&out_right);
	void shutdownCompositor(void);
	void initListeners(Viewport* leftViewport, Viewport* rightViewport);
	void shutdownListeners(void);
	
	void updateCamera(Real delta);

	void init(Viewport* leftViewport, Viewport* rightViewport);

	void updateAllDependentRenderTargets(bool isLeftEye);
	void chooseDebugPlaneMaterial(bool isLeftEye);

public:
	//--------------init--------------
	StereoManager(void);
	~StereoManager(void);
	/** The manager should be initialized with two viewports if you want dual output stereo (SM_DUALOUTPUT).
		If you want red/blue anaglyph stereo (SM_ANAGLYPH) you only need one viewport, just set the rightViewport to NULL.
		The left eye and the right eye will be composited on the leftViewport.
		The camera will be detected from the viewports.

		You can also use a configuration file that will store the stereo mode, the focal length, the eye spacing and
		the screen width. */
	void init(Viewport* leftViewport, Viewport* rightViewport, const String &fileName);
	void init(Viewport* leftViewport, Viewport* rightViewport, StereoMode mode);

	/**	Shutdown and re-init the stereo manager to change stereo mode */
	void shutdown(void);

	//--------------dependencies--------------
	/**	RenderTargets added with this method will no longer be auto-updated but will be updated
		one time before the right viewport is updated and one time before the left viewport is updated.
		It is useful if you have a render texture whose content depends on the camera position,
		for example a render texture for water reflection */
	void addRenderTargetDependency(RenderTarget *renderTarget);
	void removeRenderTargetDependency(RenderTarget *renderTarget);

	//--------------debug plane--------------
	/**	Create a representation of the focal plane in the scene. If no material name is provided,
		a default wireframe material is used. */
	void createDebugPlane(SceneManager *sceneMgr, const String &leftMaterialName = "", const String &rightMaterialName = "");
	void destroyDebugPlane(void);
	void enableDebugPlane(bool enable);
	void toggleDebugPlane(void);
	void updateDebugPlane(void);

	//--------------accessors--------------
	inline Camera *getCamera(void) const {return mCamera;}
	inline void setCamera(Camera *cam) {mCamera = cam;}

	inline Real getEyesSpacing(void) const {return mEyesSpacing;}
	inline void setEyesSpacing(Real l) {mEyesSpacing = l;}

	inline Real getFocalLength(void) const {return mFocalLength;}
	void setFocalLength(Real l);

	/**	The focal plane represents the screen in the world space. As the screen
		is not and infinite plane but a rectangle, what I call the focal plane is in fact
		a rectangle.
		When the position of the plane is fixed, its position and size doesn't change if
		you change the focal length. Instead the camera is moved to reflect the focal length.
		The FOV angle of the camera is also adjusted in order to keep the focal rectangle
		covering the whole field view.

		If you set the screen width in the manager you can achieve a 1:1 scale effect if the
		observer's distance to the screen is equal to the focal distance.*/
	inline void fixFocalPlanePos(bool fix) {mIsFocalPlaneFixed = fix;}
	inline void setScreenWidth(Real w) {mScreenWidth = w;}
	void useScreenWidth(Real w);

	inline StereoMode getStereoMode(void) {return mStereoMode;}
//    inline void setStereoMode(StereoMode& stereoMode) { mStereoMode=stereoMode; };
	/** Inverse the left eye and the right eye viewports */
	void inverseStereo(bool inverse);
	bool isStereoInversed(void) {return mIsInversed;}

	/** Only objects matching the following flags will be rendered.
		This method sets the visibility mask for the right and the left viewports and
		the according mask for each dependent render target. */
	void setVisibilityMask(uint32 leftFlags, uint32 rightFlags);

	//--------------config--------------
	/**	You can save and load the stereo configuration (mode, eyes spacing, focal length and screen width)
		to a file. Then this file can be used to initialize the manager.  */
	void StereoManager::saveConfig(const String &filename);
	void StereoManager::loadConfig(const String &filename);
};

#endif