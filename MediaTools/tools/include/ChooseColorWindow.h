#ifndef __CCHOOSECOLORWINDOW_h__
#define __CCHOOSECOLORWINDOW_h__

#include <CEGUI/CEGUIWindow.h>
#include <CEGUI/CEGUIImageset.h>
#include <OgreCEGUIRenderer.h>
#include <CEGUI/CEGUIPropertyHelper.h>
#include <CEGUI/elements/CEGUIScrollbar.h>
#include <CEGUI/elements/CEGUIPushButton.h>
#include <Ogre.h>

#include <OIS/OIS.h>

using namespace Ogre;

class CChooseColorWindow
{
public:
	///brief Constructor
	///param pWindow Choose color window
	///param ptextureName Name of the Ogre texture used to make the ColourImage.
	///param pCEGUIRenderer CEGUI Renderer used to create the CEGUI::Texture for the colour image.
			///param modifiableMaterialObject ModifiableMaterialObject to which we have to change the colour in case of colour picking on the ColourImage.
			///param redScrollBar Red scrollbar associated to the colour of the Material to be modified. Scrollbar position will be changed if a colour is picked.
			///param greenScrollBar Green scrollbar associated to the colour of the Material to be modified. Scrollbar position will be changed if a colour is picked.
			///param blueScrollBar Blue scrollbar associated to the colour of the Material to be modified. Scrollbar position will be changed if a colour is picked.
			///param colourSample Colour Sample associated to the colour of the Material to be modified, will be changed if a colour is picked.
			///param guiTemplate GUITemplate which is used to build and design the window.
	CChooseColorWindow(CEGUI::Window* pWindow, const String& pTextureName, CEGUI::OgreCEGUIRenderer* pCEGUIRenderer);
	~CChooseColorWindow(void);

	///brief Handle executed everytime the user push a button of the mouse on the ColourImage, set the colour of the ModifiableMaterialObject as the colour picked by the user on the ColourImage. It also changes the colours scrollbars and the colour sample in consequences.
	bool handleMouseButtonDown( const CEGUI::EventArgs& e);

	///brief Handle executed everytime the user free a button of the mouse on the ColourImage. It informs the class that the user isn't clicking anymore and so stop picking a colour.
	bool handleMouseButtonUp( const CEGUI::EventArgs& e);

	///brief Handle executed everytime the cursor comes to the ColourImage, set the Target Cursor.
	bool handleMouseEnters( const CEGUI::EventArgs& e);

	///brief Handle executed everytime the cursor comes out from the ColourImage, set the Arrow Cursor.
	bool handleMouseLeaves( const CEGUI::EventArgs& e);

	///brief Handle executed everytime the cursor moves, still in the ColourImage. (check if a mouse button is down, if true a colours is picking).
	bool handleMouseMove( const CEGUI::EventArgs& e);

	///brief Handle executed everytime a scroll bar changes
	bool HandleScrollBarChanged( const CEGUI::EventArgs& e);

	///brief Show or hide the window to choose colors
	void showFrame(bool pShow);

	///brief Get the color choosen by the user
	///brief	Use a Vector3 with (x;y;z) corresponding to (R;G;B) and (x;y;z) between 0 and 1 ;
	Vector3 getColor();

	///brief Set the color in the sample box. You may use it when you show this window to set the first color
	///brief	Use a Vector3 with (x;y;z) corresponding to (R;G;B) and (x;y;z) between 0 and 1 ;
	void setColor( Vector3 pColor) ;


	static bool mUseTargetCursor;

private:

	CEGUI::Window* mWindow;			///brief CEGUI::Window representing the ColourWindow on the screen.

	CEGUI::Window* mColourImageWindow;						///brief CEGUI::Window representing the ColourImage Window in the ColourWindow.
	TexturePtr mColourImage;								///brief Ogre texture of the ColourImage.
	CEGUI::Imageset* mColourImageset;						///brief CEGUI::Imageset containing the image representing the colour image displayed on the colour window. 
	CEGUI::Window* mColourSample;							///brief Colour sample associated to the colour of the Material to be modified, will be changed if a colour is picked.

	CEGUI::Scrollbar* mRedScrollBar ;							///brief Red scroll bar associated to the colour of the Material to be modified, will be changed if a colour is picked.
	CEGUI::Scrollbar* mGreenScrollBar ;						///brief Red scroll bar associated to the colour of the Material to be modified, will be changed if a colour is picked.
	CEGUI::Scrollbar* mBlueScrollBar ;							///brief Red scroll bar associated to the colour of the Material to be modified, will be changed if a colour is picked.

	bool mMouseButtonDown;									///brief Tells whether or not a button of the mouse is down. If it is true and the cursor is on the the ColourImage a colour is  picking and applied to the ModifiableMaterialObject.

};


#endif