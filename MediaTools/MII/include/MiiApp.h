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
	///brief For select an object with the mouse
	void selectObject (const OIS::MouseEvent &e);	

	///brief Make raytracing and retrun the result :
	RaySceneQueryResult& raySceneQuery( const OIS::MouseEvent &e );

protected:
	MiiModule* mii;						///brief The class for interface
	RaySceneQuery *mRaySceneQuery;		///brief The ray scene query pointer
	Selection* mSelection;				///brief The newly created object

	
private:
	///brief TRUE if the key CTRL is down
	bool mKey_LCTRL;
	///brief TRUE if the key ALT is down
	bool mKey_LALT;

	///brief Class for manage camera
	CCamera * mCameraManagement;

	///brief Mouse's parameters 
	float mMouse_Speed ;

	///brief The old Mouse position
	Vector3 mOldpos ;

	///brief Vector3 to know the value of current transformation (Rotate, Move, Scale) :
	Vector3 mTransfo ;

};

#endif //__MiiApp_h__