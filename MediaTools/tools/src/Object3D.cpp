#include "Object3D.h"
//#include "Primitives.h"
#include "ModifiedMaterialManager.h"
#include "ModifiedMaterial.h"
#include "VectorModifier.h"
#include "MeshModifier.h"

#include "SolipsisErrorHandler.h"
// Tinyxml
#include "tinyxml.h"

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
	mPoints = new vector<Vector3>;
	mFaces = new vector<Face>;
	mPointsBackup = new vector<Vector3>;
	mVertexs = 0;
	mTriangles = 0;

 	getDataFromBuffer( mPoints, mFaces );

	// backup the array vertexData
	for( vector<Vector3>::iterator i = mPoints->begin(); i != mPoints->end(); i++ )
		mPointsBackup->push_back( (*i) );

	// get the object size max / min
	getSize( mSize, mCornerMin, mCornerMax );
	
	mModified = false;
	mChilds = NULL;		// must be loaded from the .XML
	mParent = 0;				// must be loaded from the .XML

	mName = pName;

	//mType = BOX;
	mPathCutBegin = 0;
	mPathCutEnd = 1;
	mHollow = 0;
	mHollowShape = CIRCLE;
	mTwistBegin = 0;
	mTwistEnd = 0;
	mTaperX = 0;
	mTaperY = 0;
	mTopShearX = 0;
	mTopShearY = 0;
	mSkew = 0;
	mDimpleBegin = 0;
	mDimpleEnd = 1;
	mHoleSizeX = 0;
	mHoleSizeY = 0;
	mProfileCuteBegin = 0;
	mProfileCuteEnd = 1;
	mRadiusDelta = 0;
	mRevolutions = 0;

	mModifiedMaterialManager = new ModifiedMaterialManager() ;
	const MaterialPtr& tmpMaterial = mEntity->getSubEntity(0)->getMaterial()->clone("Material"+mName);
	mEntity->getSubEntity(0)->setMaterialName( tmpMaterial->getName());
	mModifiedMaterialManager->initialise(tmpMaterial);
	TexturePtr PtrTexture = TextureManager::getSingleton().load( "default_texture.tga", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
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
	mLastCommand = NONECOMMAND;

}

//-------------------------------------------------------------------------------------
Object3D::~Object3D()
{
	//delete mEntity;		
	mEntity = 0;
	mNode = 0;
	delete mPoints;		mPoints = 0;
	delete mFaces;		mFaces = 0;
	delete mChilds;		mChilds = 0;
}

//-------------------------------------------------------------------------------------
int		Object3D::loadFromFile(TiXmlDocument &doc)
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
	Real converted;
	stringstream sDebug;
	while (trans!= NULL)
	{
		from_string(trans->Attribute("type"),(int &)toAdd.first);
		from_string(trans->Attribute("value"),toAdd.second);
		mCommands.push_back(toAdd);
		from_string(toAdd.second.c_str(),converted);
		apply(toAdd.first,converted); 
		sDebug << "Applying tranformation : " << toAdd.first << " with value : " << toAdd.second << " to " << mName << endl; 
		SOLIPSISINFO(sDebug.str().c_str());
		trans = trans->NextSiblingElement("transfo");
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
	setCurrentTexture(e->FirstChildElement("texturelist")->FirstChildElement("texture")->Attribute("Name"));

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

	std::list<TCommand>::iterator itCommands = mCommands.begin();
	while (itCommands != mCommands.end())
	{
		if ((*itCommands).first != NONECOMMAND)
			toSave << "\t\t\t<transfo type =\"" << (*itCommands).first << "\" value=\"" << (*itCommands).second << "\" />" << endl;

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
	toSave << "\t\t<texturelist>" << endl;
	toSave << "\t\t\t<texture Name=\"" << getCurrentTexture()->getName() << "\" />"  << endl;
	toSave << "\t\t</texturelist>" << endl;
	toSave << "\t</material>" << endl;

	toSave << "\t<threeD>" << endl;
	Vector3 tmp = getPosition();
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
void Object3D::setSkew(int value)
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

bool Object3D::apply(Command command, Real p1)
{
	return apply(command,p1,0,0);
}

//-------------------------------------------------------------------------------------
bool Object3D::apply(Command command, Real p1, Real p2, Real p3)
{
	Vector3 rootTrans;					// root position for the transformation & deformations
	vector<Vector3>::iterator u,v,w;	// list point iteractor

	switch(command)
	{
	// basic transformations
	case TRANSLATE:
		{
			/*
			for( v = mPoints->begin(); v != mPoints->end(); v++)
				(*v) += Vector3(p1, p2, p3);

			mModified = true;
			update();
			*/

			mNode->translate( p1, p2, p3 );

			return true;
		}
		break;

	case ROTATE:
		{
			// transfomation center / axe
			rootTrans = Vector3::ZERO ; //mNode->getPosition();
			static Real angleX,angleY,angleZ;
			angleX = Math::DegreesToRadians( p1 ); // p1 * 0.01745329252);		// * PI / 180
			angleY = Math::DegreesToRadians( p2 );
			angleZ = Math::DegreesToRadians( p3 );

			// transformation ...
			for( v = mPoints->begin(); v != mPoints->end(); v++)
			{		
				(*v) -= rootTrans;
				if( p1 && !p2 && !p3 ) VectorModifier::rotateX( (*v), angleX );			// rotation on X
				else if( !p1 && p2 && !p3 ) VectorModifier::rotateY( (*v), angleY );	// rotation on Y
				else if( !p1 && !p2 && p3 ) VectorModifier::rotateZ( (*v), angleZ );	// rotation on Z
				else VectorModifier::rotateXYZ( (*v), angleX, angleY, angleZ );			// rotation on X & Y & Z
				(*v) += rootTrans;
			}

			mModified = true;
			update();
			return true;
		}
		break;

	case SCALE:
		{
			// transformation center / axe
			rootTrans = mNode->getPosition();

			// transformation ...
			for( v = mPoints->begin(); v != mPoints->end(); v++)
			{
				(*v) -= rootTrans;
				(*v) *= Vector3(p1, p2, p3);
				(*v) += rootTrans;
			}

			mModified = true;
			update();
			return true;
		}
		break;

	// advanced deformations
	case TAPERX:
		{
			Real dep = 0;
			Vector3 center;
			if( 0 < p1 && p1 <= 1 )
			{
				// on effectu une translation des sommets sur l'axe X 
				// dep = Som.Y * Transl / Size.Y
				// if( Som.X < Center.X ) 
				//		Som.X += dep
				// else 
				//		Som.X -= dep
				center = mCornerMax + mCornerMin; //mNode->getPosition().y;
				w = mPointsBackup->begin();
				for( v = mPoints->begin(); v != mPoints->end(); v++ )
				{
					//dep = (*v).y * p1 * mSize.x / mSize.y;
					//dep = p1 * (mSize.x/2 * (*v).y / mCornerMax.y);
					dep = (*w).x * 
						p1 * 
						( ( (*w).x / ( mSize.x+mCornerMin.x ) ) * 
						(*w).y / ( mSize.y+mCornerMin.y ) );
					//(*v).x += ((*w).x < center ? +dep : -dep);				// with delta incr modif
					(*v).x = (*w).x + ((*w).x < center.z ? +dep : -dep);
					w++; 
				}

				mModified = true;
				setTaperX(p1);
			}

			if( mModified ) update();
			return true;
		}
		break;

	case TAPERY:
		{
			Real dep = 0;
			Real center = 0;

			if( 0 < p1 && p1 <= 1 )
			{
				// on effectu une translation des sommets sur l'axe Z 
				// dep = Som.Y * Transl / Size.Y
				// if( Som.Z < Center.Z ) 
				//		Som.Z += dep
				// else 
				//		Som.Z -= dep
				center = mCornerMax.z + mCornerMin.z; //mNode->getPosition().z;
				w = mPointsBackup->begin();
                for( v = mPoints->begin(); v != mPoints->end(); v++ )
				{
					//dep = (*v).y * p1 * mSize.z / mSize.y;
					//dep = p1 * (mSize.z/2 * (*v).y/mCornerMax.y);
					dep = (*w).z * 
						p1 * 
						( ( (*w).z / ( mSize.z+mCornerMin.z ) ) * 
						(*w).y / ( mSize.y+mCornerMin.y ) );
					//(*v).z += ((*w).z < center ? +dep : -dep);				// with delta incr modif
					(*v).z = (*w).z + ((*w).z < center ? +dep : -dep);
					w++; 
				}

				mModified = true;
				setTaperY(p1);
			}

			if( mModified ) update();
			return true;
		}
		break;

	case PATH_CUT_BEGIN:
		setPathCutBegin( p1 );
		return false;
	case PATH_CUT_END:
		setPathCutEnd( p1 );
		return false;
		{
			//p1 = .10;
			//p2 = .90;

			p1 = mPathCutBegin;
			p2 = mPathCutEnd;

			if( 0 > p1 || p1 > 1) return false;
			if( (p1 == 0) && (p2 == 1) ) return false;
			if( p1 > p2 ) return false;	
			
			rootTrans = (mCornerMax+mCornerMin)/2;

			p1 = 2 * Math::PI * p1;
			p2 = 2 * Math::PI * -(1-p2);

			// list of points & index face in intersection
			list< pair<Vector3, unsigned int> >::iterator vi;
			list< pair<Vector3, unsigned int> > vertexIntersect;
			// list of points for the creation of the intersection object / planes
			list< Vector3 >::iterator vab;
			list< Vector3 > vertexPlaneA, vertexPlaneB;	

			list<int> deleteFace;			// list of the index of the future deleted faces

			Face face;						// Temporary triangle face
			Vector3 interPos;				// Temporary point for the intersections
			Real *vertexTemp, *vStart;		// Temporary vertex array
			unsigned* triangleTemp;			// Temporary triangle array
			
			// cette deformation consiste a couper / faire une operation booleenne entre l'objet
			// et 2 demi-plans !
			// ces demi-plans demarrent du centre de l'objet sur le plan XZ
			// pour un PATH_BEGIN = 0, on demarre le premier demi plan en -X sur le plan XZ
			// pour un PATH_END = 1, on termine le second demi plan en -X sur le plan XZ, c'est a dire
			// que l'on couvre l'objet sur 1 radian ==> on conserve la totalitee de l'objet
			//
			// pour un PATH_BEGIN = 0.25, on demarre le premier demi plan en +Y sur le plan YZ
			// pour un PATH_END = 0.75, on termine le second demi plan en -Y sur le plan YZ, c'est a dire
			// que l'on couvre l'objet sur 0.5 radian ==> on ne garde donc que la moitiee de l'objet se
			// trouvant dans le champ +X
			{}
			// A. build the planes by 2 triangular faces
			//
			//		 4 _____ 2
			//         \  / \
			//          \/   \   
			//        6 /\____\ 1
			//          \ 3  /
			//           \  /
			//            \/ 5
			//            
			static Vector3 pt1, pt2, pt3, pt4, pt5, pt6;
			// W. Update the planes position & orientation
			{
				pt1 = Vector3( rootTrans + mCornerMax * Vector3( 0, 0, 1.02 ) );
				pt2 = Vector3( rootTrans + mCornerMin * Vector3( 0, 0, 1.02 ) );
				pt3 = Vector3( rootTrans + mCornerMax * Vector3( -1.5, 0, 1.02 ) ) - rootTrans;
				pt4 = Vector3( rootTrans + mCornerMin * Vector3( +1.5, 0, 1.02 ) ) - rootTrans;
				pt5 = pt3;
				pt6 = pt4;
				VectorModifier::rotateZ( pt3, p1 );		pt3 += rootTrans;
				VectorModifier::rotateZ( pt4, p1 );		pt4 += rootTrans;
				VectorModifier::rotateZ( pt5, p2 );		pt5 += rootTrans;
				VectorModifier::rotateZ( pt6, p2 );		pt6 += rootTrans;
			}

			// Y. Build the 2 'cut' planes
			Plane planeA( pt1, pt3, pt2 );
			Plane planeB( pt1, pt2, pt5 );

			// A. ray cast for the insterection	tests
			static Ray* line = new Ray();
			line->setOrigin( pt1 );
			line->setDirection( pt2 );

			// B. List of pairs points / faces in intersection with the 1/2 planes axe
			{
				for(unsigned int id = 0; id < mTriangleCount; id++)
				{ 
					getFace( id, face );

					if( MeshModifier::interLineTriangle(
						getPoint(face.id1), getPoint(face.id2), getPoint(face.id3), 
						line->getOrigin(), line->getDirection(), 
						interPos ) )
					{
						VectorModifier::addToList( vertexIntersect, pair<Vector3, unsigned int>( interPos, id ) );
						VectorModifier::addToList( vertexPlaneA, interPos );
						VectorModifier::addToList( vertexPlaneB, interPos );
					}
				}
			}

			// C. List of pairs point / face in intersection between planes and object edges
			// for each triangular face, test the intersection between the three edges and the two planes
			{
				for(unsigned int id = 0; id < mTriangleCount; id++)
				{ 
					getFace( id, face );

					// premier demi-plan (A)
					if( MeshModifier::interLineRectangle(		// premiere arrete
						pt1, pt2, pt3, pt4,
						getPoint(face.id1),
						getPoint(face.id2),
						interPos ) )
					{
						VectorModifier::addToList( vertexIntersect, pair<Vector3, unsigned int>( interPos, id ) );
						VectorModifier::addToList( vertexPlaneA, interPos );
					}
					if( MeshModifier::interLineRectangle(		// deuxieme arrete
						pt1, pt2, pt3, pt4,
						getPoint(face.id2),
						getPoint(face.id3),
						interPos ) )
					{
						VectorModifier::addToList( vertexIntersect, pair<Vector3, unsigned int>( interPos, id ) );
						VectorModifier::addToList( vertexPlaneA, interPos );
					}
					if( MeshModifier::interLineRectangle(		// troisieme arrete
						pt1, pt2, pt3, pt4,
						getPoint(face.id3),
						getPoint(face.id1),
						interPos ) )
					{
						VectorModifier::addToList( vertexIntersect, pair<Vector3, unsigned int>( interPos, id ) );
						VectorModifier::addToList( vertexPlaneA, interPos );
					}

					// second demi-plan (B)
					if( MeshModifier::interLineRectangle(		// premiere arrete
						pt1, pt2, pt5, pt6,
						getPoint(face.id1),
						getPoint(face.id2),
						interPos ) )
					{
						VectorModifier::addToList( vertexIntersect, pair<Vector3, unsigned int>( interPos, id ) );
						VectorModifier::addToList( vertexPlaneB, interPos );
					}
					if( MeshModifier::interLineRectangle(		// deuxieme arrete
						pt1, pt2, pt5, pt6,
						getPoint(face.id2),
						getPoint(face.id3),
						interPos ) )
					{
						VectorModifier::addToList( vertexIntersect, pair<Vector3, unsigned int>( interPos, id ) );
						VectorModifier::addToList( vertexPlaneB, interPos );
					}
					if( MeshModifier::interLineRectangle(		// troisieme arrete
						pt1, pt2, pt5, pt6,
						getPoint(face.id3),
						getPoint(face.id1),
						interPos ) )
					{
						VectorModifier::addToList( vertexIntersect, pair<Vector3, unsigned int>( interPos, id ) );
						VectorModifier::addToList( vertexPlaneB, interPos );
					}
				}
			}

			// D. Delete the object faces in intersection with one plane
			{
				for( vi = vertexIntersect.begin(); vi != vertexIntersect.end(); vi++ )
				{	
					// TODO : attention lors de la suppression d'une face a ne pas forcement supprimer les
					//        3 sommets ; l'un d'eux peut etre utiliser par une autre face !!!
					VectorModifier::addToList( deleteFace, (*vi).second );
				}
			}

			// E. Update the mVertex array
			{
				HardwareVertexBufferSharedPtr vbuf = mVertexData->vertexBufferBinding->getBuffer(0 /*posElem->getSource()*/);
				Real *pVert = static_cast<Real*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));

				vertexTemp = new Real[ 
					(mVertexCount+vertexIntersect.size()+deleteFace.size()*3+vertexPlaneA.size()+vertexPlaneB.size()) * mVertexDecl/4 ];
				vStart = vertexTemp;
				// TODO : the old vertexs stay in the mVertexs array !!! -> they should be removed -> modif their indexes
				for( size_t i=0; i<(mVertexCount*mVertexDecl/4); i++ )
					*vertexTemp++ = *pVert++;

				vbuf->unlock();
			}

			// F. Update the mTriangles array
			{
				triangleTemp = new unsigned[ 
					(	mTriangleCount
						- deleteFace.size()
						+ vertexIntersect.size()
						+ vertexPlaneA.size() -2
						+ vertexPlaneB.size() -2 
					) * 3 ];
				
				size_t j = 0;
				for( size_t i=0; i<(mTriangleCount-deleteFace.size()); i++ )
				{
					for( list<int>::iterator idf=deleteFace.begin(); idf!=deleteFace.end(); idf++ )
					{
						while( j == (*idf) ) 
						{ j++; idf++; }
					}
					triangleTemp[i*3]	= mTriangles[j*3];
					triangleTemp[i*3+1]	= mTriangles[j*3+1];
					triangleTemp[i*3+2]	= mTriangles[j*3+2];
					j++;
				}
			
				mTriangleCount -= deleteFace.size();
			}

			// G. build the new cut faces between the object and the planes
			// G.1. pop the first list element (point)
			// G.2. pop the nearest neighbor from the first element
			// G.3. pop the nearest neighbor from the second element
			// G.4. build a face with those three points
			// G.5. the second point get position from the third one
			// G.6. idem G.3. ...
if( 1 )
			{
				size_t id1, id2, id3 = 0;
				Vector3 normal, second, third, first;
				bool newFaces = false;

				if( !vertexPlaneA.empty() )
				{	
					id1 = mVertexCount++;
					id2 = mVertexCount++;
					id3 = mVertexCount++;

					first = (*vertexPlaneA.begin());										// G.1.
					vertexPlaneA.remove( first );
					second = VectorModifier::getNearestPoint( vertexPlaneA, first );		// G.2.
					vertexPlaneA.remove( second );
					normal = planeA.normal;

					// first vertex															// G.4.		...
					*vertexTemp++ = first.x;	*vertexTemp++ = first.y;	*vertexTemp++ = first.z;	// position
					*vertexTemp++ = normal.x;	*vertexTemp++ = normal.y;	*vertexTemp++ = normal.z;	// normal
					*vertexTemp++ = 1;//ColourValue( 1, .8, .8, .5 );									// colour
					*vertexTemp++ = 0;			*vertexTemp++ = 0;										// tex.coord

					// second vertex
					*vertexTemp++ = second.x;	*vertexTemp++ = second.y;	*vertexTemp++ = second.z;	// position
					*vertexTemp++ = normal.x;	*vertexTemp++ = normal.y;	*vertexTemp++ = normal.z;	// normal
					*vertexTemp++ = 1;//ColourValue( 1, .8, .8, .5 );									// colour
					*vertexTemp++ = 0;			*vertexTemp++ = 0;										// tex.coord

					while( vertexPlaneA.size() )
					{
						third = VectorModifier::getNearestPoint( vertexPlaneA, second );	// G.3.		G.6.
						vertexPlaneA.remove( third );

						// third vertex
						*vertexTemp++ = third.x;	*vertexTemp++ = third.y;	*vertexTemp++ = third.z;	// position
						*vertexTemp++ = normal.x;	*vertexTemp++ = normal.y;	*vertexTemp++ = normal.z;	// normal
						*vertexTemp++ = 1;//ColourValue( 1, .8, .8, .5 );									// colour
						*vertexTemp++ = 0;			*vertexTemp++ = 0;										// tex.coord

						// face ( first, second, third )
						triangleTemp[ mTriangleCount*3 ]	= id1;
						triangleTemp[ mTriangleCount*3+1 ]	= id2;
						triangleTemp[ mTriangleCount*3+2 ]	= id3;
						mTriangleCount++;

						second = third;														// G.5.
						id2 = id3;
						id3 = mVertexCount++;
						newFaces = true;
					}
				}

				if( !vertexPlaneB.empty() )
				{
					if( id3 ) id1 = id3;
					else id1 = mVertexCount++;
					id2 = mVertexCount++;
					id3 = mVertexCount++;

					first = (*vertexPlaneB.begin());										// G.1.
					vertexPlaneB.remove( first );
					second = VectorModifier::getNearestPoint( vertexPlaneB, first );		// G.2.
					vertexPlaneB.remove( second );
					normal = planeB.normal;

					// first vertex															// G.4.		...
					*vertexTemp++ = first.x;	*vertexTemp++ = first.y;	*vertexTemp++ = first.z;	// position
					*vertexTemp++ = normal.x;	*vertexTemp++ = normal.y;	*vertexTemp++ = normal.z;	// normal
					*vertexTemp++ = 1;//ColourValue( 1, .8, .8, .5 );									// colour
					*vertexTemp++ = 0;			*vertexTemp++ = 0;										// tex.coord

					// second vertex
					*vertexTemp++ = second.x;	*vertexTemp++ = second.y;	*vertexTemp++ = second.z;	// position
					*vertexTemp++ = normal.x;	*vertexTemp++ = normal.y;	*vertexTemp++ = normal.z;	// normal
					*vertexTemp++ = 1;//ColourValue( 1, .8, .8, .5 );									// colour
					*vertexTemp++ = 0;			*vertexTemp++ = 0;										// tex.coord

					while( vertexPlaneB.size() )
					{
						third = VectorModifier::getNearestPoint( vertexPlaneB, second );	// G.3.		G.6.
						vertexPlaneB.remove( third );

						// third vertex
						*vertexTemp++ = third.x;	*vertexTemp++ = third.y;	*vertexTemp++ = third.z;	// position
						*vertexTemp++ = normal.x;	*vertexTemp++ = normal.y;	*vertexTemp++ = normal.z;	// normal
						*vertexTemp++ = 1;//ColourValue( 1, .8, .8, .5 );									// colour
						*vertexTemp++ = 0;			*vertexTemp++ = 0;										// tex.coord

						// face ( first, second, third )
						triangleTemp[ mTriangleCount*3 ]	= id1;
						triangleTemp[ mTriangleCount*3+2 ]	= id2;		// TODO : normales -> lignes inversees ??
						triangleTemp[ mTriangleCount*3+1 ]	= id3;		// TODO : normales -> lignes inversees ??
						mTriangleCount++;

						second = third;
						id2 = id3;
						id3 = mVertexCount++;
						newFaces = true;
					}
				}

				if( newFaces ) mVertexCount--;
			}

			// H. rebuild the new (deleted) faces of the object
if( 1 )
			{
				if( vertexIntersect.size() > 1 )
				{
					list<Vector3> list;
					Plane planeDef, plane;
					Ogre::Plane::Side side;
					size_t index, id1, id2, id3 = 0;
					Vector3 vv, vvv, normal, second, third, first;
					bool reverseNormal = false;
					bool newFaces = false;
					
					vertexIntersect.sort();
					vi = vertexIntersect.begin();
					while( vi != vertexIntersect.end() )
					{
						// get the 3 points from the object face (index face)
						index = (*vi).second;
						getFace( index, face );

						// take the list of points that has the same index face
						while( vi != vertexIntersect.end() && (*vi).second == index )
						{
							list.push_back( (*vi).first );
							vi++;
						}

						// only one point is in intersection so only one face will be created
						if( list.size() == 1 )
						{
							// the 3 points from the original face
							u = mPoints->begin() + face.id1;
							vertexPlaneA.push_back( (*u) );
							v = mPoints->begin() + face.id2;
							vertexPlaneA.push_back( (*v) );
							w = mPoints->begin() + face.id3;
							vertexPlaneA.push_back( (*w) );
						}

						// more than one point from this index face is in intersection
						// so more than one face will be created
						else
						{
							// first
							u = mPoints->begin() + face.id1;
							side = planeA.getSide( (*u) );
							if( side == Ogre::Plane::NO_SIDE || side == Ogre::Plane::NEGATIVE_SIDE )
								vertexPlaneA.push_back( (*u) );
							side = planeB.getSide( (*u) );
							if( side == Ogre::Plane::NO_SIDE || side == Ogre::Plane::NEGATIVE_SIDE )
								vertexPlaneB.push_back( (*u) );

							// second
							v = mPoints->begin() + face.id2;
							side = planeA.getSide( (*v) );
							if( side == Ogre::Plane::NO_SIDE || side == Ogre::Plane::NEGATIVE_SIDE )
								vertexPlaneA.push_back( (*v) );
							side = planeB.getSide( (*v) );
							if( side == Ogre::Plane::NO_SIDE || side == Ogre::Plane::NEGATIVE_SIDE )
								vertexPlaneB.push_back( (*v) );

							// third
							w = mPoints->begin() + face.id3;
							side = planeA.getSide( (*w) );
							if( side == Ogre::Plane::NO_SIDE || side == Ogre::Plane::NEGATIVE_SIDE )
								vertexPlaneA.push_back( (*w) );
							side = planeB.getSide( (*w) );
							if( side == Ogre::Plane::NO_SIDE || side == Ogre::Plane::NEGATIVE_SIDE )
								vertexPlaneB.push_back( (*w) );

							// get the points from the intersection list
							for( vab = list.begin(); vab != list.end(); vab++ )
							{
								side = planeA.getSide( (*vab)-planeA.normal );
								if( side == Ogre::Plane::NO_SIDE || side == Ogre::Plane::NEGATIVE_SIDE )
									//vertexPlaneA.push_front( (*vab) );
									vertexPlaneA.push_back( (*vab) );
								side = planeB.getSide( (*vab)-planeB.normal );
								if( side == Ogre::Plane::NO_SIDE || side == Ogre::Plane::NEGATIVE_SIDE )
									//vertexPlaneB.push_front( (*vab) );
									vertexPlaneB.push_back( (*vab) );
							}
						}

						// compute the normal from the face (u, v, w)
						planeDef.redefine( (*u), (*v), (*w) );
						
						// clear the temporary list of point
						list.clear();

						// build the new faces
						if( vertexPlaneA.size() > 2 )
						{	
							id1 = id3 ? id3 : mVertexCount++;
							id2 = mVertexCount++;
							id3 = mVertexCount++;

							first = (*vertexPlaneA.begin());
							vertexPlaneA.remove( first );
							second = VectorModifier::getNearestPoint( vertexPlaneA, first );
							vertexPlaneA.remove( second );
							plane.redefine( first, second, VectorModifier::getNearestPoint( vertexPlaneA, second ) );
							normal = planeDef.normal;

							// test if the normal is in the wrong way
							//if( plane.normal.dotProduct( Vector3::UNIT_SCALE ) * normal.dotProduct( Vector3::UNIT_SCALE ) < 0 ) 
							if( plane.normal.dotProduct( normal ) < 0 ) 
								reverseNormal = true;

							// first vertex
							*vertexTemp++ = first.x;	*vertexTemp++ = first.y;	*vertexTemp++ = first.z;	// position
							*vertexTemp++ = normal.x;	*vertexTemp++ = normal.y;	*vertexTemp++ = normal.z;	// normal
							*vertexTemp++ = 1;//ColourValue( 1, .8, .8, .5 );									// colour
							*vertexTemp++ = 0;			*vertexTemp++ = 0;										// tex.coord

							// second vertex
							*vertexTemp++ = second.x;	*vertexTemp++ = second.y;	*vertexTemp++ = second.z;	// position
							*vertexTemp++ = normal.x;	*vertexTemp++ = normal.y;	*vertexTemp++ = normal.z;	// normal
							*vertexTemp++ = 1;//ColourValue( 1, .8, .8, .5 );									// colour
							*vertexTemp++ = 0;			*vertexTemp++ = 0;										// tex.coord

							while( !vertexPlaneA.empty() )
							{
								third = VectorModifier::getNearestPoint( vertexPlaneA, second );
								vertexPlaneA.remove( third );

								// third vertex
								*vertexTemp++ = third.x;	*vertexTemp++ = third.y;	*vertexTemp++ = third.z;	// position
								*vertexTemp++ = normal.x;	*vertexTemp++ = normal.y;	*vertexTemp++ = normal.z;	// normal
								*vertexTemp++ = 1;//ColourValue( 1, .8, .8, .5 );									// colour
								*vertexTemp++ = 0;			*vertexTemp++ = 0;										// tex.coord

								// face ( first, second, third )
								triangleTemp[ mTriangleCount*3 ]	= id1;
								triangleTemp[ mTriangleCount*3+1 ]	= reverseNormal ? id3 : id2;
								triangleTemp[ mTriangleCount*3+2 ]	= reverseNormal ? id2 : id3;
								mTriangleCount++;

								second = third;
								id2 = id3;
								id3 = mVertexCount++;
								newFaces = true;
							}
							reverseNormal = false;
						}

						if( vertexPlaneB.size() > 2 )
						{	
							id1 = id3 ? id3 : mVertexCount++;
							id2 = mVertexCount++;
							id3 = mVertexCount++;

							first = (*vertexPlaneB.begin());
							vertexPlaneB.remove( first );
							second = VectorModifier::getNearestPoint( vertexPlaneB, first );
							vertexPlaneB.remove( second );
							plane.redefine( first, second, VectorModifier::getNearestPoint( vertexPlaneB, second ) );
							normal = planeDef.normal;

							// test if the normal is in the wrong way
							//if( plane.normal.dotProduct( Vector3::UNIT_SCALE ) * normal.dotProduct( Vector3::UNIT_SCALE ) < 0 ) 
							if( plane.normal.dotProduct( normal ) < 0 ) 
								reverseNormal = true;

							// first vertex
							*vertexTemp++ = first.x;	*vertexTemp++ = first.y;	*vertexTemp++ = first.z;	// position
							*vertexTemp++ = normal.x;	*vertexTemp++ = normal.y;	*vertexTemp++ = normal.z;	// normal
							*vertexTemp++ = 1;//ColourValue( 1, .8, .8, .5 );									// colour
							*vertexTemp++ = 0;			*vertexTemp++ = 0;										// tex.coord

							// second vertex
							*vertexTemp++ = second.x;	*vertexTemp++ = second.y;	*vertexTemp++ = second.z;	// position
							*vertexTemp++ = normal.x;	*vertexTemp++ = normal.y;	*vertexTemp++ = normal.z;	// normal
							*vertexTemp++ = 1;//ColourValue( 1, .8, .8, .5 );									// colour
							*vertexTemp++ = 0;			*vertexTemp++ = 0;										// tex.coord

							while( !vertexPlaneB.empty() )
							{
								third = VectorModifier::getNearestPoint( vertexPlaneB, second );
								vertexPlaneB.remove( third );

								// third vertex
								*vertexTemp++ = third.x;	*vertexTemp++ = third.y;	*vertexTemp++ = third.z;	// position
								*vertexTemp++ = normal.x;	*vertexTemp++ = normal.y;	*vertexTemp++ = normal.z;	// normal
								*vertexTemp++ = 1;//ColourValue( 1, .8, .8, .5 );									// colour
								*vertexTemp++ = 0;			*vertexTemp++ = 0;										// tex.coord
								
								// face ( first, second, third )
								triangleTemp[ mTriangleCount*3 ]	= id1;
								triangleTemp[ mTriangleCount*3+1 ]	= reverseNormal ? id3 : id2;
								triangleTemp[ mTriangleCount*3+2 ]	= reverseNormal ? id2 : id3;
								mTriangleCount++;

								second = third;
								id2 = id3;
								id3 = mVertexCount++;
								newFaces = true;
								
							}
							reverseNormal = false;
						}
					}
					if( newFaces ) mVertexCount--;
				}
			}

			// I. Update the mVertex, mTriangles arrays and resize the hardware buffer
			free( mVertexs ); //delete[] mVertexs;
			free( mTriangles ); //delete[] mTriangles;
			vertexTemp = vStart;
			resizeBuffers( mVertexCount, vertexTemp, mTriangleCount*3, triangleTemp );
//			free( triangleTemp );
//			free( vertexTemp );
			{}


/*
SOLIPSISINFO( "-----------------------------------------------" );
static char text[64];
size_t id = 0;
for( size_t i=0; i<mVertexCount*mVertexDecl/4; i+=(mVertexDecl/4) )
{
	sprintf( text, "%2u -> %f %f %f", id++, vStart[i],vStart[i+1],vStart[i+2] );
	SOLIPSISINFO( text );
}
SOLIPSISINFO( "   " );
id = 0;
for( size_t i=0; i<mTriangleCount; i++ )
{
	sprintf( text, "%2u -> %i %i %i", id++, triangleTemp[i*3],triangleTemp[i*3+1],triangleTemp[i*3+2] );
	SOLIPSISINFO( text );
}
*/


			mModified = true;
			if(mModified) 
			{
				getDataFromBuffer( mPoints, mFaces );
		//		update();
			}


/*
SOLIPSISINFO( "-----------------------------------------------" );
id = 0;
for( v=mPoints->begin(); v!=mPoints->end(); v++ )
{
	sprintf( text, "%2u -> %f %f %f", id++, (*v).x,(*v).y,(*v).z );
	SOLIPSISINFO( text );
}
SOLIPSISINFO( "   " );
id = 0;
for( vector<Face>::iterator v=mFaces->begin(); v!=mFaces->end(); v++ )
{
	sprintf( text, "%2u -> %i %i %i", id++, (*v).id1,(*v).id2,(*v).id3 );
	SOLIPSISINFO( text );
}
*/


			return true;
		}
		break;

	case DIMPLE_BEGIN:
		{
			// idem PATH_CUT mais en travaillant avec le plan ZY et non plus XZ
			// ...
		}
		break;
	case DIMPLE_END:
		{
			// idem PATH_CUT mais en travaillant avec le plan ZY et non plus XZ
			// ...
		}
		break;

	case HOLE_SIZEX:
		break;
	case HOLE_SIZEY:
		break;

	case HOLLOW_SHAPE:
		break;

	case TWIST_BEGIN:
		{
			if( 0 >= p1 && p1 >= 1 || 0 >= mTwistEnd && mTwistEnd >= 1 ) return false;

			Vector3 center = mNode->getPosition();
			Real ratio, angle;

			p1 *= Math::PI * 2; // 6.2831853;
			mTwistEnd *= Math::PI * 2;
			
			w = mPointsBackup->begin();
			for( v = mPoints->begin(); v != mPoints->end(); v++ )
			{
				(*v) -= center;
				ratio = (*w).y / mCornerMax.y / 2;		// 1 = top, 0 = bottom
				angle = ratio * (p1 - mTwistEnd) + mTwistEnd;			// ratio = 1 -> angle = p1		ratio = 0 -> angle = mTwistEnd
				(*v) = (*w);
				VectorModifier::rotateY( (*v), angle );
				(*v) += center;
				w++;
			}

			mModified = true;
			if( mModified ) update();

			return true;
		}
		break;
	case TWIST_END:
		{
			if( 0 >= mTwistBegin && mTwistBegin >= 1 || 0 >= p2 && p2 >= 1 ) return false;

			Vector3 center = mNode->getPosition();
			Real ratio, angle;

			mTwistBegin *= Math::PI * 2; // 6.2831853;
			p2 *= Math::PI * 2;
			
			w = mPointsBackup->begin();
			for( v = mPoints->begin(); v != mPoints->end(); v++ )
			{
				(*v) -= center;
				ratio = (*w).y / mCornerMax.y / 2;		// 1 = top, 0 = bottom
				angle = ratio * (mTwistBegin - p2) + p2;			// ratio = 1 -> angle = mTwistBegin		ratio = 0 -> angle = p2
				(*v) = (*w);
				VectorModifier::rotateY( (*v), angle );
				(*v) += center;
				w++;
			}

			mModified = true;
			if( mModified ) update();

		}
		break;

	case TOP_SHEARX:
		{
			if( 0 >= p1 && p1 >= 1 || 0 >= mTopShearY && mTopShearY >= 1 ) return false;

			if( p1 )
			{
				// on effectu une translation des sommets sur l'axe X 
				// Som.X += Som.Y * Transl / Size.Y
				w = mPointsBackup->begin();
                for( v = mPoints->begin(); v != mPoints->end(); v++ )
				{
					//(*v).x += (*v).y * p1 * mSize.x / mSize.y;		// other version
					//(*v).x += (*v).y * p1 * mSize.x / mCornerMax.y;				// with delta incr modif
					(*v).x = (*w).x + (*w).y * p1 * mSize.x / mCornerMax.y;
					w++;
				}

				mModified = true;
			}

			if( mTopShearY )
			{
				// on effectu une translation des sommets sur l'axe Z 
				// Som.Z += Som.Y * Transl / Size.Y
				w = mPointsBackup->begin();
                for( v = mPoints->begin(); v != mPoints->end(); v++ )
				{
					//(*v).z += (*v).y * mTopShearY * mSize.z / mCornerMax.y;				// with delta incr modif
					(*v).z = (*w).z + (*w).y * mTopShearY * mSize.z / mCornerMax.y;
					w++;
				}

				mModified = true;
			}

			if( mModified ) update();

			return true;
		}		
		break;
	case TOP_SHEARY:
		{
			if( 0 >= mTopShearX && mTopShearX >= 1 || 0 >= p2 && p2 >= 1 ) return false;

			if( mTopShearX )
			{
				// on effectu une translation des sommets sur l'axe X 
				// Som.X += Som.Y * Transl / Size.Y
				w = mPointsBackup->begin();
                for( v = mPoints->begin(); v != mPoints->end(); v++ )
				{
					//(*v).x += (*v).y * mTopShearX * mSize.x / mSize.y;		// other version
					//(*v).x += (*v).y * mTopShearX * mSize.x / mCornerMax.y;				// with delta incr modif
					(*v).x = (*w).x + (*w).y * mTopShearX * mSize.x / mCornerMax.y;
					w++;
				}

				mModified = true;
			}

			if( p2 )
			{
				// on effectu une translation des sommets sur l'axe Z 
				// Som.Z += Som.Y * Transl / Size.Y
				w = mPointsBackup->begin();
                for( v = mPoints->begin(); v != mPoints->end(); v++ )
				{
					//(*v).z += (*v).y * p2 * mSize.z / mCornerMax.y;				// with delta incr modif
					(*v).z = (*w).z + (*w).y * p2 * mSize.z / mCornerMax.y;
					w++;
				}

				mModified = true;
			}

			if( mModified ) update();

			return true;
		}		
		break;

	case SKEW:
		break;

	case REVOLUTION:
		break;

	case RADIUS_DELTA:
		break;

	default:
		break;
	}

	return true;
}

//-------------------------------------------------------------------------------------
void Object3D::updateBackup()
{
	vector< Vector3 >::iterator v = mPoints->begin();

	mPointsBackup->clear();

	for( size_t size = 0; size < mPoints->size(); size++ )
	{
		mPointsBackup->push_back( (*v) );
		v++;
	}
}

//-------------------------------------------------------------------------------------
Vector3 Object3D::getPosition()
{
	return mNode->getPosition();
}
//-------------------------------------------------------------------------------------
bool Object3D::linkObject(Object3D* pObj, SceneManager* pSceneMgr)
{
	SceneNode * pObjScenNode = pObj->getEntity()->getParentSceneNode();
	Vector3 Wpostion = pObjScenNode->getWorldPosition() ;;
	showBoundingBox(false);
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
	{			//add it ...
		addChild( pObj);			//... to mChilds
									//... to this current node	 
		pSceneMgr->getRootSceneNode()->removeChild( pObjScenNode ) ;
		mNode->addChild( pObjScenNode );
		pObjScenNode->setPosition( Wpostion - mNode->getWorldPosition() );
		pObj->setParent( this) ;	//update pObj's parent
	}
	showBoundingBox(true);
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
Vector3 Object3D::getScale()
{
	return mNode->getScale();
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
	if( !mPoints->empty() )
	{
		max = Vector3::ZERO;
		min = Vector3::ZERO;
		vector<Vector3>::iterator i;
		for( i = mPoints->begin(); i != mPoints->end(); i++ )
		{
			if( (*i).x > max.x )  max.x = (*i).x;
			if( (*i).y > max.y )  max.y = (*i).y;
			if( (*i).z > max.z )  max.z = (*i).z;

			if( (*i).x < min.x )  min.x = (*i).x;
			if( (*i).y < min.y )  min.y = (*i).y;
			if( (*i).z < min.z )  min.z = (*i).z;
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
//	if( mVertexs ) free( mVertexs );
	if( !pVertex->empty() ) pVertex->clear();
	
	// Clear the triangle array
//	if( mTriangles ) free( mTriangles );
	if( !pTriangle->empty() ) pTriangle->clear();

	// Get the informations
	MeshModifier::getMeshInformation(
		mesh,
		mVertexCount,
		mVertexs,
		mTriangleIndexCount,
		mTriangles);

	mTriangleCount = mTriangleIndexCount / 3;

	// Update the vertex array
	for( size_t i=0; i<mVertexCount; i++ )
		pVertex->push_back( mVertexs[i] );

	// Update the triangle array
	for( size_t i=0; i<mTriangleIndexCount; i+=3 )
	{
		Face face;
		face.id1 = mTriangles[i];
		face.id2 = mTriangles[i+1];
		face.id3 = mTriangles[i+2];

		pTriangle->push_back( face );
	}
}

//-------------------------------------------------------------------------------------
void Object3D::setPoint(unsigned int index, const Vector3 &value)
{
	//assert(index < mPoints->size() && "Point index is out of bounds!!");
	//(*mPoints)[index] = value;

	assert(index < mVertexCount && "Point index is out of bounds!!");
	mVertexs[index] = value;


	/*
	int idFace = (*p).second;
				getFace( idFace, face );
				
				v = mPoints->begin() + face.id1;
				(*v) = mVertexs[ face.id1 ] * .9;
	*/

	mModified = true;
} 

//-------------------------------------------------------------------------------------
Vector3 inline Object3D::getPoint(unsigned int index)
{
	//assert(index < mPoints->size() && "Point index is out of bounds!!");
	assert(index < mVertexCount && "Point index is out of bounds!!");

	return (*mPoints)[index];
}

//-------------------------------------------------------------------------------------
size_t Object3D::getNumPoints(void)
{
	//return (unsigned short)mPoints->size();
	return mVertexCount;
}

//-------------------------------------------------------------------------------------
void inline Object3D::getFace(unsigned int index, Object3D::Face &face)
{
	face.id1 = mTriangles[index*3];
	face.id2 = mTriangles[index*3+1];
	face.id3 = mTriangles[index*3+2];
}

//-------------------------------------------------------------------------------------
size_t Object3D::getNumFaces(void)
{
	//return (unsigned short)mFaces->size();
	return mTriangleCount;
}

//-------------------------------------------------------------------------------------
void inline Object3D::update()
{
	if( mEntity )
		if( mModified ) 
		{
			// update the hardware vertex buffer
			updateVertexBuffer();

			// update the bounding box corners
			getSize( mSize, mCornerMin, mCornerMax );
			mEntity->getMesh()->_setBounds( AxisAlignedBox( 
				mCornerMin.x, mCornerMin.y, mCornerMin.z,
				mCornerMax.x, mCornerMax.y, mCornerMax.z ) );
			mEntity->getMesh()->_setBoundingSphereRadius( Math::Sqrt( 3 * mCornerMax.x * mCornerMax.x ) );
		}
}

//-------------------------------------------------------------------------------------
void Object3D::updateVertexBuffer()
{
	MeshPtr mesh = mEntity->getMesh();
	SubMesh* subMesh = mesh->getSubMesh(0);

//	vertexData = mEntity->getVertexDataForBinding();
	const VertexElement* posElem = mVertexData->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

	HardwareVertexBufferSharedPtr vbuf = mVertexData->vertexBufferBinding->getBuffer(posElem->getSource());

	// Get base pointer
	Real *pVert = static_cast<Real*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));
	vector<Vector3>::iterator i = mPoints->begin();

	for( size_t v = 0; v < mVertexCount; v++)
	{
		*pVert++ = (*i).x;
		*pVert++ = (*i).y;
		*pVert++ = (*i).z; 

		pVert += mVertexDecl/4 - 3;
		i++;
	}
	
	vbuf->unlock();

	mModified = false;
}

//-------------------------------------------------------------------------------------
void Object3D::resizeBuffers( size_t vertexCount, Real* vertexData, size_t indexCount, unsigned* indexData )
{
	SubMesh* subMesh = mEntity->getMesh()->getSubMesh(0);

	// Update vertex count in the render operation
	mVertexData->vertexCount = vertexCount;

	// Create new vertex buffer
	HardwareVertexBufferSharedPtr vbuf =
		HardwareBufferManager::getSingleton().createVertexBuffer(
			mVertexDecl, //offset
			mVertexData->vertexCount, //mVertexBufferCapacity,
			//HardwareBuffer::HBU_STATIC_WRITE_ONLY);
			HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY); // TODO: Custom HBU_?

	// Upload the vertex data to the card
	vbuf->writeData( 0, vbuf->getSizeInBytes(), vertexData, true );

	// Bind buffer
	mVertexData->vertexBufferBinding->setBinding(0, vbuf);

	



	// Set parameters of the submesh
	subMesh->indexData->indexCount = indexCount;
	subMesh->indexData->indexStart = 0;

	// Create new index buffer
	HardwareIndexBufferSharedPtr ibuf = 
		HardwareBufferManager::getSingleton().createIndexBuffer(
			HardwareIndexBuffer::IT_32BIT,	//HardwareIndexBuffer::IT_16BIT
			subMesh->indexData->indexCount, //mIndexBufferCapacity,
			//HardwareBuffer::HBU_STATIC_WRITE_ONLY);
			HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY); // TODO: Custom HBU_?

	// Upload the index data to the card
	ibuf->writeData( 0, ibuf->getSizeInBytes(), indexData, true );

	// Update index count in the render operation
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
float Object3D::getShininess ()
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

	if (mChilds)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChilds->begin(); itr != mChilds->end(); itr++ )
		{
			(*itr)->scale(pValueX, pValueY, pValueZ);
		}
	}
}
//-------------------------------------------------------------------------------------
void Object3D::rotate (float pValueX, float pValueY, float pValueZ, Vector3 pCentreSelection,
					   SceneNode * pCentreRotation, SceneNode* pCentreObject)
{
	findRotationPosition( pValueX, pValueY, pValueZ, pCentreSelection, pCentreRotation, pCentreObject);
	apply( Object3D::ROTATE, pValueX, pValueY, pValueZ );

	if (mChilds)
	{
		vector< Object3D* >::iterator itr ;
		for( itr = mChilds->begin(); itr != mChilds->end(); itr++ )
		{
			(*itr)->rotate(pValueX, pValueY, pValueZ, pCentreSelection, pCentreRotation, pCentreObject);
		}
	}
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
//--------------------------------------------------------------------------------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------------


