#ifndef __Selection_h__
#define __Selection_h__

#include <Ogre.h>
#include <list>
#include "Object3D.h"

using namespace Ogre;
using namespace std;

class Object3D;
class Transformations ;

typedef  std::list<Object3D *> Object3DPtrList;
typedef	 std::list<Object3D *>::iterator Object3DPtrListIterator;

class Selection
{
public:
	Selection(SceneNode * pCentreRotation, SceneNode * pCentreObject);
	~Selection();

	bool clickNode(Entity* pEnt);		//If pNode is already selected, this function deselects it
	void deselect_all();

	bool empty_selection();
	void set_lock(bool p_lock);

	///Return the centre of the selection
	///		if no object are selected, return 0,0,0
	Vector3	getCenterPosition();			

	void move (float pValueX, float pValueY, float pValueZ);
	void scale (float pValueX, float pValueY, float pValueZ);
	void rotate (float pValueX, float pValueY, float pValueZ);

	/// Test if the selection list is empty
	bool isEmpty();
	/// Test if the selection list is empty
	void clearObjects();
	/// Add an object to the scene
	/// param pEnt A pointer to the object to add
	void add3DObject(Object3D *pObj,bool selectionnable = true);
	/// Delete an object
	/// param pEnt A pointer to object to remove
	void remove3DObject(Object3D *pObj);
	/// brief Get an object by Ogre entity
	/// param pEnt A pointer to the entity to retrieve
	Object3D* get3DObject(Entity *pEnt);
	/// Get the first selected object
	Object3D* getFirstSelectedObject();
	/// Get the first selected object
	Object3D* getNextSelectedObject();
	/// Get the number of selected objects
	size_t getNumSelectedObjects();
	/// Get the last inserted object in the list
	Object3D* geLastAddedObject();

	/// brief call updateBackup() for all object3D in the selection
	void updateBackup();

	//Class for manage all transformation :
	Transformations * mTransformation ;

private:
	SceneNode * mCentreRotation ;	//...to apply rotation
	SceneNode * mCentreObject ;		//...to apply rotation
	bool mLock;

	/// The current selection list
	Object3DPtrList		mListNode;
	/// List of all selectionnable objects
	Object3DPtrList		mObjectList;
	/// The current selected node in the list 
	Object3DPtrListIterator	mCurrentObject;
	/// Put mCentreRotation and mCentreObject on their correct position
	///		mCentreRotation on the centre of selection
	///		mCentreObject on its position after rotation
	void findRotationPosition(SceneNode * pNode, float pValueX, float pValueY, float pValueZ, Vector3 pCentreSelection);


};

#endif //__Selection_h__