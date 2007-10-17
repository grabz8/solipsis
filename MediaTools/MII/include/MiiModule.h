#include "ModuleGUI.h"
#include "Object3D.h"

using namespace std;

class Selection;
class ModuleGUI;
class CChooseColorWindow ;

class MiiModule
	: public ModuleGUI
{
public:

	MiiModule(SceneManager* pSceneMgr, Camera* pCamera,Selection *pSel);
	~MiiModule();

	bool createGUI( RenderWindow* pWindow );
	void showFrameProperties();
	void hideFrameProperties();
	bool setupEventHandlers(void);
	bool handleQuit(const CEGUI::EventArgs& e);
	// Selection
	bool handleSelection(const CEGUI::EventArgs& e);
	// Transformations : move / rotate / scale
	bool handleTransMove(const CEGUI::EventArgs& e);
	bool handleTransRotate(const CEGUI::EventArgs& e);
	bool handleTransScale(const CEGUI::EventArgs& e);
	bool handleTransLink(const CEGUI::EventArgs& e);
	// Primitiv creation (13)
	bool handleCreateBox(const CEGUI::EventArgs& e);
	bool handleCreateCorner(const CEGUI::EventArgs& e);
	bool handleCreatePyramid(const CEGUI::EventArgs& e);
	bool handleCreatePrism(const CEGUI::EventArgs& e);
	bool handleCreateCylinder(const CEGUI::EventArgs& e);
	bool handleCreateHalfCylinder(const CEGUI::EventArgs& e);
	bool handleCreateCone(const CEGUI::EventArgs& e);
	bool handleCreateHalfCone(const CEGUI::EventArgs& e);
	bool handleCreateSphere(const CEGUI::EventArgs& e);
	bool handleCreateHalfSphere(const CEGUI::EventArgs& e);
	bool handleCreateTorus(const CEGUI::EventArgs& e);
	bool handleCreateTube(const CEGUI::EventArgs& e);
	bool handleCreateRing(const CEGUI::EventArgs& e);
	// Load a mesh file
	bool handleCreateMesh(const CEGUI::EventArgs& e);
	
	// Main menu handles
	bool handleFileNew(const CEGUI::EventArgs& e);     
	bool handleFileOpen(const CEGUI::EventArgs& e); 
	bool handleFileSave(const CEGUI::EventArgs& e);
	bool handleFileClose(const CEGUI::EventArgs& e);


	// Frame General Properties part 
	/// Handle close button
	bool handleCloseProperties(const CEGUI::EventArgs& e);
	bool handleOpenProperties(const CEGUI::EventArgs& e);
	bool isPropertiesFrameOpened();

	bool handleTaperXScroll(const CEGUI::EventArgs& e);
	bool handleTaperYScroll(const CEGUI::EventArgs& e);
	bool handlePathCutBeginScroll(const CEGUI::EventArgs& e);
	bool handlePathCutEndScroll(const CEGUI::EventArgs& e);
	bool handleDimpleBeginScroll(const CEGUI::EventArgs& e);
	bool handleDimpleEndScroll(const CEGUI::EventArgs& e);
	bool handleHoleSizeXScroll(const CEGUI::EventArgs& e);
	bool handleHoleSizeYScroll(const CEGUI::EventArgs& e);
	bool handleTwistBeginScroll(const CEGUI::EventArgs& e);
	bool handleTwistEndScroll(const CEGUI::EventArgs& e);
	bool handleTopShearXScroll(const CEGUI::EventArgs& e);
	bool handleTopShearYScroll(const CEGUI::EventArgs& e);
	bool handleRadiusDeltaScroll(const CEGUI::EventArgs& e);


	//	Frame Material Properties part
	bool handleOpenChooseAmbiantColor(const CEGUI::EventArgs& e);
	bool handleOpenChooseDiffusColor(const CEGUI::EventArgs& e);
	bool handleOpenChooseSpecularColor(const CEGUI::EventArgs& e);
	bool handleCloseChooseColor(const CEGUI::EventArgs& e);
	bool handleApplyChooseColor(const CEGUI::EventArgs& e) ;
	bool handleAddTexture(const CEGUI::EventArgs& e) ;
	bool handleRemoveTexture(const CEGUI::EventArgs& e) ;
	bool handleApplyTexture(const CEGUI::EventArgs& e) ;
	bool handleCheckLock(const CEGUI::EventArgs& e) ;
	bool handleChangeShininess(const CEGUI::EventArgs& e) ;
	bool handleTexturesScrollBarChange(const CEGUI::EventArgs& e);
	bool handleClickOnMaterialPropertiesWindow(const CEGUI::EventArgs& e);
	bool handleTexturePositionScrollBar(const CEGUI::EventArgs& e);
	bool handleTextureScaleScrollBar(const CEGUI::EventArgs& e);

	// MouseListener
	bool mouseMoved( const OIS::MouseEvent &e );
	bool mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );
	bool mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );
	// KeyListener
	bool keyPressed( const OIS::KeyEvent &e );
	bool keyReleased( const OIS::KeyEvent &e );

	// Frames display
	void showFrameProperties(bool pShow);

	/// Start mode link when the user click on the Menu PopUp on "Link"
	bool mModeLink ;

	/// Update the command => backup if the command is different from the last used 
	void		upDateCommand(Object3D::Command newCommand);

private:
	/// Configure the menus
	void configureMenu(CEGUI::Window* pParent, const bool& pMenubar);
	void unConfigureMenu(CEGUI::Window* pParent, const bool& pMenubar);

	/// Configure the menus
	bool onMouseEntersMenuItem(const CEGUI::EventArgs& e);
	/// Configure the menus
	bool onMouseLeavesMenuItem(const CEGUI::EventArgs& e);
	bool onPopupMenu(const CEGUI::EventArgs& e);
	bool onMenuKey(const CEGUI::EventArgs& e);
	bool onMouseLeavesPopupMenuItem(const CEGUI::EventArgs& e);

	/// The CEGUI window sheet
	CEGUI::Window* sheet;

	/// The selection object
	Selection		*mSelection;	

	Entity			*mGenericBox;
	Entity			*mGenericPrism;
	Entity			*mGenericCylinder;
	Entity			*mGenericSphere;
	Entity			*mGenericTorus;
	Entity			*mGenericTube;
	Entity			*mGenericRing;

	//Material Properties : 
	///brief Update the window Material Properties. Put the correct color for ambient, diffuse and specular, and show the goods textures.
	void onUptadeMaterialProperties();
	///brief Close the window Material Properties. Clear the 2 lists mTexturesImageSets and mTextureImageWindows.
	void onCloseMaterialProperties();
	///biref Create a CEGUI texture and add it in the list mTexturesImageSets.
	///param pTexture texture to add
	void AddCEGUITexture( TexturePtr pTexture);
	///brief Create a new Window Sample for display textures in the Properties Window, and put it in the end of the list mTextureImageWindows.
	void addNewWindowTextureSample();
	///brief Clear the list of CEGUI Texture (mTexturesImageSets) and delete the textures record in this list.
	void clearImageSetsList();
	///brief Clear the list of CEGUI Windows Sample (mTextureImageWindows) and delete the windows record in this list.
	void clearWindowsImageList();

	///brief Class for manage the window "Choose Color"
	CChooseColorWindow * mChooseColorWindow ;
	enum ModifiedColor {
		NONE,
		AMBIANT,
		DIFFUS,
		SPECULAR };
	///biref Current color choosen
	ModifiedColor mModifiedColor ;
	///brief Variable for remember the scroll Position for the window Material Properties.
	float mPrecedentVerticalScrollPosition ;
	///brief List of all the CEGUI imageset used for making the texture images in the window Material Properties
	std::vector<CEGUI::Imageset*> mTexturesImageSets;		
	///brief List of the CEGUI::Window texture image windows, used for show textures in the window Material Properties
	std::vector<CEGUI::Window*> mTextureImageWindows;		
	///brief Number of texture selected. 0 is the default texture.
	int mNumTextureSelected ;



	/// The execution path (to go back home each time)
	Ogre::String mExecPath;

	/// bool to desactivate the handles while opening the properties frame
	bool		mNeedHandle;
};
