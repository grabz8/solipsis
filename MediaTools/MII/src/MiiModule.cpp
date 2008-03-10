#include "MiiModule.h"
#include "Object3D.h"
#include "Selection.h"
#include "Transformations.h"
#include "ChooseColorWindow.h"
#include "ModifiedMaterialManager.h"

#include "SolipsisErrorHandler.h"
#include "FileBrowser.h"
#include "Path.h"

// Tinyxml
#include "tinyxml.h"

// Directory management
#include <direct.h>
// For keyCodes
#include <OIS/OIS.h>
// Zip Management
#include "FileBuffer.h"
#include "MyZipArchive.h"

//-------------------------------------------------------------------------------------
MiiModule::MiiModule(SceneManager* pSceneMgr, Camera* pCamera,Selection *pSel) : 
	ModuleGUI(pSceneMgr, pCamera)
{
	MaterialPtr material = MaterialManager::getSingleton().create("matRed","debugger"); 
	material->setReceiveShadows(false); 
	material->getTechnique(0)->setLightingEnabled(true); 
	material->getTechnique(0)->getPass(0)->setDiffuse(1,0,0,0); 
	material->getTechnique(0)->getPass(0)->setAmbient(1,0,0); 
	material->getTechnique(0)->getPass(0)->setSelfIllumination(1,0,0); 

	material = MaterialManager::getSingleton().create("matGreen","debugger"); 
	material->setReceiveShadows(false); 
	material->getTechnique(0)->setLightingEnabled(true); 
	material->getTechnique(0)->getPass(0)->setDiffuse(0,1,0,0); 
	material->getTechnique(0)->getPass(0)->setAmbient(0,1,0); 
	material->getTechnique(0)->getPass(0)->setSelfIllumination(0,1,0); 

	material = MaterialManager::getSingleton().create("matBlue","debugger"); 
	material->setReceiveShadows(false); 
	material->getTechnique(0)->setLightingEnabled(true); 
	material->getTechnique(0)->getPass(0)->setDiffuse(0,0,1,0); 
	material->getTechnique(0)->getPass(0)->setAmbient(0,0,1); 
	material->getTechnique(0)->getPass(0)->setSelfIllumination(0,0,1); 
 
	material = MaterialManager::getSingleton().create("matRedNoDepth","debugger"); 
	material->setReceiveShadows(false); 
	material->getTechnique(0)->setLightingEnabled(true); 
	material->getTechnique(0)->getPass(0)->setDiffuse(1,0,0,0); 
	material->getTechnique(0)->getPass(0)->setAmbient(1,0,0); 
	material->getTechnique(0)->getPass(0)->setSelfIllumination(1,0,0); 
	material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);

	material = MaterialManager::getSingleton().create("matGreenNoDepth","debugger"); 
	material->setReceiveShadows(false); 
	material->getTechnique(0)->setLightingEnabled(true); 
	material->getTechnique(0)->getPass(0)->setDiffuse(0,1,0,0); 
	material->getTechnique(0)->getPass(0)->setAmbient(0,1,0); 
	material->getTechnique(0)->getPass(0)->setSelfIllumination(0,1,0); 
	material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);

	material = MaterialManager::getSingleton().create("matBlueNoDepth","debugger"); 
	material->setReceiveShadows(false); 
	material->getTechnique(0)->setLightingEnabled(true); 
	material->getTechnique(0)->getPass(0)->setDiffuse(0,0,1,0); 
	material->getTechnique(0)->getPass(0)->setAmbient(0,0,1); 
	material->getTechnique(0)->getPass(0)->setSelfIllumination(0,0,1);
	material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);

	mSelection = pSel;

	mGenericBox = mSceneMgr->createEntity( "GenericBox", "Box.mesh" );
	mGenericPrism = mSceneMgr->createEntity( "GenericPrism", "Prism.mesh" );
	mGenericCylinder = mSceneMgr->createEntity( "GenericCylinder", "Cylinder.mesh" );
	mGenericSphere = mSceneMgr->createEntity( "GenericSphere", "Sphere.mesh" );
	mGenericTorus = mSceneMgr->createEntity( "GenericTorus", "Torus.mesh" );
	mGenericTube = mSceneMgr->createEntity( "GenericTube", "Tube.mesh" );
	mGenericRing = mSceneMgr->createEntity( "GenericRing", "Ring.mesh" );

	mModifiedColor = ModifiedColor::NONE ;
	mPrecedentVerticalScrollPosition = 0;

	mExecPath = _getcwd(NULL, 0);
	SOLIPSISINFO("Current working directory is : ",mExecPath.c_str());
	mModeLink = false ;

	mTransfoButton.clear();

	mNeedHandle = true;
}

//-------------------------------------------------------------------------------------
MiiModule::~MiiModule() 
{
	mSceneMgr->destroyAllEntities();
}

//-------------------------------------------------------------------------------------
void MiiModule::configureMenu(CEGUI::Window* pParent, const bool& pMenubar) 
{
	// Recursively subscribe every menu item to the mouse enters/leaves/clicked events
	size_t childCount = pParent->getChildCount(); 
	for(size_t childIdx = 0; childIdx < childCount; childIdx++) 
	{ 
		if(pParent->getChildAtIdx(childIdx)->testClassName("MenuItem")
			|| pParent->getChildAtIdx(childIdx)->testClassName("PopupMenu") )
		{ 
			pParent->getChildAtIdx(childIdx)->subscribeEvent(CEGUI::MenuItem::EventMouseEnters,   CEGUI::Event::Subscriber(&MiiModule::onMouseEntersMenuItem, this)); 
			pParent->getChildAtIdx(childIdx)->subscribeEvent(CEGUI::MenuItem::EventMouseLeaves,   pMenubar ?	CEGUI::Event::Subscriber(&MiiModule::onMouseLeavesMenuItem, this)
																											:	CEGUI::Event::Subscriber(&MiiModule::onMouseLeavesPopupMenuItem, this)); 

		} 
		configureMenu(pParent->getChildAtIdx(childIdx), pMenubar); 
	} 
} 

//-------------------------------------------------------------------
bool MiiModule::onMouseEntersMenuItem(const CEGUI::EventArgs& e)
{
	const CEGUI::WindowEventArgs& we = static_cast<const CEGUI::WindowEventArgs&>(e);
	CEGUI::MenuItem* menuItem = static_cast<CEGUI::MenuItem*>(we.window);

	// Open or close a submenu
	if(menuItem)
	{
		if( menuItem->getPopupMenu() )
		{
			if( menuItem->isOpened() )
			{
				if(menuItem->testClassName("MenuItem"))
				{
					menuItem->closePopupMenu();
				}
			}
			else
			{
				menuItem->openPopupMenu();
			}
		}
	}
	return true;
}

//-------------------------------------------------------------------
bool MiiModule::onMouseLeavesMenuItem(const CEGUI::EventArgs& e)
{
	// Close a menu
	const CEGUI::WindowEventArgs& we = static_cast<const CEGUI::WindowEventArgs&>(e);

	CEGUI::Window* menubar = we.window;
	while(menubar)
	{
		if( menubar->testClassName("Menubar") )
		{
			// We found the root; a menu bar
			CEGUI::Vector2 childPosition = CEGUI::MouseCursor::getSingleton().getPosition();
			CEGUI::Window* windowUnderTheCursor = menubar->getTargetChildAtPosition(childPosition);
			if(!windowUnderTheCursor)
			{
				CEGUI::MenuItem* popupMenu = static_cast<CEGUI::Menubar*>(menubar)->getPopupMenuItem();
				if(popupMenu)
				{
					// This does not close sub-popup menus, only the current one
					popupMenu->closePopupMenu();
				}
			}
			break;
		}
		menubar = menubar->getParent();
	}

	return true;
}

//-------------------------------------------------------------------
bool MiiModule::onMouseLeavesPopupMenuItem(const CEGUI::EventArgs& e)
{
	// Close a popup menu
	const CEGUI::WindowEventArgs& we = static_cast<const CEGUI::WindowEventArgs&>(e);

	CEGUI::Window* menubar = we.window;
	CEGUI::Window* popupParent;
	while(menubar)
	{
		popupParent = menubar->getParent();
		if( popupParent
			&& !popupParent->testClassName("Menubar")
			&& !popupParent->testClassName("PopupMenu")
			&& !popupParent->testClassName("MenuItem")
			)
		{
			// We found the root; a popup menu
			CEGUI::Window* popupMenu = menubar;
			menubar = menubar->getParent();
			CEGUI::Vector2 childPosition = CEGUI::MouseCursor::getSingleton().getPosition();
			CEGUI::Window* windowUnderTheCursor = menubar->getTargetChildAtPosition(childPosition);
			if(!windowUnderTheCursor)
			{
				popupMenu->hide();
			}
			break;
		}
		menubar = menubar->getParent();
	}

	return true;
}

//-------------------------------------------------------------------
bool MiiModule::onMenuKey(const CEGUI::EventArgs& e)
{
	// First : test if the properties tab is opened => do nothing if it is !! 
	if (sheet->getChild("FrmProperties")->isVisible())
		return true;


	// Open or close a menu
	const CEGUI::KeyEventArgs& ke = static_cast<const CEGUI::KeyEventArgs&>(e);
	if(ke.sysKeys == CEGUI::Key::LeftAlt
		|| ke.sysKeys == CEGUI::Key::RightAlt
		|| ke.sysKeys == CEGUI::Key::Slash
		|| ke.codepoint == 47) // Slash
	{
		CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
		CEGUI::Menubar* menuBar = static_cast<CEGUI::Menubar*>(winMgr.getWindow("Root/FrameWindow/Menubar"));
		CEGUI::MenuItem* menuItem = menuBar->getPopupMenuItem();
		if(menuItem)
		{
			// There's a menu opened, let's close it
			menuBar->changePopupMenuItem(0);
		}
		else
		{
			// The menu is closed, let's open the first menu
			menuItem = static_cast<CEGUI::MenuItem*>(winMgr.getWindow("Root/FrameWindow/Menubar/File"));
			menuBar->changePopupMenuItem(menuItem);

			// Select the first item from this menu ??
		}
	}

	return true;
}

//-------------------------------------------------------------------
bool MiiModule::onPopupMenu(const CEGUI::EventArgs& e)
{
	// First : test if the properties tab is opened => do nothing if it is !! 
	if (sheet->getChild("FrmProperties")->isVisible())
		return true;

	// Open a context-sensitive menu
	const CEGUI::MouseEventArgs& me = static_cast<const CEGUI::MouseEventArgs&>(e);
	CEGUI::WindowManager& winMgr = CEGUI::WindowManager::getSingleton();
	CEGUI::PopupMenu* popupMenu = 0;
	popupMenu = static_cast<CEGUI::PopupMenu*>(winMgr.getWindow("Root/Popup/PopupMenus/SheetPopup/AutoPopup"));

	if(me.button == CEGUI::RightButton && (!mSelection->isEmpty()))
	{
		String windowName = me.window->getName().c_str();
		
		// Make the window the popup's parent
		me.window->addChildWindow(popupMenu);

		// Position of the popup menu
		CEGUI::UVector2 popupPosition;

		// Ensure the popup menu will be fully displayed horizontally on the screen
		float screenWidth = CEGUI::System::getSingleton().getRenderer()->getWidth();
		float popupMenuWidth = popupMenu->getWidth().d_offset;
		if(popupMenuWidth < screenWidth)
		{
			// The x coordinate is either the mouse position or the right side of the
			// screen minus the width of the popup menu.  This ensures that the popup
			// menu is fully displayed rather than clipped.
			float x;
			//if(mouseToLeftOfPopupMenu)
			//{
				x = me.position.d_x + popupMenuWidth > screenWidth ? screenWidth - popupMenuWidth
																	: me.position.d_x - /*mouseOffset*/ 5.0f;
			//}
			//else
			//{
			//	x = me.position.d_x < popupMenuWidth ? 0.0f
			//											: me.position.d_x - popupMenuWidth + mouseOffset;
			//}
			popupPosition.d_x = cegui_absdim(CEGUI::CoordConverter::screenToWindowX(*me.window, x));
		}
		else
		{
			// The popup menu is too wide to fit within the screen
			popupPosition.d_x = CEGUI::UDim(0.0f, 0.0f);
		}

		// Ensure the popup menu will be fully displayed vertically on the screen
		float screenHeight = CEGUI::System::getSingleton().getRenderer()->getHeight();
		float popupMenuHeight = popupMenu->getHeight().d_offset;
		if(popupMenuHeight < screenHeight)
		{
			// The y coordinate is either the mouse position or the bottom side of the
			// screen minus the height of the popup menu.  This ensures that the popup
			// menu is fully displayed rather than clipped.
			float y = me.position.d_y + popupMenuHeight > screenHeight ? screenHeight - popupMenuHeight
																		: me.position.d_y - /*mouseOffset*/ 5.0f;
			popupPosition.d_y = cegui_absdim(CEGUI::CoordConverter::screenToWindowY(*me.window, y));
		}
		else
		{
			// The popup menu is too tall to fit within the screen
			popupPosition.d_y = CEGUI::UDim(0.0f, 0.0f);
		}

		// Position the context menu
		popupMenu->setPosition(popupPosition);

		// Show the popup menu
		popupMenu->show();

		// Ensure it appears on top of the other widgets
		popupMenu->moveToFront();
	}
	else
	{
		// Show the popup menu
		popupMenu->hide();
		return false;
	}
	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::createGUI( RenderWindow* pWindow )
{
	// Set up GUI system
	mGUIRenderer = new CEGUI::OgreCEGUIRenderer(pWindow, Ogre::RENDER_QUEUE_OVERLAY, false, 3000, mSceneMgr);
	mGUISystem = new CEGUI::System(mGUIRenderer);

	// This is where your CEGUI log is created. It is currently set to Informative. There are four settings: Standard, Errors, Informative and Insane.
	CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);

	// Creates a new CEGUI System, using the 'Taharez Look' for the scheme and mouse cursor, with 'BlueHighway-12' for the font.
	CEGUI::SchemeManager::getSingleton().loadScheme((CEGUI::utf8*)"TaharezLookSkin.scheme");
	mGUISystem->setDefaultMouseCursor((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow");
	mGUISystem->setDefaultFont((CEGUI::utf8*)"BlueHighway-12");

	sheet = CEGUI::WindowManager::getSingleton().loadWindowLayout((CEGUI::utf8*)"mii.xml"); 
	mGUISystem->setGUISheet(sheet);

	configureMenu( CEGUI::WindowManager::getSingleton().getWindow("Root/FrameWindow/Menubar"), true );
	CEGUI::WindowManager::getSingleton().getWindow("Root/FrameWindow/Menubar")->show();

	sheet->getChild("FrmProperties")->setVisible(false);
	sheet->getChild("Root/Popup")->setVisible(false);

	mChooseColorWindow = new CChooseColorWindow( sheet->getChild("FrmChooseColor"), "colours.bmp", mGUIRenderer) ;
	mChooseColorWindow->showFrame(false) ;

	CEGUI::Listbox *lst = (CEGUI::Listbox *)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page4/ListInfos");
	lst->addItem(new CEGUI::ListboxTextItem("Test Item 1"));
	lst->addItem(new CEGUI::ListboxTextItem("Test Item 2"));
	lst->addItem(new CEGUI::ListboxTextItem("Test Item 3"));
	lst->addItem(new CEGUI::ListboxTextItem("Test Item 4"));

	CEGUI::Combobox *cmb = (CEGUI::Combobox *)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ComboType");
	cmb->addItem(new CEGUI::ListboxTextItem("Box"));
	cmb->addItem(new CEGUI::ListboxTextItem("Corner"));
	cmb->addItem(new CEGUI::ListboxTextItem("Pyramid"));
	cmb->addItem(new CEGUI::ListboxTextItem("Prism"));
	cmb->addItem(new CEGUI::ListboxTextItem("Cylinder"));
	cmb->addItem(new CEGUI::ListboxTextItem("HalfCylinder"));
	cmb->addItem(new CEGUI::ListboxTextItem("Cone"));
	cmb->addItem(new CEGUI::ListboxTextItem("HalfCone"));
	cmb->addItem(new CEGUI::ListboxTextItem("Sphere"));
	cmb->addItem(new CEGUI::ListboxTextItem("HalfSphere"));
	cmb->addItem(new CEGUI::ListboxTextItem("Torus"));
	cmb->addItem(new CEGUI::ListboxTextItem("Tube"));
	cmb->addItem(new CEGUI::ListboxTextItem("Ring"));
	cmb->addItem(new CEGUI::ListboxTextItem("Import..."));

	cmb = (CEGUI::Combobox *)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ComboHollowShape");
	cmb->addItem(new CEGUI::ListboxTextItem("Circle"));
	cmb->addItem(new CEGUI::ListboxTextItem("Square"));
	cmb->addItem(new CEGUI::ListboxTextItem("Triangle"));

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::setupEventHandlers(void)
{
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();

	// File Menu
	wmgr.getWindow((CEGUI::utf8*)"Root/FrameWindow/Menubar/File/New")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleFileNew, this));
	wmgr.getWindow((CEGUI::utf8*)"Root/FrameWindow/Menubar/File/Open")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleFileOpen, this));
	wmgr.getWindow((CEGUI::utf8*)"Root/FrameWindow/Menubar/File/Close")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleFileClose, this));
	wmgr.getWindow((CEGUI::utf8*)"Root/FrameWindow/Menubar/File/Save")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleFileSave, this));


	// create primitives
	wmgr.getWindow((CEGUI::utf8*)"Root/FrameWindow/Menubar/Create/Box")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleCreateBox, this));
	wmgr.getWindow((CEGUI::utf8*)"Root/FrameWindow/Menubar/Create/Corner")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleCreateCorner, this));
	wmgr.getWindow((CEGUI::utf8*)"Root/FrameWindow/Menubar/Create/Pyramid")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleCreatePyramid, this));
	wmgr.getWindow((CEGUI::utf8*)"Root/FrameWindow/Menubar/Create/Prism")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleCreatePrism, this));
	wmgr.getWindow((CEGUI::utf8*)"Root/FrameWindow/Menubar/Create/Cylinder")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleCreateCylinder, this));
	wmgr.getWindow((CEGUI::utf8*)"Root/FrameWindow/Menubar/Create/HalfCylinder")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleCreateHalfCylinder, this));
	wmgr.getWindow((CEGUI::utf8*)"Root/FrameWindow/Menubar/Create/Cone")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleCreateCone, this));
	wmgr.getWindow((CEGUI::utf8*)"Root/FrameWindow/Menubar/Create/HalfCone")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleCreateHalfCone, this));
	wmgr.getWindow((CEGUI::utf8*)"Root/FrameWindow/Menubar/Create/Sphere")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleCreateSphere, this));
	wmgr.getWindow((CEGUI::utf8*)"Root/FrameWindow/Menubar/Create/HalfSphere")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleCreateHalfSphere, this));
	wmgr.getWindow((CEGUI::utf8*)"Root/FrameWindow/Menubar/Create/Torus")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleCreateTorus, this));
	wmgr.getWindow((CEGUI::utf8*)"Root/FrameWindow/Menubar/Create/Tube")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleCreateTube, this));
	wmgr.getWindow((CEGUI::utf8*)"Root/FrameWindow/Menubar/Create/Ring")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleCreateRing, this));

	// load a mesh file
//	wmgr.getWindow((CEGUI::utf8*)"cmdLoadMesh")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleCreateMesh, this));

	// transformations
	wmgr.getWindow((CEGUI::utf8*)"Root/Popup/PopupMenus/SheetPopup/Move")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleTransMove, this));
	wmgr.getWindow((CEGUI::utf8*)"Root/Popup/PopupMenus/SheetPopup/Rotate")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleTransRotate, this));
	wmgr.getWindow((CEGUI::utf8*)"Root/Popup/PopupMenus/SheetPopup/Scale")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleTransScale, this));
	wmgr.getWindow((CEGUI::utf8*)"Root/Popup/PopupMenus/SheetPopup/Link")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleTransLink, this));
	wmgr.getWindow((CEGUI::utf8*)"Root/Popup/PopupMenus/SheetPopup/Properties")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleOpenProperties, this));

	// General Properties
	wmgr.getWindow((CEGUI::utf8*)"FrmProperties")->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&MiiModule::handleCloseProperties, this));

	// Material Properties :
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page3/ChooseColorAmbiantButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleOpenChooseAmbiantColor, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page3/ChooseColorDiffusButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleOpenChooseDiffusColor, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page3/ChooseColorSpecularButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleOpenChooseSpecularColor, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page3/CheckLockAmbiantDiffus")->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged, CEGUI::Event::Subscriber(&MiiModule::handleCheckLock, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page3/ShininessScrollBar")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleChangeShininess, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page3/TransparencyScrollBar")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleTransparencyScrollBar, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page3/TexturesScrollBar")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleTexturesScrollBarChange, this));
		//For define the position of the texture with U and V
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page3/UTextureScrollBar")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleTexturePositionScrollBar, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page3/VTextureScrollBar")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleTexturePositionScrollBar, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page3/ScaleUTextureScrollBar")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleTextureScaleScrollBar, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page3/ScaleVTextureScrollBar")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleTextureScaleScrollBar, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page3/RotateTextureScrollBar")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleTextureRotateScrollBar, this));
		//Window for add, delete and apply texture :
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page3/AddTextureButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleAddTexture, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page3/ApplyTextureButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleApplyTexture, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page3/DeleteTextureButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleRemoveTexture, this));
		//Window for choose color :
	wmgr.getWindow((CEGUI::utf8*) "FrmChooseColor")->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked, CEGUI::Event::Subscriber(&MiiModule::handleCloseChooseColor, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmChooseColor/ApplyChooseColorButton")->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&MiiModule::handleApplyChooseColor, this));

	// popup menu
	wmgr.getWindow("root")->subscribeEvent(CEGUI::Window::EventMouseButtonUp,CEGUI::Event::Subscriber(&MiiModule::onPopupMenu, this));

	/// Properties -> Model Frame
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/RotationXScroll")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleRotationXScroll, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/RotationYScroll")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleRotationYScroll, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/RotationZScroll")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleRotationZScroll, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/ScaleXScroll")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleScaleXScroll, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/ScaleYScroll")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleScaleYScroll, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/ScaleZScroll")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleScaleZScroll, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/TaperXScroll")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleTaperXScroll, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/TaperYScroll")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleTaperYScroll, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/PathCutBeginScroll")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handlePathCutBeginScroll, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/PathCutEndScroll")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handlePathCutEndScroll, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/DimpleBeginScroll")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleDimpleBeginScroll, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/DimpleEndScroll")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleDimpleEndScroll, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/HoleXScroll")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleHoleSizeXScroll, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/HoleYScroll")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleHoleSizeYScroll, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/ComboHollowShape")->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&MiiModule::handleHollowShapeCombo, this));
	// TODO : Manage Hollow Shape Changes
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/TwistBeginScroll")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleTwistBeginScroll, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/TwistEndScroll")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleTwistEndScroll, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/TopShearXScroll")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleTopShearXScroll, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/TopShearYScroll")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleTopShearYScroll, this));
	// TODO : Manage Skew Changes
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/SkewScroll")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleSkewScroll, this));
	// TODO : Manage Revolutions Changes
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/RevolutionsEdit")->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&MiiModule::handleRevolutionsText, this));
	wmgr.getWindow((CEGUI::utf8*) "FrmProperties/TabCtrl/Page2/RadiusDeltaScroll")->subscribeEvent(CEGUI::Scrollbar::EventScrollPositionChanged, CEGUI::Event::Subscriber(&MiiModule::handleRadiusDeltaScroll, this));

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleQuit(const CEGUI::EventArgs& e)
{
	//static_cast<MiiApp*>(mApp)->requestShutdown();
	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::mouseMoved( const OIS::MouseEvent &e )
{
	if (CChooseColorWindow::mUseTargetCursor)
	{
		mGUISystem->setDefaultMouseCursor(
			(CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseTarget");
	}
	else
	{
		mGUISystem->setDefaultMouseCursor(
			(CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow");
	}

	CEGUI::System::getSingleton().injectMouseMove(e.state.X.rel,e.state.Y.rel);
	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id )
{	
	return CEGUI::System::getSingleton().injectMouseButtonDown(convertOgreButtonToCegui(id));
}

//-------------------------------------------------------------------------------------
bool MiiModule::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id )
{
	return CEGUI::System::getSingleton().injectMouseButtonUp(convertOgreButtonToCegui(id));
}

using namespace OIS;

//-------------------------------------------------------------------------------------
bool MiiModule::keyPressed( const OIS::KeyEvent &e )
{
	bool key_injected = CEGUI::System::getSingleton().injectKeyDown(e.key);
	bool char_injected = CEGUI::System::getSingleton().injectChar(e.text);

	if (!sheet->getChild("FrmProperties")->isVisible())	
		//Handle SUPPR key if the frame is not opened
	{
		if ((e.key == KC_DELETE) && !(key_injected || char_injected))
		{
			Object3D* obj = mSelection->getFirstSelectedObject();
			if (obj)
			{
				char_injected = true;
				do
				{
					// Delete 3D entity
					mSceneMgr->getRootSceneNode()->removeAndDestroyChild(obj->getEntity()->getParentSceneNode()->getName() );
					mSelection->remove3DObject(obj);
					obj = mSelection->getFirstSelectedObject();
				} while (obj != NULL);
				mSelection->deselect_all();
				mSelection->mTransformation->showGizmosMove(false);
				mSelection->mTransformation->showGizmosRotate(false);
				mSelection->mTransformation->showGizmosScale(false);
			}
		}	    
	}
	return (key_injected || char_injected);
}

//-------------------------------------------------------------------------------------
bool MiiModule::keyReleased( const OIS::KeyEvent &e )
{
	return CEGUI::System::getSingleton().injectKeyUp(e.key);;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleSelection(const CEGUI::EventArgs& e)
{
	mSelection->mTransformation->eventSelection() ;
	return true;
}
//-------------------------------------------------------------------------------------
bool MiiModule::handleTransMove(const CEGUI::EventArgs& e)
{
	mSelection->mTransformation->eventMove() ;
	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleTransRotate(const CEGUI::EventArgs& e)
{
	mSelection->mTransformation->eventRotation() ;
	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleTransScale(const CEGUI::EventArgs& e)
{
	mSelection->mTransformation->eventScale();
	return true;
}
//-------------------------------------------------------------------------------------
bool MiiModule::handleTransLink(const CEGUI::EventArgs& e)
{
	mModeLink = true ;
	return true;
}
//-------------------------------------------------------------------------------------
bool MiiModule::handleCreateBox(const CEGUI::EventArgs& e)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Box%.3u",++num);

//	genMeshBox( String(name) + ".mesh", 100, 100, 100 );
	MeshPtr mptr = mGenericBox->getMesh()->clone( String(name) + ".mesh" );
	Entity* entity = mSceneMgr->createEntity( String(name), String(name) + ".mesh" );
//	Entity* entity = mSceneMgr->createEntity( String(name), "Prefab_Cube" );
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
	node->attachObject( entity );

	Object3DBox* obj = new Object3DBox( String(name), node );
	mSelection->add3DObject(obj);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleCreateCorner(const CEGUI::EventArgs& e)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Corner%.3u",++num);

	//genMeshCorner( String(name) + ".mesh", 100, 100, 100 );
	MeshPtr mptr = mGenericBox->getMesh()->clone( String(name) + ".mesh" );
	Entity* entity = mSceneMgr->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
	node->attachObject( entity );

	Object3DCorner* obj = new Object3DCorner( String(name), node );
	mSelection->add3DObject(obj);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleCreatePyramid(const CEGUI::EventArgs& e)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Pyramid%.3u",++num);

	//genMeshPyramid( String(name) + ".mesh", 100, 100, 100 );
	MeshPtr mptr = mGenericBox->getMesh()->clone( String(name) + ".mesh" );
	Entity* entity = mSceneMgr->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
	node->attachObject( entity );

	Object3DPyramid* obj = new Object3DPyramid( String(name), node );
	mSelection->add3DObject(obj);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleCreatePrism(const CEGUI::EventArgs& e)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Prism%.3u",++num);

	//genMeshPrism( String(name) + ".mesh", 100, 100 );
	MeshPtr mptr = mGenericPrism->getMesh()->clone( String(name) + ".mesh" );
	Entity *entity = mSceneMgr->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
	node->attachObject( entity );

	Object3DPrism* obj = new Object3DPrism( String(name), node );
	mSelection->add3DObject(obj);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleCreateCylinder(const CEGUI::EventArgs& e)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Cylinder%.3u",++num);

	MeshPtr mptr = mGenericCylinder->getMesh()->clone( String(name) + ".mesh" );
	Entity *entity = mSceneMgr->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
	node->attachObject( entity );

	Object3DCylinder* obj = new Object3DCylinder( String(name), node );
	mSelection->add3DObject(obj);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleCreateHalfCylinder(const CEGUI::EventArgs& e)
{
	static int num = -1;
	char name[31];
	sprintf(name, "HalfCylinder%.3u",++num);

	MeshPtr mptr = mGenericCylinder->getMesh()->clone( String(name) + ".mesh" );
	Entity *entity = mSceneMgr->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
	node->attachObject( entity );

	Object3DHalfCylinder* obj = new Object3DHalfCylinder( String(name), node );
	mSelection->add3DObject(obj);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleCreateCone(const CEGUI::EventArgs& e)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Cone%.3u",++num);

	MeshPtr mptr = mGenericCylinder->getMesh()->clone( String(name) + ".mesh" );
	Entity *entity = mSceneMgr->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
	node->attachObject( entity );

	Object3DCone* obj = new Object3DCone( String(name), node );
	mSelection->add3DObject(obj);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleCreateHalfCone(const CEGUI::EventArgs& e)
{
	static int num = -1;
	char name[31];
	sprintf(name, "HalfCone%.3u",++num);

	MeshPtr mptr = mGenericCylinder->getMesh()->clone( String(name) + ".mesh" );
	Entity *entity = mSceneMgr->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
	node->attachObject( entity );

	Object3DHalfCone* obj = new Object3DHalfCone( String(name), node );
	mSelection->add3DObject(obj);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleCreateSphere(const CEGUI::EventArgs& e)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Sphere%.3u",++num);

	MeshPtr mptr = mGenericSphere->getMesh()->clone( String(name) + ".mesh" );
	Entity *entity = mSceneMgr->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
	node->attachObject( entity );

	Object3DSphere* obj = new Object3DSphere( String(name), node );
	mSelection->add3DObject(obj);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleCreateHalfSphere(const CEGUI::EventArgs& e)
{
	static int num = -1;
	char name[31];
	sprintf(name, "HalfSphere%.3i",++num);

	MeshPtr mptr = mGenericSphere->getMesh()->clone( String(name) + ".mesh" );
	Entity *entity = mSceneMgr->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
	node->attachObject( entity );

	Object3DHalfSphere* obj = new Object3DHalfSphere( String(name), node );
	mSelection->add3DObject(obj);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleCreateTorus(const CEGUI::EventArgs& e)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Torus%.3i",++num);

	MeshPtr mptr = mGenericTorus->getMesh()->clone( String(name) + ".mesh" );
	Entity *entity = mSceneMgr->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
	node->attachObject( entity );

	Object3DTorus* obj = new Object3DTorus( String(name), node );
	mSelection->add3DObject(obj);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleCreateTube(const CEGUI::EventArgs& e)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Tube%.3i",++num);

	MeshPtr mptr = mGenericTube->getMesh()->clone( String(name) + ".mesh" );
	Entity *entity = mSceneMgr->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
	node->attachObject( entity );

	Object3DTube* obj = new Object3DTube( String(name), node );
	mSelection->add3DObject(obj);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleCreateRing(const CEGUI::EventArgs& e)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Ring%.3i",++num);

	MeshPtr mptr = mGenericRing->getMesh()->clone( String(name) + ".mesh" );
	Entity *entity = mSceneMgr->createEntity( String(name), String(name) + ".mesh" );
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
	node->attachObject( entity );

	Object3DRing* obj = new Object3DRing( String(name), node );
	mSelection->add3DObject(obj);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleCreateMesh(const CEGUI::EventArgs& e)
{
	static int num = -1;
	char name[31];
	sprintf(name, "Box%.3u",++num);

	MeshPtr mptr = mGenericBox->getMesh()->clone( String(name) + ".mesh" );
	Entity* entity = mSceneMgr->createEntity( String(name), String(name) + ".mesh" );
//	Entity* entity = mSceneMgr->createEntity( "openFileName.mesh", String(name) + ".mesh" );
	SceneNode* node = mSceneMgr->getRootSceneNode()->createChildSceneNode( String(name) + ".node" );
	node->attachObject( entity );

	Object3DOther* obj = new Object3DOther( String(name), node );
	mSelection->add3DObject(obj);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::isPropertiesFrameOpened()
{
	return sheet->getChild("FrmProperties")->isVisible();
}


//-------------------------------------------------------------------------------------
bool MiiModule::handleCloseProperties(const CEGUI::EventArgs& e)
{
	if (mSelection->getNumSelectedObjects() > 1)
	{
		showFrameProperties(false);
		// No update if multi-selection
		return true;
	}

	Object3D *obj = mSelection->getFirstSelectedObject();

	// First properties page
	obj->setName(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/EditName"))->getText().c_str());
	obj->setTags(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/EditTags"))->getText().c_str());
	obj->setCreator(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/EditCreator"))->getText().c_str());
	obj->setOwner(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/EditOwner"))->getText().c_str());
	obj->setGroup(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/EditGroup"))->getText().c_str());
	obj->setDesc(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/EditDesc"))->getText().c_str());
	obj->setCanBeModified(((CEGUI::Checkbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/CheckModification"))->isSelected());
	obj->setCanBeCopied(((CEGUI::Checkbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/CheckCopy"))->isSelected());

	// Transform Attributes 
	// Type
	// TODO : obj change of primitive type
	// obj->setType(((CEGUI::Combobox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ComboType"))->getText());
	// Taper X
	obj->setTaperX(CEGUI::PropertyHelper::stringToFloat(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TaperXEdit"))->getText()));	
	// Taper Y
	obj->setTaperY(CEGUI::PropertyHelper::stringToFloat(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TaperYEdit"))->getText()));	
	// Path Cut Begin
	obj->setPathCutBegin(CEGUI::PropertyHelper::stringToFloat(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/PathCutBeginEdit"))->getText()));
	// Path Cut End
	obj->setPathCutEnd(CEGUI::PropertyHelper::stringToFloat(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/PathCutEndEdit"))->getText()));
	// Dimple Begin
	obj->setDimpleBegin(CEGUI::PropertyHelper::stringToFloat(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/DimpleBeginEdit"))->getText()));
	// Dimple End
	obj->setDimpleEnd(CEGUI::PropertyHelper::stringToFloat(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/DimpleEndEdit"))->getText()));
	// Hole X
	obj->setHoleSizeX(CEGUI::PropertyHelper::stringToFloat(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/HoleXEdit"))->getText()));
	// Hole Y
	obj->setHoleSizeY(CEGUI::PropertyHelper::stringToFloat(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/HoleYEdit"))->getText()));
	// Hollow Shape
	obj->setHollowShape((Object3D::Shape)CEGUI::PropertyHelper::stringToInt(((CEGUI::Combobox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ComboHollowShape"))->getEditbox()->getText()));
	// Twist Begin
	obj->setTwistBegin(CEGUI::PropertyHelper::stringToFloat(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TwistBeginEdit"))->getText()));
	// Twist End
	obj->setTwistEnd(CEGUI::PropertyHelper::stringToFloat(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TwistEndEdit"))->getText()));
	// TopShear X
	obj->setTopShearX(CEGUI::PropertyHelper::stringToFloat(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TopShearXEdit"))->getText()));
	// TopShear Y
	obj->setTopShearY(CEGUI::PropertyHelper::stringToFloat(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TopShearYEdit"))->getText()));
	// Skew
	obj->setSkew(CEGUI::PropertyHelper::stringToInt(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/SkewEdit"))->getText()));
	// Radius Delta
	obj->setRadiusDelta(CEGUI::PropertyHelper::stringToFloat(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/RadiusDeltaEdit"))->getText()));
	// Revolutions
	obj->setRevolutions(CEGUI::PropertyHelper::stringToInt(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/RevolutionsEdit"))->getText()));

	// 3D Attributes
	obj->setCollisionnable(((CEGUI::Checkbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page4/CheckCollision"))->isSelected());
	obj->setEnableGravity(((CEGUI::Checkbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page4/CheckGravity"))->isSelected());

	//Material :
	onCloseMaterialProperties();

	showFrameProperties(false);
	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleOpenProperties(const CEGUI::EventArgs& e)
{
	showFrameProperties(true);
	mSelection->mTransformation->eventSelection();
	return true;
}

//-------------------------------------------------------------------------------------
void MiiModule::showFrameProperties(bool pShow)
{
	mNeedHandle = false;

	//Clear list of transformation button :
	if(! mTransfoButton.empty())
	{
		CEGUI::Window * PropertiesWindow = (CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"FrmProperties/TabCtrl/Page2"));

		std::vector<CEGUI::PushButton*>::iterator itr ;
		for( itr=mTransfoButton.begin() ; itr != mTransfoButton.end() ; itr++ )
		{
			PropertiesWindow->removeChildWindow((*itr));
			(*itr)->destroy();
		}
		mTransfoButton.clear();
	}

	//...
	if (!pShow)
	{	// Always hide properties when asked 	
		sheet->getChild("FrmProperties")->setVisible( pShow );
		// Show back the main menu....
		CEGUI::WindowManager::getSingleton().getWindow("Root/FrameWindow/Menubar")->setVisible(true);
	}
	else 
	{
		if (!mSelection->isEmpty())
		{
			//General :
			CEGUI::Editbox *text  =  NULL;
			CEGUI::Checkbox *chk = NULL;
			CEGUI::Combobox	*cmb = NULL;
			CEGUI::Scrollbar *hscb = NULL;
			CEGUI::Listbox *lstBox = NULL;

			if (mSelection->getNumSelectedObjects() > 1)
			{
				// First properties page
				{
					text  =  (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/EditName");
					text->setText("...");
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/EditCreator");
					text->setText("...");	
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/EditOwner");
					text->setText("...");	
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/EditGroup");
					text->setText("...");	
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/EditDesc");
					text->setText("...");	
					chk = (CEGUI::Checkbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/CheckModification");
					chk->setSelected(false);
					chk = (CEGUI::Checkbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/CheckCopy");
					chk->setSelected(false);
				}

				// Transform Attributes 
				{
					// Type
					cmb = (CEGUI::Combobox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ComboType");
					cmb->getEditbox()->setText("...");
					// Taper X
					hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TaperXScroll");
					hscb->setScrollPosition(0.f);
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TaperXEdit");
					text->setText(CEGUI::PropertyHelper::floatToString(0.f));	
					// Taper Y
					hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TaperYScroll");
					hscb->setScrollPosition(0.f);
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TaperYEdit");
					text->setText(CEGUI::PropertyHelper::floatToString(0.f));	
					// Path Cut Begin
					hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/PathCutBeginScroll");
					hscb->setScrollPosition(0.f);
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/PathCutBeginEdit");
					text->setText(CEGUI::PropertyHelper::floatToString(0.f));	
					// Path Cut End
					hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/PathCutEndScroll");
					hscb->setScrollPosition(0.f);
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/PathCutEndEdit");
					text->setText(CEGUI::PropertyHelper::floatToString(0.f));	
					// Dimple Begin
					hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/DimpleBeginScroll");
					hscb->setScrollPosition(0.f);
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/DimpleBeginEdit");
					text->setText(CEGUI::PropertyHelper::floatToString(0.f));	
					// Dimple End
					hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/DimpleEndScroll");
					hscb->setScrollPosition(0.f);
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/DimpleEndEdit");
					text->setText(CEGUI::PropertyHelper::floatToString(0.f));	
					// Hole X
					hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/HoleXScroll");
					hscb->setScrollPosition(0.f);
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/HoleXEdit");
					text->setText(CEGUI::PropertyHelper::floatToString(0.f));	
					// Hole Y
					hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/HoleYScroll");
					hscb->setScrollPosition(0.f);
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/HoleYEdit");
					text->setText(CEGUI::PropertyHelper::floatToString(0.f));	
					// Hollow Shape
					cmb = (CEGUI::Combobox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ComboHollowShape");
					cmb->getEditbox()->setText("NONE");
					// Twist Begin
					hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TwistBeginScroll");
					hscb->setScrollPosition(0.f);
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TwistBeginEdit");
					text->setText(CEGUI::PropertyHelper::floatToString(0.f));	
					// Twist End
					hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TwistEndScroll");
					hscb->setScrollPosition(0.f);
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TwistEndEdit");
					text->setText(CEGUI::PropertyHelper::floatToString(0.f));	
					// TopShear X
					hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TopShearXScroll");
					hscb->setScrollPosition(0.f);
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TopShearXEdit");
					text->setText(CEGUI::PropertyHelper::floatToString(0.f));	
					// TopShear Y
					hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TopShearYScroll");
					hscb->setScrollPosition(0.f);
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TopShearYEdit");
					text->setText(CEGUI::PropertyHelper::floatToString(0.f));	
					// Skew
					hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/SkewScroll");
					hscb->setScrollPosition(0.f);
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/SkewEdit");
					text->setText(CEGUI::PropertyHelper::floatToString(0));	
					// Revolutions
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/RevolutionsEdit");
					text->setText(CEGUI::PropertyHelper::intToString(0));	
					// Radius Delta
					hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/RadiusDeltaScroll");
					hscb->setScrollPosition(0.f);
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/RadiusDeltaEdit");
					text->setText(CEGUI::PropertyHelper::floatToString(0.f));	
				}

				
				// Update List of Transformation //(TODO)
				
				{
				}

				// 3D Attributes
				{
					chk = (CEGUI::Checkbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page4/CheckCollision");
					chk->setSelected(false);
					chk = (CEGUI::Checkbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page4/CheckGravity");
					chk->setSelected(false);
				}

				// Fill the informations list box
				{
					lstBox = (CEGUI::Listbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page4/ListInfos");
					lstBox->resetList();
					lstBox->addItem(new CEGUI::ListboxTextItem(String("Faces count : ...")));
					lstBox->addItem(new CEGUI::ListboxTextItem(String("Vertex count : ...") ));
					lstBox->addItem(new CEGUI::ListboxTextItem(String("Primitives count : ...")));
					lstBox->addItem(new CEGUI::ListboxTextItem(String("Mesh size : ...")));
					lstBox->addItem(new CEGUI::ListboxTextItem(String("Primitives count : ....")));
				}
			}
			else
			{
				Object3D *obj = mSelection->getFirstSelectedObject();
				/// First Properties page
				{
					text  =  (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/EditName");
					text->setText(obj->getName());	
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/EditCreator");
					text->setText(obj->getCreator());	
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/EditOwner");
					text->setText(obj->getOwner());	
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/EditGroup");
					text->setText(obj->getGroup());	
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/EditDesc");
					text->setText(obj->getDesc());	
				

					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/EditTags");
					text->setText(obj->getTags());	
					chk = (CEGUI::Checkbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/CheckModification");
					chk->setSelected(obj->getCanBeModified());
					chk = (CEGUI::Checkbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page1/CheckCopy");
					chk->setSelected(obj->getCanBeCopied());
                }

				// Transform Attributes 
				UpdateTransfoSrollBarToInitialPosition();

				// Update List of Transformation
				{
					cmb = (CEGUI::Combobox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ComboType");
					CEGUI::PushButton* primitiveButton = (CEGUI::PushButton*)(CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"FrmProperties/TabCtrl/Page2/TransfoButton"));

					CEGUI::String primitiveType = cmb->getListboxItemFromIndex(obj->getTypeAsInt())->getText() ;
					primitiveButton->setText( primitiveType );
				}
				
				// 3D Attributes
				{
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page4/EditPositionX");
					text->setText(CEGUI::PropertyHelper::floatToString(obj->getPosition().x));	
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page4/EditPositionY");
					text->setText(CEGUI::PropertyHelper::floatToString(obj->getPosition().y));	
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page4/EditPositionZ");
					text->setText(CEGUI::PropertyHelper::floatToString(obj->getPosition().z));	
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page4/EditOrientationX");
					text->setText(CEGUI::PropertyHelper::floatToString(obj->getOrientation().x));	
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page4/EditOrientationY");
					text->setText(CEGUI::PropertyHelper::floatToString(obj->getOrientation().y));	
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page4/EditOrientationZ");
					text->setText(CEGUI::PropertyHelper::floatToString(obj->getOrientation().z));	
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page4/EditScaleX");
					text->setText(CEGUI::PropertyHelper::floatToString(obj->getScale().x));	
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page4/EditScaleY");
					text->setText(CEGUI::PropertyHelper::floatToString(obj->getScale().y));	
					text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page4/EditScaleZ");
					text->setText(CEGUI::PropertyHelper::floatToString(obj->getScale().z));	
					chk = (CEGUI::Checkbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page4/CheckCollision");
					chk->setSelected(obj->getCollisionnable());
					chk = (CEGUI::Checkbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page4/CheckGravity");
					chk->setSelected(obj->getEnableGravity());
				}

				// Fill the informations list box
				{
					lstBox = (CEGUI::Listbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page4/ListInfos");
					lstBox->resetList();
					lstBox->addItem(new CEGUI::ListboxTextItem(String("Vertex count : ") + CEGUI::PropertyHelper::intToString(obj->getVertexCount())));
					lstBox->addItem(new CEGUI::ListboxTextItem(String("Triangle count : ") + CEGUI::PropertyHelper::intToString(obj->getTriCount())));
					lstBox->addItem(new CEGUI::ListboxTextItem(String("Primitives count : ") + CEGUI::PropertyHelper::intToString(obj->getPrimitivesCount()).c_str()));
					Vector3 objSize =  obj->getMeshSize();
					String s = String("Mesh size : (") + CEGUI::PropertyHelper::floatToString(objSize.x).c_str() + String(",") \
						+ CEGUI::PropertyHelper::floatToString(objSize.y).c_str() + String(",") \
						+ CEGUI::PropertyHelper::floatToString(objSize.z).c_str() + String(")");
					lstBox->addItem(new CEGUI::ListboxTextItem(s));
				}

				//Build the list of transformations :
				std::list<Object3D::TCommand>::iterator itr ;
				for(itr= obj->mCommandList.begin() ; itr != obj->mCommandList.end() ; itr++)
				{
//					AddTransformationToList( itr->first );
				}
				if(!mTransfoButton.empty())
				{
//					AddTransformationToList( itr->first );	//Don't forget current transfo :
				}
			}

			/// Update Material Properties :
			/// if there are more than one object selected, the window Material Properties only affects the first object
			onUptadeMaterialProperties() ;

			CEGUI::FrameWindow *frm = (CEGUI::FrameWindow *) sheet->getChild("FrmProperties");
			frm->setCloseButtonEnabled(true);
			frm->setVisible( pShow );

			CEGUI::WindowManager::getSingleton().getWindow("Root/FrameWindow/Menubar")->setVisible(false);
		}
	}
	mNeedHandle = true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleFileNew(const CEGUI::EventArgs& e)     
{
#ifdef WIN32
	MessageBox(NULL,"Handle File NEW","Information",MB_OK | MB_ICONINFORMATION); 
#else
	std::cerr << " Handle File NEW " << std::endl;
#endif
	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleFileOpen(const CEGUI::EventArgs& e) 
{
	char* pathToLoad = FileBrowser::displayWindowForLoading( 
			"Solipsis Object File,(*.sof)\0*.sof\0|Object Files XML,(*.xml)\0*.xml", string("") ); 

	// Go back to the main directory
	_chdir(mExecPath.c_str());

	if (pathToLoad != NULL)
	{
        Ogre::String path = pathToLoad;
		Path FilePath (	path ) ;

		//Get current path
		size_t nameSizeChar = path.find_last_of( '\\' );
		std::string texturepath (path, 0, nameSizeChar+1);

		//get only name of file (without extension)
		size_t extPos = path.find_last_of( '.' );
		std::string name (FilePath.getLastFileName(false) , nameSizeChar+1, FilePath.getLastFileName(false).length());


		// If the ext is a sof => We have to extract the file 
		Ogre::String ext = FilePath.getExtension() ;	//path.substr(path.length() - 4, path.length());
		if (ext == "sof")
		{

			// Go back to the main directory
			_chdir(mExecPath.c_str());

			MyZipArchive zz (path) ;

			if(zz.isArchivePresent() )
			{
				//Adding the zip to the ressource location and load all the medias in the zip.
				//ResourceGroupManager::getSingleton().createResourceGroup(name + "Resources");
				ResourceGroupManager::getSingleton().addResourceLocation(FilePath.getUniversalPath(),"Zip");//, name + "Resources");
				//ResourceGroupManager::getSingleton().initialiseResourceGroup(name + "Resources");

				for( int i=0 ; i<zz.getNbFile() ; i++)	//search all XML files
				{
					Path currentFileName ( zz.getName(i) ) ;
					string ext =  currentFileName.getExtension() ;

					if ( (strcmp ( currentFileName.getExtension().c_str() , "xml")) == 0 )
					{
						//we find a XML file, so we create an object :
						FileBuffer buff = zz.readFile( zz.getName(i) );
						TiXmlDocument doc;
						doc.Parse(buff.getBufferFormatedToText().c_str());
						if (doc.Error())
						{
							SOLIPSISWARNING("Unable to read the sof file",path.c_str());
							SOLIPSISWARNING("Error returned bu TinyXML",doc.ErrorDesc());
						}
						else
							createObjectWithXML(doc, "", e) ;
					}
				}

				//create all relation Parent-Childs :
				for( int i=0 ; i<zz.getNbFile() ; i++)	//for each XML files
				{
					Path currentFileName ( zz.getName(i) ) ;
					string ext =  currentFileName.getExtension() ;

					if ( (strcmp ( currentFileName.getExtension().c_str() , "xml")) == 0 )
					{
						//we find a XML file, so we open it : 
						FileBuffer buff = zz.readFile( zz.getName(i) );
						TiXmlDocument doc;
						doc.Parse(buff.getBufferFormatedToText().c_str());
						
						if (doc.Error())
						{
							SOLIPSISWARNING("Unable to read the sof file",path.c_str());
							SOLIPSISWARNING("Error returned bu TinyXML",doc.ErrorDesc());
						}
						else	//we test if this object has got parent
						{
							TiXmlElement *e = doc.RootElement()->FirstChildElement("properties");
							String parentName ;
							String currentObjName ;
							from_string(e->FirstChildElement("objparent")->Attribute("Name"),parentName);
							from_string(e->FirstChildElement("objname")->Attribute("Name"),currentObjName);
							if(strcmp (parentName.c_str(), "NULL") != 0)	//if there is a parent 
							{
								Object3D * objParent = mSelection->get3DObject(parentName);
								Object3D * currentObj = mSelection->get3DObject(currentObjName);
								objParent->linkObject( currentObj, mSceneMgr) ;
								objParent->showBoundingBox(false); 
							}
						}	

					}
				}

			}
		}
		else
		{	
			//if use has choose directly an XML fils :
			TiXmlDocument doc;
			bool loadOK = doc.LoadFile( pathToLoad );
			if (!loadOK)
			{
				// Go back to the main directory
				_chdir(mExecPath.c_str());
				SOLIPSISWARNING("Unable to load file",pathToLoad);
				SOLIPSISWARNING("Error code ",doc.ErrorDesc());
				return true; 
			}

			// Go back to the main directory
			_chdir(mExecPath.c_str());

			//create new object :
			createObjectWithXML( doc, texturepath, e);
		}

		// Go back to the main directory
		_chdir(mExecPath.c_str());

	}

//#ifdef WIN32
//	MessageBox(NULL,"Handle File OPEN","Information",MB_OK | MB_ICONINFORMATION); 
//#else
//	std::cerr << " Handle File OPEN" << std::endl;
//#endif
	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleFileSave(const CEGUI::EventArgs& e)
{
	//Get the complete name to save file (a complete name is the path, the name and the extension of the file)
	char * completeFileName = FileBrowser::displayWindowForSaving("Solipsis Files (*.sof)\0*.sof\0", "*.sof");
	
	// Go back to the main directory
	_chdir(mExecPath.c_str());

	if (completeFileName == NULL)	//if Click on CANCEL
		return true ;

	//Create Path :
	std::string strCompleteFileName ( completeFileName);
	Path FilePath (	strCompleteFileName );

	//Get only path
	size_t nameSizeChar = strCompleteFileName.find_last_of( '\\' );
	std::string directory (strCompleteFileName, 0, nameSizeChar+1);

	//get only name of file (without extension)
	size_t extPos = strCompleteFileName.find_last_of( '.' );
	std::string name (FilePath.getLastFileName(false) , nameSizeChar+1, FilePath.getLastFileName(false).length());

	//Save SOF :
	Ogre::String fileZipToSave = directory + name + Ogre::String(".sof"); 
	MyZipArchive zz(fileZipToSave.c_str());

	if( zz.isArchivePresent() )	//if this archive is already present ...
	{
		for (int i=zz.getNbFile(); i>=0  ; i--)	//...remove all files :
		{
			zz.removeFile(zz.getName(i)) ;
		}
	}

	//Save all objects in this scene
	std::list<Object3D *> listObj = mSelection->getObjectList();
	std::list<Object3D *>::iterator itr ;
	for(itr=listObj.begin() ; itr != listObj.end() ; itr++ )
	{
		// Update command list with the last called 
		upDateCommand(Object3D::NONE, (*itr) );

		//Save object in XML :
		Ogre::String fileToSave = directory + (*itr)->getName() + Ogre::String(".xml");
		(*itr)->saveToFile(fileToSave.c_str());
		zz.writeFile(fileToSave);
		
		//Save textures :
		std::string texturePath ;
		for (int i=1; i< (*itr)->getMaterialManager()->getNbTexture(); i++)	//begin to 1 to do not save the default texture !
		{
			texturePath = (*itr)->getMaterialManager()->getTexture(i)->getName();

			//Get only name :
			Path path(texturePath);
			size_t nameSizeChar = path.getFormatedPath().find_last_of( '\\' );
			std::string fileName (path.getFormatedPath(), nameSizeChar+1, path.getFormatedPath().length() );


			if ( ! zz.isFilePresent( fileName ))
				zz.writeFile( texturePath) ;
		}
		
		SOLdeleteFile(fileToSave.c_str());
	}



	// Go back to the main directory
	_chdir(mExecPath.c_str());

	

//#ifdef WIN32
//	MessageBox(NULL,"Handle File SAVE","Information",MB_OK | MB_ICONINFORMATION); 
//#else
//	std::cerr << " Handle File SAVE " << std::endl;
//#endif
	return true;
}
//-------------------------------------------------------------------------------------
bool MiiModule::handleFileClose(const CEGUI::EventArgs& e)
{
#ifdef WIN32
	MessageBox(NULL,"Handle File CLOSE","Information",MB_OK | MB_ICONINFORMATION); 
#else
	std::cerr << " Handle File CLOSE " << std::endl;
#endif
	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleRotationXScroll(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;

	CEGUI::Scrollbar *hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/RotationXScroll");
	float value = hscb->getScrollPosition();
	((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/RotationXEdit"))->setText(CEGUI::PropertyHelper::floatToString(value));
	
	Object3D *obj = mSelection->getFirstSelectedObject();	
	upDateCommand(Object3D::ROTATE, obj);
	obj->apply(Object3D::ROTATE,value,0,0);

	return true;
}
//-------------------------------------------------------------------------------------
bool MiiModule::handleRotationYScroll(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;

	CEGUI::Scrollbar *hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/RotationXScroll");
	float value = hscb->getScrollPosition();
	((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/RotationXEdit"))->setText(CEGUI::PropertyHelper::floatToString(value));
	
	Object3D *obj = mSelection->getFirstSelectedObject();	
	upDateCommand(Object3D::ROTATE, obj);
	obj->apply(Object3D::ROTATE,0,value,0);

	return true;
}
//-------------------------------------------------------------------------------------
bool MiiModule::handleRotationZScroll(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;

	CEGUI::Scrollbar *hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/RotationXScroll");
	float value = hscb->getScrollPosition();
	((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/RotationXEdit"))->setText(CEGUI::PropertyHelper::floatToString(value));
	
	Object3D *obj = mSelection->getFirstSelectedObject();	
	upDateCommand(Object3D::ROTATE, obj);
	obj->apply(Object3D::ROTATE,0,0,value);

	return true;
}
//-------------------------------------------------------------------------------------
bool MiiModule::handleScaleXScroll(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;

	CEGUI::Scrollbar *hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ScaleXScroll");
	float value = hscb->getScrollPosition();
	((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ScaleXEdit"))->setText(CEGUI::PropertyHelper::floatToString(value));
	
	Object3D *obj = mSelection->getFirstSelectedObject();	
	upDateCommand(Object3D::SCALE, obj);
	obj->apply(Object3D::SCALE,value, 1, 1);

	return true;
}
//-------------------------------------------------------------------------------------
bool MiiModule::handleScaleYScroll(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;

	CEGUI::Scrollbar *hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ScaleYScroll");
	float value = hscb->getScrollPosition();
	((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ScaleYEdit"))->setText(CEGUI::PropertyHelper::floatToString(value));
	
	Object3D *obj = mSelection->getFirstSelectedObject();
	upDateCommand(Object3D::SCALE, obj);
	obj->apply(Object3D::SCALE, 1, value, 1);

	return true;}
//-------------------------------------------------------------------------------------
bool MiiModule::handleScaleZScroll(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;

	CEGUI::Scrollbar *hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ScaleZScroll");
	float value = hscb->getScrollPosition();
	((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ScaleZEdit"))->setText(CEGUI::PropertyHelper::floatToString(value));
	
	Object3D *obj = mSelection->getFirstSelectedObject();
	upDateCommand(Object3D::SCALE, obj);
	obj->apply(Object3D::SCALE, 1, 1, value);

	return true;}
//-------------------------------------------------------------------------------------
bool MiiModule::handleTaperXScroll(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;

	CEGUI::Scrollbar *hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TaperXScroll");
	float value = hscb->getScrollPosition();
	((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TaperXEdit"))->setText(CEGUI::PropertyHelper::floatToString(value));
	
	Object3D *obj = mSelection->getFirstSelectedObject();
	upDateCommand(Object3D::TAPERX, obj);
	obj->apply(Object3D::TAPERX,value);

	return true;
}
//-------------------------------------------------------------------------------------
bool MiiModule::handleTaperYScroll(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;

	CEGUI::Scrollbar *hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TaperYScroll");
	float value = hscb->getScrollPosition();
	((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TaperYEdit"))->setText(CEGUI::PropertyHelper::floatToString(value));
	
	Object3D *obj = mSelection->getFirstSelectedObject();
	upDateCommand(Object3D::TAPERY, obj);
	obj->apply(Object3D::TAPERY,value);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handlePathCutBeginScroll(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;

	CEGUI::Scrollbar *hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/PathCutBeginScroll");
	float value = hscb->getScrollPosition();
	((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/PathCutBeginEdit"))->setText(CEGUI::PropertyHelper::floatToString(value));
	
	Object3D *obj = mSelection->getFirstSelectedObject();
	upDateCommand(Object3D::PATH_CUT_BEGIN, obj);
	obj->apply(Object3D::PATH_CUT_BEGIN,value);
	
	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handlePathCutEndScroll(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;

	CEGUI::Scrollbar *hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/PathCutEndScroll");
	float value = hscb->getScrollPosition();
	((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/PathCutEndEdit"))->setText(CEGUI::PropertyHelper::floatToString(value));
	
	Object3D *obj = mSelection->getFirstSelectedObject();
	upDateCommand(Object3D::PATH_CUT_END, obj);
	obj->apply(Object3D::PATH_CUT_END,value);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleDimpleBeginScroll(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;

	CEGUI::Scrollbar *hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/DimpleBeginScroll");
	float value = hscb->getScrollPosition();
	((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/DimpleBeginEdit"))->setText(CEGUI::PropertyHelper::floatToString(value));
	
	Object3D *obj = mSelection->getFirstSelectedObject();
	upDateCommand(Object3D::DIMPLE_BEGIN, obj);
	obj->apply(Object3D::DIMPLE_BEGIN,value);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleDimpleEndScroll(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;

	CEGUI::Scrollbar *hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/DimpleEndScroll");
	float value = hscb->getScrollPosition();
	((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/DimpleEndEdit"))->setText(CEGUI::PropertyHelper::floatToString(value));
	
	Object3D *obj = mSelection->getFirstSelectedObject();
	upDateCommand(Object3D::DIMPLE_END, obj);
	obj->apply(Object3D::DIMPLE_END,value);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleHoleSizeXScroll(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;

	CEGUI::Scrollbar *hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/HoleXScroll");
	float value = hscb->getScrollPosition();
	((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/HoleXEdit"))->setText(CEGUI::PropertyHelper::floatToString(value));
	
	Object3D *obj = mSelection->getFirstSelectedObject();
	upDateCommand(Object3D::HOLE_SIZEX, obj);
	obj->apply(Object3D::HOLE_SIZEX,value);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleHoleSizeYScroll(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;

	CEGUI::Scrollbar *hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/HoleYScroll");
	float value = hscb->getScrollPosition();
	((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/HoleYEdit"))->setText(CEGUI::PropertyHelper::floatToString(value));
	
	Object3D *obj = mSelection->getFirstSelectedObject();
	upDateCommand(Object3D::HOLE_SIZEY, obj);
	obj->apply(Object3D::HOLE_SIZEY,value);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleHollowShapeCombo(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;
	
	CEGUI::Combobox	*cmb = (CEGUI::Combobox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ComboHollowShape");
	size_t value = cmb->getItemIndex( cmb->getSelectedItem() );

	Object3D *obj = mSelection->getFirstSelectedObject();
	upDateCommand(Object3D::HOLLOW_SHAPE, obj);
	obj->apply(Object3D::HOLLOW_SHAPE,value);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleTwistBeginScroll(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;

	CEGUI::Scrollbar *hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TwistBeginScroll");
	float value = hscb->getScrollPosition();
	((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TwistBeginEdit"))->setText(CEGUI::PropertyHelper::floatToString(value));
	
	Object3D *obj = mSelection->getFirstSelectedObject();
	upDateCommand(Object3D::TWIST_BEGIN, obj);
	obj->apply(Object3D::TWIST_BEGIN,value);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleTwistEndScroll(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;

	CEGUI::Scrollbar *hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TwistEndScroll");
	float value = hscb->getScrollPosition();
	((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TwistEndEdit"))->setText(CEGUI::PropertyHelper::floatToString(value));
	
	Object3D *obj = mSelection->getFirstSelectedObject();
	upDateCommand(Object3D::TWIST_END, obj);
	obj->apply(Object3D::TWIST_END,value);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleTopShearXScroll(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;

	CEGUI::Scrollbar *hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TopShearXScroll");
	float value = hscb->getScrollPosition();
	((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TopShearXEdit"))->setText(CEGUI::PropertyHelper::floatToString(value));
	
	Object3D *obj = mSelection->getFirstSelectedObject();
	upDateCommand(Object3D::TOP_SHEARX, obj);
	obj->apply(Object3D::TOP_SHEARX,value);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleTopShearYScroll(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;

	CEGUI::Scrollbar *hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TopShearYScroll");
	float value = hscb->getScrollPosition();
	((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TopShearYEdit"))->setText(CEGUI::PropertyHelper::floatToString(value));
	
	Object3D *obj = mSelection->getFirstSelectedObject();
	upDateCommand(Object3D::TOP_SHEARY, obj);
	obj->apply(Object3D::TOP_SHEARY,value);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleSkewScroll(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;

	CEGUI::Scrollbar *hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/SkewScroll");
	float value = hscb->getScrollPosition();
	((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/SkewEdit"))->setText(CEGUI::PropertyHelper::floatToString(value));
	
	Object3D *obj = mSelection->getFirstSelectedObject();
	upDateCommand(Object3D::SKEW, obj);
	obj->apply(Object3D::SKEW,value);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleRadiusDeltaScroll(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;

	CEGUI::Scrollbar *hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/RadiusDeltaScroll");
	float value = hscb->getScrollPosition();
	((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/RadiusDeltaEdit"))->setText(CEGUI::PropertyHelper::floatToString(value));
	
	Object3D *obj = mSelection->getFirstSelectedObject();
	upDateCommand(Object3D::RADIUS_DELTA, obj);
	obj->apply(Object3D::RADIUS_DELTA,value);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleRevolutionsText(const CEGUI::EventArgs& e)
{
	if (!mNeedHandle) // Do not need 3D update
		return true;

	//unsigned int value = CEGUI::PropertyHelper::stringToInt(((CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/RevolutionsEdit"))->getText());
	CEGUI::Editbox* text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/RevolutionsEdit");
	unsigned int value = CEGUI::PropertyHelper::stringToInt(text->getText());
	
	if( value < 1 )
	{
		value = 1;
		text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/RevolutionsEdit");
		text->setText(CEGUI::PropertyHelper::intToString(1));
	}

	Object3D *obj = mSelection->getFirstSelectedObject();
	upDateCommand(Object3D::REVOLUTION, obj);
	obj->apply(Object3D::REVOLUTION,value);

	return true;
}

//-------------------------------------------------------------------------------------
void MiiModule::upDateCommand(Object3D::Command pCommand, Object3D* obj)
{ 
	// (TODO)
	if (!mNeedHandle)
		// Do not need to update command => we are updating 2D	
		return;

	if( obj->mCommandLast == Object3D::NONE ) 
		obj->mCommandLast = pCommand;
	else
	{
		Object3D::TCommand commandToBackup;
		commandToBackup.first = pCommand;
		commandToBackup.second = Vector3(-1,-1,-1);

		Object3D::Command oldCommand;
		if( obj->addCommand( commandToBackup, oldCommand ) )
		{
			stringstream is;
			is << "[" << obj->getName() << "] -> ";// << commandToBackup.first << " (" << commandToBackup.second << ")";
			switch( commandToBackup.first )
			{
			case Object3D::TRANSLATE : is << "TRANSLATE"; break;
			case Object3D::ROTATE : is << "ROTATE"; break;
			case Object3D::SCALE : is << "SCALE"; break;
			case Object3D::TAPERX : 
			case Object3D::TAPERY : is << "TAPER"; break;
			case Object3D::TOP_SHEARX: 
			case Object3D::TOP_SHEARY : is << "TOP_SHEAR"; break;
			case Object3D::TWIST_BEGIN : 
			case Object3D::TWIST_END : is << "TWIST"; break;
			case Object3D::PATH_CUT_BEGIN : 
			case Object3D::PATH_CUT_END : is << "PATH_CUT"; break;
			case Object3D::DIMPLE_BEGIN : 
			case Object3D::DIMPLE_END : is << "DIMPLE"; break;
			case Object3D::HOLE_SIZEX: 
			case Object3D::HOLE_SIZEY : 
			case Object3D::HOLLOW_SHAPE : is << "HOLE"; break;
			case Object3D::SKEW : 
			case Object3D::REVOLUTION : 
			case Object3D::RADIUS_DELTA : is << "SKEW"; break;
			default: is << "ERROR"; break;
			}
			is << " : " << commandToBackup.second;
			SOLIPSISINFO( is.str().c_str() );

			// Update scroll Bars :
			UpdateTransfoSrollBarToInitialPosition();

			//add button to list of transformations :
// verifier pour les autres occurences de cette fonction ...
//			AddTransformationToList( oldCommand );
		}
	}
}

//-------------------------------------------------------------------------------------
Object3D * MiiModule::createObjectWithXML(TiXmlDocument doc, string path, const CEGUI::EventArgs& e)
{
	Ogre::String primType = doc.RootElement()->FirstChildElement("model")->FirstChildElement("primitive")->Attribute("Name");
	Object3D::Type type = objectStringToType(primType);

	switch (type) {
		case Object3D::Type::BOX :
			{
				handleCreateBox(e);
			}
			break;
		case Object3D::Type::CONE :
			{
				handleCreateCone(e);
			}
			break;
		case Object3D::Type::CORNER :
			{
				handleCreateCorner(e);
			}
			break;
		case Object3D::Type::CYLINDER :
			{
				handleCreateCylinder(e);
			}
			break;
		case Object3D::Type::HALF_CONE :
			{
				handleCreateHalfCone(e);
			}
			break;
		case Object3D::Type::HALF_CYLINDER :
			{
				handleCreateHalfCylinder(e);
			}
			break;
		case Object3D::Type::HALF_SPHERE :
			{
				handleCreateHalfSphere(e);
			}
			break;
		case Object3D::Type::PRISM :
			{
				handleCreatePrism(e);
			}
			break;
		case Object3D::Type::RING :
			{
				handleCreateRing(e);
			}
			break;
		case Object3D::Type::SPHERE :
			{
				handleCreateSphere(e);
			}
			break;
		case Object3D::Type::TORUS :
			{
				handleCreateTorus(e);
			}
			break;
		case Object3D::Type::TUBE :
			{
				handleCreateTube(e);
			}
			break;
		case Object3D::Type::PYRAMID :
			{
				handleCreatePyramid(e);
			}
			break;
	}

	Object3D * newObject ;
	newObject = mSelection->geLastAddedObject();

	//Test the name of this object :
		//get the name of the new object
	TiXmlElement *XMLfile = doc.RootElement()->FirstChildElement("properties");
	String testName = XMLfile->FirstChildElement("objname")->Attribute("Name");
		//search if an object has already this name 
	Object3D * ObjectWithSameName = mSelection->get3DObject( testName ) ;
	if ( ObjectWithSameName != NULL )
	{		
		SOLIPSISWARNING("ERROR when open file, this name already exists. The object are automaticly renamed.",testName.c_str());
		
		//Make a new name for the object :
		do 
		{
			testName += "_" ;
		}
		while(mSelection->get3DObject( testName ) != NULL) ;

		//rename the old object : 
		ObjectWithSameName->setName( testName );
		//We rename the object already present in the scene, because in the XML file,
		//	childs have recover their parent with their name ! So if we change the name
		//	of the new object, we won't assign correct childs
	}

	newObject->loadFromFile(doc, path.c_str());


	// Go back to the main directory
	_chdir(mExecPath.c_str());

	return newObject ;
}

//-------------------------------------------------------------------------------------
void MiiModule::UpdateTransfoSrollBarToInitialPosition()
{
	//General :
	CEGUI::Combobox	*cmb = NULL;
	CEGUI::Scrollbar *hscb = NULL;
	CEGUI::Editbox *text  =  NULL;
	Object3D * obj = mSelection->getFirstSelectedObject() ;

	mNeedHandle = false ;

	// Type
	cmb = (CEGUI::Combobox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ComboType");
	cmb->getEditbox()->setText(cmb->getListboxItemFromIndex(obj->getTypeAsInt())->getText());

	size_t value = cmb->getEditbox()->getID();
char text2[11];
sprintf( text2, " °L° : %2u", value );
SOLIPSISINFO( text2 );

	// scale
	Vector3 scale = obj->getScale();
	hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ScaleXScroll");
	hscb->setScrollPosition(scale.x);
	hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ScaleYScroll");
	hscb->setScrollPosition(scale.y);
	hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ScaleZScroll");
	hscb->setScrollPosition(scale.z);
	text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ScaleXEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(scale.x));
	text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ScaleYEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(scale.y));
	text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ScaleZEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(scale.z));

	// Taper X
	hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TaperXScroll");
	hscb->setScrollPosition(obj->getTaperX());
	text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TaperXEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(obj->getTaperX()));	
	// Taper Y
	hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TaperYScroll");
	hscb->setScrollPosition(obj->getTaperY());
	text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TaperYEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(obj->getTaperY()));	
	// Path Cut Begin
	hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/PathCutBeginScroll");
	hscb->setScrollPosition(obj->getPathCutBegin());
	text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/PathCutBeginEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(obj->getPathCutBegin()));	
	// Path Cut End
	hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/PathCutEndScroll");
	hscb->setScrollPosition(obj->getPathCutEnd());
	text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/PathCutEndEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(obj->getPathCutEnd()));	
	// Dimple Begin
	hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/DimpleBeginScroll");
	hscb->setScrollPosition(obj->getDimpleBegin());
	text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/DimpleBeginEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(obj->getDimpleBegin()));	
	// Dimple End
	hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/DimpleEndScroll");
	hscb->setScrollPosition(obj->getDimpleEnd());
	text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/DimpleEndEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(obj->getDimpleEnd()));	
	// Hole X
	hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/HoleXScroll");
	hscb->setScrollPosition(obj->getHoleSizeX());
	text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/HoleXEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(obj->getHoleSizeX()));	
	// Hole Y
	hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/HoleYScroll");
	hscb->setScrollPosition(obj->getHoleSizeY());
	text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/HoleYEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(obj->getHoleSizeY()));	
	// Hollow Shape
	cmb = (CEGUI::Combobox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/ComboHollowShape");
	cmb->getEditbox()->setText(cmb->getListboxItemFromIndex((int)obj->getHollowShape())->getText());
	// Twist Begin
	hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TwistBeginScroll");
	hscb->setScrollPosition(obj->getTwistBegin());
	text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TwistBeginEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(obj->getTwistBegin()));	
	// Twist End
	hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TwistEndScroll");
	hscb->setScrollPosition(obj->getTwistEnd());
	text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TwistEndEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(obj->getTwistEnd()));	
	// TopShear X
	hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TopShearXScroll");
	hscb->setScrollPosition(obj->getTopShearX());
	text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TopShearXEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(obj->getTopShearX()));	
	// TopShear Y
	hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TopShearYScroll");
	hscb->setScrollPosition(obj->getTopShearY());
	text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/TopShearYEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(obj->getTopShearY()));	
	// Skew
	hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/SkewScroll");
	hscb->setScrollPosition(obj->getRadiusDelta());
	text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/SkewEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(obj->getSkew()));	
	// Revolutions
	text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/RevolutionsEdit");
	text->setText(CEGUI::PropertyHelper::intToString(obj->getRevolutions()));	
	// Radius Delta
	hscb = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/RadiusDeltaScroll");
	hscb->setScrollPosition(obj->getRadiusDelta());
	text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page2/RadiusDeltaEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(obj->getRadiusDelta()));	



	mNeedHandle = true ;
}
//-------------------------------------------------------------------------------------
void MiiModule::AddTransformationToList( Object3D::Command pCommand )
{
	Object3D * obj = mSelection->getFirstSelectedObject() ;
	int nbCommands = mTransfoButton.size() ;

	CEGUI::PushButton* primitiveButton = (CEGUI::PushButton*)(CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"FrmProperties/TabCtrl/Page2/TransfoButton"));

	//create new name :
	char nbCommandsStr [10] ;
	itoa(nbCommands, nbCommandsStr, 10);
	CEGUI::String newName = primitiveButton->getName() + nbCommandsStr;

	//create new window :
	CEGUI::PushButton * newButton =  (CEGUI::PushButton *)(CEGUI::WindowManager::getSingleton().createWindow("TaharezLook/Button",newName));
	CEGUI::Window * PropertiesWindow = (CEGUI::WindowManager::getSingleton().getWindow((CEGUI::utf8*)"FrmProperties/TabCtrl/Page2"));
	PropertiesWindow->addChildWindow(newButton);
		
	// Get the button label
	CEGUI::String transName;
	switch( pCommand )
	{
	case Object3D::TRANSLATE : transName = "TRANSLATE"; break;
	case Object3D::ROTATE : transName = "ROTATE"; break;
	case Object3D::SCALE : transName = "SCALE"; break;
	case Object3D::TAPERX : 
	case Object3D::TAPERY : transName = "TAPER"; break;
	case Object3D::TOP_SHEARX: 
	case Object3D::TOP_SHEARY : transName = "TOP_SHEAR"; break;
	case Object3D::TWIST_BEGIN : 
	case Object3D::TWIST_END : transName = "TWIST"; break;
	case Object3D::PATH_CUT_BEGIN : 
	case Object3D::PATH_CUT_END : transName = "PATH_CUT"; break;
	case Object3D::DIMPLE_BEGIN : 
	case Object3D::DIMPLE_END : transName = "DIMPLE"; break;
	case Object3D::HOLE_SIZEX: 
	case Object3D::HOLE_SIZEY : 
	case Object3D::HOLLOW_SHAPE : transName = "HOLE"; break;
	case Object3D::SKEW : 
	case Object3D::REVOLUTION : 
	case Object3D::RADIUS_DELTA : transName = "SKEW"; break;
	default: transName = "ERROR"; break;
	}

	// Update property of this window :
	newButton->setProperty("UnifiedPosition",primitiveButton->getProperty("UnifiedPosition"));
	newButton->setProperty("UnifiedSize",primitiveButton->getProperty("UnifiedSize"));
	newButton->setProperty("Font",primitiveButton->getProperty("Font"));
	newButton->show();
//	newButton->setText( nbCommandsStr );
	newButton->setText( transName.c_str() );

	// Update Size :
    CEGUI::UDim newPosition = primitiveButton->getXPosition() ;
	newPosition.d_scale += ((nbCommands+1)*(primitiveButton->getSize().d_x.d_scale + 0.025)) ;
	newButton->setXPosition( newPosition );
	
	mTransfoButton.push_back( newButton );
}

