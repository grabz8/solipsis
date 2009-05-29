/*
	This file is part of NaviLibrary, a library that allows developers to create and 
	interact with web-content as an overlay or material in Ogre3D applications.

	Copyright (C) 2009 Adam J. Simmons
	http://princeofcode.com/navilibrary.php

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Navi.h"
#include "NaviUtilities.h"
#include <OgreBitwise.h>

using namespace Ogre;
using namespace NaviLibrary;
using namespace NaviLibrary::NaviUtilities;

Navi::Navi(const std::string& name, unsigned short width, unsigned short height, const NaviPosition &naviPosition,
			bool asyncRender, int maxAsyncRenderRate, Ogre::uchar zOrder, Tier tier, Ogre::Viewport* viewport)
{
	webView = 0;
	naviName = name;
	naviWidth = width;
	naviHeight = height;
	overlay = 0;
	movable = true;
	maxUpdatePS = 0;
	lastUpdateTime = 0;
	opacity = 1;
	usingMask = false;
	ignoringTrans = true;
	transparent = 0.05;
	isWebViewTransparent = false;
	ignoringBounds = false;
	okayToDelete = false;
	compensateNPOT = false;
	texWidth = width;
	texHeight = height;
	alphaCache = 0;
	alphaCachePitch = 0;
	matPass = 0;
	baseTexUnit = 0;
	maskTexUnit = 0;
	fadeValue = 1;
	isFading = false;
	deltaFadePerMS = 0;
	lastFadeTimeMS = 0;
	texFiltering = Ogre::FO_NONE;
	tooltipsEnabled = true;
	needsForceRender = false;
	alwaysReceivesKeyboard = false;
	hasInternalKeyboardFocus = false;
	resizeParameters = std::pair<int, int>(0, 0);

	createMaterial();
	
	overlay = new NaviOverlay(name + "_overlay", viewport, width, height, naviPosition, getMaterialName(), zOrder, tier);

	if(compensateNPOT)
		overlay->panel->setUV(0, 0, (Real)naviWidth/(Real)texWidth, (Real)naviHeight/(Real)texHeight);	

	createWebView(asyncRender, maxAsyncRenderRate);
}

Navi::Navi(const std::string& name, unsigned short width, unsigned short height, 
			bool asyncRender, int maxAsyncRenderRate, Ogre::FilterOptions texFiltering)
{
	webView = 0;
	naviName = name;
	naviWidth = width;
	naviHeight = height;
	overlay = 0;
	movable = false;
	maxUpdatePS = 0;
	lastUpdateTime = 0;
	opacity = 1;
	usingMask = false;
	ignoringTrans = true;
	transparent = 0.05;
	ignoringBounds = false;
	okayToDelete = false;
	compensateNPOT = false;
	texWidth = width;
	texHeight = height;
	alphaCache = 0;
	alphaCachePitch = 0;
	matPass = 0;
	baseTexUnit = 0;
	maskTexUnit = 0;
	fadeValue = 1;
	isFading = false;
	deltaFadePerMS = 0;
	lastFadeTimeMS = 0;
	this->texFiltering = texFiltering;
	tooltipsEnabled = true;
	needsForceRender = false;
	alwaysReceivesKeyboard = false;
	hasInternalKeyboardFocus = false;
	resizeParameters = std::pair<int, int>(0, 0);

	createMaterial();
	createWebView(asyncRender, maxAsyncRenderRate);	
}

Navi::~Navi()
{
	if(alphaCache)
		delete[] alphaCache;

	if(webView)
		webView->destroy();

	if(overlay)
		delete overlay;

	MaterialManager::getSingletonPtr()->remove(naviName + "Material");
	TextureManager::getSingletonPtr()->remove(naviName + "Texture");
	if(usingMask) TextureManager::getSingletonPtr()->remove(naviName + "MaskTexture");
}

void Navi::createWebView(bool asyncRender, int maxAsyncRenderRate)
{
	webView = Awesomium::WebCore::Get().createWebView(naviWidth, naviHeight, false, asyncRender, maxAsyncRenderRate);
	webView->setListener(this);

	bind("drag", NaviDelegate(this, &Navi::onRequestDrag));
}

void Navi::createMaterial()
{
	limit<float>(opacity, 0, 1);

	if(!Bitwise::isPO2(naviWidth) || !Bitwise::isPO2(naviHeight))
	{
		if(Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(RSC_NON_POWER_OF_2_TEXTURES))
		{
			if(Root::getSingleton().getRenderSystem()->getCapabilities()->getNonPOW2TexturesLimited())
				compensateNPOT = true;
		}
		else compensateNPOT = true;
		
		if(compensateNPOT)
		{
			texWidth = Bitwise::firstPO2From(naviWidth);
			texHeight = Bitwise::firstPO2From(naviHeight);
		}
	}

	// Create the texture
	TexturePtr texture = TextureManager::getSingleton().createManual(
		naviName + "Texture", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		TEX_TYPE_2D, texWidth, texHeight, 0, PF_BYTE_BGRA,
		TU_DYNAMIC_WRITE_ONLY_DISCARDABLE, this);

	HardwarePixelBufferSharedPtr pixelBuffer = texture->getBuffer();
	pixelBuffer->lock(HardwareBuffer::HBL_DISCARD);
	const PixelBox& pixelBox = pixelBuffer->getCurrentLock();
	texDepth = Ogre::PixelUtil::getNumElemBytes(pixelBox.format);
	texPitch = (pixelBox.rowPitch*texDepth);

	uint8* pDest = static_cast<uint8*>(pixelBox.data);

	memset(pDest, 128, texHeight*texPitch);

	pixelBuffer->unlock();

	MaterialPtr material = MaterialManager::getSingleton().create(naviName + "Material", 
		ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	matPass = material->getTechnique(0)->getPass(0);
	matPass->setSceneBlending(SBT_TRANSPARENT_ALPHA);
	matPass->setDepthWriteEnabled(false);

	baseTexUnit = matPass->createTextureUnitState(naviName + "Texture");
	
	baseTexUnit->setTextureFiltering(texFiltering, texFiltering, FO_NONE);
	if(texFiltering == FO_ANISOTROPIC)
		baseTexUnit->setTextureAnisotropy(4);
}

// This is for when the rendering device has a hiccup and loses the dynamic texture
void Navi::loadResource(Resource* resource)
{
	Texture *tex = static_cast<Texture*>(resource); 

	tex->setTextureType(TEX_TYPE_2D);
	tex->setWidth(texWidth);
	tex->setHeight(texHeight);
	tex->setNumMipmaps(0);
	tex->setFormat(PF_BYTE_BGRA);
	tex->setUsage(TU_DYNAMIC_WRITE_ONLY_DISCARDABLE);
	tex->createInternalResources();

	needsForceRender = true;

	if(overlay)
		resetPosition();
}

void Navi::update()
{
	resizeIfNeeded();

	if(maxUpdatePS)
		if(timer.getMilliseconds() - lastUpdateTime < 1000 / maxUpdatePS)
			return;

	updateFade();

	if(usingMask)
		baseTexUnit->setAlphaOperation(LBX_SOURCE1, LBS_MANUAL, LBS_CURRENT, fadeValue * opacity);
	else if(isWebViewTransparent)
		baseTexUnit->setAlphaOperation(LBX_BLEND_TEXTURE_ALPHA, LBS_MANUAL, LBS_TEXTURE, fadeValue * opacity);
	else
		baseTexUnit->setAlphaOperation(LBX_SOURCE1, LBS_MANUAL, LBS_CURRENT, fadeValue * opacity);

	if(!needsForceRender)
		if(!webView->isDirty())
			return;

	TexturePtr texture = TextureManager::getSingleton().getByName(naviName + "Texture");
	
	HardwarePixelBufferSharedPtr pixelBuffer = texture->getBuffer();
	pixelBuffer->lock(HardwareBuffer::HBL_DISCARD);
	const PixelBox& pixelBox = pixelBuffer->getCurrentLock();

	uint8* destBuffer = static_cast<uint8*>(pixelBox.data);

	webView->render(destBuffer, (int)texPitch, (int)texDepth);

	if(isWebViewTransparent && !usingMask && ignoringTrans)
	{
		for(int row = 0; row < texHeight; row++)
			for(int col = 0; col < texWidth; col++)
				alphaCache[row * alphaCachePitch + col] = destBuffer[row * texPitch + col * 4 + 3];
	}

	pixelBuffer->unlock();

	lastUpdateTime = timer.getMilliseconds();
	needsForceRender = false;
}

void Navi::updateFade()
{
	if(isFading)
	{
		fadeValue += deltaFadePerMS * (timer.getMilliseconds() - lastFadeTimeMS);

		if(fadeValue > 1)
		{
			fadeValue = 1;
			isFading = false;
		}
		else if(fadeValue < 0)
		{
			fadeValue = 0;
			isFading = false;
			overlay->hide();
		}

		lastFadeTimeMS = timer.getMilliseconds();
	}
}

void Navi::resizeIfNeeded()
{
	if(!resizeParameters.first)
		return;

	int width = resizeParameters.first;
	int height = resizeParameters.second;

	resizeParameters.first = 0;
	resizeParameters.second = 0;

	if(width == naviWidth && height == naviHeight)
		return;

	naviWidth = width;
	naviHeight = height;

	int newTexWidth = naviWidth;
	int newTexHeight = naviHeight;

	if(!Bitwise::isPO2(naviWidth) || !Bitwise::isPO2(naviHeight))
	{
		if(Root::getSingleton().getRenderSystem()->getCapabilities()->hasCapability(RSC_NON_POWER_OF_2_TEXTURES))
		{
			if(Root::getSingleton().getRenderSystem()->getCapabilities()->getNonPOW2TexturesLimited())
				compensateNPOT = true;
		}
		else compensateNPOT = true;
		
		if(compensateNPOT)
		{
			newTexWidth = Bitwise::firstPO2From(naviWidth);
			newTexHeight = Bitwise::firstPO2From(naviHeight);
		}
	}

	overlay->resize(naviWidth, naviHeight);
	webView->resize(naviWidth, naviHeight);

	if(newTexWidth == texWidth && newTexHeight == texHeight)
		return;

	texWidth = newTexWidth;
	texHeight = newTexHeight;

	matPass->removeAllTextureUnitStates();
	maskTexUnit = 0;

	Ogre::TextureManager::getSingleton().remove(naviName + "Texture");

	TexturePtr texture = TextureManager::getSingleton().createManual(
		naviName + "Texture", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		TEX_TYPE_2D, texWidth, texHeight, 0, PF_BYTE_BGRA,
		TU_DYNAMIC_WRITE_ONLY_DISCARDABLE, this);

	HardwarePixelBufferSharedPtr pixelBuffer = texture->getBuffer();
	pixelBuffer->lock(HardwareBuffer::HBL_DISCARD);
	const PixelBox& pixelBox = pixelBuffer->getCurrentLock();
	texDepth = Ogre::PixelUtil::getNumElemBytes(pixelBox.format);
	texPitch = (pixelBox.rowPitch*texDepth);

	uint8* pDest = static_cast<uint8*>(pixelBox.data);

	memset(pDest, 128, texHeight*texPitch);

	pixelBuffer->unlock();

	baseTexUnit = matPass->createTextureUnitState(naviName + "Texture");
	
	baseTexUnit->setTextureFiltering(texFiltering, texFiltering, FO_NONE);
	if(texFiltering == FO_ANISOTROPIC)
		baseTexUnit->setTextureAnisotropy(4);

	if(usingMask)
	{
		setMask(maskImageParameters.first, maskImageParameters.second);
	}
	else if(alphaCache)
	{
		delete[] alphaCache;
		alphaCache = new unsigned char[texWidth * texHeight];
		alphaCachePitch = texWidth;
	}
}

bool Navi::isPointOverMe(int x, int y)
{
	if(isMaterialOnly())
		return false;
	if(!overlay->isVisible)
		return false;

	int localX = overlay->getRelativeX(x);
	int localY = overlay->getRelativeY(y);

	if(localX > 0 && localX < overlay->width)
		if(localY > 0 && localY < overlay->height)
			return !ignoringTrans || !alphaCache ? true : 
				alphaCache[localY * alphaCachePitch + localX] > 255 * transparent;

	return false;
}

void Navi::loadURL(const std::string& url)
{
	webView->loadURL(url);
}

void Navi::loadFile(const std::string& file)
{
	webView->loadFile(file);
}

void Navi::loadHTML(const std::string& html)
{
	webView->loadHTML(html);
}

void Navi::evaluateJS(const std::string& javascript, const Awesomium::JSArguments& args)
{
	if(!args.size())
	{
		webView->executeJavascript(javascript);
		return;
	}

	std::string resultScript;
	char paramName[15];
	unsigned int i, count;

	for(i = 0, count = 0; i < javascript.length(); i++)
	{
		if(javascript[i] == '?')
		{
			count++;
			if(count <= args.size())
			{
				sprintf(paramName, "__p00%d", count - 1);
				setProperty(paramName, args[count-1]);
				resultScript += "Client.";
				resultScript += paramName;
			}
			else
			{
				resultScript += "undefined";
			}
		}
		else
		{
			resultScript.push_back(javascript[i]);
		}
	}

	webView->executeJavascript(resultScript);
}

Awesomium::FutureJSValue Navi::evaluateJSWithResult(const std::string& javascript, const Awesomium::JSArguments& args)
{
	if(!args.size())
		return webView->executeJavascriptWithResult(javascript);
	
	std::string resultScript;
	char paramName[15];
	unsigned int i, count;

	for(i = 0, count = 0; i < javascript.length(); i++)
	{
		if(javascript[i] == '?')
		{
			count++;
			if(count <= args.size())
			{
				sprintf(paramName, "__p00%d", count - 1);
				setProperty(paramName, args[count-1]);
				resultScript += "Client.";
				resultScript += paramName;
			}
			else
			{
				resultScript += "undefined";
			}
		}
		else
		{
			resultScript.push_back(javascript[i]);
		}
	}

	return webView->executeJavascriptWithResult(resultScript);
}

void Navi::bind(const std::string& name, const NaviDelegate& callback)
{
	delegateMap[name] = callback;

	webView->setCallback(name);
}

void Navi::setProperty(const std::string& name, const Awesomium::JSValue& value)
{
	webView->setProperty(name, value);
}

void Navi::setTransparent(bool isTransparent)
{
	if(!isTransparent)
	{
		if(alphaCache && !usingMask)
		{
			delete[] alphaCache;
			alphaCache = 0;
		}
	}
	else
	{
		if(!alphaCache && !usingMask)
		{
			alphaCache = new unsigned char[texWidth * texHeight];
			alphaCachePitch = texWidth;
		}
	}

	webView->setTransparent(isTransparent);
	isWebViewTransparent = isTransparent;
}

void Navi::setIgnoreBounds(bool ignoreBounds)
{
	ignoringBounds = ignoreBounds;
}

void Navi::setIgnoreTransparent(bool ignoreTrans, float threshold)
{
	ignoringTrans = ignoreTrans;

	limit<float>(threshold, 0, 1);

	transparent = threshold;
}

void Navi::setMask(std::string maskFileName, std::string groupName)
{
	if(usingMask)
	{
		if(maskTexUnit)
		{
			matPass->removeTextureUnitState(1);
			maskTexUnit = 0;
		}

		if(!TextureManager::getSingleton().getByName(naviName + "MaskTexture").isNull())
			TextureManager::getSingleton().remove(naviName + "MaskTexture");
	}

	if(alphaCache)
	{
		delete[] alphaCache;
		alphaCache = 0;
	}

	if(maskFileName == "")
	{
		usingMask = false;
		maskImageParameters.first = "";
		maskImageParameters.second = "";

		if(isWebViewTransparent)
		{
			setTransparent(true);
			update();
		}

		return;
	}

	maskImageParameters.first = maskFileName;
	maskImageParameters.second = groupName;

	if(!maskTexUnit)
	{
		maskTexUnit = matPass->createTextureUnitState();
		maskTexUnit->setIsAlpha(true);
		maskTexUnit->setTextureFiltering(FO_NONE, FO_NONE, FO_NONE);
		maskTexUnit->setColourOperationEx(LBX_SOURCE1, LBS_CURRENT, LBS_CURRENT);
		maskTexUnit->setAlphaOperation(LBX_MODULATE);
	}
	
	Image srcImage;
	srcImage.load(maskFileName, groupName);

	Ogre::PixelBox srcPixels = srcImage.getPixelBox();
	unsigned char* conversionBuf = 0;
	
	if(srcImage.getFormat() != Ogre::PF_BYTE_A)
	{
		size_t dstBpp = Ogre::PixelUtil::getNumElemBytes(Ogre::PF_BYTE_A);
		conversionBuf = new unsigned char[srcImage.getWidth() * srcImage.getHeight() * dstBpp];
		Ogre::PixelBox convPixels(Ogre::Box(0, 0, srcImage.getWidth(), srcImage.getHeight()), Ogre::PF_BYTE_A, conversionBuf);
		Ogre::PixelUtil::bulkPixelConversion(srcImage.getPixelBox(), convPixels);
		srcPixels = convPixels;
	}

	TexturePtr maskTexture = TextureManager::getSingleton().createManual(
		naviName + "MaskTexture", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		TEX_TYPE_2D, texWidth, texHeight, 0, PF_BYTE_A, TU_STATIC_WRITE_ONLY);

	HardwarePixelBufferSharedPtr pixelBuffer = maskTexture->getBuffer();
	pixelBuffer->lock(HardwareBuffer::HBL_DISCARD);
	const PixelBox& pixelBox = pixelBuffer->getCurrentLock();
	size_t maskTexDepth = Ogre::PixelUtil::getNumElemBytes(pixelBox.format);
	alphaCachePitch = pixelBox.rowPitch;

	alphaCache = new unsigned char[alphaCachePitch*texHeight];

	uint8* buffer = static_cast<uint8*>(pixelBox.data);

	memset(buffer, 0, alphaCachePitch * texHeight);

	size_t minRowSpan = std::min(alphaCachePitch, srcPixels.rowPitch);
	size_t minHeight = std::min(texHeight, (unsigned short)srcPixels.getHeight());

	if(maskTexDepth == 1)
	{
		for(unsigned int row = 0; row < minHeight; row++)
			memcpy(buffer + row * alphaCachePitch, (unsigned char*)srcPixels.data + row * srcPixels.rowPitch, minRowSpan);

		memcpy(alphaCache, buffer, alphaCachePitch*texHeight);
	}
	else if(maskTexDepth == 4)
	{
		size_t destRowOffset, srcRowOffset, cacheRowOffset;

		for(unsigned int row = 0; row < minHeight; row++)
		{
			destRowOffset = row * alphaCachePitch * maskTexDepth;
			srcRowOffset = row * srcPixels.rowPitch;
			cacheRowOffset = row * alphaCachePitch;

			for(unsigned int col = 0; col < minRowSpan; col++)
				alphaCache[cacheRowOffset + col] = buffer[destRowOffset + col * maskTexDepth + 3] = ((unsigned char*)srcPixels.data)[srcRowOffset + col];
		}
	}
	else
	{
		OGRE_EXCEPT(Ogre::Exception::ERR_RT_ASSERTION_FAILED, 
			"Unexpected depth and format were encountered while creating a PF_BYTE_A HardwarePixelBuffer. Pixel format: " + 
			StringConverter::toString(pixelBox.format) + ", Depth:" + StringConverter::toString(maskTexDepth), "Navi::setMask");
	}

	pixelBuffer->unlock();

	if(conversionBuf)
		delete[] conversionBuf;

	maskTexUnit->setTextureName(naviName + "MaskTexture");
	usingMask = true;
}

void Navi::setMaxUPS(unsigned int maxUPS)
{
	maxUpdatePS = maxUPS;
}

void Navi::setMovable(bool isMovable)
{
	if(!isMaterialOnly())
		movable = isMovable;
}

void Navi::setEnableTooltips(bool isEnabled)
{
	tooltipsEnabled = isEnabled;

	if(!isEnabled)
		NaviManager::Get().handleTooltip(this, L"");
}

void Navi::setAlwaysReceivesKeyboard(bool isEnabled)
{
	alwaysReceivesKeyboard = isEnabled;
}

void Navi::setOpacity(float opacity)
{
	limit<float>(opacity, 0, 1);
	
	this->opacity = opacity;
}

void Navi::setPosition(const NaviPosition &naviPosition)
{
	if(overlay)
		overlay->setPosition(naviPosition);
}

void Navi::resetPosition()
{
	if(overlay)
		overlay->resetPosition();
}

void Navi::hide(bool fade, unsigned short fadeDurationMS)
{
	updateFade();

	if(fade)
	{
		isFading = true;
		deltaFadePerMS = -1 / (double)fadeDurationMS;
		lastFadeTimeMS = timer.getMilliseconds();
	}
	else
	{
		isFading = false;
		fadeValue = 0;
		overlay->hide();
	}
}

void Navi::show(bool fade, unsigned short fadeDurationMS)
{
	updateFade();

	if(fade)
	{
		isFading = true;
		deltaFadePerMS = 1 / (double)fadeDurationMS;
		lastFadeTimeMS = timer.getMilliseconds();
	}
	else
	{
		isFading = false;
		fadeValue = 1;
	}

	overlay->show();
}

void Navi::focus()
{
	if(overlay)
		NaviManager::GetPointer()->focusNavi(0, 0, this);
	else
		webView->focus();
}

void Navi::moveNavi(int deltaX, int deltaY)
{
	if(overlay)
		overlay->move(deltaX, deltaY);
}

void Navi::getExtents(unsigned short &width, unsigned short &height)
{
	width = naviWidth;
	height = naviHeight;
}

int Navi::getRelativeX(int absX)
{
	if(isMaterialOnly())
		return 0;
	else
		return overlay->getRelativeX(absX);
}

int Navi::getRelativeY(int absY)
{
	if(isMaterialOnly())
		return 0;
	else
		return overlay->getRelativeY(absY);
}

bool Navi::isMaterialOnly()
{
	return !overlay;
}

NaviOverlay* Navi::getOverlay()
{
	return overlay;
}

std::string Navi::getName()
{
	return naviName;
}

std::string Navi::getMaterialName()
{
	return naviName + "Material";
}

bool Navi::getVisibility()
{
	if(isMaterialOnly())
		return fadeValue != 0;
	else
		return overlay->isVisible;
}

void Navi::getDerivedUV(Ogre::Real& u1, Ogre::Real& v1, Ogre::Real& u2, Ogre::Real& v2)
{
	u1 = v1 = 0;
	u2 = v2 = 1;

	if(compensateNPOT)
	{
		u2 = (Ogre::Real)naviWidth/texWidth;
		v2 = (Ogre::Real)naviHeight/(Ogre::Real)texHeight;
	}
}

void Navi::injectMouseMove(int xPos, int yPos)
{
	webView->injectMouseMove(xPos, yPos);
}

void Navi::injectMouseWheel(int relScroll)
{
	webView->injectMouseWheel(relScroll);
}

void Navi::injectMouseDown(int xPos, int yPos)
{
	if(hasInternalKeyboardFocus)
		NaviManager::Get().handleKeyboardFocusChange(this, true);

	webView->injectMouseDown(Awesomium::LEFT_MOUSE_BTN);
}

void Navi::injectMouseUp(int xPos, int yPos)
{
	webView->injectMouseUp(Awesomium::LEFT_MOUSE_BTN);
}

void Navi::captureImage(const std::string& filename)
{
	Ogre::Image result;

	int bpp = isWebViewTransparent? 4 : 3;

	unsigned char* buffer = OGRE_ALLOC_T(unsigned char, naviWidth * naviHeight * bpp, Ogre::MEMCATEGORY_GENERAL);

	webView->render(buffer, naviWidth * bpp, bpp);

	result.loadDynamicImage(buffer, naviWidth, naviHeight, 1, isWebViewTransparent? Ogre::PF_BYTE_BGRA : Ogre::PF_BYTE_BGR, false);
	result.save(Awesomium::WebCore::Get().getBaseDirectory() + "\\" + filename);

	OGRE_FREE(buffer, Ogre::MEMCATEGORY_GENERAL);
}

void Navi::resize(int width, int height)
{
	resizeParameters.first = width;
	resizeParameters.second = height;
}

void Navi::zoomIn()
{
	webView->zoomIn();
}

void Navi::zoomOut()
{
	webView->zoomOut();
}

void Navi::resetZoom()
{
	webView->resetZoom();
}

Navi* Navi::addEventListener(NaviEventListener* newListener)
{
    if(newListener)
    {
        for(std::vector<NaviEventListener*>::iterator i = eventListeners.begin(); i != eventListeners.end(); ++i)
            if(*i == newListener) return this;

        eventListeners.push_back(newListener);
    }

    return this;
}

Navi* Navi::removeEventListener(NaviEventListener* removeListener)
{
    for(std::vector<NaviEventListener*>::iterator i = eventListeners.begin(); i != eventListeners.end();)
    {
        if(*i == removeListener)
            i = eventListeners.erase(i);
        else
            ++i;
    }

    return this;
}

void Navi::onBeginNavigation(const std::string& url, const std::wstring& frameName)
{
}

void Navi::onBeginLoading(const std::string& url, const std::wstring& frameName, int statusCode, const std::wstring& mimeType)
{

    loadingUrl = url;
    statusCode = statusCode;
}

void Navi::onFinishLoading()
{
    for(std::vector<NaviEventListener*>::const_iterator nel = eventListeners.begin(); nel != eventListeners.end(); ++nel)
        (*nel)->onNavigateComplete(this, loadingUrl,statusCode);
}

void Navi::onCallback(const std::string& name, const Awesomium::JSArguments& args)
{
	std::map<std::string, NaviDelegate>::iterator i = delegateMap.find(name);

	if(i != delegateMap.end())
		i->second(this, args);


    for(std::vector<NaviEventListener*>::const_iterator nel = eventListeners.begin(); nel != eventListeners.end(); ++nel)
        (*nel)->onCallback(name, args);
}

void Navi::onReceiveTitle(const std::wstring& title, const std::wstring& frameName)
{
 
}


void Navi::onChangeTargetURL(const std::string& url)
{
    for(std::vector<NaviEventListener*>::const_iterator nel = eventListeners.begin(); nel != eventListeners.end(); ++nel)
        (*nel)->onLocationChange(this, url);
}

void Navi::onChangeTooltip(const std::wstring& tooltip)
{
	if(tooltipsEnabled)
		NaviManager::Get().handleTooltip(this, tooltip);
}

#if defined(_WIN32)
void Navi::onChangeCursor(const HCURSOR& cursor)
{
}
#endif

void Navi::onChangeKeyboardFocus(bool isFocused)
{
	NaviManager::Get().handleKeyboardFocusChange(this, isFocused);
	hasInternalKeyboardFocus = isFocused;
}

void Navi::onRequestDrag(Navi *caller, const Awesomium::JSArguments &args)
{
	if(overlay)
		NaviManager::Get().handleRequestDrag(this);
}