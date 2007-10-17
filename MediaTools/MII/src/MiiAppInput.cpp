#include "MiiApp.h"
#include "MiiModule.h"
#include "Camera.h"
#include "Selection.h"
#include "Transformations.h"
#include "ChooseColorWindow.h"

bool MiiApp::mouseMoved( const OIS::MouseEvent &e ) 
{
	using namespace OIS;

	// ray listener ...
	// check if the cursor is over a axe / plan transformation
	{}
	if (e.state.buttonDown(MB_Left))
	{
		SceneNode * node ;
		if( false )			// area selection
		{}
		else				// transformation : move / rotate / scale
		{
			if(! mSelection->empty_selection())
			{
				///Calculate drag and drop :
				Vector3 dragNdrop = mSelection->mTransformation->drapNdrop(e);
				if( dragNdrop != Vector3::ZERO)
				{
					//Apply the transformation
					node = mSceneMgr->getSceneNode("NodeSelection");
					switch(mSelection->mTransformation->getMode() )
					{
						case Transformations::Mode::MOVE :	//Move objects
						{
							mSelection->move( dragNdrop.x, dragNdrop.y, dragNdrop.z ); //move objects
							node->translate( dragNdrop.x, dragNdrop.y, dragNdrop.z );	//move gizmos 
							break;
						}
						case Transformations::Mode::ROTATE :	//Rotate objects
						{
							mSelection->rotate( dragNdrop.z/2, dragNdrop.x/2, -dragNdrop.y/2 ); //rotate objects
							break;
						}
						case Transformations::Mode::SCALE :	//Scale objects, but not gizmos
						{
							mSelection->scale(dragNdrop.x , dragNdrop.y , dragNdrop.z);
							break;
						}
					}
				}
			}
		}
	}
	else if (e.state.buttonDown(MB_Middle))
	{
		if( mKey_LALT )		//ArcBall around the selection
		{
			mCameraManagement->rotateCamera( (-mRotate * mMouse_Speed * e.state.X.rel) ,
											 (-mRotate * mMouse_Speed * e.state.Y.rel) ,
											  mSelection->getCenterPosition() ); 
		}
		else	//no key down
		{		// ... so translate camera
			Vector3 mouvement (- (e.state.X.rel )* mMouse_Speed, (e.state.Y.rel)* mMouse_Speed ,	0);
			mCameraManagement->translateCamera( mouvement.x , mouvement.y , mouvement.z );
		}
	}
	else if (e.state.buttonDown(MB_Right))
	{
	}
	else if ((e.state.X.rel == 0) && (e.state.Y.rel == 0) )	//Wheel ...
	{
		mCameraManagement->zoomCamera(- e.state.Z.rel) ;
	}


	// CEGUI
	mii->mouseMoved(e);

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiApp::mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id ) 
{
	// CEGUI
	if(mii->mousePressed(e,id))
		return true;

	using namespace OIS;
	switch ( id )
	{
	case MB_Left:	//Select Object and apply transformations
		{	
			if (mii->isPropertiesFrameOpened())
				return true;

			selectObject(e) ;
			//put the gizmos in the centre of selection
			mSceneMgr->getSceneNode("NodeSelection")->setPosition( mSelection->getCenterPosition() );

			if(mSelection->getNumSelectedObjects() != 0)
			{
				mSelection->mTransformation->firstClickForTransformation(e) ;
			}
			else
			{
				mSelection->mTransformation->eventSelection() ;
			}
		}
		break;

	case MB_Middle :	//Move Camera
		break ;
	}

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiApp::mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id ) 
{ 

	// CEGUI
	if(mii->mouseReleased(e,id))
		return true;

	switch ( id )
	{
		case OIS::MB_Left:	//Select Object and apply transformations
		{
			mii->mModeLink = false ;
			mSelection->updateBackup() ;
			mSelection->mTransformation->releasedClickForTransformation( e) ;
			break;
		}
		case OIS::MB_Middle :
		{
			break;
		}
	}

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiApp::keyPressed( const OIS::KeyEvent &e ) 
{
	// CEGUI
	if (mii->keyPressed(e))
		return true; // Message has been caught by a 2D interface

	using namespace OIS;
	switch ( e.key ) 
	{
	case KC_ESCAPE: 
		mContinue = false;
		break;
	case KC_HOME :
		mCameraManagement->resetCamera() ;
		break ;
	case KC_UP:
	case KC_W:
		mDirection.z -= mMove;
		break;

	case KC_DOWN:
	case KC_S:
		mDirection.z += mMove;
		break;

	case KC_LEFT:
	case KC_A:
		mDirection.x -= mMove;
		break;

	case KC_RIGHT:
	case KC_D:
		mDirection.x += mMove;
		break;

	case KC_PGDOWN:
	case KC_E:
		mDirection.y -= mMove;
		break;

	case KC_PGUP:
	case KC_Q:
		mDirection.y += mMove;
		break;

	case KC_LCONTROL:
		mKey_LCTRL = true;
		mSelection->set_lock(true);
		break;

	case KC_LMENU:
		mKey_LALT = true;
		break;

	case KC_NUMPAD4 :
		mCameraManagement->rotateCamera( 10, 0, mSelection->getCenterPosition() );
		break ;
	case KC_NUMPAD6 :
		mCameraManagement->rotateCamera( -10, 0, mSelection->getCenterPosition() );
		break ;
	case KC_NUMPAD8:
		mCameraManagement->rotateCamera( 0, -10, mSelection->getCenterPosition() );
		break;
	case KC_NUMPAD5 :
		mCameraManagement->rotateCamera( 0, 10, mSelection->getCenterPosition() );
		break;
	}

	return true;
}

//-------------------------------------------------------------------------------------
bool MiiApp::keyReleased( const OIS::KeyEvent &e ) 
{ 
	// CEGUI
	if (mii->keyReleased(e))
		return true;

	using namespace OIS;
	switch ( e.key ) 
	{
	case KC_UP:
	case KC_W:
		mDirection.z = 0;
		break;

	case KC_DOWN:
	case KC_S:
		mDirection.z = 0;
		break;

	case KC_LEFT:
	case KC_A:
		mDirection.x = 0;
		break;

	case KC_RIGHT:
	case KC_D:
		mDirection.x = 0;
		break;

	case KC_PGDOWN:
	case KC_E:
		mDirection.y = 0;
		break;

	case KC_PGUP:
	case KC_Q:
		mDirection.y = 0;
		break;

	case KC_LCONTROL:
		mKey_LCTRL = false;
		mSelection->set_lock(false);
		break;

	case KC_LMENU:
		mKey_LALT = false;
		break;
	}

	return true;
}

//-------------------------------------------------------------------------------------
RaySceneQueryResult& MiiApp::raySceneQuery( const OIS::MouseEvent &e )
{
	// Setup the ray scene query, use CEGUI's mouse position
	CEGUI::Point mousePos = CEGUI::MouseCursor::getSingleton().getPosition();
	Ray mouseRay = mCamera->getCameraToViewportRay( 
		mousePos.d_x/float(e.state.width), 
		mousePos.d_y/float(e.state.height) );
	mRaySceneQuery->setRay( mouseRay );
	mRaySceneQuery->setSortByDistance( true, 5 );

	return mRaySceneQuery->execute();
}

//-------------------------------------------------------------------------------------
void MiiApp::selectObject(const OIS::MouseEvent &e)
{
	RaySceneQueryResult &result = raySceneQuery(e);// Execute query

	RaySceneQueryResult::iterator itrRSQR =  result.begin( ); //+1
	bool clickOnGizmo = false ;

	if( ! result.empty() )
	{
		Ogre::String name;
		//Test if we click on a gizmo...
		for(itrRSQR =result.begin() ; itrRSQR!=result.end() ;itrRSQR++)
		{
			name = itrRSQR->movable->getName();
			if( (name == "moveX") ||  (name == "moveY") || (name == "moveZ") ||
				(name == "scaleX") ||  (name == "scaleY") || (name == "scaleZ") ||
				(name == "rotateX") ||  (name == "rotateY") || (name == "rotateZ") )
			{
				 clickOnGizmo = true ;
				break;
			}
		}
		//if we have clicked on a gizmo, we don't modify the selection
		if( !clickOnGizmo)
		{
			if(!(mii->mModeLink))
				if( !mKey_LCTRL )	//clear selection if CTRL is not down
					mSelection->deselect_all();	

			itrRSQR = result.begin( ) ;//+1;
			for ( ;itrRSQR != result.end(); itrRSQR++)	//search the first entity object
			{
				if( itrRSQR->movable->getMovableType() == "Entity" ) //add or delete to the current selection
				{
					String name = itrRSQR->movable->getName();
					Entity* ent = mSceneMgr->getEntity(name);
					
					if(mii->mModeLink)	//if mode Link...
					{
						Object3D * obj = mSelection->get3DObject( ent );
						if (obj != NULL)	//if it is a selectionnable object ...
							if( obj != mSelection->getFirstSelectedObject()) //if it is not itself ...
								mSelection->getFirstSelectedObject()->linkObject( obj, mSceneMgr);
					}
					else				//if mode Selection...
					{
						mSelection->clickNode(ent);
					}
					return ;
				}
			}
		}
	}
	else if( !mKey_LCTRL )	//clear selection
	{
		mSelection->deselect_all();	
	}

	mRaySceneQuery->clearResults();
}



//-------------------------------------------------------------------------------------
