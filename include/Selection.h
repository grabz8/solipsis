/**
	\file 
		Selection.h
	\brief
		Solipsis class for manage the selection of objects 3D
	\author
		ARTEFACTO - Patrice DESFONDS & Stéphane CHAPLAIN
*/


#ifndef __Selection_h__
#define __Selection_h__

#include <Ogre.h>
#include <list>
#include "Object3D.h"

namespace Solipsis {

using namespace Ogre;
using namespace std;

class Object3D;
class Transformations ;

typedef  std::list<Object3D *> Object3DPtrList;
typedef	 std::list<Object3D *>::iterator Object3DPtrListIterator;

class Selection
{
public:
/*
 *
 *	\brief 
 *		Constructor
 * 
 *		This class uses this hierarchy :
 *
 *								mSceneMgr
 *								/		\
 *							   /		 \
 *				NodeCentreRotation		NodeSelection
 *						/					\
 *					   /					 \
 *			NodeCentreObject			Gizmos of Transformation
 *									 (see the class Transformation)
 *
 *
 *	\param pCentreRotation = Node for calculate the rotation of multi-selection. 
 *								It must be a child of Ogre::SceneManager
 *	\param pCentreObject = Node for calculate the rotation of multi-selection.
 *								It must be a child of pCentreRotation
 *
*/
	Selection(SceneNode * pCentreRotation, SceneNode * pCentreObject);

/*
 *
 *	\brief 
 *		Destructor
 *
*/
	~Selection();

/*
 *
 *	\brief 
 *		Select or deselect (if it is already selected) an object. This function show or hide the bounding box.
 *		Call this function when user has clicked on an entity.
 *
 *	\param pEnt = Entity clicked by user
 *	\return Always return TRUE
*/
	bool clickNode(Entity* pEnt);

/*
 *
 *	\brief 
 *		Deselect all selection and hide bounding box
 *
*/
	void deselect_all();

/*
 *
 *	\brief 
 *		Set a lock for the selection. (for exemple, it is lock on when user pressed the key CTRL)
 *
 *	\param p_lock = TRUE for lock on
 *
*/
	void set_lock(bool p_lock);

/*
 *
 *	\brief 
 *		Get the centre of the selection
 *
 *	\return the centre of the selection. If no object are selected, return (0,0,0).
 *
*/	
	Vector3	getCenterPosition();			

/*
 *
 *	\brief 
 *		translate selection
 *
 *	\param pValueX, pValueY, pValueZ = values representing the translation
 *
*/	
	void move (float pValueX, float pValueY, float pValueZ);

/*
 *
 *	\brief 
 *		Scale selection
 *
 *	\param pValueX, pValueY, pValueZ = values representing the scale transformation
 *
*/	
	void scale (float pValueX, float pValueY, float pValueZ);

/*
 *
 *	\brief 
 *		rotate selection
 *
 *	\param pValueX, pValueY, pValueZ = values representing the rotation transformation
 *
*/
	void rotate (float pValueX, float pValueY, float pValueZ);

/*
 *
 *	\brief 
 *		Get if the selection is empty
 *
 *	\return TRUE if the selection is empty, else return FALSE.
 *
*/
	bool isEmpty();

/*
 *
 *	\brief 
 *		clear the selection list of Objects 3D
 *
*/
	void clearObjects();

/*
 *
 *	\brief 
 *		Add an object to the scene
 *
 *	\param pEnt = A pointer to the object to add
 *
*/
	void add3DObject(Object3D *pObj,bool selectionnable = true);

/*
 *
 *	\brief 
 *		Delete an object
 *
 *	\param pEnt = A pointer to object to remove
 *
*/
	void remove3DObject(Object3D *pObj);

/*
 *
 *	\brief 
 *		Get an object by Ogre entity
 *
 *	\param pEnt = A pointer to the entity to retrieve
 *	\return a pointer of Objetc3D corresponding with pEnt
 *
*/
	Object3D* get3DObject(Entity *pEnt);

/*
 *
 *	\brief 
 *		Get an object by Ogre entity
 *
 *	\param pEnt = A pointer to the entity to retrieve
 *	\return a pointer of Objetc3D corresponding with pEnt
 *
*/
	Object3D* get3DObject(const String pName);


/*
 *
 *	\brief 
 *		Get the first selected object
 *
 *	\return a pointer of Objetc3D corresponding with the first selected Object 
 *
*/
	Object3D* getFirstSelectedObject();

/*
 *
 *	\brief 
 *		Get the next selected object
 *
 *	\return a pointer of Objetc3D corresponding with the next selected Object 
 *
*/
	Object3D* getNextSelectedObject();

/*
 *
 *	\brief 
 *		Get the number of selected objects
 *
 *	\return the number of selected objects
 *
*/
	size_t getNumSelectedObjects();

/*
 *
 *	\brief 
 *		Get the last inserted object in the list
 *
 *	\return a pointer of Objetc3D corresponding with the last inserted object in the list
 *
*/
	Object3D* geLastAddedObject();

/*
 *
 *	\brief 
 *		Get the list of object present in the scene.
 *
 *	\return the list of Object present in the scene.
*/
	const Object3DPtrList getObjectList();

/*
 *
 *	\brief 
 *		call updateBackup() for all object3D in the selection
 *
*/
	void updateBackup();


	///brief Class for manage all transformations
	Transformations * mTransformation ;

private:
	SceneNode *			mCentreRotation ;	/// brief Node to apply rotation
	SceneNode *			mCentreObject ;		/// brief Node to apply rotation
	bool mLock;								///	biref Boolean for know if the lock is on or not

	
	Object3DPtrList		mListNode;			/// brief The current selection list
	Object3DPtrList		mObjectList;		///	biref List of all selectionnable objects
	
	Object3DPtrListIterator	mCurrentObject;	///	biref The current selected node in the list 

/*
 *
 *	\brief 
 *		Private function for calculate the rotation of multi-selection.
 *		Put mCentreRotation and mCentreObject on their correct position
 *			mCentreRotation on the centre of selection
 *			mCentreObject on its position after rotation
 *
 *	\param pNode = current node to apply rotation
 *	\param pValueX, pValueY, pValueZ = Values for rotation
 *	\param pCentreSelection = 3D point for the centre of selection
*/
	void findRotationPosition(SceneNode * pNode, float pValueX, float pValueY, float pValueZ, Vector3 pCentreSelection);


};

} //namespace 

#endif //__Selection_h__