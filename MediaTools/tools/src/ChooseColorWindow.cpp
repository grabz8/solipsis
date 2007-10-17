#include ".\choosecolorwindow.h"

bool CChooseColorWindow::mUseTargetCursor;

CChooseColorWindow::CChooseColorWindow(CEGUI::Window* pWindow, const String& pTextureName, CEGUI::OgreCEGUIRenderer* pCEGUIRenderer):
	mWindow(pWindow)
{
	mUseTargetCursor = false ;
	mMouseButtonDown = false ;
	mColourImageWindow = mWindow->getChild("FrmChooseColor/ColourImage");
	mColourSample = mWindow->getChild("FrmChooseColor/ColorSample");

	mRedScrollBar = (CEGUI::Scrollbar*)mWindow->getChild("FrmChooseColor/RedScrollBar");
	mGreenScrollBar = (CEGUI::Scrollbar*)mWindow->getChild("FrmChooseColor/GreenScrollBar");
	mBlueScrollBar = (CEGUI::Scrollbar*)mWindow->getChild("FrmChooseColor/BlueScrollBar");


	//Loading the texture and fit in to the ColourImage
	mColourImage = TextureManager::getSingleton().load(pTextureName,ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	CEGUI::Texture* colourImageTexture = pCEGUIRenderer->createTexture(pTextureName);
	mColourImageset = CEGUI::ImagesetManager::getSingleton().createImageset("ImageSetColourImage"+pTextureName,colourImageTexture);
	mColourImageset->defineImage("ColourImage"+pTextureName, 
                CEGUI::Point(0, 0),
                CEGUI::Size(colourImageTexture->getWidth(),colourImageTexture->getHeight()),
                CEGUI::Point(0,0));
	mColourImageWindow->setProperty("Image", CEGUI::PropertyHelper::imageToString(
		&mColourImageset->getImage("ColourImage"+pTextureName)));

	//Events whith the mouse :
	mColourImageWindow->subscribeEvent(
		CEGUI::Window::EventMouseButtonDown,
		CEGUI::Event::Subscriber(&CChooseColorWindow::handleMouseButtonDown, this));
	mColourImageWindow->subscribeEvent(
		CEGUI::Window::EventMouseButtonUp,
		CEGUI::Event::Subscriber(&CChooseColorWindow::handleMouseButtonUp, this));
	mColourImageWindow->subscribeEvent(
		CEGUI::Window::EventMouseEnters,
		CEGUI::Event::Subscriber(&CChooseColorWindow::handleMouseEnters, this));
	mColourImageWindow->subscribeEvent(
		CEGUI::Window::EventMouseLeaves,
		CEGUI::Event::Subscriber(&CChooseColorWindow::handleMouseLeaves, this));
	mColourImageWindow->subscribeEvent(
		CEGUI::Window::EventMouseMove,
		CEGUI::Event::Subscriber(&CChooseColorWindow::handleMouseMove, this));

	//Events with scroll bars :
	mRedScrollBar->subscribeEvent(
		CEGUI::Scrollbar::EventScrollPositionChanged,
		CEGUI::Event::Subscriber(&CChooseColorWindow::HandleScrollBarChanged, this) );
	mGreenScrollBar->subscribeEvent(
		CEGUI::Scrollbar::EventScrollPositionChanged,
		CEGUI::Event::Subscriber(&CChooseColorWindow::HandleScrollBarChanged, this) );
	mBlueScrollBar->subscribeEvent(
		CEGUI::Scrollbar::EventScrollPositionChanged,
		CEGUI::Event::Subscriber(&CChooseColorWindow::HandleScrollBarChanged, this) );

}
//-------------------------------------------------------------------------------------
CChooseColorWindow::~CChooseColorWindow(void)
{
	mColourImageset->undefineAllImages();
	CEGUI::ImagesetManager::getSingleton().destroyImageset(mColourImageset);
	mColourImage->unload();
}
//-------------------------------------------------------------------------------------
void CChooseColorWindow::showFrame(bool pShow)
{
	mWindow->setVisible( pShow );
}
//-------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------------------------------------------------------
bool CChooseColorWindow::handleMouseButtonDown(const CEGUI::EventArgs& e)
{
	mMouseButtonDown = true;
    
	CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition(); 


	//getting the index of the texel of the texture the user has clicked on
	CEGUI::Rect rect = mColourImageWindow->getPixelRect();
	CEGUI::Rect rectWindow = mWindow->getPixelRect();

	float relativeXClickedColourImagePixel = (mousePos.d_x - rect.d_left) / rect.getWidth();
	if (relativeXClickedColourImagePixel < 0) relativeXClickedColourImagePixel = 0;
	if (relativeXClickedColourImagePixel > 1) relativeXClickedColourImagePixel = 1;

	float relativeYClickedColourImagePixel =  (mousePos.d_y - rect.d_top) / rect.getWidth();
	if (relativeYClickedColourImagePixel < 0) relativeYClickedColourImagePixel = 0;
	if (relativeYClickedColourImagePixel > 1) relativeYClickedColourImagePixel = 1;

	size_t xTexel = (size_t)(relativeXClickedColourImagePixel * (mColourImage->getWidth()-1)  +  0.5f);
	size_t yTexel = (size_t)(relativeYClickedColourImagePixel * (mColourImage->getHeight()-1)  +  0.5f);
	size_t indexTexel = yTexel * mColourImage->getWidth()  +  xTexel;


	//then getting the colour of the texel whose we just found the index.
	HardwarePixelBufferSharedPtr pixelBuffer = mColourImage->getBuffer();
	uchar* textureData;
	PixelBox pixelBox(mColourImage->getWidth(),
		mColourImage->getHeight(), 
		mColourImage->getDepth(), 
		mColourImage->getFormat(), 
		textureData = new uchar[ 4 * mColourImage->getWidth() * mColourImage->getHeight() ]);
	pixelBuffer->blitToMemory(pixelBox);


	ColourValue texelColour(textureData[4*indexTexel + 2]/255.0f,textureData[4*indexTexel + 1]/255.0f,textureData[4*indexTexel]/255.0f,1.0f);

	delete textureData;

	//Set color in scroll bars :
	setColor( Vector3 (texelColour.r, texelColour.g, texelColour.b) );

	//finally, set the modifiablematerialobject's colour with the colour found
	mColourSample->setProperty("ImageColours",
		CEGUI::PropertyHelper::colourToString(CEGUI::colour(
		texelColour.r,
		texelColour.g,
		texelColour.b)));
 	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
bool CChooseColorWindow::handleMouseButtonUp( const CEGUI::EventArgs& e)
{
	mMouseButtonDown = false;
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
bool CChooseColorWindow::handleMouseEnters( const CEGUI::EventArgs& e)
{
	mUseTargetCursor = true;
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
bool CChooseColorWindow::handleMouseLeaves( const CEGUI::EventArgs& e)
{
	mUseTargetCursor = false;
	return true;
}

//--------------------------------------------------------------------------------------------------------------------------------------------------
bool CChooseColorWindow::handleMouseMove( const CEGUI::EventArgs& e)
{
	if (mMouseButtonDown) return handleMouseButtonDown(e);
	return true;
}

Vector3 CChooseColorWindow::getColor()
{
	Vector3 tmp = Vector3::ZERO ;
	
	tmp.x = mRedScrollBar->getScrollPosition() ;
	tmp.y = mGreenScrollBar->getScrollPosition() ;
	tmp.z = mBlueScrollBar->getScrollPosition() ;

	return tmp ;
}

void CChooseColorWindow::setColor( Vector3 pColor) 
{
	mRedScrollBar->setScrollPosition( pColor.x ) ;
	mGreenScrollBar->setScrollPosition( pColor.y ) ;
	mBlueScrollBar->setScrollPosition( pColor.z ) ;
}

bool CChooseColorWindow::HandleScrollBarChanged( const CEGUI::EventArgs& e)
{
	Vector3 tmp (mRedScrollBar->getScrollPosition(),mGreenScrollBar->getScrollPosition(),mBlueScrollBar->getScrollPosition());
	mColourSample->setProperty("ImageColours",
		CEGUI::PropertyHelper::colourToString(CEGUI::colour(
		tmp.x,
		tmp.y,
		tmp.z)));
	return true ;
}

