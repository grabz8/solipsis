#include "MiiModule.h"
#include "Selection.h"
#include "Object3D.h"
#include "ChooseColorWindow.h"
#include "ModifiedMaterialManager.h"
#include "ModifiedMaterial.h"
#include "FileBrowser.h"
#include <string>
#include "Path.h"
// Directory management
#include <direct.h>



bool MiiModule::handleOpenChooseAmbiantColor(const CEGUI::EventArgs& e)
{
	mChooseColorWindow->showFrame(true);
	sheet->getChild("FrmChooseColor")->activate();
	mModifiedColor = ModifiedColor::AMBIANT ;
	return true ;
}
//-------------------------------------------------------------------------------------
bool MiiModule::handleOpenChooseDiffusColor(const CEGUI::EventArgs& e)
{
	mChooseColorWindow->showFrame(true);
	sheet->getChild("FrmChooseColor")->activate();
	mModifiedColor = ModifiedColor::DIFFUS ;
	return true ;
}
//-------------------------------------------------------------------------------------
bool MiiModule::handleOpenChooseSpecularColor(const CEGUI::EventArgs& e)
{
	mChooseColorWindow->showFrame(true);
	sheet->getChild("FrmChooseColor")->activate();
	mModifiedColor = ModifiedColor::SPECULAR ;
	return true ;
}
//-------------------------------------------------------------------------------------
bool MiiModule::handleCloseChooseColor(const CEGUI::EventArgs& e)
{
	mChooseColorWindow->showFrame(false) ;
	mModifiedColor = ModifiedColor::NONE ;
	return true ;
}
//-------------------------------------------------------------------------------------
bool MiiModule::handleApplyChooseColor(const CEGUI::EventArgs& e)
{
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window* colourSample;
	Object3D * obj = mSelection->getFirstSelectedObject();
	
	Vector3 color = mChooseColorWindow->getColor() ;
	
	CEGUI::Checkbox* mCheckBox = (CEGUI::Checkbox*)(wmgr.getWindow((CEGUI::utf8*)"FrmProperties/TabCtrl/Page3/CheckLockAmbiantDiffus"));
	
	ColourValue mAmbiantColor = obj->getAmbiant() ;
	ColourValue mDiffusColor = obj->getDiffus() ;
	ColourValue mSpecularColor = obj->getSpecular() ;

	switch(mModifiedColor)
	{
		case ModifiedColor::AMBIANT :
		{
			mAmbiantColor = ColourValue (color.x,color.y,color.z);
			if(mCheckBox->isSelected())
			{
				mDiffusColor = mAmbiantColor;
			}
			break ;
		}
		case ModifiedColor::DIFFUS :
		{
			mDiffusColor = ColourValue (color.x,color.y,color.z);
			if(mCheckBox->isSelected())
			{
				mAmbiantColor = mDiffusColor;
			}
			break ;
		}
		case ModifiedColor::SPECULAR :
		{
			mSpecularColor = ColourValue (color.x,color.y,color.z);
			break ;
		}
	}

	colourSample = wmgr.getWindow((CEGUI::utf8*)"FrmProperties/TabCtrl/Page3/ColorSampleAmbiant");
	colourSample->setProperty("ImageColours",
		CEGUI::PropertyHelper::colourToString(CEGUI::colour(
		mAmbiantColor.r,
		mAmbiantColor.g,
		mAmbiantColor.b)));

	colourSample = wmgr.getWindow((CEGUI::utf8*)"FrmProperties/TabCtrl/Page3/ColorSampleDiffus");
	colourSample->setProperty("ImageColours",
		CEGUI::PropertyHelper::colourToString(CEGUI::colour(
		mDiffusColor.r,
		mDiffusColor.g,
		mDiffusColor.b)));

	colourSample = wmgr.getWindow((CEGUI::utf8*)"FrmProperties/TabCtrl/Page3/ColorSampleSpecular");
	colourSample->setProperty("ImageColours",
		CEGUI::PropertyHelper::colourToString(CEGUI::colour(
		mSpecularColor.r,
		mSpecularColor.g,
		mSpecularColor.b)));

	//Apply ambiant, specular and diffus on the texture
	obj->setAmbiant(mAmbiantColor);
	obj->setDiffus(mDiffusColor);
	obj->setSpecular(mSpecularColor); // reapply all colors 

	// Close the ChooseColorWindow
	mChooseColorWindow->showFrame(false);	

	return true ;
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleAddTexture(const CEGUI::EventArgs& e) 
{
	char * PathTexture = FileBrowser::displayWindowForLoading( 
			"Image Files (*.png;*.bmp;*.jpg)\0*.png;*.bmp;*.jpg\0", string("") ); 
	
	// Go back to the main directory
	_chdir(mExecPath.c_str());

	if (PathTexture != NULL)
	{
		//Put the scroll bar on 0 :
		CEGUI::Scrollbar * tmpScrollBar ;
		tmpScrollBar = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/TexturesScrollBar");
		tmpScrollBar->setScrollPosition( 0);
		mPrecedentVerticalScrollPosition = 0;

		String TextureFilePath (PathTexture);
		Object3D * obj = mSelection->getFirstSelectedObject();
		
		Path pathTexture (TextureFilePath);
		Ogre::String pTextureName = pathTexture.getLastFileName(true);

		//Create the new OGRE texture with the file selected :
		TexturePtr PtrTexture = TextureManager::getSingleton().load( TextureFilePath, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		//Test if this texture is not already in the list :
		if( obj->getMaterialManager()->isPresentInList( PtrTexture ) )
		{
			MessageBox(NULL,"This Texture is already open","Error",MB_OK|MB_ICONEXCLAMATION);
			return false;
		}

		//Create the new CEGUI texture to show it in the Window Properties
		AddCEGUITexture(PtrTexture) ;

		//Create a window Sample to show this texture :
		addNewWindowTextureSample( );
		
		//Put the texture on the Window Texture Sample  :
		CEGUI::Window * tmpFrame = mTextureImageWindows.back() ;
		CEGUI::Imageset * textureImageset = mTexturesImageSets.back() ;
		char textureNum [3];
		int n = (int)(mTexturesImageSets.size()) -1;
		itoa(n, textureNum, 10);
		String textureName = "Texture" + mSelection->getFirstSelectedObject()->getName() + textureNum ;
		tmpFrame->setProperty("Image", CEGUI::PropertyHelper::imageToString( 
			&textureImageset->getImage(textureName)));

		//Add texture for the object (with obj->mModifiedMaterialManager)
		obj->addTexture(PtrTexture);
	}

	return true ;
}
//-------------------------------------------------------------------------------------
void  MiiModule::AddCEGUITexture( TexturePtr pTexture)
{
	int tmpNum = (int)mTexturesImageSets.size() ;
	char textureNum [3];
	itoa(tmpNum, textureNum, 10);

	CEGUI::Texture* colourImageTexture = mGUIRenderer->createTexture(pTexture);
	String textureName = "Texture" + mSelection->getFirstSelectedObject()->getName() + textureNum ;

	if( CEGUI::ImagesetManager::getSingleton().isImagesetPresent(textureName))
	{
		CEGUI::ImagesetManager::getSingleton().destroyImageset( textureName);
	}

	CEGUI::Imageset* textureImageset = CEGUI::ImagesetManager::getSingleton().createImageset(textureName,colourImageTexture);
	textureImageset->defineImage(textureName, 
				CEGUI::Point(0, 0),
				CEGUI::Size(colourImageTexture->getWidth(),colourImageTexture->getHeight()),
				CEGUI::Point(0,0));

	//Remember this Imageset in the list :
	mTexturesImageSets.push_back(textureImageset);
}

//-------------------------------------------------------------------------------------
bool MiiModule::handleApplyTexture(const CEGUI::EventArgs& e) 
{
	Object3D * obj = mSelection->getFirstSelectedObject();

	//get selected texture :
	TexturePtr textureSelected = obj->getMaterialManager()->getTexture( mNumTextureSelected );
	obj->setCurrentTexture( textureSelected );

	return true ;
}
//-------------------------------------------------------------------------------------
void MiiModule::onUptadeMaterialProperties()
{
	CEGUI::Window * tmpFrame ;

	mNumTextureSelected = 0;
	Object3D *obj = mSelection->getFirstSelectedObject();

	clearImageSetsList() ;//clear the list of current textures
	clearWindowsImageList(); //clear the list of current window sample

	String textureName ;
	char textureNum [3] ;
	
	CEGUI::Texture* colourImageTexture ;
	CEGUI::Imageset* textureImageset ;
	
	//Create CEGUI texture and displays them in the Window Material Properties
	for(int i=0 ; i<obj->getMaterialManager()->getNbTexture() ; i++)
	{
		itoa(i, textureNum, 10) ;
		textureName = "Texture" + mSelection->getFirstSelectedObject()->getName() + textureNum ;
		
		//Create ImageSet for CEGUI
		TexturePtr textureTMP = obj->getMaterialManager()->getTexture(i) ;
		colourImageTexture = mGUIRenderer->createTexture(textureTMP);
		textureImageset = CEGUI::ImagesetManager::getSingleton().createImageset(textureName,colourImageTexture);
		textureImageset->defineImage(textureName, 
				CEGUI::Point(0, 0),
				CEGUI::Size(colourImageTexture->getWidth(),colourImageTexture->getHeight()),
				CEGUI::Point(0,0));

		//Create the new Window Sample :
		addNewWindowTextureSample();

		//Put the CEGUI texture in the new window sample :
		tmpFrame = mTextureImageWindows.back() ;
		tmpFrame->setProperty("Image", CEGUI::PropertyHelper::imageToString(&textureImageset->getImage(textureName)));
		
		//Remember this Imageset in the list :
		mTexturesImageSets.push_back(textureImageset);

		//select it if it's the current texture :
		if (obj->getCurrentTexture() == textureTMP)
		{
			mNumTextureSelected = i ;
			
			//...and move the image selection :
			CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
			CEGUI::Window * SelectWindow = (wmgr.getWindow((CEGUI::utf8*)"FrmProperties/TabCtrl/Page3/SelectSample"));

			CEGUI::UDim newPos = tmpFrame->getXPosition();
			newPos.d_scale -= 0.01 ;
			SelectWindow->setXPosition( newPos );
		}
	}

	ColourValue mAmbiantColor = obj->getAmbiant() ;
	ColourValue mDiffusColor = obj->getDiffus() ;
	ColourValue mSpecularColor = obj->getSpecular() ;

	CEGUI::Editbox *text  =  NULL;
	
	tmpFrame = CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/ColorSampleAmbiant");
	tmpFrame->setProperty("ImageColours",CEGUI::PropertyHelper::colourToString(CEGUI::colour( mAmbiantColor.r, mAmbiantColor.g, mAmbiantColor.b)));
	tmpFrame = CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/ColorSampleDiffus");
	tmpFrame->setProperty("ImageColours",CEGUI::PropertyHelper::colourToString(CEGUI::colour( mDiffusColor.r, mDiffusColor.g, mDiffusColor.b)));
	tmpFrame = CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/ColorSampleSpecular");
	tmpFrame->setProperty("ImageColours",CEGUI::PropertyHelper::colourToString(CEGUI::colour( mSpecularColor.r, mSpecularColor.g, mSpecularColor.b)));
	
	//set current value for shininess scrollBar :
	tmpFrame = CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/ShininessScrollBar");
	((CEGUI::Scrollbar*)tmpFrame)->setScrollPosition( obj->getShininess() );
	//set current value for shininess edit :
    text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/ShininessEdit");
	text->setText(CEGUI::PropertyHelper::floatToString( obj->getShininess() ));
 

	//set the scroll bar for select texture to 0 :
	CEGUI::Scrollbar * tmpScrollBar ;
	mPrecedentVerticalScrollPosition = 0;
	tmpScrollBar = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/TexturesScrollBar");
	tmpScrollBar->setScrollPosition( 0);
	
	//set scroll bars positions for the position of the texture:
	Ogre::Vector2 UV = obj->getMaterialManager()->getTextureScroll() ;
	tmpScrollBar = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/UTextureScrollBar");
	tmpScrollBar->setScrollPosition( UV.x) ;
	tmpScrollBar = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/VTextureScrollBar");
	tmpScrollBar->setScrollPosition( UV.y) ;
	//set the editBox for the texture position UV
    text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/UTextureEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(UV.x));
    text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/VTextureEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(UV.y));

	//set scroll bars positions for the scale of the texture:
	UV = obj->getMaterialManager()->getTextureScale() ;
	tmpScrollBar = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/ScaleUTextureScrollBar");
	tmpScrollBar->setScrollPosition( UV.x/2.0) ;
	tmpScrollBar = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/ScaleVTextureScrollBar");
	tmpScrollBar->setScrollPosition( UV.y/2.0) ;
	//set the editBox for the texture scale UV
    text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/ScaleUTextureEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(UV.x));
    text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/ScaleVTextureEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(UV.y));

}
//-------------------------------------------------------------------------------------
bool MiiModule::handleChangeShininess(const CEGUI::EventArgs& e) 
{
	Object3D * obj = mSelection->getFirstSelectedObject();

	CEGUI::Window * tmpFrame ;
	tmpFrame = CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/ShininessScrollBar");
	float tmpShininess = ((CEGUI::Scrollbar*)tmpFrame)->getScrollPosition();
	
	obj->setShininess( tmpShininess);

	CEGUI::Editbox *text  =  NULL;
	//set current value for shininess edit :
    text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/ShininessEdit");
	text->setText(CEGUI::PropertyHelper::floatToString( obj->getShininess() ));
 

	return true;
}
//-------------------------------------------------------------------------------------
bool MiiModule::handleCheckLock(const CEGUI::EventArgs& e)
{
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Checkbox* mCheckBox = (CEGUI::Checkbox*)(wmgr.getWindow((CEGUI::utf8*)"FrmProperties/TabCtrl/Page3/CheckLockAmbiantDiffus"));

	Object3D * obj = mSelection->getFirstSelectedObject();
	ColourValue mAmbiantColor = obj->getAmbiant() ;
	ColourValue mDiffusColor = obj->getDiffus() ;
	ColourValue mSpecularColor = obj->getSpecular() ;

	if (mCheckBox->isSelected())
	{// Synchronise the 2 colors (Diffuse and Ambiant)		
		
		mDiffusColor = mAmbiantColor;
		
		//Apply ambiant, specular and diffus on the texture
		obj->setAmbiant(mAmbiantColor);
		obj->setDiffus(mDiffusColor);
		// Update color preview of diffuse color
		CEGUI::Window* colourSample;
		colourSample = wmgr.getWindow((CEGUI::utf8*)"FrmProperties/TabCtrl/Page3/ColorSampleDiffus");
		colourSample->setProperty("ImageColours",
			CEGUI::PropertyHelper::colourToString(CEGUI::colour(
			mDiffusColor.r,
			mDiffusColor.g,
			mDiffusColor.b)));
	}
	return true;
}
//-------------------------------------------------------------------------------------
bool MiiModule::handleTexturesScrollBarChange(const CEGUI::EventArgs& e)
{
	CEGUI::Window * tmpFrame ;
	float TotalDistance = 0;
	
	CEGUI::Scrollbar * tmpScrollBar ;
	tmpScrollBar = (CEGUI::Scrollbar*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/TexturesScrollBar");

	int nbImageSet = (int) mTextureImageWindows.size() ;

	//get the dimension beetween the first and the last window
	String windowSampleName =  "FrmProperties/TabCtrl/Page3/TextureSample";
	char textureNum [3] ;
	itoa(nbImageSet-1, textureNum, 10) ;
	windowSampleName += textureNum ;

	tmpFrame = CEGUI::WindowManager::getSingleton().getWindow(windowSampleName);
	TotalDistance = tmpFrame->getXPosition().d_scale + tmpFrame->getSize().d_x.d_scale;

	windowSampleName =  "FrmProperties/TabCtrl/Page3/TextureSample0";
	tmpFrame = CEGUI::WindowManager::getSingleton().getWindow(windowSampleName);
	TotalDistance -= tmpFrame->getXPosition().d_scale ;

	if (TotalDistance > 0.925)
	{
		//Calculate the vertical decalage
		float HorizontalDecalage = (mPrecedentVerticalScrollPosition - tmpScrollBar->getScrollPosition())* (TotalDistance-0.925) ;

		//Apply this decalage for all windows in the list :	
		CEGUI::UDim newPosition;
		
		CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
		CEGUI::Window * SelectWindow = (wmgr.getWindow((CEGUI::utf8*)"FrmProperties/TabCtrl/Page3/SelectSample"));
		newPosition = SelectWindow->getXPosition() ;
		newPosition.d_scale += HorizontalDecalage ;
		SelectWindow->setXPosition( newPosition );
		
		for(int i=0; i<nbImageSet ; i++ )
		{
			newPosition = mTextureImageWindows[i]->getXPosition() ;
			newPosition.d_scale += HorizontalDecalage ;
			mTextureImageWindows[i]->setXPosition( newPosition );
		}
	}

	mPrecedentVerticalScrollPosition = tmpScrollBar->getScrollPosition();
	return true;
}
//-------------------------------------------------------------------------------------
void MiiModule::onCloseMaterialProperties()
{
	//clear the list of CEGUI Textures (mTexturesImageSets)
	clearImageSetsList() ;

	//clear the list of Window Sample (mTextureImageWindows)
	clearWindowsImageList() ;

}
//-------------------------------------------------------------------------------------
void MiiModule::clearImageSetsList()
{
	if (!mTexturesImageSets.empty() )
	{
		std::vector<CEGUI::Imageset*>::iterator imageSetIterator ;
		for( imageSetIterator= mTexturesImageSets.begin() ;
			imageSetIterator != mTexturesImageSets.end() ;
			imageSetIterator++)
		{
			CEGUI::Imageset* imageSet = (*imageSetIterator);
			
			if( CEGUI::ImagesetManager::getSingleton().isImagesetPresent(imageSet->getName()))
			{
				imageSet->undefineAllImages();
				CEGUI::ImagesetManager::getSingleton().destroyImageset( imageSet);
			}
		}
		mTexturesImageSets.clear();
	}
}
//-------------------------------------------------------------------------------------
void MiiModule::clearWindowsImageList()
{
	if(! mTextureImageWindows.empty() )
	{
		CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
		CEGUI::Window * PropertiesWindow = (wmgr.getWindow((CEGUI::utf8*)"FrmProperties/TabCtrl/Page3"));
		std::vector<CEGUI::Window*>::iterator itr;
		for(itr = mTextureImageWindows.begin();
			itr <mTextureImageWindows.end();
			itr++)
		{
			PropertiesWindow->removeChildWindow( (*itr) );
			wmgr.destroyWindow( (*itr)->getName() );
		}
		mTextureImageWindows.clear();
	}
}
//-------------------------------------------------------------------------------------
void MiiModule::addNewWindowTextureSample()
{					
									//Create a CEGUI frame for show a texture
	int pNumber = (int) mTextureImageWindows.size() ;
	String textureName ;
	String WindowFrameName ;

	char textureNum [3];
	itoa(pNumber, textureNum, 10) ;
	
	
	//create new name :
	WindowFrameName = "FrmProperties/TabCtrl/Page3/TextureSample" ;
	WindowFrameName = WindowFrameName + textureNum ;

	//create new window :
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window * tmpFrame =  wmgr.createWindow("TaharezLook/StaticImage",WindowFrameName);
	CEGUI::Window * PropertiesWindow = (wmgr.getWindow((CEGUI::utf8*)"FrmProperties/TabCtrl/Page3"));
	PropertiesWindow->addChildWindow(tmpFrame);
	
	//Update property of this window :
	tmpFrame->setProperty("UnifiedPosition","{{0.025,0},{0.58,0}}");
	tmpFrame->setProperty("UnifiedSize","{{0.3,0},{0.3,0}}");
	tmpFrame->setProperty("Image","set:TaharezLook image:ListboxSelectionBrush");
	tmpFrame->setProperty("AlwaysOnTop","True");
	CEGUI::UDim newPosition = tmpFrame->getXPosition() ;
	if(pNumber!=0)
		newPosition.d_scale += (pNumber*(tmpFrame->getSize().d_x.d_scale + 0.025)) ;
	tmpFrame->setXPosition (newPosition);		

	//Remember it on the list mTextureImageWindows :
	mTextureImageWindows.push_back(tmpFrame);

	//Attach event :
	tmpFrame->subscribeEvent(CEGUI::Window::EventMouseClick, CEGUI::Event::Subscriber(&MiiModule::handleClickOnMaterialPropertiesWindow, this));

}
//-------------------------------------------------------------------------------------
bool MiiModule::handleClickOnMaterialPropertiesWindow(const CEGUI::EventArgs& e)
{
	CEGUI::WindowEventArgs& we = (CEGUI::WindowEventArgs&)e;

	CEGUI::UDim newPos = we.window->getXPosition();
	newPos.d_scale -= 0.01 ;

	//Update window selection :
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Window * SelectWindow = (wmgr.getWindow((CEGUI::utf8*)"FrmProperties/TabCtrl/Page3/SelectSample"));
	SelectWindow->setXPosition( newPos );

	//Get the number of texture selected and remember it with mNumTextureSelected
	CEGUI::String textureWindowName = we.window->getName();
	CEGUI::String textureNum = textureWindowName.substr(String("FrmProperties/TabCtrl/Page3/TextureSample").length());
	const char * num = textureNum.c_str() ;
	mNumTextureSelected = atoi(num);

	//test if we can remove this texture and uptade remove button :
	bool enable =  (bool)textureWindowName.compare( "FrmProperties/TabCtrl/Page3/TextureSample0")  ;
	CEGUI::PushButton * RemoveButton = (CEGUI::PushButton *)(wmgr.getWindow((CEGUI::utf8*)"FrmProperties/TabCtrl/Page3/DeleteTextureButton"));
	RemoveButton->setEnabled(enable) ;
	

	return true ;
}
//-------------------------------------------------------------------------------------
bool MiiModule::handleRemoveTexture(const CEGUI::EventArgs& e) 
{
	Object3D * obj = mSelection->getFirstSelectedObject();

	TexturePtr texturToDelete = obj->getMaterialManager()->getTexture( mNumTextureSelected );
	obj->deleteTexture( texturToDelete );

	mNumTextureSelected = 0;
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::PushButton * RemoveButton = (CEGUI::PushButton *)(wmgr.getWindow((CEGUI::utf8*)"FrmProperties/TabCtrl/Page3/DeleteTextureButton"));
	RemoveButton->setEnabled(false) ;
	
	onUptadeMaterialProperties();

	return true;
}
//-------------------------------------------------------------------------------------
bool MiiModule::handleTexturePositionScrollBar(const CEGUI::EventArgs& e)
{
	//get scroll bars positions :
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Scrollbar * tmpScrollBar = (CEGUI::Scrollbar *)(wmgr.getWindow((CEGUI::utf8*)"FrmProperties/TabCtrl/Page3/UTextureScrollBar"));
	float U = tmpScrollBar->getScrollPosition();
	tmpScrollBar = (CEGUI::Scrollbar *)(wmgr.getWindow((CEGUI::utf8*)"FrmProperties/TabCtrl/Page3/VTextureScrollBar"));
	float V = tmpScrollBar->getScrollPosition();

	//set correct edit box :
	CEGUI::Editbox *text  =  NULL;
	text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/UTextureEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(U));
    text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/VTextureEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(V));

	//Apply position to the texture :
	Object3D * obj = mSelection->getFirstSelectedObject();
	obj->setTextureScroll( U, V);
	
	return true ;
}
//-------------------------------------------------------------------------------------
bool MiiModule::handleTextureScaleScrollBar(const CEGUI::EventArgs& e)
{
	//get scroll bars positions :
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();
	CEGUI::Scrollbar * tmpScrollBar = (CEGUI::Scrollbar *)(wmgr.getWindow((CEGUI::utf8*)"FrmProperties/TabCtrl/Page3/ScaleUTextureScrollBar"));
	float U = tmpScrollBar->getScrollPosition();
	tmpScrollBar = (CEGUI::Scrollbar *)(wmgr.getWindow((CEGUI::utf8*)"FrmProperties/TabCtrl/Page3/ScaleVTextureScrollBar"));
	float V = tmpScrollBar->getScrollPosition();

	//set correct edit box :
	CEGUI::Editbox *text  =  NULL;
	text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/ScaleUTextureEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(U*2));
    text = (CEGUI::Editbox*)CEGUI::WindowManager::getSingleton().getWindow("FrmProperties/TabCtrl/Page3/ScaleVTextureEdit");
	text->setText(CEGUI::PropertyHelper::floatToString(V*2));

	//Apply position to the texture :
	Object3D * obj = mSelection->getFirstSelectedObject();
	obj->setTextureScale( U*2, V*2);
	
	return true ;
}