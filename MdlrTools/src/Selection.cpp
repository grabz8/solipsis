#include "Selection.h"
#include "Object3D.h"
#include "Transformations.h"


using namespace std;

namespace Solipsis {

//-------------------------------------------------------------------------------------
Selection::Selection(SceneNode * pCentreRotation, SceneNode * pCentreObject)
	:mLock(false), mCentreRotation(pCentreRotation) , mCentreObject(pCentreObject) 
{
	mListNode.clear();
	mObjectList.clear();

	mTransformation = new Transformations () ;
}

//-------------------------------------------------------------------------------------
Selection::~Selection()
{
	mListNode.clear();
	delete mTransformation ;
}

//-------------------------------------------------------------------------------------
bool Selection::clickNode(Entity* pEnt)
{
	Object3DPtrListIterator itr;
	Object3D* obj = get3DObject(pEnt) ;
	if (!obj)
		return true;
		
	//test if obj is a child :
	while(obj->getParent() != NULL )
	{
		obj = obj->getParent() ;
	}
	//... Obj is the parent !

	if( !mLock )
		deselect_all();

	// Show the bounding box to highlight the selected object
	if ( !mListNode.empty() )		// some objects have already been selected
	{
		bool found = false;
		// test if this object has already benn added to the list
		for( itr = mListNode.begin(); itr != mListNode.end(); itr++ )
		{
			if((*itr) == obj)
			{ 
				found = true; 
				break; 
			}
		}
		
		if( found )		
		{
			// remove this object from the list
			obj->showBoundingBox(false) ;
			itr = mListNode.erase(itr);
		} 
		else
		{
			// add the selected object to the selection
			obj->showBoundingBox(true) ;
			mListNode.push_back(obj);
		}
	}
	else	// no objects have already been selected
	{
		// add the selected object to the selection
		obj->showBoundingBox(true);
		mListNode.push_back(obj);
	}

	return true;
}

//-------------------------------------------------------------------------------------
void Selection::deselect_all()
{
	// Turn off bounding box and clear selection
	if( !mListNode.empty() )
	{
		Object3DPtrListIterator itr;

		for( itr = mListNode.begin(); itr != mListNode.end(); itr++ )
			(*itr)->showBoundingBox(false);
		mListNode.clear();
	}
}

//-------------------------------------------------------------------------------------
void Selection::set_lock(bool p_lock)
{
	mLock = p_lock ;
}

//-------------------------------------------------------------------------------------
Vector3	Selection::getCenterPosition()
{
	Vector3 tmp (0,0,0);
	if( getNumSelectedObjects() != 0 )//!mListNode.empty() )
	{
		Object3DPtrListIterator itr;
		//Object3D *Debug;

		for( itr = mListNode.begin(); itr != mListNode.end(); itr++ )
		{
			//Debug = (*itr);
			tmp += (*itr)->getPosition();
		}
		
		tmp /= mListNode.size();
	}

	return tmp ;
}

//-------------------------------------------------------------------------------------
void Selection::move (float pValueX, float pValueY, float pValueZ)
{
	if( !mListNode.empty() )
	{
		Object3DPtrListIterator itr;

		for( itr = mListNode.begin(); itr != mListNode.end(); itr++ )
			(*itr)->apply( Object3D::TRANSLATE, pValueX, pValueY, pValueZ );
	}
}
//-------------------------------------------------------------------------------------
void Selection::scale (float pValueX, float pValueY, float pValueZ)
{
	if( !mListNode.empty() )
	{
		Object3DPtrListIterator itr;

		for( itr = mListNode.begin(); itr != mListNode.end(); itr++ )
			//get3DObject((*itr)->getEntity())->scale(pValueX, pValueY, pValueZ);		
			(*itr)->apply( Object3D::SCALE, pValueX, pValueY, pValueZ );
	}
}
//-------------------------------------------------------------------------------------
void Selection::rotate (float pValueX, float pValueY, float pValueZ)
{
	if( !mListNode.empty() )
	{
		Object3DPtrListIterator itr;

		Vector3 centreSelection = getCenterPosition() ;

		for( itr = mListNode.begin(); itr != mListNode.end(); itr++ )
			//get3DObject((*itr)->getEntity())->rotate( pValueX, pValueY, pValueZ, centreSelection, mCentreRotation, mCentreObject) ; 
			(*itr)->apply( Object3D::ROTATE, pValueX, pValueY, pValueZ );
	}
}
//-------------------------------------------------------------------------------------
void Selection::moveTo (float pValueX, float pValueY, float pValueZ)
{
	if( !mListNode.empty() )
	{
		Object3DPtrListIterator itr;

		Vector3 vec = getCenterPosition() ;

		for( itr = mListNode.begin(); itr != mListNode.end(); itr++ )
			(*itr)->apply( Object3D::TRANSLATE, pValueX-vec.x, pValueY-vec.y, pValueZ-vec.z );
	}
}
//-------------------------------------------------------------------------------------
void Selection::scaleTo (float pValueX, float pValueY, float pValueZ)
{
	if( !mListNode.empty() )
	{
		Object3DPtrListIterator itr;

		for( itr = mListNode.begin(); itr != mListNode.end(); itr++ )
		{
			//Vector3 vec = (*itr)->getScale();
			//get3DObject((*itr)->getEntity())->scale(pValueX, pValueY, pValueZ);		
			(*itr)->apply( Object3D::SCALE, pValueX, pValueY, pValueZ );
			//(*itr)->apply( Object3D::SCALE, vec.x-pValueX, vec.y-pValueY, vec.z-pValueZ );
		}
	}
}
//-------------------------------------------------------------------------------------
void Selection::rotateTo (float pValueX, float pValueY, float pValueZ)
{
	if( !mListNode.empty() )
	{
		Object3DPtrListIterator itr;
		static Vector3 vec = Vector3::ZERO;

		//Vector3 centreSelection = getCenterPosition() ;

		for( itr = mListNode.begin(); itr != mListNode.end(); itr++ )
			//get3DObject((*itr)->getEntity())->rotate( pValueX, pValueY, pValueZ, centreSelection, mCentreRotation, mCentreObject) ; 
			(*itr)->apply( Object3D::ROTATE, pValueX-vec.x, pValueY-vec.y, pValueZ-vec.z );

		vec = Vector3(pValueX, pValueY, pValueZ);
	}
}
//-------------------------------------------------------------------------------------
bool Selection::isEmpty()
{
	return (mListNode.empty());
}

//-------------------------------------------------------------------------------------
void Selection::add3DObject(Object3D *pObj,bool selectionnable)
{
	if (selectionnable)
		mObjectList.push_back(pObj);
}

//-------------------------------------------------------------------------------------
Object3D* Selection::geLastAddedObject()
{	
	// last iterator should not be valid
	std::list<Object3D *>::iterator itObj = mObjectList.end();
	itObj --;
	return (*itObj);
}


//-------------------------------------------------------------------------------------
void Selection::remove3DObject(Object3D *pObj)
{
	// First remove object from selectionnable list
	std::list<Object3D *>::iterator itObj = mObjectList.begin();
	while (itObj != mObjectList.end())
	{
		if ((*itObj) == pObj)
		{
			// Delete the object		
			delete (*itObj);
			// Found it we can erase the element and go out
			itObj = mObjectList.erase(itObj);
			break;
		}
		itObj++;
	}

	// Second => Remove it from current selection list
	itObj = mListNode.begin();
	while (itObj != mListNode.end())
	{
		if ((*itObj) == pObj)
		{
			// Found it we can erase the element and go out
			itObj = mListNode.erase(itObj);
			mCurrentObject = mListNode.begin();
			return;
		}
		itObj++;
	}

}

//-------------------------------------------------------------------------------------
Object3D* Selection::get3DObject(Entity *pEnt)
{
	std::list<Object3D *>::iterator itObj = mObjectList.begin();
	while (itObj != mObjectList.end())
	{
		if ((*itObj)->getEntity() == pEnt)
		{
			return (*itObj);
		}
		itObj++;
	}
	return NULL;
}
//-------------------------------------------------------------------------------------
Object3D* Selection::get3DObject(const String pName)
{
	std::list<Object3D *>::iterator itObj = mObjectList.begin();
	while (itObj != mObjectList.end())
	{
		if (strcmp ( (*itObj)->getName().c_str() , pName.c_str()) == 0 )
		{
			return (*itObj);
		}
		itObj++;
	}
	return NULL;
}
//-------------------------------------------------------------------------------------
void Selection::clearObjects()
{
	std::list<Object3D *>::iterator itObj = mObjectList.begin();
	while (itObj != mObjectList.end())
	{
		// Delete the object		
		delete (*itObj);
		// Found it we can erase the element and go out
		itObj = mObjectList.erase(itObj);
		itObj++;
	}
}
//-------------------------------------------------------------------------------------
Object3D* Selection::getFirstSelectedObject()
{
	if (mListNode.empty())
		// No selected object	
		return NULL;
	mCurrentObject = mListNode.begin();
	return (*mCurrentObject);
}
//-------------------------------------------------------------------------------------
Object3D* Selection::getNextSelectedObject()
{
	// go to the next object in the list
	mCurrentObject++;
	if (mCurrentObject == mListNode.end())
		// The end of the list
		return NULL;
	return (*mCurrentObject);
}
//-------------------------------------------------------------------------------------
size_t Selection::getNumSelectedObjects()
{
	return mListNode.size();
}
//-------------------------------------------------------------------------------------
void Selection::findRotationPosition(SceneNode * pNode, float pValueX, float pValueY, float pValueZ, Vector3 pCentreSelection)
{
	mCentreRotation->setOrientation( mCentreRotation->getInitialOrientation() );
	mCentreObject->setOrientation( mCentreObject->getInitialOrientation() );
	mCentreObject->setPosition( mCentreObject->getInitialPosition() );

	mCentreRotation->setPosition( pCentreSelection ) ;
	Vector3 ObjectPosition = pNode->getWorldPosition() - mCentreRotation->getWorldPosition() ;
	mCentreObject->translate( ObjectPosition ) ;
	//... mCentreRotation and mCentreObject are correctly positionned

	mCentreRotation->pitch(Degree(pValueX));
	mCentreRotation->yaw(Degree(pValueY));
	mCentreRotation->roll(Degree(pValueZ));

	//Now we put pNode on mCentreObject ...
	ObjectPosition = mCentreObject->getWorldPosition();
	pNode->setPosition( ObjectPosition );
}

//-------------------------------------------------------------------------------------
void Selection::updateBackup()
{
	if( getNumSelectedObjects() != 0 )//!mListNode.empty() )
	{
		Object3DPtrListIterator itr;
		for( itr = mListNode.begin(); itr != mListNode.end(); itr++ )
		{
			(*itr)->updateBackup();
		}
	}
}
//-------------------------------------------------------------------------------------
const Object3DPtrList Selection::getObjectList()
{
	return mObjectList ;
}

};