/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author ARTEFACTO

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

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
    mListNodeSinceLastSave.clear();
	mObjectList.clear();

	mTransformation = new Transformations () ;
}

//-------------------------------------------------------------------------------------
Selection::~Selection()
{
	mListNode.clear();
    mListNodeSinceLastSave.clear();
    clearObjects();
	delete mTransformation;
}

//-------------------------------------------------------------------------------------
bool Selection::selectObject3D(Object3D* pObj)
{
	Object3DPtrListIterator itr;

    if (!pObj)
		return true;

	//test if pObj is a child :
	while(pObj->getParent() != NULL )
	{
		pObj = pObj->getParent() ;
	}
	//... pObj is the parent !

	if( !mLock )
		deselect_all();

    // Add this object into the selected objects list
    // Show the bounding box to highlight the selected object
    for( itr = mListNode.begin(); itr != mListNode.end(); ++itr)
        if ((*itr) == pObj)
            break; 
    if (itr == mListNode.end())
    {
        // add the selected object to the selection
        pObj->showBoundingBox(true) ;
        mListNode.push_back(pObj);
    }
    else
    {
        // remove this object from the list
        pObj->showBoundingBox(false) ;
        mListNode.erase(itr);
    }

    // Add this object into the selected objects list since last save
    for( itr = mListNodeSinceLastSave.begin(); itr != mListNodeSinceLastSave.end(); ++itr)
        if ((*itr) == pObj)
            break; 
    if (itr == mListNodeSinceLastSave.end())
        mListNodeSinceLastSave.push_back(pObj);

	return true;
}

//-------------------------------------------------------------------------------------
bool Selection::clickNode(Entity* pEnt)
{
	Object3D* obj = get3DObject(pEnt) ;

	return selectObject3D(obj);
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
            //(*itr)->apply( Object3D::SCALE, pValueX, pValueY, pValueZ );



			Vector3 vec = (*itr)->getScale();
            if( abs(vec.x - pValueX) > 0 ) vec.x = pValueX;
            if( abs(vec.y - pValueY) > 0 ) vec.y = pValueY;
            if( abs(vec.z - pValueZ) > 0 ) vec.z = pValueZ;
			(*itr)->apply( Object3D::SCALE, vec.x, vec.y, vec.z );
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
	Object3DPtrList::iterator itObj = mObjectList.end();
    if (itObj == mObjectList.begin())
        return NULL;

	itObj--;
	return (*itObj);
}

//-------------------------------------------------------------------------------------
void Selection::remove3DObject(Object3D *pObj)
{
	// First remove object from selectionnable list
    for (Object3DPtrList::iterator itObj = mObjectList.begin(); itObj != mObjectList.end(); ++itObj)
		if ((*itObj) == pObj)
		{
			// Delete the object		
			delete (*itObj);
			// Found it we can erase the element and go out
			mObjectList.erase(itObj);
			break;
		}

	// Second => Remove it from current selection list
    for (Object3DPtrList::iterator itObj = mListNode.begin(); itObj != mListNode.end(); ++itObj)
		if ((*itObj) == pObj)
		{
			// Found it we can erase the element and go out
			mListNode.erase(itObj);
			mCurrentObject = mListNode.begin();
			break;
		}

	// Third => Remove it from selected list since last save
    remove3DObjectFromListSinceLastSave(pObj);
}

//-------------------------------------------------------------------------------------
void Selection::remove3DObjectFromListSinceLastSave(Object3D *pObj)
{
    for (Object3DPtrList::iterator itObj = mListNodeSinceLastSave.begin(); itObj != mListNodeSinceLastSave.end(); ++itObj)
		if ((*itObj) == pObj)
		{
			// Found it we can erase the element and go out
			mListNodeSinceLastSave.erase(itObj);
			break;
		}
}

//-------------------------------------------------------------------------------------
Object3D* Selection::get3DObject(Entity *pEnt)
{
    for (Object3DPtrList::iterator itObj = mObjectList.begin(); itObj != mObjectList.end(); ++itObj)
		if ((*itObj)->getEntity() == pEnt)
			return (*itObj);

    return NULL;
}
//-------------------------------------------------------------------------------------
Object3D* Selection::get3DObject(const EntityUID& pUid)
{
    for (Object3DPtrList::iterator itObj = mObjectList.begin(); itObj != mObjectList.end(); ++itObj)
        if ((*itObj)->getEntityUID() == pUid)
			return (*itObj);

	return NULL;
}
//-------------------------------------------------------------------------------------
void Selection::clearObjects()
{
	// Delete objects
    while (!mObjectList.empty())
    {
        Object3DPtrList::iterator itObj = mObjectList.begin();
		delete (*itObj);
		mObjectList.erase(itObj);
    }
}

//-------------------------------------------------------------------------------------
void Selection::clearSavedObjects()
{
    mListNodeSinceLastSave.clear();
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
void Selection::findRotationPosition(SceneNode * pNode, float pValueX, float pValueY, float pValueZ, Vector3 pCentreSelection)
{
	mCentreRotation->setOrientation( mCentreRotation->getInitialOrientation() );
	mCentreObject->setOrientation( mCentreObject->getInitialOrientation() );
	mCentreObject->setPosition( mCentreObject->getInitialPosition() );

	mCentreRotation->setPosition( pCentreSelection ) ;
#if (OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR < 6)
	Vector3 ObjectPosition = pNode->getWorldPosition() - mCentreRotation->getWorldPosition() ;
#else
	Vector3 ObjectPosition = pNode->_getDerivedPosition() - mCentreRotation->_getDerivedPosition() ;
#endif
	mCentreObject->translate( ObjectPosition ) ;
	//... mCentreRotation and mCentreObject are correctly positionned

	mCentreRotation->pitch(Degree(pValueX));
	mCentreRotation->yaw(Degree(pValueY));
	mCentreRotation->roll(Degree(pValueZ));

	//Now we put pNode on mCentreObject ...
#if (OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR < 6)
	ObjectPosition = mCentreObject->getWorldPosition();
#else
	ObjectPosition = mCentreObject->_getDerivedPosition();
#endif

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
size_t Selection::getNumSelectedObjects()
{
	return mListNode.size();
}

//-------------------------------------------------------------------------------------
const Object3DPtrList& Selection::getSelectedObjectList()
{
	return mListNode;
}

//-------------------------------------------------------------------------------------
const Object3DPtrList& Selection::getSelectedObjectListSinceLastSave()
{
	return mListNodeSinceLastSave ;
}

//-------------------------------------------------------------------------------------
const Object3DPtrList& Selection::getObjectList()
{
	return mObjectList ;
}

};