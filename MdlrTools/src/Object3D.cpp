#include "Object3D.h"
#include "ModifiedMaterialManager.h"
#include "ModifiedMaterial.h"
#include "VectorModifier.h"
#include "MeshModifier.h"
#include "FileBrowser.h"
#include "SolidBoolOp.h"

#include "SolipsisErrorHandler.h"
// Tinyxml
#include "../Dependencies/tinyxml/include/tinyxml.h"

namespace Solipsis {

//-------------------------------------------------------------------------------------
Object3D::Object3D(String pName, SceneNode* pNode)
{
	mNode = pNode;
	mEntity = (Entity*)pNode->getAttachedObject(pName);

	// get the size for a vertex declaration (position, normal, colour, tex_coord ...)
	mVertexData = 
		mEntity->getMesh()->getSubMesh(0)->useSharedVertices ? 
		mEntity->getMesh()->sharedVertexData : mEntity->getMesh()->getSubMesh(0)->vertexData;
	mVertexDecl = mVertexData->vertexDeclaration->getVertexSize(0);

	// get the object datas
	mVertex = 0;
	mIndex = 0;
 	getDataFromBuffer( NULL, NULL );

	// create the (backup) buffers
	mBufPrim		= new Buffer();
	mBufBackup		= new Buffer();
	mBufCurrent		= new Buffer();

	// initialize the (backup) buffers
	mBufCurrent->vertexCount	= mBufBackup->vertexCount	= mBufPrim->vertexCount	= mVertexCount;
	mBufCurrent->indexCount		= mBufBackup->indexCount	= mBufPrim->indexCount	= mIndexCount;
	mBufPrim->vertex			= new Real [ mVertexCount*mVertexDecl/4 ];
	mBufBackup->vertex			= new Real [ mVertexCount*mVertexDecl/4 ];
	mBufCurrent->vertex			= new Real [ mVertexCount*mVertexDecl/4 ];
	mBufPrim->index				= new unsigned int [ mIndexCount ];
	mBufBackup->index			= new unsigned int [ mIndexCount ];
	mBufCurrent->index			= new unsigned int [ mIndexCount ];

	for( unsigned int i = 0; i < mVertexCount*mVertexDecl/4; i++ )
		mBufCurrent->vertex[i]	= mBufBackup->vertex[i]	= mBufPrim->vertex[i]	= mVertex[i];
	for( unsigned int i = 0; i < mIndexCount; i++ )
		mBufCurrent->index[i]	= mBufBackup->index[i]	= mBufPrim->index[i]	= mIndex[i];

	// get the object size max / min
	getSize( mSize, mCornerMin, mCornerMax );

	mBufCurrent->size		= mBufBackup->size		= mBufPrim->size			= mSize;
	mBufCurrent->cornerMax	= mBufBackup->cornerMax	= mBufPrim->cornerMax	= mCornerMax;
	mBufCurrent->cornerMin	= mBufBackup->cornerMin	= mBufPrim->cornerMin	= mCornerMin;

	mChilds = NULL;		// must be loaded from the .XML
	mParent = 0;		// must be loaded from the .XML

	mName = pName;

	//mType = OTHER;
	resetParameters();

	mModifiedMaterialManager = new ModifiedMaterialManager() ;
	const MaterialPtr& tmpMaterial = mEntity->getSubEntity(0)->getMaterial()->clone("Material"+mName);
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

	// Init the last command 
	mCommandLast = NONE;
}

//-------------------------------------------------------------------------------------
Object3D::~Object3D()
{
	//delete mEntity;		
	mEntity = 0;
	mNode = 0;
	delete mChilds;		mChilds = 0;
}

//-------------------------------------------------------------------------------------
int		Object3D::loadFromFile(TiXmlDocument &doc, string texturepath)
{
	Ogre::String extractedAttribute;

	TiXmlElement *e = doc.RootElement()->FirstChildElement("properties");
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
	from_string(e->FirstChildElement("taperx")->Attribute("value"),mTaperX);
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
		toAdd.second.y = atoi(valueTransfo.c_str() )  / 1000.;
		from_string( trans->Attribute("valueZ"),valueTransfo );
		toAdd.second.z = atoi(valueTransfo.c_str() )  / 1000.;

		Vector3 v = toAdd.second;
		switch( toAdd.first )
		{
		case TRANSLATE : 
			if( v.x || v.y || v.z ) apply( TRANSLATE, v.x, v.y, v.z );
			break;
		case ROTATE : 
			if( v.x || v.y || v.z ) apply( ROTATE, v.x, v.y, v.z );
			break;
		case SCALE : 
			if( v.x != 1 || v.y != 1 || v.z != 1 ) 
			{
				setScale( v.x, v.y, v.z );
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
		resetParameters();

		mCommandList.push_back(toAdd);
		mCommandLast = toAdd.first;

		sDebug << "Applying tranformation : " << toAdd.first << " with value : " << toAdd.second << " to " << mName << endl; 
		SOLIPSISINFO( sDebug.str().c_str() );
		trans = trans->NextSiblingElement( "transfo" );
	}

	e = doc.RootElement()->FirstChildElement("material");
	ColourValue cv;
	from_string(e->FirstChildElement("matambiant")->Attribute("r"),cv.r);
	from_string(e->FirstChildElement("matambiant")->Attribute("g"),cv.g);
	from_string(e->FirstChildElement("matambiant")->Attribute("b"),cv.b);
	from_string(e->FirstChildElement("matambiant")->Attribute("a"),cv.a);
	setAmbiant(cv);
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
	float value = 0 ;
	from_string(e->FirstChildElement("matshin")->Attribute("value"),value);
	setShininess( value);
	from_string(e->FirstChildElement("matopac")->Attribute("value"),value);
	setAlpha( value);
		//texture scroll, scale and rotate :
	Ogre::Vector2 tmpVec  ;
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
		texture = TextureManager::getSingleton().load( (texturepath + trans->Attribute("Name")) , ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		addTexture( texture) ;

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
	mNode->setPosition(tmp);
	from_string(e->FirstChildElement("objorientation")->Attribute("x"),tmp.x);
	from_string(e->FirstChildElement("objorientation")->Attribute("y"),tmp.y);
	from_string(e->FirstChildElement("objorientation")->Attribute("z"),tmp.z);
	// TODO : Restore Orientation
	from_string(e->FirstChildElement("objscale")->Attribute("x"),tmp.x);
	from_string(e->FirstChildElement("objscale")->Attribute("y"),tmp.y);
	from_string(e->FirstChildElement("objscale")->Attribute("z"),tmp.z);
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
	String parentName = "NULL" ;
	if(mParent != NULL)
		parentName = mParent->getName() ;
	toSave << "\t\t<objparent Name=\"" << parentName << "\" />" << endl;
	toSave << "\t</properties>" << endl;

	toSave << "\t<model>" << endl;
	toSave << "\t\t<primitive Name=\"" << SOLTYPESTRING[(int)mType] << "\" />" << endl;
	toSave << "\t\t<taperx value=\"" << mTaperX << "\" />" << endl;
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
	toSave << "\t\t<transformation>" << endl;

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
	ColourValue cv  = getAmbiant();
	toSave << "\t\t<matambiant r=\"" << cv.r << "\" g=\"" << cv.g << "\" b=\"" << cv.b << "\" a=\"" << cv.a << "\" />" << endl;
	cv  = getDiffus();
	toSave << "\t\t<matdiff r=\"" << cv.r << "\" g=\"" << cv.g << "\" b=\"" << cv.b << "\" a=\"" << cv.a << "\" />" << endl;
	cv  = getSpecular();
	toSave << "\t\t<matspec r=\"" << cv.r << "\" g=\"" << cv.g << "\" b=\"" << cv.b << "\" a=\"" << cv.a << "\" />" << endl;
	float value  = getShininess();
	toSave << "\t\t<matshin value=\"" << value << "\" />" << endl;
	value = mModifiedMaterialManager->getAlpha();
	toSave << "\t\t<matopac value=\"" << value << "\" />" << endl;
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
		texturePath = mModifiedMaterialManager->getTexture(i)->getName();
		nameSizeChar = texturePath.find_last_of( '\\' );
		std::string textureName (texturePath, nameSizeChar+1,texturePath.length() );

		if( mModifiedMaterialManager->getTexture(i) == mModifiedMaterialManager->getCurrentTexture() )
		{
			currentTexture = "true" ;
		}
		else
			currentTexture = "false" ;

		//save in XML :
		toSave << "\t\t\t<texture Name=\"" << textureName << "\" currenttexture=\"" << currentTexture << "\" />"  << endl;		
	}

	toSave << "\t\t</texturelist>" << endl;
	toSave << "\t</material>" << endl;

	toSave << "\t<threeD>" << endl;
	Vector3 tmp = getPosition(true);
	toSave << "\t\t<objposition x=\"" << tmp.x << "\" y=\"" << tmp.y << "\" z=\"" << tmp.z << "\" />" << endl;
	tmp = getOrientation();
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
void Object3D::setScale(Real pX, Real pY, Real pZ )
{
	mScaleX = pX;
	mScaleY = pY;
	mScaleZ = pZ;
}
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
void Object3D::setHollow(Real value)
{
	mHollow = value;
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
	mRotationX = 0;
	mRotationY = 0;
	mRotationZ = 0;
	mScaleX = 1;
	mScaleY = 1;
	mScaleZ = 1;
	mPathCutBegin = 0;
	mPathCutEnd = 1;
	mTwistBegin = 0;
	mTwistEnd = 0;
	mTaperX = 0;
	mTaperY = 0;
	mTopShearX = 0;
	mTopShearY = 0;
	mDimpleBegin = 0;
	mDimpleEnd = 1;
	mHoleSizeX = 0;
	mHoleSizeY = 0;
	mHollow = 0;
	mHollowShape = SQUARE;
	mProfileCuteBegin = 0;
	mProfileCuteEnd = 1;
	mSkew = 0;
	mRadiusDelta = 0;
	mRevolutions = 1;
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
	Real *vertex = 0;
	Real *vertexA = 0;
	Real *vertexB = 0;
	unsigned int vertexCount, indexCount, *index = 0;
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


	switch( command )
	{
	// basic transformations
	case TRANSLATE:
		{
			if(0)
			{
				for(unsigned int i=0; i<mVertexCount; i++)
				{
					unsigned int id = i*mVertexDecl/4;
					mBufCurrent->vertex[id] += p1;
					mBufCurrent->vertex[id+1] += p2;
					mBufCurrent->vertex[id+2] += p3;
				}

				update();
			}
			else
				mNode->translate( p1, p2, p3 );
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

				mBufCurrent->vertex[id] = vertex.x;			mBufCurrent->vertex[id+3] = normal.x;
				mBufCurrent->vertex[id+1] = vertex.y;		mBufCurrent->vertex[id+4] = normal.y;
				mBufCurrent->vertex[id+2] = vertex.z;		mBufCurrent->vertex[id+5] = normal.z;
			}

			update();
			break;
		}

	case SCALE:
		{
			//if( p1 <= 0 && p2 <= 0 && p3 <= 0 ) return false;
			if( p1 != 1 ) { p2 = mScaleY; p3 = mScaleZ; }
			else if( p2 != 1 ) { p1 = mScaleX; p3 = mScaleZ; }
			else if( p3 != 1 ) { p1 = mScaleX; p2 = mScaleY; }
			setScale( p1, p2, p3 );

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

			// on effectu une translation des sommets sur l'axe X 
			// Som.X += Som.Y * Transl / Size.Y
/*			w = mPointsBackup->begin();
			for( v = mPoints->begin(); v != mPoints->end(); v++ )
			{
			//(*v).x += (*v).y * p1 * mSize.x / mSize.y;		// other version
			//(*v).x += (*v).y * p1 * mSize.x / mCornerMax.y;				// with delta incr modif
			(*v).x = (*w).x + (*w).y * p1 * mSize.x / mCornerMax.y;
			w++;
			}
*/
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

			// on effectu une translation des sommets sur l'axe Z 
			// Som.Z += Som.Y * Transl / Size.Y
/*			w = mPointsBackup->begin();
			for( v = mPoints->begin(); v != mPoints->end(); v++ )
			{
				//(*v).z += (*v).y * p2 * mSize.z / mCornerMax.y;				// with delta incr modif
				(*v).z = (*w).z + (*w).y * p2 * mSize.z / mCornerMax.y;
				w++;
			}
*/
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

			p1 *= Math::PI * 2; // 6.2831853;
			Vector3 center = mNode->getPosition();
			Real ratio, angle;
			
/*			w = mPointsBackup->begin();
			for( v = mPoints->begin(); v != mPoints->end(); v++ )
			{
				(*v) -= center;
				ratio = (*w).y / mCornerMax.y / 2;		// 1 = top, 0 = bottom
				angle = ratio * (p1 );//- mTwistEnd) + mTwistEnd;			// ratio = 1 -> angle = p1		ratio = 0 -> angle = mTwistEnd
				(*v) = (*w);
				VectorModifier::rotateY( (*v), angle );
				(*v) += center;
				w++;
			}
*/
			for(unsigned int i=0; i<mVertexCount; i++)
			{
				unsigned int id = i*mVertexDecl/4;
				Vector3 v;
				v.x = mBufBackup->vertex[id];
				v.y = mBufBackup->vertex[id+1];
				v.z = mBufBackup->vertex[id+2];

				v -= center;
				ratio = mBufBackup->vertex[id+1] / mCornerMax.y / 2;			// 1 = top, 0 = bottomm
				angle = ratio * p1;
				VectorModifier::rotateY( v, angle );
				if( 0 < mTwistEnd )
				{
					ratio = 0.5 - mBufBackup->vertex[id+1] / mCornerMax.y / 2;	// 1 = top, 0 = bottom
					angle = ratio * mTwistEnd * Math::PI * 2;
					VectorModifier::rotateY( v, -angle );
				}
				v += center;

				mBufCurrent->vertex[id] = v.x;
				mBufCurrent->vertex[id+1] = v.y;
				mBufCurrent->vertex[id+2] = v.z;
			}

			update();
			break;
		}
	case TWIST_END:
		{
			if( 0 > p1 || p1 > 1 ) return false;
			setTwistEnd( p1 );

			p1 *= Math::PI * 2; // 6.2831853;
			Vector3 center = mNode->getPosition();
			Real ratio, angle;
		
/*			w = mPointsBackup->begin();
			for( v = mPoints->begin(); v != mPoints->end(); v++ )
			{
				(*v) -= center;
				ratio = 0.5 - ( (*w).y / mCornerMax.y / 2) ;		// 1 = top, 0 = bottom
				//angle = ratio * (mTwistBegin - p1) + p1;			// ratio = 1 -> angle = mTwistBegin		ratio = 0 -> angle = p2
				angle = ratio * ( -p1);// + p1;
				(*v) = (*w);
				VectorModifier::rotateY( (*v), angle );
				(*v) += center;
				w++;
			}
*/
			for(unsigned int i=0; i<mVertexCount; i++)
			{
				unsigned int id = i*mVertexDecl/4;
				Vector3 v;
				v.x = mBufBackup->vertex[id];
				v.y = mBufBackup->vertex[id+1];
				v.z = mBufBackup->vertex[id+2];

				v -= center;
				if( 0 < mTwistBegin )
				{
					ratio = mBufBackup->vertex[id+1] / mCornerMax.y / 2;		// 1 = top, 0 = bottomm
					angle = ratio * mTwistBegin * Math::PI * 2;
					VectorModifier::rotateY( v, angle );
				}
				ratio = 0.5 - mBufBackup->vertex[id+1] / mCornerMax.y / 2;		// 1 = top, 0 = bottom
				angle = ratio * p1;
				VectorModifier::rotateY( v, -angle );
				v += center;

				mBufCurrent->vertex[id] = v.x;
				mBufCurrent->vertex[id+1] = v.y;
				mBufCurrent->vertex[id+2] = v.z;
			}

			update();
			break;
		}

	case PATH_CUT_BEGIN:
		if( (p1 == 0) && (mPathCutEnd == 1) ) return false;
		if( (p1 - mPathCutEnd) >= 0 ) return false;
		setPathCutBegin( p1 );
		mAngleBegin = mPathCutBegin;
		mAngleEnd = mPathCutEnd;
		path_cut = true;
		break;
	case PATH_CUT_END:
		if( (mPathCutBegin == 0) && (p1 == 1) ) return false;
		if( (mPathCutBegin - p1) >= 0 ) return false;
		setPathCutEnd( p1 );
		mAngleBegin = mPathCutBegin;
		mAngleEnd = mPathCutEnd;
		path_cut = true;
		break;

	case DIMPLE_BEGIN:
		if( (p1 == 0) && (mDimpleEnd == 1) ) return false;
		if( (p1 - mDimpleEnd) >= 0 ) return false;
		setDimpleBegin( p1 );
		mAngleBegin = mDimpleBegin;
		mAngleEnd = mDimpleEnd;
		dimple = true;
		break;
	case DIMPLE_END:
		if( (mDimpleBegin == 0) && (p1 == 1) ) return false;
		if( (mDimpleBegin - p1) >= 0 ) return false;
		setDimpleEnd( p1 );
		mAngleBegin = mDimpleBegin;
		mAngleEnd = mDimpleEnd;
		dimple = true;
		break;

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
		solidA = sbo->dataLoad( 
			mBufBackup->vertex, mBufBackup->vertexCount, mVertexDecl/4,
			mBufBackup->index, mBufBackup->indexCount/3 );
		solidA->DataCloseCallback();

		// SOLID_B : generate a new SOLID
		if( hole )
			MeshModifier::genCylinder(
				command,
				vertex, vertexCount, mVertexDecl/4, 
				index, indexCount,
				hollow_sides, mHoleSizeY, mHoleSizeX,
				mCornerMax, mCornerMin );
		else
			MeshModifier::genCylinderCut(
				command,
				vertex, vertexCount, mVertexDecl/4, 
				index, indexCount, 
				mAngleBegin, mAngleEnd, 
				mCornerMax, mCornerMin );
		solidB = sbo->dataLoad( vertex, vertexCount, mVertexDecl/4, index, indexCount/3 );
		solidB->DataCloseCallback();

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
		delete vertex;		vertex = new Real [vertexCount * mVertexDecl/4];
        unsigned int i;
		for(i=0; i<vertexCountA*mVertexDecl/4; i++)	*vertex++ = *vertexA++;
		for(i=0; i<vertexCountB*mVertexDecl/4; i++)	*vertex++ = *vertexB++;
		// index data
		delete index;		index = new unsigned int [indexCount];
		for(i=0; i<indexCountA; i++)	*index++ = *indexA++;
		for(i=0; i<indexCountB; i++)	*index++ = *indexB++ + vertexCountA;
		// replace pointers at startup
		vertex -= vertexCount * mVertexDecl/4;
		index -= indexCount;

		// Update vertex & index datas
		resizeBuffers( vertex, vertexCount, index, indexCount );
		path_cut = dimple = hole = false;

		delete mBufCurrent->vertex;
		delete mBufCurrent->index;

		mBufCurrent->vertex			= vertex;
		mBufCurrent->vertexCount	= vertexCount;
		mBufCurrent->index			= index;
		mBufCurrent->indexCount		= indexCount;
//		updateBoundingBox();
	}

	// skew deformation
	if( skew && (mType == CYLINDER || mType == TORUS || mType == TUBE) )
	{
		delete vertex;	vertex = 0;
		delete index;	index = 0;

		// Generate the new modified object3D
		MeshModifier::genCylinderSkew(
			command,
			vertex, vertexCount, mVertexDecl/4, 
			index, indexCount,
			16, 1, 1,
			mSkew, mRadiusDelta, mRevolutions, mType,
			mBufCurrent->cornerMax, mBufCurrent->cornerMin );

		// Update vertex & index datas
		resizeBuffers( vertex, vertexCount, index, indexCount );
		skew = false;

		delete mBufCurrent->vertex;
		delete mBufCurrent->index;

		mBufCurrent->vertex			= vertex;
		mBufCurrent->vertexCount	= vertexCount;
		mBufCurrent->index			= index;
		mBufCurrent->indexCount		= indexCount;
	}

	// childs
 	if (mChilds)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChilds->begin(); itr != mChilds->end(); itr++ )
		{
			if(command == ROTATE)
			{
				(*itr)->mCentreSelection = mCentreSelection ;
				(*itr)->mCentreRotation = mCentreRotation ;
				(*itr)->mCentreObject = mCentreObject ;
				(*itr)->findRotationPosition( p1, p2, p2, mCentreSelection, mCentreRotation, mCentreObject);
			}
			(*itr)->apply( command, p1, p2, p3 ) ;
		}
	}

	mNode->_updateBounds();
	return true;
}

//-------------------------------------------------------------------------------------
Vector3 Object3D::getPosition(bool worldPosition )
{
	if (worldPosition)
		return mNode->getWorldPosition();

	return mNode->getPosition();
}
//-------------------------------------------------------------------------------------
bool Object3D::linkObject(Object3D* pObj, SceneManager* pSceneMgr)
{
	SceneNode * pObjScenNode = pObj->getEntity()->getParentSceneNode();
	Vector3 Wpostion = pObjScenNode->getWorldPosition() ;;
	if( isLink(pObj) )
	{			//remove it ...
		removeChild( pObj);			//to mChilds
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
			addChild( pObj);			//... to mChilds
										//... to this current node	 
			pSceneMgr->getRootSceneNode()->removeChild( pObjScenNode ) ;
			mNode->addChild( pObjScenNode );
			pObjScenNode->setPosition( Wpostion - mNode->getWorldPosition() );
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
	if (!mChilds)
		return false;

	vector< Object3D* >::iterator itr ;
	for( itr = mChilds->begin(); itr != mChilds->end(); itr++ )
	{
		if ( (*itr) == pObj )
			return true;
	}
	return false;
}
//-------------------------------------------------------------------------------------
Vector3 Object3D::getOrientation()
{
	return Vector3(mNode->getOrientation().getYaw().valueDegrees(),mNode->getOrientation().getPitch().valueDegrees(),mNode->getOrientation().getRoll().valueDegrees());
}

//-------------------------------------------------------------------------------------
void Object3D::addChild(Object3D* pChild)
{
	if( !mChilds )
		mChilds = new vector< Object3D* >;

	mChilds->push_back( pChild ) ;
}

//-------------------------------------------------------------------------------------
vector< Object3D* >* Object3D::getChilds()
{
	return mChilds;
}

//-------------------------------------------------------------------------------------
void Object3D::removeChild(Object3D* pChild)
{
	if( mChilds )
	{
		vector< Object3D* >::iterator i ;
		for(i = mChilds->begin(); i!=mChilds->end(); i++)
		{
			if ( (*i) == pChild )
			{
				mChilds->erase( i);
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
		Real *vertex = mBufCurrent->vertex; // = mVertex;

		//for(unsigned int i=0; i<mVertexCount; i++)
		for(unsigned int i=0; i<mBufCurrent->vertexCount; i++)
		{
			unsigned int id = i*mVertexDecl/4;

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
		mVertex,
		mIndexCount,
		mIndex);

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
	Real *vertexNew = mBufCurrent->vertex;

	for(unsigned int i=0; i<mVertexCount*mVertexDecl/4; i++)
		*vertex++ = *vertexNew++;		//vertex[i] = mBufCurrent->vertex[i];

	vbuf->unlock();
}

//-------------------------------------------------------------------------------------
void Object3D::resizeBuffers( Real* pVertexData, size_t pVertexCount, unsigned* pIndexData, size_t pIndexCount )
{
	SubMesh* subMesh = mEntity->getMesh()->getSubMesh(0);

	// Update vertex count in the render operation
	mVertexData->vertexCount = pVertexCount;

	// Create new vertex buffer
	HardwareVertexBufferSharedPtr vbuf =
		HardwareBufferManager::getSingleton().createVertexBuffer(
			mVertexDecl,
			mVertexData->vertexCount, //mVertexBufferCapacity,
			//HardwareBuffer::HBU_STATIC_WRITE_ONLY);
			HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY); // TODO: Custom HBU_?

	// Upload the vertex data to the card
	vbuf->writeData( 0, vbuf->getSizeInBytes(), pVertexData, true );

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
	ibuf->writeData( 0, ibuf->getSizeInBytes(), pIndexData, true );

	// Update index
	subMesh->indexData->indexBuffer = ibuf;

	// Notify mesh object that it has been loaded
	mEntity->getMesh()->load();
}

//-------------------------------------------------------------------------------------
void Object3D::showBoundingBox(bool pValue)
{
	mNode->showBoundingBox(pValue);
	if (mChilds)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChilds->begin(); itr != mChilds->end(); itr++ )
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
void Object3D::addTexture(TexturePtr texture)
{
	//Test if this texture already exists :
	TexturePtr PrecPtrTexture = mModifiedMaterialManager->getTexture(texture->getName() );	//return NULL if this texture doesn't exist
	if ( PrecPtrTexture == TexturePtr () )	//if (PrecPtrTexture == NULL) ...
	{
		mModifiedMaterialManager->addTexture(texture);
	}
	
	if (mChilds)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChilds->begin(); itr != mChilds->end(); itr++ )
		{
			(*itr)->addTexture(texture);
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

	//Apply remove for all childs :
	if (mChilds)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChilds->begin(); itr != mChilds->end(); itr++ )
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

	if (mChilds)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChilds->begin(); itr != mChilds->end(); itr++ )
		{
			(*itr)->setCurrentTexture(texture);
		}
	}
}
//-------------------------------------------------------------------------------------
void Object3D::setCurrentTexture(const TexturePtr pTexture)
{
	//test if this texture is in the list
	if( ! mModifiedMaterialManager->isPresentInList( pTexture ) )
	{
		//we add it if it isn't present :
		mModifiedMaterialManager->addTexture( pTexture );
	}

	mModifiedMaterialManager->setCurrentTexture( pTexture) ;

	if (mChilds)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChilds->begin(); itr != mChilds->end(); itr++ )
		{
			(*itr)->setCurrentTexture(pTexture);
		}
	}
}
//-------------------------------------------------------------------------------------
TexturePtr Object3D::getCurrentTexture()
{
	return mModifiedMaterialManager->getCurrentTexture() ;
}
//-------------------------------------------------------------------------------------
void Object3D::setAmbiant( const ColourValue pColor)
{
	mModifiedMaterialManager->getModifiedMaterial()->setAmbient( pColor);
	if (mChilds)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChilds->begin(); itr != mChilds->end(); itr++ )
		{
			(*itr)->setAmbiant(pColor);
		}
	}
}
//-------------------------------------------------------------------------------------
void Object3D::setDiffus( const ColourValue pColor)
{
	mModifiedMaterialManager->getModifiedMaterial()->setDiffus( pColor);
	if (mChilds)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChilds->begin(); itr != mChilds->end(); itr++ )
		{
			(*itr)->setDiffus( pColor);
		}
	}
}
//-------------------------------------------------------------------------------------
void Object3D::setSpecular( const ColourValue pColor)
{
	mModifiedMaterialManager->getModifiedMaterial()->setSpecular( pColor);
	if (mChilds)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChilds->begin(); itr != mChilds->end(); itr++ )
		{
			(*itr)->setSpecular( pColor);
		}
	}
}
//-------------------------------------------------------------------------------------
void Object3D::setShininess ( const float pColor)
{
	mModifiedMaterialManager->getModifiedMaterial()->setShininess ( pColor);
	if (mChilds)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChilds->begin(); itr != mChilds->end(); itr++ )
		{
			(*itr)->setShininess( pColor);
		}
	}
}
//-------------------------------------------------------------------------------------
ColourValue Object3D::getAmbiant()
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
void Object3D::move (float pValueX, float pValueY, float pValueZ)
{
	mNode->translate(pValueX, pValueY, pValueZ, Node::TS_WORLD);
}
//-------------------------------------------------------------------------------------
void Object3D::scale (float pValueX, float pValueY, float pValueZ)
{
	apply( Object3D::SCALE, pValueX, pValueY, pValueZ);
	mNode->_updateBounds();

}
//-------------------------------------------------------------------------------------
void Object3D::rotate (float pValueX, float pValueY, float pValueZ, Vector3 pCentreSelection,
					   SceneNode * pCentreRotation, SceneNode* pCentreObject)
{
	mCentreSelection = pCentreSelection ;
	mCentreRotation = pCentreRotation;
	mCentreObject = pCentreObject ;

	findRotationPosition( pValueX, pValueY, pValueZ, mCentreSelection, mCentreRotation, mCentreObject);
	apply( Object3D::ROTATE, pValueX, pValueY, pValueZ );


}
//-------------------------------------------------------------------------------------
void Object3D::findRotationPosition( float pValueX, float pValueY, float pValueZ, Vector3 pCentreSelection,
									 SceneNode * pCentreRotation, SceneNode* pCentreObject)
{
	pCentreRotation->setOrientation( pCentreRotation->getInitialOrientation() );
	pCentreObject->setOrientation( pCentreObject->getInitialOrientation() );
	pCentreObject->setPosition( pCentreObject->getInitialPosition() );

	pCentreRotation->setPosition( pCentreSelection ) ;
	Vector3 ObjectPosition = mNode->getWorldPosition() - pCentreRotation->getWorldPosition() ;
	pCentreObject->translate( ObjectPosition ) ;
	//... pCentreRotation and pCentreObject are correctly positionned

	pCentreRotation->pitch(Degree(pValueX));
	pCentreRotation->yaw(Degree(pValueY));
	pCentreRotation->roll(Degree(pValueZ));

	//Now we put mNode on pCentreObject ...
	ObjectPosition = pCentreObject->getWorldPosition() - mNode->getParentSceneNode()->getWorldPosition();
	mNode->setPosition( ObjectPosition );
}
//-------------------------------------------------------------------------------------
void Object3D::setTextureScroll(float pU, float pV)
{
	mModifiedMaterialManager->setTextureScroll( pU, pV) ;

	if (mChilds)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChilds->begin(); itr != mChilds->end(); itr++ )
		{
			(*itr)->setTextureScroll(pU, pV) ;
		}
	}
}
//-------------------------------------------------------------------------------------
void Object3D::setTextureScale(float pU, float pV)
{
	mModifiedMaterialManager->setTextureScale( pU, pV) ;

	if (mChilds)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChilds->begin(); itr != mChilds->end(); itr++ )
		{
			(*itr)->setTextureScale(pU, pV) ;
		}
	}
}
//-------------------------------------------------------------------------------------
void Object3D::setTextureRotate(Ogre::Radian pAngle)
{
	mModifiedMaterialManager->setTextureRotate( pAngle) ;

	if (mChilds)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChilds->begin(); itr != mChilds->end(); itr++ )
		{
			(*itr)->setTextureRotate(pAngle) ;
		}
	}
}
//-------------------------------------------------------------------------------------
void Object3D::setAlpha(float pValue)
{
	mModifiedMaterialManager->setAlpha( pValue ) ;

	if (mChilds)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChilds->begin(); itr != mChilds->end(); itr++ )
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
bool Object3D::addCommand( TCommand &pTCommand, Command &pOldCommand ) 
{
	bool updateVertex = false;
	bool updateVertexIndex = false;

	// test for a new transformation
	if( pTCommand.first == mCommandLast )
		// always on the same transformation, no need to update the vertex & index buffers
		return false;
	else
	{
		// another transformation has been used
		// and it can be a neightboor
		// if it's not the case, push this command
		if( mCommandLast == TRANSLATE )
		{
			pTCommand.second = Vector3( mNode->getPosition().x, mNode->getPosition().y, mNode->getPosition().z );
			goto UPDATE;
		}
		else if( mCommandLast == ROTATE )
		{
			pTCommand.second = Vector3( mRotationX, mRotationY, mRotationZ );
			updateVertex = true;
			goto UPDATE;
		} 
		else if( mCommandLast == SCALE )
		{
			pTCommand.second = Vector3( mScaleX, mScaleY, mScaleZ );
			updateVertex = true;
			goto UPDATE;
		}
		else if( mCommandLast == TAPERX && pTCommand.first != TAPERY
			||   mCommandLast == TAPERY && pTCommand.first != TAPERX )
		{
			pTCommand.second = Vector3( mTaperX, mTaperY, 0 ); 
			updateVertex = true;
			goto UPDATE;
		}
		else if( mCommandLast == TOP_SHEARX && pTCommand.first != TOP_SHEARY 
			||   mCommandLast == TOP_SHEARY && pTCommand.first != TOP_SHEARX )
		{
			pTCommand.second = Vector3( mTopShearX, mTopShearY, 0 ); 
			updateVertex = true;
			goto UPDATE;
		}
		else if( mCommandLast == TWIST_BEGIN && pTCommand.first != TWIST_END
			||   mCommandLast == TWIST_END && pTCommand.first != TWIST_BEGIN )
		{
			pTCommand.second = Vector3( mTwistBegin, mTwistEnd, 0 ); 
			updateVertex = true;
			goto UPDATE;
		}
		else if( mCommandLast == PATH_CUT_BEGIN && pTCommand.first != PATH_CUT_END
			||   mCommandLast == PATH_CUT_END && pTCommand.first != PATH_CUT_BEGIN )
		{
			pTCommand.second = Vector3( mPathCutBegin, mPathCutEnd, 0 ); 
			updateVertexIndex = true;
			goto UPDATE;
		}
		else if( mCommandLast == DIMPLE_BEGIN && pTCommand.first != DIMPLE_END
			||   mCommandLast == DIMPLE_END && pTCommand.first != DIMPLE_BEGIN )
		{
			pTCommand.second = Vector3( mDimpleBegin, mDimpleEnd, 0 ); 
			updateVertexIndex = true;
			goto UPDATE;
		}
		else if( mCommandLast == HOLE_SIZEX && pTCommand.first != HOLE_SIZEY 
			||   mCommandLast == HOLE_SIZEX && pTCommand.first != HOLLOW_SHAPE
			||   mCommandLast == HOLE_SIZEY && pTCommand.first != HOLE_SIZEX 
			||   mCommandLast == HOLE_SIZEY && pTCommand.first != HOLLOW_SHAPE
			||   mCommandLast == HOLLOW_SHAPE && pTCommand.first != HOLE_SIZEX
			||   mCommandLast == HOLLOW_SHAPE && pTCommand.first != HOLE_SIZEY )
		{
			pTCommand.second = Vector3( mHoleSizeX, mHoleSizeY, mHollowShape ); 
			updateVertexIndex = true;
			goto UPDATE;
		}
		else if( mCommandLast == SKEW && pTCommand.first != REVOLUTION
			||   mCommandLast == SKEW && pTCommand.first != RADIUS_DELTA
			||   mCommandLast == REVOLUTION && pTCommand.first != SKEW
			||   mCommandLast == REVOLUTION && pTCommand.first != RADIUS_DELTA
			||   mCommandLast == RADIUS_DELTA && pTCommand.first != SKEW
			||   mCommandLast == RADIUS_DELTA && pTCommand.first != REVOLUTION )
		{
			pTCommand.second = Vector3( mSkew, mRadiusDelta, mRevolutions ); 
			updateVertexIndex = true;
			goto UPDATE;
		}
		else
		{
			// the new command is a neightboor of the last one ( TAPERX & TAPERY for example )
			// so do nothing ...
		}
	}


UPDATE:

	if( updateVertex || updateVertexIndex )
	{
		//restoreBuffer( mBufCurrent, mBufBackup );
		restoreBufferVertex( mBufCurrent, mBufBackup );
		if( updateVertexIndex ) 
			restoreBufferIndex( mBufCurrent, mBufBackup );

		resetParameters();

		pOldCommand = mCommandLast;
		Command temp = pTCommand.first;
		pTCommand.first = mCommandLast;
		mCommandList.push_back( pTCommand );
		mCommandLast = temp;

		return true;
	}

	return false;
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
	delete pBufOld->vertex;		//pBufOld->vertex = 0;
	pBufOld->vertex				= new Real [pBufNew->vertexCount*mVertexDecl/4];

	Real *vBackup = pBufOld->vertex;
	Real *vCurrent = pBufNew->vertex;
	for(unsigned int i=0; i<pBufNew->vertexCount*mVertexDecl/4; i++)
		*vBackup++ = *vCurrent++;		//pBufOld->vertex[i]	= pBufNew->vertex[i];

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
	delete pBufOld->index;		//pBufOld->index = 0;
	pBufOld->index				= new unsigned int [pBufNew->indexCount];

	unsigned int *iBackup = pBufOld->index;
	unsigned int *iCurrent = pBufNew->index;
	for(unsigned int i=0; i<pBufNew->indexCount; i++)
		*iBackup++ = *iCurrent++;		//pBufOld->index[i]	= pBufNew->index[i];

	return true;
}

//-------------------------------------------------------------------------------------
bool Object3D::undo()
{
	if( mCommandList.empty() ) return false;

	mCommandList.pop_back();
	if( mCommandList.empty() )
	{
		restoreBuffer( mBufPrim, mBufCurrent );
		updateBoundingBox();
	}

	restoreBuffer( mBufPrim, mBufBackup );
	resizeBuffers( 
		mBufCurrent->vertex, mBufCurrent->vertexCount,
		mBufCurrent->index, mBufCurrent->indexCount );

	list<TCommand>::iterator cmd;
	for( cmd = mCommandList.begin(); cmd != mCommandList.end(); cmd++ )
	{
		Vector3 v = (*cmd).second;
		switch( (*cmd).first )
		{
		case TRANSLATE: 
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
	}

//	resizeBuffers( 
//		mBufCurrent->vertex, mBufCurrent->vertexCount,
//		mBufCurrent->index, mBufCurrent->indexCount );

	return true;
}

}//namespace
