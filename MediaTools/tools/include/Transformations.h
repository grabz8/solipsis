#ifndef __Transformation_h__
#define __Transformation_h__

#include <Ogre.h>
#include <OIS/OIS.h>
#include <String>

using namespace Ogre;
using namespace std;


class Transformations
{
public:
	Transformations(void);
	~Transformations(void);

	///Create gizmos for move, rotate and scale
	///	Call this function before use the others
	void createGizmos(SceneNode* pGizmosParentNode, SceneManager * pSceneMgr, Camera* pCamera);
									
	///Event set mode :
	void eventSelection();
	void eventMove();
	void eventRotation();
	void eventScale();

	///Attach the goods plans and update mOldPos :
	void firstClickForTransformation( const OIS::MouseEvent &e);
	///Detach plans
	void releasedClickForTransformation ( const OIS::MouseEvent &e);
	///Calcul the new vector for the selected object
	///	Next you have to apply this on the selection and test mMode for know
	///	if we are in MOVE, ROTATE or SCALE	
	Vector3 drapNdrop( const OIS::MouseEvent &e ) ;

	///Show Gizmos
	void showGizmosMove(bool pShow);
	void showGizmosScale(bool pShow);
	void showGizmosRotate(bool pShow);

	enum Mode {
		SELECT,
		MOVE,
		ROTATE,
		SCALE };
	///Return the mode choosen by the user :
	Mode getMode() ;

	enum AxeClicked {
		NONE,
		CAMERA,		//Plane for camera only
		X,
		Y,
		Z };

private :
	//Gizmos :
	//--------------
	void createGizmosMove(SceneManager * pSceneMgr);
	void createGizmosScale(SceneManager * pSceneMgr);
	void createGizmosRotate(SceneManager * pSceneMgr);

	SceneNode* m_GizmosAxesNode;
	//Node for attach Gizmos :
	SceneNode * mNode_X;
	SceneNode * mNode_Y;
	SceneNode * mNode_Z;
	//Object for gizmos :
		//Gizmos move :
	Entity* m_moveX; 
	Entity* m_moveY; 
	Entity* m_moveZ;
		//Gizmos scale :
	Entity* m_scaleX ; 
	Entity* m_scaleY ; 
	Entity* m_scaleZ ;
		//Gizmos rotate :
	Entity* m_rotateX ;
	Entity* m_rotateY ;
	Entity* m_rotateZ ;

	//Attach or dettach Gizmos
	void attachMoveGizmos(bool pAttach) ;				//if true->attach else->detach
	void attachScaleGizmos(bool pAttach) ;
	void attachRotateGizmos(bool pAttach) ;

	//Dummy plane :
	Entity * mPlaneX;
	Entity * mPlaneY;
	Entity * mPlaneZ;

	///When the user click on an axe (X, Y, Z), this function detect this axe and
	///	put the result in mAxeClicked :
	void onClickToTransformObject(const OIS::MouseEvent &e, const String pNameAxeX,
								const String pNameAxeY, const String pNameAxeZ );	

	///Ray tracing :
	RaySceneQueryResult& raySceneQuery( const OIS::MouseEvent &e );
	RaySceneQuery *mRaySceneQuery;		// The ray scene query pointer

	///Give the mouse position (relative to the good dummy plane)
	Vector3 getMousePosOnDummyPlane (const OIS::MouseEvent &e) ;


	///For remember the first axe and the first plane clicked by the user
	AxeClicked mAxeClicked ;
	AxeClicked mPlaneClicked ;


	///Transformations :
	///-------------------------
	Mode mMode ;		//SELECT, MOVE, ROTATE, or SCALE
	Vector3 mOldpos;

	Camera* mCamera;

};

#endif //__Transformation_h__