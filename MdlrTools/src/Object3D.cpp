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

#include "Object3D.h"
#include "ModifiedMaterialManager.h"
#include "ModifiedMaterial.h"
#include "VectorModifier.h"
#include "MeshModifier.h"
#include "FileBrowser.h"
#include "SolidBoolOp.h"

#include "SolipsisErrorHandler.h"

#include "tinyxml.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
Object3D::Object3D(const EntityUID& pEntityUID, const String& pName, SceneNode* pNode)
{
	mNode = pNode;
	mEntity = (Entity*)pNode->getAttachedObject(pEntityUID);

	// get the size for a vertex declaration (position, normal, colour, tex_coord ...)
	mVertexData = 
		mEntity->getMesh()->getSubMesh(0)->useSharedVertices ? 
		mEntity->getMesh()->sharedVertexData : mEntity->getMesh()->getSubMesh(0)->vertexData;
	mVertexDecl = mVertexData->vertexDeclaration->getVertexSize(0);

	// get the object datas
	//mVertex = 0;
	//mIndex = 0;
 	getDataFromBuffer( NULL, NULL );

	// create the (backup) buffers
	mBufPrim		= new Buffer();
	mBufBackup		= new Buffer();
	mBufCurrent		= new Buffer();

	// initialize the (backup) buffers
	mBufCurrent->vertexCount	= mBufBackup->vertexCount	= mBufPrim->vertexCount	= mVertexCount;
	mBufCurrent->indexCount		= mBufBackup->indexCount	= mBufPrim->indexCount	= mIndexCount;
	mBufPrim->vertex.resize(mVertexCount*mVertexDecl/4);
	mBufBackup->vertex.resize(mVertexCount*mVertexDecl/4);
	mBufCurrent->vertex.resize(mVertexCount*mVertexDecl/4);
	mBufPrim->index.resize(mIndexCount);
	mBufBackup->index.resize(mIndexCount);
	mBufCurrent->index.resize(mIndexCount);

	for( unsigned int i = 0; i < mVertexCount*mVertexDecl/4; i++ )
		mBufCurrent->vertex[i]	= mBufBackup->vertex[i]	= mBufPrim->vertex[i]	= mVertex[i];

	for( unsigned int i = 0; i < mIndexCount; i++ )
		mBufCurrent->index[i]	= mBufBackup->index[i]	= mBufPrim->index[i]	= mIndex[i];

	// get the object size max / min
	getSize( mSize, mCornerMin, mCornerMax );

	mBufCurrent->size		= mBufBackup->size		= mBufPrim->size		= mSize;
	mBufCurrent->cornerMax	= mBufBackup->cornerMax	= mBufPrim->cornerMax	= mCornerMax;
	mBufCurrent->cornerMin	= mBufBackup->cornerMin	= mBufPrim->cornerMin	= mCornerMin;

	mChildren = NULL;		// TODO : must be loaded from the .XML
	mParent = 0;		    // TODO : must be loaded from the .XML

    mEntityUID = pEntityUID;
	mName = pName;

	//mType = OTHER;
	resetParameters();

	mModifiedMaterialManager = new ModifiedMaterialManager() ;
    // TODO 
	const MaterialPtr& tmpMaterial = mEntity->getSubEntity(0)->getMaterial()->clone("Material" + mEntityUID);
	mEntity->getSubEntity(0)->setMaterialName( tmpMaterial->getName());
	mModifiedMaterialManager->initialise(tmpMaterial);
	TexturePtr PtrTexture = TextureManager::getSingleton().load( "default_texture.jpg", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	mModifiedMaterialManager->addTexture(PtrTexture);
	mModifiedMaterialManager->setCurrentTexture(PtrTexture );


	mCreatorName = mOwnerName = mGroupName = mDesc = mTags = "";

#ifdef WIN32
	char buffTmp[256];
	DWORD size = 256;
	if (GetUserName(buffTmp,&size))
		mCreatorName = mOwnerName = buffTmp;
#else
	std::cerr << "Automatically get the user Name not tested : getgid will do the job ?? << endl; 
#endif

	mCanBeModified = mCanBeCopied = false;
	mEnableCollision = mEnableGravity = false;
	mPrimitivesCount = 0;

	mCentreSelection = Vector3::ZERO;

	// Init the last command 
	mCommandLast = NONE;
}

//-------------------------------------------------------------------------------------
Object3D::~Object3D()
{
	while (mModifiedMaterialManager->getNbTexture() > 1)
        mModifiedMaterialManager->deleteTexture(mModifiedMaterialManager->getTexture(mModifiedMaterialManager->getNbTexture() - 1));

    // Cloned material should be freed when no more referenced
    // but here we will remove it from resources list in order to free
    // the resource name right now
    const MaterialPtr& clonedMaterial = mModifiedMaterialManager->getModifiedMaterial()->getOwner();
    MaterialManager::getSingleton().remove((ResourcePtr&)clonedMaterial);

    // Cloned mesh should be freed when no more referenced
    // but here we will remove it from resources list in order to free
    // the resource name right now
    const MeshPtr& clonedMesh = mEntity->getMesh();
	MeshManager::getSingleton().remove((ResourcePtr&)clonedMesh);
    mNode->detachObject(mEntity);
    mNode->getCreator()->destroyMovableObject(mEntity);
	mEntity = 0;
    mNode->getCreator()->destroySceneNode(mNode->getName());
	mNode = 0;
	delete mChildren;		mChildren = 0;

	delete mModifiedMaterialManager;
}

//-------------------------------------------------------------------------------------
int		Object3D::loadFromFile(TiXmlDocument &doc, string texturepath)
{
	Ogre::String extractedAttribute;

	TiXmlElement *e = doc.RootElement()->FirstChildElement("properties");
    mEntityUID = e->FirstChildElement("objuid")->Attribute("Uid");
	mName = e->FirstChildElement("objname")->Attribute("Name");
	mTags = e->FirstChildElement("objtags")->Attribute("Name");
	mDesc = e->FirstChildElement("objdesc")->Attribute("Name");
	mOwnerName = e->FirstChildElement("objowner")->Attribute("Name");
	mCreatorName = e->FirstChildElement("objcreator")->Attribute("Name");
	mGroupName = e->FirstChildElement("objgroup")->Attribute("Name");
	from_string(e->FirstChildElement("objrigths")->Attribute("mod"),mCanBeModified);
	from_string(e->FirstChildElement("objrigths")->Attribute("cop"),mCanBeCopied);

	e = doc.RootElement()->FirstChildElement("model");
	// we do not need to restore the primitive type that has been restored before 
/*	from_string(e->FirstChildElement("taperx")->Attribute("value"),mTaperX);
	from_string(e->FirstChildElement("tapery")->Attribute("value"),mTaperY);
	from_string(e->FirstChildElement("pathcutbegin")->Attribute("value"),mPathCutBegin);
	from_string(e->FirstChildElement("pathcutend")->Attribute("value"),mPathCutEnd);
	from_string(e->FirstChildElement("dimplebegin")->Attribute("value"),mDimpleBegin);
	from_string(e->FirstChildElement("dimpleend")->Attribute("value"),mDimpleEnd);
	from_string(e->FirstChildElement("holex")->Attribute("value"),mHoleSizeX);
	from_string(e->FirstChildElement("holey")->Attribute("value"),mHoleSizeY);
	from_string(e->FirstChildElement("hollowshape")->Attribute("value"),(int &)mHollowShape);
	from_string(e->FirstChildElement("twistbegin")->Attribute("value"),mTwistBegin);
	from_string(e->FirstChildElement("twistend")->Attribute("value"),mTwistEnd);
	from_string(e->FirstChildElement("topshearx")->Attribute("value"),mTopShearX);
	from_string(e->FirstChildElement("topsheary")->Attribute("value"),mTopShearY);
	from_string(e->FirstChildElement("skew")->Attribute("value"),mSkew);
	from_string(e->FirstChildElement("revolutions")->Attribute("value"),mRevolutions);
	from_string(e->FirstChildElement("radiusdelta")->Attribute("value"),mRadiusDelta);
	*/	
	//resetParameters();

	TiXmlElement *trans = e->FirstChildElement("transformation")->FirstChildElement("transfo");
	TCommand toAdd;
	//	Real converted;
	stringstream sDebug;
	String valueTransfo ;
	while (trans!= NULL)
	{
		from_string( trans->Attribute("type"),(int &)toAdd.first );
		from_string( trans->Attribute("valueX"),valueTransfo );
		toAdd.second.x = atoi(valueTransfo.c_str() ) / 1000.;
		from_string( trans->Attribute("valueY"),valueTransfo );
		toAdd.second.y = atoi(valueTransfo.c_str() ) / 1000.;
		from_string( trans->Attribute("valueZ"),valueTransfo );
		toAdd.second.z = atoi(valueTransfo.c_str() ) / 1000.;

		Vector3 v = toAdd.second;
		switch( toAdd.first )
		{
		case TRANSLATE : 
			if( v.x || v.y || v.z ) apply( TRANSLATE, v.x, v.y, v.z );
			break;
		case ROTATE : 
			if( v.x || v.y || v.z ) 
            {
                //setRotate( v.x, v.y, v.z );
                apply( ROTATE, v.x, v.y, v.z );
            }
			break;
		case SCALE : 
			if( v.x != 1 || v.y != 1 || v.z != 1 ) 
			{
				//setScale( v.x, v.y, v.z );
				apply( SCALE, v.x, v.y, v.z );
			}
			break;
		case TAPERX : 
		case TAPERY : 
			if( v.x ) apply( TAPERX, v.x, 0, 0 );
			if( v.y ) apply( TAPERY, v.y, 0, 0 );
			break;
		case TOP_SHEARX : 
		case TOP_SHEARY : 
			if( v.x ) apply( TOP_SHEARX, v.x, 0, 0 );
			if( v.y ) apply( TOP_SHEARY, v.y, 0, 0 );
			break;
		case TWIST_BEGIN :
		case TWIST_END : 
			if( v.x ) apply( TWIST_BEGIN, v.x, 0, 0 );
			if( v.y ) apply( TWIST_END, v.y, 0, 0 );
			break;
		case PATH_CUT_BEGIN : 
		case PATH_CUT_END : 
			if( v.x ) apply( PATH_CUT_BEGIN, v.x, 0, 0 );
			if( v.y ) apply( PATH_CUT_END, v.y, 0, 0 );
			break;
		case DIMPLE_BEGIN : 
		case DIMPLE_END : 
			if( v.x ) apply( DIMPLE_BEGIN, v.x, 0, 0 );
			if( v.y ) apply( DIMPLE_END, v.y, 0, 0 );
			break;
		case HOLE_SIZEX : 
		case HOLE_SIZEY : 
		case HOLLOW_SHAPE : 
			if( v.x ) apply( HOLE_SIZEX, v.x, 0, 0 );
			if( v.y ) apply( HOLE_SIZEY, v.y, 0, 0 );
			if( v.z ) apply( HOLLOW_SHAPE, v.z, 0, 0 );
			break;
		case SKEW : 
		case REVOLUTION : 
		case RADIUS_DELTA : 
			if( v.x ) apply( SKEW, v.x, 0, 0 );
			if( v.y ) apply( REVOLUTION, v.y, 0, 0 );
			if( v.z ) apply( RADIUS_DELTA, v.z, 0, 0 );
			break;
		}
		restoreBuffer( mBufCurrent, mBufBackup );
//		resetParameters();

		mCommandList.push_back(toAdd);
		mCommandLast = toAdd.first;

		sDebug << "Applying tranformation : " << toAdd.first << " with value : " << toAdd.second << " to " << mName << endl; 
		SOLIPSISINFO( sDebug.str().c_str() );
		trans = trans->NextSiblingElement( "transfo" );
	}

	e = doc.RootElement()->FirstChildElement("material");
	ColourValue cv;
	from_string(e->FirstChildElement("matambient")->Attribute("r"),cv.r);
	from_string(e->FirstChildElement("matambient")->Attribute("g"),cv.g);
	from_string(e->FirstChildElement("matambient")->Attribute("b"),cv.b);
	from_string(e->FirstChildElement("matambient")->Attribute("a"),cv.a);
	setAmbient(cv);
	from_string(e->FirstChildElement("matdiff")->Attribute("r"),cv.r);
	from_string(e->FirstChildElement("matdiff")->Attribute("g"),cv.g);
	from_string(e->FirstChildElement("matdiff")->Attribute("b"),cv.b);
	from_string(e->FirstChildElement("matdiff")->Attribute("a"),cv.a);
	setDiffus(cv);
	from_string(e->FirstChildElement("matspec")->Attribute("r"),cv.r);
	from_string(e->FirstChildElement("matspec")->Attribute("g"),cv.g);
	from_string(e->FirstChildElement("matspec")->Attribute("b"),cv.b);
	from_string(e->FirstChildElement("matspec")->Attribute("a"),cv.a);
	setSpecular(cv);
	float value = 0;
	from_string(e->FirstChildElement("matshin")->Attribute("value"),value);
	setShininess( value);
	from_string(e->FirstChildElement("matopac")->Attribute("value"),value);
	setAlpha(value);
    if (value < 1.0)
        setSceneBlendType(SBT_TRANSPARENT_ALPHA);
    int cullingMode;
    TiXmlElement* element = e->FirstChildElement("matdblside");
    if( element )
    {
        from_string(element->Attribute("value"),cullingMode);
        setCullingMode( (CullingMode)cullingMode);
    }

	//texture scroll, scale and rotate :
	Ogre::Vector2 tmpVec;
	from_string(e->FirstChildElement("texturescroll")->Attribute("u"),tmpVec.x);
	from_string(e->FirstChildElement("texturescroll")->Attribute("v"),tmpVec.y);
	setTextureScroll( tmpVec.x, tmpVec.y );
	from_string(e->FirstChildElement("texturescale")->Attribute("u"),tmpVec.x);
	from_string(e->FirstChildElement("texturescale")->Attribute("v"),tmpVec.y);
	setTextureScale( tmpVec.x, tmpVec.y );
	from_string(e->FirstChildElement("texturerotate")->Attribute("value"),value);
	setTextureRotate( Degree(value));
	//Textures List :
	trans = e->FirstChildElement("texturelist")->FirstChildElement("texture");
	TexturePtr texture ;
	string currenttexture ;
	//add texture to the current list :
	while (trans != NULL)
	{
        TextureExtParamsMap textureExtParamsMap;
    	TiXmlElement *textureExtParamsMapElt = trans->FirstChildElement("textureExtParamsMap");
        if (textureExtParamsMapElt != 0)
        {
            TiXmlElement *textureExtParamElt = textureExtParamsMapElt->FirstChildElement("param");
            while (textureExtParamElt != 0)
            {
                textureExtParamsMap[textureExtParamElt->Attribute("Name")] = textureExtParamElt->Attribute("Value");
                textureExtParamElt = textureExtParamElt->NextSiblingElement("param");
            }
        }
        if (mModifiedMaterialManager->getMMMTextureManager() != 0)
        {
            texture = mModifiedMaterialManager->getMMMTextureManager()->loadTexture(this, trans->Attribute("Name"), textureExtParamsMap);
        }
        else
        {
            //texture = TextureManager::getSingleton().load( (texturepath + trans->Attribute("Name")) , ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		    texture = TextureManager::getSingleton().load( trans->Attribute("Name") , ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
        }

		addTexture (texture, textureExtParamsMap);

		from_string( trans->Attribute("currenttexture") , currenttexture);
		if( strcmp (currenttexture.c_str() , "true" ) == 0 )
		{
			setCurrentTexture( texture );
		}
		trans = trans->NextSiblingElement("texture");
	}
	

	e = doc.RootElement()->FirstChildElement("threeD");
	Vector3 tmp;
	from_string(e->FirstChildElement("objposition")->Attribute("x"),tmp.x);
	from_string(e->FirstChildElement("objposition")->Attribute("y"),tmp.y);
	from_string(e->FirstChildElement("objposition")->Attribute("z"),tmp.z);

/*	std::string parentName;
	from_string(e->FirstChildElement("objparent")->Attribute("Name"),parentName);
	Object3D* parent = getParent();
	if( pos == Vector3::ZERO )
	{
		// place this object where it has been saved the last time
		if( parent )
			tmp	+= parent->getPosition(false);
	}
	else
	{
		// place this object where the avatar is
		if( parent )
			tmp += pos;
		else
			tmp = pos;
	}
*/
	mNode->setPosition(tmp);
	mCentreSelection = tmp;

	//from_string(e->FirstChildElement("objorientation")->Attribute("x"),tmp.x);
	//from_string(e->FirstChildElement("objorientation")->Attribute("y"),tmp.y);
	//from_string(e->FirstChildElement("objorientation")->Attribute("z"),tmp.z);
	// TODO : Restore Orientation
	//from_string(e->FirstChildElement("objscale")->Attribute("x"),tmp.x);
	//from_string(e->FirstChildElement("objscale")->Attribute("y"),tmp.y);
	//from_string(e->FirstChildElement("objscale")->Attribute("z"),tmp.z);
	// TODO : Restore Scale
	from_string(e->FirstChildElement("physics")->Attribute("col"),mEnableCollision);
	from_string(e->FirstChildElement("physics")->Attribute("grav"),mEnableGravity);

	e = doc.RootElement()->FirstChildElement("animations");

	e = doc.RootElement()->FirstChildElement("extras");

	e = doc.RootElement()->FirstChildElement("content");

	return 0; // No error
}

//-------------------------------------------------------------------------------------
int		Object3D::saveToFile(const char* fileName)
{
	Ogre::String stringToSave;
    ofstream toSave;
	toSave.open(fileName, ios_base::out | ios_base::trunc);
	if (!toSave.is_open())
	{
		SOLIPSISERROR("Unable to open dest file",fileName);
		return SOL_ERROR_CANNOTOPENFILE; 
	}
	// <?xml version="1.0" encoding="ISO-8859-1" ?>
	toSave << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>" << endl;
	toSave << "<!-- @version: 040611 -->" << endl;
	toSave << "<SOLObject>" << endl;
	toSave << "\t<properties>" << endl;
	toSave << "\t\t<modelerversion Name=\"" << SOLMODVERSION << "\" />" << endl;
    toSave << "\t\t<objuid Uid=\"" << mEntityUID << "\" />" << endl;
	toSave << "\t\t<objname Name=\"" << mName << "\" />" << endl;

	// TODO : See how to save / restore TAGS without carriarge returns => The XML would be more human readable
	//stringToSave = replaceSubstr(mTags,"\n","\\n");
	//toSave << "\t\t<objtags Name=\"" << stringToSave << "\" /> " << endl;
	//stringToSave = replaceSubstr(mDesc,"\n","\\n");
	//toSave << "\t\t<objdesc Name=\"" << stringToSave << "\" />" << endl;
	toSave << "\t\t<objtags Name=\"" << mTags << "\" /> " << endl;
	toSave << "\t\t<objdesc Name=\"" << mDesc << "\" />" << endl;


	toSave << "\t\t<objowner Name=\"" << mOwnerName << "\" />" << endl;
	toSave << "\t\t<objcreator Name=\"" << mCreatorName << "\" />" << endl;
	toSave << "\t\t<objgroup Name=\"" << mGroupName << "\" />" << endl;
	toSave << "\t\t<objrigths mod=\"" << mCanBeModified << "\" cop=\"" << mCanBeCopied<< "\" />" << endl;
	EntityUID parentUid = "NULL" ;
	if(mParent != NULL)
        parentUid = mParent->getEntityUID() ;
	toSave << "\t\t<objparent Uid=\"" << parentUid << "\" />" << endl;
	toSave << "\t</properties>" << endl;

	toSave << "\t<model>" << endl;
	toSave << "\t\t<primitive Name=\"" << SOLTYPESTRING[(int)mType] << "\" />" << endl;
/*	toSave << "\t\t<taperx value=\"" << mTaperX << "\" />" << endl;
	toSave << "\t\t<tapery value=\"" << mTaperY << "\" />" << endl;
	toSave << "\t\t<pathcutbegin value=\"" << mPathCutBegin << "\" />" << endl;
	toSave << "\t\t<pathcutend value=\"" << mPathCutEnd << "\" />" << endl;
	toSave << "\t\t<dimplebegin value=\"" << mDimpleBegin << "\" />" << endl;
	toSave << "\t\t<dimpleend value=\"" << mDimpleEnd << "\" />" << endl;
	toSave << "\t\t<holex value=\"" << mHoleSizeX << "\" />" << endl;
	toSave << "\t\t<holey value=\"" << mHoleSizeY << "\" />" << endl;
	toSave << "\t\t<hollowshape value=\"" << mHollowShape << "\" />" << endl;
	toSave << "\t\t<twistbegin value=\"" << mTwistBegin << "\" />" << endl;
	toSave << "\t\t<twistend value=\"" << mTwistBegin << "\" />" << endl;
	toSave << "\t\t<topshearx value=\"" << mTopShearX << "\" />" << endl;
	toSave << "\t\t<topsheary value=\"" << mTopShearY << "\" />" << endl;
	toSave << "\t\t<skew value=\"" << mSkew << "\" />" << endl;
	toSave << "\t\t<revolutions value=\"" << mRevolutions << "\" />" << endl;
	toSave << "\t\t<radiusdelta value=\"" << mRadiusDelta << "\" />" << endl;
*/	toSave << "\t\t<transformation>" << endl;
	std::list<TCommand>::iterator itCommands = mCommandList.begin();
	char valueTransfo [20] ;
	while (itCommands != mCommandList.end())
	{
		if ((*itCommands).first != NONE)
		{
			toSave << "\t\t\t<transfo type =\"" << (*itCommands).first ;
			itoa( (*itCommands).second.x*1000, valueTransfo, 10);
			toSave << "\" valueX=\"" << valueTransfo ;
			itoa( (*itCommands).second.y*1000, valueTransfo, 10);
			toSave << "\" valueY=\"" << valueTransfo ;
			itoa( (*itCommands).second.z*1000, valueTransfo, 10);
			toSave << "\" valueZ=\"" << valueTransfo << "\" />" << endl;
		}

		itCommands++;
	}
	toSave << "\t\t</transformation>" << endl;
	toSave << "\t</model>" << endl;

	toSave << "\t<material>" << endl;
	ColourValue cv  = getAmbient();
	toSave << "\t\t<matambient r=\"" << cv.r << "\" g=\"" << cv.g << "\" b=\"" << cv.b << "\" a=\"" << cv.a << "\" />" << endl;
	cv  = getDiffus();
	toSave << "\t\t<matdiff r=\"" << cv.r << "\" g=\"" << cv.g << "\" b=\"" << cv.b << "\" a=\"" << cv.a << "\" />" << endl;
	cv  = getSpecular();
	toSave << "\t\t<matspec r=\"" << cv.r << "\" g=\"" << cv.g << "\" b=\"" << cv.b << "\" a=\"" << cv.a << "\" />" << endl;
	float value  = getShininess();
	toSave << "\t\t<matshin value=\"" << value << "\" />" << endl;
	value = mModifiedMaterialManager->getAlpha();
	toSave << "\t\t<matopac value=\"" << value << "\" />" << endl;
    int cullingMode = getCullingMode();
	toSave << "\t\t<matdblside value=\"" << cullingMode << "\" />" << endl;
	//texture scroll, scale and rotate :
	Ogre::Vector2 tmpVec  = mModifiedMaterialManager->getTextureScroll();
	toSave << "\t\t<texturescroll u=\"" << tmpVec.x << "\" v=\"" << tmpVec.y << "\" />" << endl;
	tmpVec  = mModifiedMaterialManager->getTextureScale();
	toSave << "\t\t<texturescale u=\"" << tmpVec.x << "\" v=\"" << tmpVec.y << "\" />" << endl;
	value  = mModifiedMaterialManager->getTextureRotate().valueDegrees();
	toSave << "\t\t<texturerotate value=\"" << value << "\" />" << endl;
	//texture list :
	toSave << "\t\t<texturelist>" << endl;
	std::string texturePath ;
	size_t nameSizeChar = 0;


	std::string filePath (fileName);
	nameSizeChar = filePath.find_last_of( '\\' );
	std::string textureNewPath ( filePath, 0, nameSizeChar+1 );
	std::string currentTexture ;

	for (int i=1; i< mModifiedMaterialManager->getNbTexture(); i++)	//begin to 1 to do not save the default texture !
	{
        TexturePtr texturei = mModifiedMaterialManager->getTexture(i);
		texturePath = texturei->getName();
		nameSizeChar = texturePath.find_last_of( '\\' );
		std::string textureName (texturePath, nameSizeChar+1,texturePath.length() );

		if( texturei == mModifiedMaterialManager->getCurrentTexture() )
		{
			currentTexture = "true" ;
		}
		else
			currentTexture = "false" ;

        TextureExtParamsMap *textureExtParamsMap = mModifiedMaterialManager->getTextureExtParamsMap(texturei);

		//save in XML :
		toSave << "\t\t\t<texture Name=\"" << textureName << "\" currenttexture=\"" << currentTexture << "\">" << endl;	
        if (textureExtParamsMap != 0)
        {
            toSave << "\t\t\t\t<textureExtParamsMap>" << endl;
            for(TextureExtParamsMap::const_iterator it=textureExtParamsMap->begin();it!=textureExtParamsMap->end();++it)
            {
                TiXmlString xmlStringIn(it->second.c_str());
                TiXmlString xmlStringOut;
                TiXmlBase::EncodeString(xmlStringIn, &xmlStringOut);
                toSave << "\t\t\t\t\t<param Name=\"" << it->first << "\" Value=\"" << xmlStringOut.c_str() << "\" />" << endl;
            }
            toSave << "\t\t\t\t</textureExtParamsMap>" << endl;
        }
        toSave << "\t\t\t</texture>" << endl;
	}

	toSave << "\t\t</texturelist>" << endl;
	toSave << "\t</material>" << endl;

	toSave << "\t<threeD>" << endl;
	Vector3 tmp = getPosition(false);
	toSave << "\t\t<objposition x=\"" << tmp.x << "\" y=\"" << tmp.y << "\" z=\"" << tmp.z << "\" />" << endl;
	tmp = getRotate(); //getOrientation();
	toSave << "\t\t<objorientation x=\"" << tmp.x << "\" y=\"" << tmp.y << "\" z=\"" << tmp.z << "\" />" << endl;
	tmp = getScale();
	toSave << "\t\t<objscale x=\"" << tmp.x << "\" y=\"" << tmp.y << "\" z=\"" << tmp.z << "\" />" << endl;
	toSave << "\t\t<physics col=\"" << mEnableCollision << "\" grav=\"" << mEnableGravity << "\" />" << endl;
	toSave << "\t</threeD>" << endl;

	toSave << "\t<animations>" << endl;
	toSave << "\t</animations>" << endl;

	toSave << "\t<extras>" << endl;
	toSave << "\t</extras>" << endl;

	toSave << "\t<content>" << endl;
	toSave << "\t</content>" << endl;

	toSave << "</SOLObject>" << endl;
	toSave.close();

	return 0; // no error
}

//-------------------------------------------------------------------------------------
void Object3D::setEntityUID(const EntityUID& entityUID)
{
	mEntityUID = entityUID;
}

//-------------------------------------------------------------------------------------
const EntityUID& Object3D::getEntityUID()
{
	return mEntityUID;
}

//-------------------------------------------------------------------------------------
void Object3D::setName(String name)
{
	mName = name;
}

//-------------------------------------------------------------------------------------
String Object3D::getName()
{
	return mName;
}

//-------------------------------------------------------------------------------------
void Object3D::setTranslate(Real pX, Real pY, Real pZ )
{
	mTranslateX = pX;
	mTranslateY = pY;
	mTranslateZ = pZ;
}

//-------------------------------------------------------------------------------------
void Object3D::setRotate(Real pX, Real pY, Real pZ )
{
	mRotateX = pX;
	mRotateY = pY;
	mRotateZ = pZ;
}

//-------------------------------------------------------------------------------------
Vector3 Object3D::getRotate() 
{
    return Vector3 (mRotateX, mRotateY, mRotateZ) ;
};

//-------------------------------------------------------------------------------------
void Object3D::setScale(Real pX, Real pY, Real pZ )
{
	mScaleX = pX;
	mScaleY = pY;
	mScaleZ = pZ;
}

//-------------------------------------------------------------------------------------
Vector3 Object3D::getScale() 
{
    return Vector3 (mScaleX, mScaleY, mScaleZ) ;
};

//-------------------------------------------------------------------------------------
void Object3D::setPathCutBegin(Real value)
{
	mPathCutBegin = value;
}

//-------------------------------------------------------------------------------------
void Object3D::setPathCutEnd(Real value)
{
	mPathCutEnd = value;
}

//-------------------------------------------------------------------------------------
void Object3D::setTwistBegin(Real value)
{
	mTwistBegin = value;
}

//-------------------------------------------------------------------------------------
void Object3D::setTwistEnd(Real value)
{
	mTwistEnd = value;
}

//-------------------------------------------------------------------------------------
void Object3D::setTaperX(Real value)
{
	mTaperX = value;
}

//-------------------------------------------------------------------------------------
void Object3D::setTaperY(Real value)
{
	mTaperY = value;
}

//-------------------------------------------------------------------------------------
void Object3D::setTopShearX(Real value)
{
	mTopShearX = value;
}

//-------------------------------------------------------------------------------------
void Object3D::setTopShearY(Real value)
{
	mTopShearY = value;
}

//-------------------------------------------------------------------------------------
void Object3D::setDimpleBegin(Real value)
{
	mDimpleBegin = value;
}

//-------------------------------------------------------------------------------------
void Object3D::setDimpleEnd(Real value)
{
	mDimpleEnd = value;
}

//-------------------------------------------------------------------------------------
void Object3D::setHoleSizeX(Real value)
{
	mHoleSizeX = value;
}

//-------------------------------------------------------------------------------------
void Object3D::setHoleSizeY(Real value)
{
	mHoleSizeY = value;
}

//-------------------------------------------------------------------------------------
void Object3D::setHollowShape(Shape shape)
{
	mHollowShape = shape;
}

//-------------------------------------------------------------------------------------
void Object3D::setSkew(Real value)
{
	mSkew = value;
}

//-------------------------------------------------------------------------------------
void Object3D::setRevolutions(int value)
{
	mRevolutions = value;
}

//-------------------------------------------------------------------------------------
void Object3D::setRadiusDelta(Real value)
{
	mRadiusDelta = value;
}

//-------------------------------------------------------------------------------------
void Object3D::resetParameters()
{
	mTranslateX = mTranslateY = mTranslateZ = 0;
mRotateX = mRotateY = mRotateZ = 0;
mScaleX = mScaleY = mScaleZ = 1;

	mTaperX = mTaperY = 0;
	mTopShearX = mTopShearY = 0;
	mTwistBegin = mTwistEnd = 0;
	mDimpleBegin = 0;
	mDimpleEnd = 1;
	mPathCutBegin = 0;
	mPathCutEnd = 1;
	mHoleSizeX = mHoleSizeY = 0;
	mHollowShape = SQUARE;
	mSkew = 0;
	mRevolutions = 1;
	mRadiusDelta = 0;
}

//-------------------------------------------------------------------------------------
bool Object3D::apply(Command command, Real p1)
{
	return apply(command,p1,0,0);
}

//-------------------------------------------------------------------------------------
bool Object3D::apply(Command command, Real p1, Real p2, Real p3)
{
	Vector3 rootTrans;					// root position for the transformation & deformations
	realvector vertex;
	Real *vertexA = 0;
	Real *vertexB = 0;
	uintvector	index;
	unsigned int vertexCount, indexCount = 0;
	unsigned int vertexCountA, indexCountA, *indexA = 0;
	unsigned int vertexCountB, indexCountB, *indexB = 0;
	Real mAngleBegin, mAngleEnd;
	bool path_cut = false; 
	bool dimple = false;
	bool hole = false;
	bool skew = false;
	static unsigned int hollow_sides = 4;
	static SBO *sbo = new SBO();
	SOLID *solidA, *solidB;


	mCommandLast = command;
	switch( command )
	{
	// basic transformations
	case TRANSLATE:
		{
			mNode->translate( p1, p2, p3 );

			mTranslateX += p1;
			mTranslateY += p2;
			mTranslateZ += p3;

			break;
		}

	case ROTATE:
		{
            // transfomation center / axe
			rootTrans = Vector3::ZERO; //mNode->getPosition();
			static Real angleX,angleY,angleZ;
			angleX = Math::DegreesToRadians( p1 ); // p1 * 0.01745329252);		// * PI / 180
			angleY = Math::DegreesToRadians( p2 );
			angleZ = Math::DegreesToRadians( p3 );
			Vector3 vertex, normal;
			unsigned int id;

            Vector3 rotate = getRotate();
            setRotate( rotate.x + p1, rotate.y + p2, rotate.z + p3 );

			// transformation ...
			for(unsigned int i=0; i<mVertexCount; i++)
			{
				id = i*mVertexDecl/4;
				vertex.x = mBufCurrent->vertex[id];			normal.x = mBufCurrent->vertex[id+3];
				vertex.y = mBufCurrent->vertex[id+1];		normal.y = mBufCurrent->vertex[id+4];
				vertex.z = mBufCurrent->vertex[id+2];		normal.z = mBufCurrent->vertex[id+5];

				//vertex -= rootTrans;
				if( p1 && !p2 && !p3 )			// rotation on X
				{
					VectorModifier::rotateX( vertex, angleX );
					VectorModifier::rotateX( normal, angleX );
				}
				else if( !p1 && p2 && !p3 )		// rotation on Y
				{
					VectorModifier::rotateY( vertex, angleY );
					VectorModifier::rotateY( normal, angleY );
				}
				else if( !p1 && !p2 && p3 )		// rotation on Z
				{
					VectorModifier::rotateZ( vertex, angleZ );
					VectorModifier::rotateZ( normal, angleZ );
				}
				else							// rotation on X & Y & Z
				{
					VectorModifier::rotateXYZ( vertex, angleX, angleY, angleZ );	
					VectorModifier::rotateXYZ( normal, angleX, angleY, angleZ );
				}
				//vertex += rootTrans;

				mBufCurrent->vertex[id]   = vertex.x;		mBufCurrent->vertex[id+3] = normal.x;
				mBufCurrent->vertex[id+1] = vertex.y;		mBufCurrent->vertex[id+4] = normal.y;
				mBufCurrent->vertex[id+2] = vertex.z;		mBufCurrent->vertex[id+5] = normal.z;
			}

			update();
			break;
		}

	case SCALE:
		{
			Real s1 = p1;
			Real s2 = p2;
			Real s3 = p3;
//			if( p1 != 1 ) { s2 = mScaleY; s3 = mScaleZ; }
//			else if( p2 != 1 ) { s1 = mScaleX; s3 = mScaleZ; }
//			else if( p3 != 1 ) { s1 = mScaleX; s2 = mScaleY; }

			//if( p1 == 1. ) s1 = mScaleX;
			//if( p2 == 1. ) s2 = mScaleY;
			//if( p3 == 1. ) s3 = mScaleZ;

			setScale( s1, s2, s3 );

			for(unsigned int i=0; i<mVertexCount; i++)
			{
				unsigned int id = i*mVertexDecl/4;
				mBufCurrent->vertex[id]		= mBufBackup->vertex[id] * p1;
				mBufCurrent->vertex[id+1]	= mBufBackup->vertex[id+1] * p2;
				mBufCurrent->vertex[id+2]	= mBufBackup->vertex[id+2] * p3;
			}

			update();
			break;
		}

	// advanced deformations
	case TAPERX:
		{
			if( 0 > p1 || p1 > 1 ) return false;
			setTaperX( p1 );
			
			for(unsigned int i=0; i<mVertexCount; i++)
			{
				unsigned int id = i*mVertexDecl/4;
				Real dep = mBufBackup->vertex[id] * p1 * mBufBackup->vertex[id+1] / mBufBackup->size.y;//( mSize.y + mCornerMin.y );
				mBufCurrent->vertex[id] = mBufBackup->vertex[id] - dep;
			}

			update();
			break;
		}
	case TAPERY:
		{
			if( 0 > p1 || p1 > 1 ) return false;
			setTaperY(p1);

			for(unsigned int i=0; i<mVertexCount; i++)
			{
				unsigned int id = i*mVertexDecl/4;
				Real dep = mBufBackup->vertex[id+2] * p1 * mBufBackup->vertex[id+1] / mBufBackup->size.y;//( mSize.y + mCornerMin.y );
				mBufCurrent->vertex[id+2] = mBufBackup->vertex[id+2] - dep;
			}

			update();
			break;
		}

	case TOP_SHEARX:
		{
			if( 0 > p1 || p1 > 1 ) return false;
			setTopShearX( p1 );

			for(unsigned int i=0; i<mVertexCount; i++)
			{
				unsigned int id = i*mVertexDecl/4;
				mBufCurrent->vertex[id] = mBufBackup->vertex[id];
				//mBufCurrent->vertex[id] += p1 * mSize.x * mBufBackup->vertex[id+1] / mCornerMax.y;
				mBufCurrent->vertex[id] += p1 * mBufBackup->size.x * mBufBackup->vertex[id+1] / mBufBackup->cornerMax.y;
			}

			update();
			break;
		}		
	case TOP_SHEARY:
		{
			if( 0 > p1 || p1 > 1 ) return false;
			setTopShearY( p1 );

			for(unsigned int i=0; i<mVertexCount; i++)
			{
				unsigned int id = i*mVertexDecl/4;
				mBufCurrent->vertex[id+2] = mBufBackup->vertex[id+2];
				//mBufCurrent->vertex[id+2] += p1 * mSize.z * mBufBackup->vertex[id+1] / mCornerMax.y;
				mBufCurrent->vertex[id+2] += p1 * mBufBackup->size.z * mBufBackup->vertex[id+1] / mBufBackup->cornerMax.y;
			}

			update();
			break;
		}

	case TWIST_BEGIN:
		{
			if( 0 > p1 || p1 > 1 ) return false;
			setTwistBegin( p1 );

			p1 *= Math::TWO_PI; // 6.2831853;
			Real ratio, angle;
//			Vector3 center = mNode->getPosition();
			
			for(unsigned int i=0; i<mVertexCount; i++)
			{
				unsigned int id = i*mVertexDecl/4;
				Vector3 v;
				v.x = mBufBackup->vertex[id];
				v.y = mBufBackup->vertex[id+1];
				v.z = mBufBackup->vertex[id+2];

//				v -= center;
				ratio = mBufBackup->vertex[id+1] / mCornerMax.y / 2;			// 1 = top, 0 = bottomm
				angle = ratio * p1;
				VectorModifier::rotateY( v, angle );
				if( 0 < mTwistEnd )
				{
					ratio = 0.5 - mBufBackup->vertex[id+1] / mCornerMax.y / 2;	// 1 = top, 0 = bottom
					angle = ratio * mTwistEnd * Math::TWO_PI;
					VectorModifier::rotateY( v, -angle );
				}
//				v += center;

				mBufCurrent->vertex[id] = v.x;
				mBufCurrent->vertex[id+1] = v.y;
				mBufCurrent->vertex[id+2] = v.z;
			}
			p1 /= Math::TWO_PI;

			update();
			break;
		}
	case TWIST_END:
		{
			if( 0 > p1 || p1 > 1 ) return false;
			setTwistEnd( p1 );

			p1 *= Math::TWO_PI; // 6.2831853;
			Real ratio, angle;
//			Vector3 center = mNode->getPosition();
		
			for(unsigned int i=0; i<mVertexCount; i++)
			{
				unsigned int id = i*mVertexDecl/4;
				Vector3 v;
				v.x = mBufBackup->vertex[id];
				v.y = mBufBackup->vertex[id+1];
				v.z = mBufBackup->vertex[id+2];

//				v -= center;
				if( 0 < mTwistBegin )
				{
					ratio = mBufBackup->vertex[id+1] / mCornerMax.y / 2;		// 1 = top, 0 = bottomm
					angle = ratio * mTwistBegin * Math::TWO_PI;
					VectorModifier::rotateY( v, angle );
				}
				ratio = 0.5 - mBufBackup->vertex[id+1] / mCornerMax.y / 2;		// 1 = top, 0 = bottom
				angle = ratio * p1;
				VectorModifier::rotateY( v, -angle );
//				v += center;

				mBufCurrent->vertex[id] = v.x;
				mBufCurrent->vertex[id+1] = v.y;
				mBufCurrent->vertex[id+2] = v.z;
			}
			p1 /= Math::TWO_PI;

			update();
			break;
		}

	case PATH_CUT_BEGIN:
		{
			if( (p1 == 0) && (mPathCutEnd == 1) ) return false;
			if( (p1 - mPathCutEnd) >= 0 ) return false;
			setPathCutBegin( p1 );
			mAngleBegin = mPathCutBegin;
			mAngleEnd = mPathCutEnd;
			path_cut = true;
			break;
		}
	case PATH_CUT_END:
		{
			if( (mPathCutBegin == 0) && (p1 == 1) ) return false;
			if( (mPathCutBegin - p1) >= 0 ) return false;
			setPathCutEnd( p1 );
			mAngleBegin = mPathCutBegin;
			mAngleEnd = mPathCutEnd;
			path_cut = true;
			break;
		}

	case DIMPLE_BEGIN:
		{
			if( (p1 == 0) && (mDimpleEnd == 1) ) return false;
			if( (p1 - mDimpleEnd) >= 0 ) return false;
			setDimpleBegin( p1 );
			mAngleBegin = mDimpleBegin;
			mAngleEnd = mDimpleEnd;
			dimple = true;
			break;
		}
	case DIMPLE_END:
		{
			if( (mDimpleBegin == 0) && (p1 == 1) ) return false;
			if( (mDimpleBegin - p1) >= 0 ) return false;
			setDimpleEnd( p1 );
			mAngleBegin = mDimpleBegin;
			mAngleEnd = mDimpleEnd;
			dimple = true;
			break;
		}

	case HOLE_SIZEX:
		setHoleSizeX( p1 );
		hole = true;
		break;
	case HOLE_SIZEY:
		setHoleSizeY( p1 );
		hole = true;
		break;
	case HOLLOW_SHAPE:
		switch( int(p1) )
		{
		case 0: setHollowShape( CIRCLE ); hollow_sides = 16; break;
		default:
		case 1: setHollowShape( SQUARE ); hollow_sides = 4; break;
		case 2: setHollowShape( TRIANGLE ); hollow_sides = 3; break;
		}
		hole = true;
		break;

	case SKEW:
		setSkew( p1 );
		skew = true;
		break;
	case REVOLUTION:
		setRevolutions( p1 );
		skew = true;
		break;
	case RADIUS_DELTA:
		setRadiusDelta( p1 );
		skew = true;
		break;
	}

	//path_cut / dimple / hole deformation
	if( path_cut || dimple || hole )
	{
		// SOLID_A
// ***********************************************************************************
float* vvv;
unsigned int* iii;

vvv = new float[mBufBackup->vertexCount*mVertexDecl/4];
iii = new unsigned int[mBufBackup->indexCount];
for(realvector::iterator vv=mBufBackup->vertex.begin(); vv!=mBufBackup->vertex.end(); vv++)
	*vvv++ = (*vv);
for(uintvector::iterator ii=mBufBackup->index.begin(); ii!=mBufBackup->index.end(); ii++)
	*iii++ = (*ii);
vvv -= mBufBackup->vertexCount*mVertexDecl/4;
iii -= mBufBackup->indexCount;
// ***********************************************************************************
		solidA = sbo->dataLoad( 
//			(float *) &(mBufBackup->vertex), mBufBackup->vertexCount, mVertexDecl/4,
//			(unsigned int *) &(mBufBackup->index), mBufBackup->indexCount/3 );
			vvv, mBufBackup->vertexCount, mVertexDecl/4,
			iii, mBufBackup->indexCount/3 );
		solidA->DataCloseCallback();
// ***********************************************************************************
delete iii;
delete vvv;
iii = NULL;
vvv = NULL;
// ***********************************************************************************

		// SOLID_B : generate a new SOLID
		if( hole )
			MeshModifier::genCylinder(
				command,
				&vertex, vertexCount, mVertexDecl/4, 
				&index, indexCount,
				hollow_sides, mHoleSizeY, mHoleSizeX,
				mCornerMax, mCornerMin );
		else
			MeshModifier::genCylinderCut(
				command,
				&vertex, vertexCount, mVertexDecl/4, 
				&index, indexCount, 
				mAngleBegin, mAngleEnd, 
				mCornerMax, mCornerMin );

// ***********************************************************************************
vvv = new float[vertexCount*mVertexDecl/4];
iii = new unsigned int[indexCount];
for(realvector::iterator vv=vertex.begin(); vv!=vertex.end(); vv++)
	*vvv++ = (*vv);
for(uintvector::iterator ii=index.begin(); ii!=index.end(); ii++)
	*iii++ = (*ii);
vvv -= vertexCount*mVertexDecl/4;
iii -= indexCount;
// ***********************************************************************************
		solidB = sbo->dataLoad(
//			(float*) &vertex, vertexCount, mVertexDecl/4,
//			(unsigned int*) &index, indexCount/3 );
			vvv, vertexCount, mVertexDecl/4,
			iii, indexCount/3 );
		solidB->DataCloseCallback();
// ***********************************************************************************
delete iii;
delete vvv;
iii = NULL;
vvv = NULL;
// ***********************************************************************************

		// Apply a boolean operation from the solid B on A
		sbo->applyBoolOp( SBO::OP_DIFFERENCE, solidA, solidB );
		sbo->OutputSolid( solidA, vertexA, vertexCountA, mVertexDecl/4, indexA, indexCountA );
		sbo->OutputSolid( solidB, vertexB, vertexCountB, mVertexDecl/4, indexB, indexCountB );
//		delete solidA;
//		delete solidB;

		// Merge the 2 modified solids
		// sizes
		vertexCount = vertexCountA + vertexCountB;
		indexCount = indexCountA + indexCountB;
		// vertex data
		vertex.clear();
		vertex.resize(vertexCount * mVertexDecl/4);
        unsigned int i;
		for(i=0; i<vertexCountA*mVertexDecl/4; i++) vertex[i] = *vertexA++;
		for(i=vertexCountA*mVertexDecl/4; i<(vertexCountA+vertexCountB)*mVertexDecl/4; i++) vertex[i] = *vertexB++;
		// index data
		index.clear();
		index.resize(indexCount);
		for(i=0; i<indexCountA; i++) index[i] = *indexA++;
		for(i=indexCountA; i<indexCountA+indexCountB; i++) index[i] = *indexB++ + vertexCountA;
		// replace pointers at startup
		/*vertex -= vertexCount * mVertexDecl/4;
		index -= indexCount;*/

		// Update vertex & index datas
		resizeBuffers( vertex, vertexCount, index, indexCount );
		path_cut = dimple = hole = false;

		mBufCurrent->vertex.clear();
		mBufCurrent->index.clear();
		mBufCurrent->vertex			= vertex;
		mBufCurrent->vertexCount	= vertexCount;
		mBufCurrent->index			= index;
		mBufCurrent->indexCount		= indexCount;
//		updateBoundingBox();
	}

	// skew deformation
	if( skew && (mType == CYLINDER || mType == TORUS || mType == TUBE) )
	{
		vertex.clear();
		index.clear();

		// Generate the new modified object3D
		MeshModifier::genCylinderSkew(
			command,
			&vertex, vertexCount, mVertexDecl/4, 
			&index, indexCount,
			16, 1, 1,
			mSkew, mRadiusDelta, mRevolutions, mType,
			mBufCurrent->cornerMax, mBufCurrent->cornerMin );

		// Update vertex & index datas
		resizeBuffers( vertex, vertexCount, index, indexCount );
		skew = false;

		mBufCurrent->vertex.clear();
		mBufCurrent->index.clear();
		mBufCurrent->vertex			= vertex;
		mBufCurrent->vertexCount	= vertexCount;
		mBufCurrent->index			= index;
		mBufCurrent->indexCount		= indexCount;
	}

	// children
 	if (mChildren)
	{
		vector< Object3D* >::iterator itr;
		for( itr = mChildren->begin(); itr != mChildren->end(); itr++ )
		{
			switch( command )
			{
			case TRANSLATE: continue;
			case ROTATE: (*itr)->rotateFromParent( p1, p2, p3 ); break;
			}

			(*itr)->apply( command, p1, p2, p3 ) ;
		}
	}

	mNode->_updateBounds();
	return true;
}

//-------------------------------------------------------------------------------------
void Object3D::setPosition(Vector3 pos)
{
	mNode->setPosition( pos );
}

//-------------------------------------------------------------------------------------
Vector3 Object3D::getPosition(bool worldPosition )
{
	if (worldPosition)
#if (OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR < 6)
		return mNode->getWorldPosition();
#else
		return mNode->_getDerivedPosition();
#endif
	return mNode->getPosition();
}

//-------------------------------------------------------------------------------------
Vector3 Object3D::getOrientation()
{
	return Vector3(mNode->getOrientation().getYaw().valueDegrees(),mNode->getOrientation().getPitch().valueDegrees(),mNode->getOrientation().getRoll().valueDegrees());
}

//-------------------------------------------------------------------------------------
bool Object3D::linkObject(Object3D* pObj, SceneManager* pSceneMgr)
{
	SceneNode * pObjScenNode = pObj->getEntity()->getParentSceneNode();
#if (OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR < 6)
	Vector3 Wpostion = pObjScenNode->getWorldPosition() ;
#else
	Vector3 Wpostion = pObjScenNode->_getDerivedPosition() ;
#endif
	if( isLink(pObj) )
	{			//remove it ...
		removeChild( pObj);			//to mChildren
									//to this current node :
		mNode->removeChild( pObjScenNode );
		pSceneMgr->getRootSceneNode()->addChild( pObjScenNode );
		pObjScenNode->setPosition( Wpostion );
		pObj->setParent( NULL );	//update pObj's parent
	}
	else
	{
		if(pObj->getParent() == NULL)//if this object hasn't parent :
		{
			//we can add it ...
			addChild( pObj);			//... to mChildren
										//... to this current node	 
			pSceneMgr->getRootSceneNode()->removeChild( pObjScenNode ) ;
			mNode->addChild( pObjScenNode );
#if (OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR < 6)
			pObjScenNode->setPosition( Wpostion - mNode->getWorldPosition() );
#else
			pObjScenNode->setPosition( Wpostion - mNode->_getDerivedPosition() );
#endif
			pObj->setParent( this) ;	//update pObj's parent
		}
		else	//if this object has a parent :
		{
			pObj->getParent()->linkObject(pObj, pSceneMgr) ;
		}
	}
	return true;
}
//-------------------------------------------------------------------------------------
bool Object3D::isLink(Object3D* pObj)
{
	if (!mChildren)
		return false;

	vector< Object3D* >::iterator itr ;
	for( itr = mChildren->begin(); itr != mChildren->end(); itr++ )
	{
		if ( (*itr) == pObj )
			return true;
	}
	return false;
}

//-------------------------------------------------------------------------------------
void Object3D::addChild(Object3D* pChild)
{
	if( !mChildren )
		mChildren = new vector< Object3D* >;

	mChildren->push_back( pChild ) ;
}

//-------------------------------------------------------------------------------------
vector< Object3D* >* Object3D::getChildren()
{
	return mChildren;
}

//-------------------------------------------------------------------------------------
void Object3D::removeChild(Object3D* pChild)
{
	if( mChildren )
	{
		vector< Object3D* >::iterator i ;
		for(i = mChildren->begin(); i!=mChildren->end(); i++)
		{
			if ( (*i) == pChild )
			{
				mChildren->erase( i);
				return ;
			}
		}
	}
}

//-------------------------------------------------------------------------------------
void Object3D::setParent(Object3D* pParent)
{
	mParent = pParent ;
}

//-------------------------------------------------------------------------------------
void Object3D::getSize(Vector3 &size, Vector3 &min, Vector3 &max)
{
	//if( mVertexCount )
	if( mBufCurrent->vertexCount )
	{
		max = Vector3::ZERO;
		min = Vector3::ZERO;
		realvector vertex = mBufCurrent->vertex; // = mVertex;

		unsigned int id;
		//for(unsigned int i=0; i<mVertexCount; i++)
		for(unsigned int i=0; i<mBufCurrent->vertexCount; i++)
		{
			id = i*mVertexDecl/4;

			if( vertex[id] > max.x )		max.x = vertex[id];
			if( vertex[id+1] > max.y )		max.y = vertex[id+1];
			if( vertex[id+2] > max.z )		max.z = vertex[id+2];

			if( vertex[id] < min.x )		min.x = vertex[id];
			if( vertex[id+1] < min.y )		min.y = vertex[id+1];
			if( vertex[id+2] < min.z )		min.z = vertex[id+2];
		}

		size.x = max.x - min.x;
		size.y = max.y - min.y;
		size.z = max.z - min.z;
	}
}

//-------------------------------------------------------------------------------------
void Object3D::getDataFromBuffer(vector<Vector3>* pVertex, vector<Face>* pTriangle)
{
	Mesh* mesh = mEntity->getMesh().get();

	// Clear the vertex array
//	if( mVertex ) free( mVertex );

	// Clear the triangle array
//	if( mIndex ) free( mTriangle );

	// Get the informations
	MeshModifier::getMeshInformation(
		mesh,
		mVertexCount,
		&mVertex,
		mIndexCount,
		&mIndex);

	mTriangleCount = mIndexCount / 3;
}

//-------------------------------------------------------------------------------------
void Object3D::setPoint(unsigned int index, const Vector3 &value)
{
	assert(index < mVertexCount && "Point index is out of bounds!!");
	mVertex[index*mVertexDecl/4] = value.x;
	mVertex[index*mVertexDecl/4+1] = value.y;
	mVertex[index*mVertexDecl/4+2] = value.z;
} 

//-------------------------------------------------------------------------------------
Vector3 inline Object3D::getPoint(unsigned int index)
{
	assert(index < mVertexCount && "Point index is out of bounds!!");

	Vector3 v;
	v.x = mVertex[index*mVertexDecl/4];
	v.y = mVertex[index*mVertexDecl/4+1];
	v.z = mVertex[index*mVertexDecl/4+2];

	return v;
}

//-------------------------------------------------------------------------------------
size_t Object3D::getNumPoints(void)
{
	return mVertexCount;
}

//-------------------------------------------------------------------------------------
void inline Object3D::getFace(unsigned int index, Object3D::Face &face)
{
	if( index < mIndexCount )
	{
		face.id1 = mIndex[index*3];
		face.id2 = mIndex[index*3+1];
		face.id3 = mIndex[index*3+2];
	}
}

//-------------------------------------------------------------------------------------
size_t Object3D::getNumFaces(void)
{
	return mIndexCount;
}

//-------------------------------------------------------------------------------------
void inline Object3D::update()
{
	if( mEntity )
	{
		// update the hardware vertex buffer
		updateBufferVertex();

		// update the bounding box corners
		updateBoundingBox();
	}
}

//-------------------------------------------------------------------------------------
void inline Object3D::updateBoundingBox()
{
	getSize( mSize, mCornerMin, mCornerMax );

	mEntity->getMesh()->_setBounds( AxisAlignedBox( 
		mCornerMin.x, mCornerMin.y, mCornerMin.z,
		mCornerMax.x, mCornerMax.y, mCornerMax.z ) );
	mEntity->getMesh()->_setBoundingSphereRadius( Math::Sqrt( 3 * mCornerMax.x * mCornerMax.x ) );
}
//-------------------------------------------------------------------------------------
void Object3D::updateBufferVertex()
{
	MeshPtr mesh = mEntity->getMesh();
	SubMesh* subMesh = mesh->getSubMesh(0);

	const VertexElement* posElem = mVertexData->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

	HardwareVertexBufferSharedPtr vbuf = mVertexData->vertexBufferBinding->getBuffer(posElem->getSource());
	Real *vertex = static_cast<Real*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));

	for(unsigned int i=0; i<mVertexCount*mVertexDecl/4; i++)
		vertex[i] = mBufCurrent->vertex[i];

	vbuf->unlock();
}

//-------------------------------------------------------------------------------------
void Object3D::resizeBuffers( realvector &pVertexData, size_t pVertexCount, uintvector &pIndexData, size_t pIndexCount )
{
	SubMesh* subMesh = mEntity->getMesh()->getSubMesh(0);

	// Update vertex count in the render operation
	mVertexData->vertexCount = pVertexCount;

	Real *vertextmp = new Real[pVertexData.size()];
	for (int i= 0 ; i<(int)pVertexData.size();i++)
	{
		vertextmp[i] = pVertexData[i];
	}

	unsigned int *indextmp = new unsigned int [pIndexData.size()];
	for (int i= 0 ; i<(int)pIndexData.size();i++)
	{
		indextmp[i] = pIndexData[i];
	}

	// Create new vertex buffer
	HardwareVertexBufferSharedPtr vbuf =
		HardwareBufferManager::getSingleton().createVertexBuffer(
			mVertexDecl,
			mVertexData->vertexCount, //mVertexBufferCapacity,
			//HardwareBuffer::HBU_STATIC_WRITE_ONLY);
			HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY); // TODO: Custom HBU_?

	// Upload the vertex data to the card
	vbuf->writeData( 0, vbuf->getSizeInBytes(), vertextmp, true );

	// Bind buffer
	mVertexData->vertexBufferBinding->setBinding(0, vbuf);



	// Set parameters of the submesh
	// Update index count in the render operation
	subMesh->indexData->indexCount = pIndexCount;
	subMesh->indexData->indexStart = 0;

	// Create new index buffer
	HardwareIndexBufferSharedPtr ibuf = 
		HardwareBufferManager::getSingleton().createIndexBuffer(
		HardwareIndexBuffer::IT_32BIT,	//HardwareIndexBuffer::IT_16BIT
		subMesh->indexData->indexCount, //mIndexBufferCapacity,
		//HardwareBuffer::HBU_STATIC_WRITE_ONLY);
		HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY); // TODO: Custom HBU_?

	// Upload the index data to the card
	ibuf->writeData( 0, ibuf->getSizeInBytes(), indextmp, true );

	// Update index
	subMesh->indexData->indexBuffer = ibuf;

	// Notify mesh object that it has been loaded
	mEntity->getMesh()->load();

	delete vertextmp; vertextmp = 0;
	delete indextmp; indextmp = 0;
}

//-------------------------------------------------------------------------------------
void Object3D::showBoundingBox(bool pValue)
{
	mNode->showBoundingBox(pValue);
	if (mChildren)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChildren->begin(); itr != mChildren->end(); itr++ )
		{
			(*itr)->showBoundingBox(pValue);
		}
	}
}
//-------------------------------------------------------------------------------------
bool Object3D::getShowBoundingBox()
{
	return mNode->getShowBoundingBox() ;
}
//-------------------------------------------------------------------------------------
TexturePtr Object3D::getTexture(const String& name)
{
	return mModifiedMaterialManager->getTexture( name );
}
//-------------------------------------------------------------------------------------
void Object3D::addTexture(TexturePtr texture, const TextureExtParamsMap& textureExtParamsMap)
{
	//Test if this texture already exists :
	TexturePtr PrecPtrTexture = mModifiedMaterialManager->getTexture(texture->getName() );	//return NULL if this texture doesn't exist
	if ( PrecPtrTexture == TexturePtr () )	//if (PrecPtrTexture == NULL) ...
	{
		mModifiedMaterialManager->addTexture(texture, textureExtParamsMap);
	}
	
	if (mChildren)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChildren->begin(); itr != mChildren->end(); itr++ )
		{
			(*itr)->addTexture(texture, textureExtParamsMap);
		}
	}
}

//-------------------------------------------------------------------------------------
void Object3D::deleteTexture(TexturePtr pTexture) 
{
	//Remove texture if it exists :
	if (mModifiedMaterialManager->isPresentInList( pTexture ) )
	{
		mModifiedMaterialManager->deleteTexture( pTexture ) ;
	}

	//Apply remove for all children :
	if (mChildren)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChildren->begin(); itr != mChildren->end(); itr++ )
		{
			(*itr)->deleteTexture( pTexture);
		}
	}

}
//-------------------------------------------------------------------------------------
void Object3D::setCurrentTexture(const String& textureName)
{
	TexturePtr texture = TextureManager::getSingleton().getByName(textureName) ;
	//test if this texture is in the list
	if( ! mModifiedMaterialManager->isPresentInList( texture ) )
	{
		//we add it if it isn't present :
		mModifiedMaterialManager->addTexture( texture );
	}

	mModifiedMaterialManager->setCurrentTexture( texture) ;

	if (mChildren)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChildren->begin(); itr != mChildren->end(); itr++ )
		{
			(*itr)->setCurrentTexture(texture);
		}
	}
}
//-------------------------------------------------------------------------------------
void Object3D::setCurrentTexture(const TexturePtr texture, const TextureExtParamsMap& textureExtParamsMap)
{
	//test if this texture is in the list
	if( ! mModifiedMaterialManager->isPresentInList(texture))
	{
		//we add it if it isn't present :
		mModifiedMaterialManager->addTexture(texture, textureExtParamsMap);
	}

	mModifiedMaterialManager->setCurrentTexture(texture);

	if (mChildren)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChildren->begin(); itr != mChildren->end(); itr++ )
		{
			(*itr)->setCurrentTexture(texture, textureExtParamsMap);
		}
	}
}
//-------------------------------------------------------------------------------------
TexturePtr Object3D::getCurrentTexture()
{
	return mModifiedMaterialManager->getCurrentTexture() ;
}
//-------------------------------------------------------------------------------------
TextureExtParamsMap* Object3D::getCurrentTextureExtParamsMap()
{
	return mModifiedMaterialManager->getTextureExtParamsMap(mModifiedMaterialManager->getCurrentTexture()) ;
}
//-------------------------------------------------------------------------------------
void Object3D::setAmbient( const ColourValue pColor)
{
	mModifiedMaterialManager->getModifiedMaterial()->setAmbient( pColor);
	if (mChildren)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChildren->begin(); itr != mChildren->end(); itr++ )
		{
			(*itr)->setAmbient(pColor);
		}
	}
}
//-------------------------------------------------------------------------------------
void Object3D::setDiffus( const ColourValue pColor)
{
	mModifiedMaterialManager->getModifiedMaterial()->setDiffus( pColor);
	if (mChildren)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChildren->begin(); itr != mChildren->end(); itr++ )
		{
			(*itr)->setDiffus( pColor);
		}
	}
}
//-------------------------------------------------------------------------------------
void Object3D::setSpecular( const ColourValue pColor)
{
	mModifiedMaterialManager->getModifiedMaterial()->setSpecular( pColor);
	if (mChildren)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChildren->begin(); itr != mChildren->end(); itr++ )
		{
			(*itr)->setSpecular( pColor);
		}
	}
}
//-------------------------------------------------------------------------------------
void Object3D::setShininess ( const float pColor)
{
	mModifiedMaterialManager->getModifiedMaterial()->setShininess ( pColor);
	if (mChildren)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChildren->begin(); itr != mChildren->end(); itr++ )
		{
			(*itr)->setShininess( pColor);
		}
	}
}
//-------------------------------------------------------------------------------------
ColourValue Object3D::getAmbient()
{
	return mModifiedMaterialManager->getModifiedMaterial()->getAmbient() ;
}
//-------------------------------------------------------------------------------------
ColourValue Object3D::getDiffus()
{
	return mModifiedMaterialManager->getModifiedMaterial()->getDiffus() ;
}
//-------------------------------------------------------------------------------------
ColourValue Object3D::getSpecular()
{
	return mModifiedMaterialManager->getModifiedMaterial()->getSpecular() ;
}
//-------------------------------------------------------------------------------------
float Object3D::getShininess()
{
	return mModifiedMaterialManager->getModifiedMaterial()->getShininess() ;
}

//-------------------------------------------------------------------------------------
void Object3D::setCullingMode( CullingMode pMode)
{
    mModifiedMaterialManager->getModifiedMaterial()->setCullingMode( pMode );
}

//-------------------------------------------------------------------------------------
CullingMode Object3D::getCullingMode()
{
    return mModifiedMaterialManager->getModifiedMaterial()->getCullingMode();
}

//-------------------------------------------------------------------------------------
void Object3D::rotateFromParent( float pValueX, float pValueY, float pValueZ )
{
	Object3D* parent = getParent();
#if (OGRE_VERSION_MAJOR <= 1 && OGRE_VERSION_MINOR < 6)
	Vector3 dec = mNode->getWorldPosition() - parent->mNode->getWorldPosition();
#else
	Vector3 dec = mNode->_getDerivedPosition() - parent->mNode->_getDerivedPosition();
#endif

//	if( pValueX )		VectorModifier::rotateX( dec, -pValueX * Math::PI / 180 );
//	else if( pValueY )	VectorModifier::rotateY( dec, +pValueX * Math::PI / 180 );
//	else				VectorModifier::rotateZ( dec, +pValueZ * Math::PI / 180 );

	VectorModifier::rotateXYZ( dec, 
		-pValueX * Math::PI / 180,
		+pValueY * Math::PI / 180,
		+pValueZ * Math::PI / 180 );

	dec -= mNode->getPosition();
	mNode->translate( dec.x, dec.y, dec.z, Node::TS_PARENT );
}

//-------------------------------------------------------------------------------------
void Object3D::setTextureScroll(float pU, float pV)
{
	mModifiedMaterialManager->setTextureScroll( pU, pV) ;

	if (mChildren)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChildren->begin(); itr != mChildren->end(); itr++ )
		{
			(*itr)->setTextureScroll(pU, pV) ;
		}
	}
}
//-------------------------------------------------------------------------------------
void Object3D::setTextureScale(float pU, float pV)
{
	mModifiedMaterialManager->setTextureScale( pU, pV) ;

	if (mChildren)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChildren->begin(); itr != mChildren->end(); itr++ )
		{
			(*itr)->setTextureScale(pU, pV) ;
		}
	}
}
//-------------------------------------------------------------------------------------
void Object3D::setTextureRotate(Ogre::Radian pAngle)
{
	mModifiedMaterialManager->setTextureRotate( pAngle) ;

	if (mChildren)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChildren->begin(); itr != mChildren->end(); itr++ )
		{
			(*itr)->setTextureRotate(pAngle) ;
		}
	}
}
//-------------------------------------------------------------------------------------
void Object3D::setAlpha(float pValue)
{
	mModifiedMaterialManager->setAlpha(pValue);

	if (mChildren)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChildren->begin(); itr != mChildren->end(); itr++ )
		{
			(*itr)->setAlpha( pValue ) ;
		}
	}
}
//-------------------------------------------------------------------------------------
float Object3D::getAlpha()
{
	return mModifiedMaterialManager->getAlpha() ;
}
//-------------------------------------------------------------------------------------
void Object3D::setSceneBlendType(SceneBlendType pSceneBlendType)
{
	mModifiedMaterialManager->setSceneBlendType(pSceneBlendType);

	if (mChildren)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChildren->begin(); itr != mChildren->end(); itr++ )
		{
			(*itr)->setSceneBlendType( pSceneBlendType ) ;
		}
	}
}
//-------------------------------------------------------------------------------------
SceneBlendType Object3D::getSceneBlendType()
{
	return mModifiedMaterialManager->getSceneBlendType() ;
}
//-------------------------------------------------------------------------------------
bool Object3D::addCommand( TCommand &pCommandNew, Command &pCommandOld, bool pForSave ) 
{
	bool updateVertex = false;
	bool updateVertexIndex = false;
	std::list<TCommand>::iterator cmd;
	Command temp;

	// it' the same command so do nothing for the moment
	if( mCommandLast == pCommandNew.first ) 
		return false;
	if( mCommandLast == TAPERX && pCommandNew.first == TAPERY ||
		mCommandLast == TAPERY && pCommandNew.first == TAPERX ||
		mCommandLast == TOP_SHEARX && pCommandNew.first == TOP_SHEARY ||
		mCommandLast == TOP_SHEARY && pCommandNew.first == TOP_SHEARX ||
		mCommandLast == TWIST_BEGIN && pCommandNew.first == TWIST_END ||
		mCommandLast == TWIST_END && pCommandNew.first == TWIST_BEGIN ||
		mCommandLast == PATH_CUT_BEGIN && pCommandNew.first == PATH_CUT_END ||
		mCommandLast == PATH_CUT_END && pCommandNew.first == PATH_CUT_BEGIN ||
		mCommandLast == DIMPLE_BEGIN && pCommandNew.first == DIMPLE_END ||
		mCommandLast == DIMPLE_END && pCommandNew.first == DIMPLE_BEGIN ||
		mCommandLast == HOLE_SIZEX && pCommandNew.first == HOLE_SIZEY || pCommandNew.first == HOLLOW_SHAPE ||
		mCommandLast == HOLE_SIZEY && pCommandNew.first == HOLE_SIZEX || pCommandNew.first == HOLLOW_SHAPE ||
		mCommandLast == HOLLOW_SHAPE && pCommandNew.first == HOLE_SIZEX || pCommandNew.first == HOLE_SIZEY ||
		mCommandLast == SKEW && pCommandNew.first == REVOLUTION || pCommandNew.first == RADIUS_DELTA ||
		mCommandLast == REVOLUTION && pCommandNew.first == SKEW || pCommandNew.first == RADIUS_DELTA ||
		mCommandLast == RADIUS_DELTA && pCommandNew.first == SKEW || pCommandNew.first == REVOLUTION )
		return false;

	// it' not the first new command
	if( !mCommandList.empty() )
	{
		// so take the last command added
		cmd = mCommandList.end();
		cmd--;

		// and update its parameters
		if( mCommandLast == TRANSLATE )
		{
			(*cmd).second = getTranslate();
			updateVertex = true;
		}
		else if( mCommandLast == ROTATE )
		{
			(*cmd).second = getRotate();
			updateVertex = true;
		} 
		else if( mCommandLast == SCALE )
		{
			(*cmd).second = getScale();
			updateVertex = true;
		}
		else if( mCommandLast == TAPERX && pCommandNew.first != TAPERY
			||   mCommandLast == TAPERY && pCommandNew.first != TAPERX )
		{
			(*cmd).second = Vector3( mTaperX, mTaperY, 0 ); 
			updateVertex = true;
		}
		else if( mCommandLast == TOP_SHEARX && pCommandNew.first != TOP_SHEARY 
			||   mCommandLast == TOP_SHEARY && pCommandNew.first != TOP_SHEARX )
		{
			(*cmd).second = Vector3( mTopShearX, mTopShearY, 0 ); 
			updateVertex = true;
		}
		else if( mCommandLast == TWIST_BEGIN && pCommandNew.first != TWIST_END
			||   mCommandLast == TWIST_END && pCommandNew.first != TWIST_BEGIN )
		{
			(*cmd).second = Vector3( mTwistBegin, mTwistEnd, 0 ); 
			updateVertex = true;
		}
		else if( mCommandLast == PATH_CUT_BEGIN && pCommandNew.first != PATH_CUT_END
			||   mCommandLast == PATH_CUT_END && pCommandNew.first != PATH_CUT_BEGIN )
		{
			(*cmd).second = Vector3( mPathCutBegin, mPathCutEnd, 0 ); 
			updateVertexIndex = true;
		}
		else if( mCommandLast == DIMPLE_BEGIN && pCommandNew.first != DIMPLE_END
			||   mCommandLast == DIMPLE_END && pCommandNew.first != DIMPLE_BEGIN )
		{
			(*cmd).second = Vector3( mDimpleBegin, mDimpleEnd, 0 ); 
			updateVertexIndex = true;
		}
		else if( mCommandLast == HOLE_SIZEX && pCommandNew.first != HOLE_SIZEY && pCommandNew.first != HOLLOW_SHAPE
			||   mCommandLast == HOLE_SIZEY && pCommandNew.first != HOLE_SIZEX && pCommandNew.first != HOLLOW_SHAPE
			||   mCommandLast == HOLLOW_SHAPE && pCommandNew.first != HOLE_SIZEX && pCommandNew.first != HOLE_SIZEY )
		{
			(*cmd).second = Vector3( mHoleSizeX, mHoleSizeY, mHollowShape ); 
			updateVertexIndex = true;
		}
		else if( mCommandLast == SKEW && pCommandNew.first != REVOLUTION&& pCommandNew.first != RADIUS_DELTA
			||   mCommandLast == REVOLUTION && pCommandNew.first != SKEW&& pCommandNew.first != RADIUS_DELTA
			||   mCommandLast == RADIUS_DELTA && pCommandNew.first != SKEW && pCommandNew.first != REVOLUTION )
		{
			(*cmd).second = Vector3( mSkew, mRadiusDelta, mRevolutions ); 
			updateVertexIndex = true;
		}
		else
		{
			// ...
		}
	}

	// the last command has been updated
	// then update th evertex & index buffers of the object 3D
	if( !pForSave && (updateVertex || updateVertexIndex) )
	{
		restoreBufferVertex( mBufCurrent, mBufBackup );
		if( updateVertexIndex ) 
			restoreBufferIndex( mBufCurrent, mBufBackup );

		resetParameters();
		mBufCurrent->size		= mSize;
		mBufCurrent->cornerMax	= mCornerMax;
		mBufCurrent->cornerMin	= mCornerMin;
	}

	// add the new command to the command stack
	mCommandList.push_back( pCommandNew );

	// and do the same thing to the childrens
	if( mChildren )
		for( std::vector< Object3D* >::iterator child = mChildren->begin(); child != mChildren->end(); child++ )
			(*child)->addCommand( pCommandNew, temp );

	// replace the last command by the current one
	mCommandLast = pCommandNew.first;

	return true;
}

//-------------------------------------------------------------------------------------
bool Object3D::restoreBuffer( Buffer* pBufNew, Buffer* pBufOld )
{
	return ( restoreBufferVertex( pBufNew, pBufOld ) && restoreBufferIndex( pBufNew, pBufOld ) );
}

//-------------------------------------------------------------------------------------
bool Object3D::restoreBufferVertex( Buffer* pBufNew, Buffer* pBufOld )
{
	if( pBufNew == 0 || pBufOld == 0 ) 
		// one or two buffers are empty ! so do nothing
		return false;

	// Restore / copy the old buffer by the new one
	pBufOld->vertexCount		= pBufNew->vertexCount;
	pBufOld->vertex.resize(pBufNew->vertexCount*mVertexDecl/4);
	pBufOld->vertex = pBufNew->vertex;

	// update the bounding box sizes
	pBufOld->size		= pBufNew->size;
	pBufOld->cornerMax	= pBufNew->cornerMax;
	pBufOld->cornerMin	= pBufNew->cornerMin;

	return true;
}

//-------------------------------------------------------------------------------------
bool Object3D::restoreBufferIndex( Buffer* pBufNew, Buffer* pBufOld )
{
	if( pBufNew == 0 || pBufOld == 0 ) 
		// one or two buffers are empty ! so do nothing
		return false;

	// Restore / copy the old buffer by the new one
	pBufOld->indexCount			= pBufNew->indexCount;
	pBufOld->index.resize(pBufNew->indexCount);
	pBufOld->index = pBufNew->index;	

	return true;
}

//-------------------------------------------------------------------------------------
bool Object3D::undo()
{
	// there is no more command in the stack
	// so nothing to do
	if( mCommandList.empty() ) return false;

	// there are some command in the stack
	// so remove the last one from the stack
	mCommandList.pop_back();

	// and restart from the primitiv shape
	restoreBuffer( mBufPrim, mBufBackup );
	restoreBuffer( mBufPrim, mBufCurrent );
	if( getParent() == NULL ) 
		//mNode->setPosition( Vector3::ZERO );
		mNode->setPosition( mCentreSelection );

	// reset the parameters & the bounding box sizes
	resetParameters();
	mSize		= mBufCurrent->size			= mBufBackup->size			= mBufPrim->size;
	mCornerMax	= mBufCurrent->cornerMax	= mBufBackup->cornerMax		= mBufPrim->cornerMax;
	mCornerMin	= mBufCurrent->cornerMin	= mBufBackup->cornerMin		= mBufPrim->cornerMin;

	// apply the previews commands (transformations)
	for( list<TCommand>::iterator cmd = mCommandList.begin(); cmd != mCommandList.end(); cmd++ )
	{
		Vector3 v = (*cmd).second;
		switch( (*cmd).first )
		{
		case TRANSLATE: 
			if( getParent() != NULL ) 
				break;
			if( v.x || v.y || v.z ) apply( TRANSLATE, v.x, v.y, v.z );
			break;
		case ROTATE: 
			if( v.x || v.y || v.z ) apply( ROTATE, v.x, v.y, v.z );
			break;
		case SCALE: 
			if( v.x != 1 || v.y != 1 || v.z != 1 ) 
			{
				setScale( v.x, v.y, v.z );
				apply( SCALE, v.x, v.y, v.z );
			}
			break;
		case TAPERX: 
		case TAPERY: 
			if( v.x ) apply( TAPERX, v.x, 0, 0 );
			if( v.y ) apply( TAPERY, v.y, 0, 0 );
			break;
		case TOP_SHEARX: 
		case TOP_SHEARY: 
			if( v.x ) apply( TOP_SHEARX, v.x, 0, 0 );
			if( v.y ) apply( TOP_SHEARY, v.y, 0, 0 );
			break;
		case TWIST_BEGIN:
		case TWIST_END: 
			if( v.x ) apply( TWIST_BEGIN, v.x, 0, 0 );
			if( v.y ) apply( TWIST_END, v.y, 0, 0 );
			break;
		case PATH_CUT_BEGIN: 
		case PATH_CUT_END: 
			if( v.x ) apply( PATH_CUT_BEGIN, v.x, 0, 0 );
			if( v.y ) apply( PATH_CUT_END, v.y, 0, 0 );
			break;
		case DIMPLE_BEGIN: 
		case DIMPLE_END: 
			if( v.x ) apply( DIMPLE_BEGIN, v.x, 0, 0 );
			if( v.y ) apply( DIMPLE_END, v.y, 0, 0 );
			break;
		case HOLE_SIZEX: 
		case HOLE_SIZEY: 
		case HOLLOW_SHAPE: 
			if( v.x ) apply( HOLE_SIZEX, v.x, 0, 0 );
			if( v.y ) apply( HOLE_SIZEY, v.y, 0, 0 );
			if( v.z ) apply( HOLLOW_SHAPE, v.z, 0, 0 );
			break;
		case SKEW: 
		case REVOLUTION: 
		case RADIUS_DELTA: 
			if( v.x ) apply( SKEW, v.x, 0, 0 );
			if( v.y ) apply( REVOLUTION, v.y, 0, 0 );
			if( v.z ) apply( RADIUS_DELTA, v.z, 0, 0 );
			break;
		}
		restoreBuffer( mBufCurrent, mBufBackup );
		resetParameters();
		updateBoundingBox();
	}

	// reset the parameters & the bounding box sizes
//	resetParameters();
//	updateBoundingBox();
	mBufCurrent->size		= mBufBackup->size		= mSize;
	mBufCurrent->cornerMax	= mBufBackup->cornerMax	= mCornerMax;
	mBufCurrent->cornerMin	= mBufBackup->cornerMin	= mCornerMin;

	// update the vertex & index hardware buffers
	resizeBuffers( 
		mBufCurrent->vertex, mBufCurrent->vertexCount,
		mBufCurrent->index, mBufCurrent->indexCount );

	// that was the last command in the stack so reinitialize the bounding box
	if( mCommandList.empty() )
	{
		mCommandLast = NONE;

		apply( TRANSLATE, -1 );		// to reinitialize the bounding box ???
		apply( TRANSLATE, +1 );		// to reinitialize the bounding box ???
	}

	// do all the same for the childrens
	if ( mChildren )//&& !children->empty() )
		for( vector< Object3D* >::iterator child = mChildren->begin(); child != mChildren->end(); child++ )	
			(*child)->undo();

	return true;
}

}//namespace
