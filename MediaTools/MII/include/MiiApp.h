#ifndef __MiiApp_h__
#define __MiiApp_h__

#include <Application.h>

class Selection;
class CCamera ;
class MiiModule;

namespace CEGUI {
	class EventArgs;
}

class MiiApp : public Application
{
public:
	MiiApp(void);
	~MiiApp();

	void init(const char* resName);

protected:
	void createScene(void);
	void destroyScene(void);
	bool frameStarted(const FrameEvent& evt);

	//---------------------------------------------------------------------------------------------
	//Input funcitons :
	bool mouseMoved( const OIS::MouseEvent &e );
	bool mousePressed( const OIS::MouseEvent &e, OIS::MouseButtonID id );
	bool mouseReleased( const OIS::MouseEvent &e, OIS::MouseButtonID id );
	bool keyPressed( const OIS::KeyEvent &e );
	bool keyReleased( const OIS::KeyEvent &e );

	// Tools menu part 
	bool reconfigureDisplay(const CEGUI::EventArgs& e);
	bool handleFileExit(const CEGUI::EventArgs& e); 

	//----------------------------------------------------------------------------------------------
	// Functions with mouse for Select, Move, Rotate and Scale
	void selectObject (const OIS::MouseEvent &e);			//Select an object with the mouse

	RaySceneQueryResult& raySceneQuery( const OIS::MouseEvent &e );			//***

protected:
	MiiModule* mii;
	RaySceneQuery *mRaySceneQuery;		// The ray scene query pointer		//***
	Selection* mSelection;				// The newly created object

	
private:
	bool mKey_LCTRL;
	bool mKey_LALT;

	CCamera * mCameraManagement;

	//mouse's parameters:
	float mMouse_Speed ;

	Vector3 mOldpos ;

};

#endif //__MiiApp_h__