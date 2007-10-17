#ifndef __ModuleGUI_h__
#define __ModuleGUI_h__

//mem probs without this next one
#include <OgreNoMemoryMacros.h>
#include <CEGUI/CEGUIImageset.h>
#include <CEGUI/CEGUISystem.h>
#include <CEGUI/CEGUILogger.h>
#include <CEGUI/CEGUISchemeManager.h>
#include <CEGUI/CEGUIWindowManager.h>
#include <CEGUI/CEGUIWindow.h>
#include <CEGUI/CEGUIPropertyHelper.h>
#include <CEGUI/CEGUICoordConverter.h>
#include <CEGUI/elements/CEGUICombobox.h>
#include <CEGUI/elements/CEGUIComboDropList.h>
#include <CEGUI/elements/CEGUICheckbox.h>
#include <CEGUI/elements/CEGUIListbox.h>
#include <CEGUI/elements/CEGUIListboxTextItem.h>
#include <CEGUI/elements/CEGUIPushButton.h>
#include <CEGUI/elements/CEGUIScrollbar.h>
#include <CEGUI/elements/CEGUIEditBox.h>
#include <CEGUI/elements/CEGUIFrameWindow.h>
#include <CEGUI/elements/CEGUIMenuItem.h>
#include <CEGUI/elements/CEGUIMenuBar.h>
#include <CEGUI/elements/CEGUIPopupMenu.h>
#include <OgreCEGUIRenderer.h>
#include <OgreCEGUIResourceProvider.h>
//regular mem handler
#include <OgreMemoryMacros.h>
#include <Ogre.h>

//Use this define to signify OIS will be used as a DLL
//(so that dll import/export macros are in effect)
#define OIS_DYNAMIC_LIB
#include <OIS/OIS.h>

using namespace Ogre;

//-------------------------------------------------------------------------------------
static CEGUI::MouseButton convertOgreButtonToCegui(int buttonID)
{
	using namespace OIS; 

	switch (buttonID)
	{
	case OIS::MB_Left:
		return CEGUI::LeftButton;
	case OIS::MB_Right:
		return CEGUI::RightButton;
	case OIS::MB_Middle:
		return CEGUI::MiddleButton;
		// Not sure what to do with this one...
		//   case MouseEvent::BUTTON3_MASK:
		//       return CEGUI::X1Button;
	default:
		return CEGUI::LeftButton;
	}
}

//-------------------------------------------------------------------------------------
class ModuleGUI
//	: public OIS::MouseListener, public OIS::KeyListener
{
public:
	ModuleGUI(SceneManager* pSceneMgr, Camera* pCamera);
	~ModuleGUI();

protected:
	virtual bool createGUI( RenderWindow* pWindow );
	virtual bool setupEventHandlers(void);
	virtual bool handleQuit(const CEGUI::EventArgs& e);		// this is juste an example as handle !

	// MouseListener
	virtual bool mouseMoved( const OIS::MouseEvent &e );
	virtual bool mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );
	virtual bool mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );
	// KeyListener
	virtual bool keyPressed( const OIS::KeyEvent &e );
	virtual bool keyReleased( const OIS::KeyEvent &e );

protected:
	SceneManager* mSceneMgr;
	Camera* mCamera;

	CEGUI::OgreCEGUIRenderer* mGUIRenderer;
	CEGUI::System* mGUISystem;
	CEGUI::Window* mEditorGuiSheet;
};

#endif //__ModuleGUI_h__