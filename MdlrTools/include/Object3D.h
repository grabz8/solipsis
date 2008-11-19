/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author ARTEFACTO - Gilles GAUDIN & Stephane CHAPLAIN & Patrice DESFONDS

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

/*
		COPYRIGHTS (c) ARTEFACTO 
		
			!! Licence GPL !!	
*/

#ifndef __Object3D_h__
#define __Object3D_h__

#include <Ogre.h>
#include "ModifiedMaterialManager.h"

#include <XmlDatas.h>

using namespace Ogre;
using namespace std;

class TiXmlDocument;

namespace Solipsis {

#define PRIMCOUNT 14
const string SOLTYPESTRING[] = { "PLANE", "BOX", "CORNER", "PYRAMID","PRISM","CYLINDER","HALF_CYLINDER","CONE","HALF_CONE","SPHERE","HALF_SPHERE","TORUS","TUBE","RING","OTHER"};
typedef std::vector<unsigned int> uintvector;
typedef std::vector<Real> realvector;

//-------------------------------------------------------------------------------------
/// brief ...
/// file Object3D.h
/// author Gilles GAUDIN & Stephane CHAPLAIN & Patrice DESFONDS
/// date 2007.mm.dd
class Object3D
{
public:
	/// brief The 13 primitives that could be created by default
	enum Type { 
		PLANE,
		BOX, CORNER, PYRAMID,
		PRISM, 
		CYLINDER, HALF_CYLINDER, CONE, HALF_CONE,
		SPHERE, HALF_SPHERE,
		TORUS, 
		TUBE, 
		RING,
		OTHER };

	/// brief The 10 possible actions that could be applied on an object
	enum Command { 
		NONE,
		// the 3 basic transformations
		TRANSLATE, ROTATE, 
		SCALE, SCALEX, SCALEY, SCALEZ, 
		// the other 10 mesh transformation
		TAPERX, TAPERY, 
		TOP_SHEARX,TOP_SHEARY, 
		TWIST_BEGIN,TWIST_END, 
		PATH_CUT_BEGIN, PATH_CUT_END, 
		DIMPLE_BEGIN, DIMPLE_END, 
		HOLE_SIZEX, HOLE_SIZEY, HOLLOW_SHAPE, 
		SKEW, REVOLUTION, RADIUS_DELTA }; 

	/// brief The type of hollow shape that could be used
	enum Shape { CIRCLE, SQUARE, TRIANGLE };

	/// brief The structure for a triangle face
	typedef struct Face { unsigned int id1; unsigned int id2; unsigned int id3; };
	typedef struct FaceVec { Vector3 pt1; Vector3 pt2; Vector3 pt3; };

	/// brief The structure for the backup : vertex & index buffers
	typedef struct Buffer {
		realvector vertex;
		unsigned int vertexCount;
		uintvector index;
		unsigned int indexCount;
		Vector3 size;
		Vector3 cornerMax;
		Vector3 cornerMin;
	};

	/// brief The commands history list 
	typedef std::pair<Command,Vector3> TCommand;

public:
	/// brief Constructor
	Object3D(const EntityUID& entityUID, const String& name, SceneNode* node);
	/// brief Destructor
	~Object3D();


	/// \brief Load a object3D from a file 
	/// \param fileName = The path to the file to load
	int		loadFromFile(TiXmlDocument &doc, string texturepath);
	/// Save a object3D to a file 
	/// \param fileName = The path to the file to save
	int		saveToFile(const char* fileName);


	/// brief ...
	void setEntityUID(const EntityUID& entityUID);
	/// brief
	const EntityUID& getEntityUID();
	/// brief ...
	void setName(String name);
	/// brief
	String getName();
	/// brief Reset all transformation & deformation parameters
	void resetParameters();

	/// brief set/get the translation parameter values
	void setTranslate(Real pX, Real pY, Real pZ );
	Vector3 getTranslate() {return Vector3 (mTranslateX, mTranslateY, mTranslateZ) ;};
	/// brief set/get the rotation parameter values
	void setRotate(Real pX, Real pY, Real pZ );
	Vector3 getRotate();
	/// brief set/get the scale parameter values
	void setScale(Real pX, Real pY, Real pZ );
	Vector3 getScale();
	/// brief set/get the Taper X parameter value
	virtual void setTaperX(Real value);
	Real getTaperX() {return mTaperX;};
	/// brief set/get the Taper Y parameter value
	virtual void setTaperY(Real value);
	Real getTaperY() {return mTaperY;};
	/// brief set/get the Path Cut Begin parameter value
	virtual void setPathCutBegin(Real value);
	Real getPathCutBegin() {return mPathCutBegin;};
	/// brief set/get the Path Cut End parameter value
	virtual void setPathCutEnd(Real value);
	Real getPathCutEnd() {return mPathCutEnd;};
	/// brief set/get the Dimple Begin parameter value
	virtual void setDimpleBegin(Real value);
	Real getDimpleBegin() {return mDimpleBegin;};
	/// brief set/get the Dimple End parameter value
	virtual void setDimpleEnd(Real value);
	Real getDimpleEnd() {return mDimpleEnd;};
	/// brief set/get the Hole X parameter value
	virtual void setHoleSizeX(Real value);
	Real getHoleSizeX() {return mHoleSizeX;};
	/// brief set/get the Hole Y parameter value
	virtual void setHoleSizeY(Real value);
	Real getHoleSizeY() {return mHoleSizeY;};
	/// brief set/get the Hole Shape parameter value
	virtual void setHollowShape(Shape shape);
	Real getHollowShape() {return mHollowShape;};
	/// brief set/get the Twist Begin parameter value
	virtual void setTwistBegin(Real value);
	Real getTwistBegin() {return mTwistBegin;};
	/// brief set/get the Twist End parameter value
	virtual void setTwistEnd(Real value);
	Real getTwistEnd() {return mTwistEnd;};
	/// brief set/get the Top Shear X parameter value
	virtual void setTopShearX(Real value);
	Real getTopShearX() {return mTopShearX;};
	/// brief set/get the Top Shear Y parameter value
	virtual void setTopShearY(Real value);
	Real getTopShearY() {return mTopShearY;};
	/// brief set/get the Skew parameter value
	virtual void setSkew(Real value);
	Real getSkew() {return mSkew;};
	/// brief set/get the Revolutions parameter value
	virtual void setRevolutions(int value);
	int getRevolutions() {return mRevolutions;};
	/// brief set/get the Radius Delta parameter value
	virtual void setRadiusDelta(Real value);
	Real getRadiusDelta() {return mRadiusDelta;};

	/// brief set/get the object center position
	/// param worldPosition = if TRUE, return the world position
	/// return the object center position
	virtual void setPosition(Vector3 pos);
	Vector3 getPosition(bool worldPosition = false);
	/// brief Return the object orientation
	Vector3 getOrientation();	
	
	/// brief Link a child to the object. The node of pObj become the child of the node of this object
	///		and pObj is added to the list mChildren. If oObj is already linked, it unlinks it.
	///	 param pObj Object to link with this Object.
	///	 param pSceneMgr Current SceneManager
	///	 return TRUE if node is linked correctly and FALSE if pObj is unlinked.
	bool linkObject(Object3D* pObj, SceneManager* pSceneMgr);
	/// Test if an object is already link whith this object
	///	 param pObj Object3D to test.
	///	 return TRUE is pObj is already link, else return FALSE.
	bool isLink(Object3D* pObj);
	/// Get the list of child
	///	 return mChildren
	vector<Object3D*>* getChildren();
    /// Return the current parent of this object
	///	 return mParent ;
	Object3D* getParent() { return mParent; }
	
	

	/// brief ...
	Entity*	getEntity() { return mEntity; };

	/// Get the creator name 
	Ogre::String &	getCreator() {return mCreatorName;}; 
	/// Set the creator name 
	void	setCreator(const char *newCreator) {mCreatorName = newCreator;}; 
	/// Get the owner name 
	Ogre::String &	getOwner() {return mOwnerName;}; 
	/// Set the owner name 
	void	setOwner(const char *newOwner) {mOwnerName = newOwner;}; 
	/// Get the group name 
	Ogre::String &	getGroup() {return mGroupName;}; 
	/// Set the group name 
	void	setGroup(const char *newGroup) {mGroupName = newGroup;}; 
	/// Get the description 
	Ogre::String &	getDesc() {return mDesc;}; 
	/// Set the description 
	void	setDesc(const char *newDesc) {mDesc = newDesc;}; 
	/// Get the tags 
	Ogre::String &	getTags() {return mTags;}; 
	/// Set the tags
	void	setTags(const char *newTags) {mTags = newTags;}; 
	/// Get the modification bool 
	bool	getCanBeModified() {return mCanBeModified;}; 
	/// Set the modification bool 
	void	setCanBeModified(bool newCanBeModified) {mCanBeModified = newCanBeModified;}; 
	/// Get the copy bool 
	bool	getCanBeCopied() {return mCanBeCopied;}; 
	/// Set the copy bool 
	void	setCanBeCopied(bool newCanBeCopied) {mCanBeCopied = newCanBeCopied;}; 
	/// Get the collision bool 
	bool	getCollisionnable() {return mEnableCollision;}; 
	/// Set the collision bool 
	void	setCollisionnable(bool newEnableCollision) {mEnableCollision = newEnableCollision;}; 
	/// Get the gravity bool 
	bool	getEnableGravity() {return mEnableGravity;}; 
	/// Set the gravity bool 
	void	setEnableGravity(bool newEnableGravity) {mEnableGravity = newEnableGravity;}; 

	/// Get the primitive type as int
	int getTypeAsInt() {return (int)mType;};

	///brief Get a pointer in the Material manager of this object
	///return mModifiedMaterialManager
	ModifiedMaterialManager * getMaterialManager(){return mModifiedMaterialManager ;} ;
	///brief Method which return a texture with a given name which belongs to the map of textures of the object.
	TexturePtr getTexture(const String& name);
	///brief Method which add a texture to the object and to all the children, this texture will be choosable by the user who will be able to apply it on the object.
	///brief  Add texture only if it doesn't already added.
	///param texture Ogre texture to add
	///param textureExtParamsMap Texture extended parameters of added texture.
	void addTexture(TexturePtr texture, const TextureExtParamsMap& textureExtParamsMap = TextureExtParamsMap());
	///brief Method which delete a texture to the object textures list. If it is the current texture, changes to deflaut texture.
	///param texture Ogre texture to delete
	void deleteTexture(TexturePtr pTexture) ;
	///brief Method which set the given texture as the current texture of the object and for all children. 
	///param texture Texture to set as current texture.
	void setCurrentTexture(const String& textureName);
	///brief Method which set the given texture as the current texture of the object and for all children. 
	///param texture Texture to set as current texture.
	///param textureExtParamsMap Texture extended parameters of current texture.
	void setCurrentTexture(const TexturePtr pTexture, const TextureExtParamsMap& textureExtParamsMap = TextureExtParamsMap());
	///brief Method which return the current texture applied on the object.
	///return the current texture applied on the object.
	TexturePtr getCurrentTexture();
	///brief Method which return the current texture extended parameters applied on the object.
	///return the current texture extended parameters applied on the object.
	TextureExtParamsMap* getCurrentTextureExtParamsMap();

	void setAmbient( const ColourValue pColor);
	void setDiffus( const ColourValue pColor);
	void setSpecular( const ColourValue pColor);
	void setShininess ( const float pColor);
	ColourValue getAmbient();
	ColourValue getDiffus();
	ColourValue getSpecular();
	float getShininess ();

	///brief Sets the translation offset of the texture (ie scrolls the texture) and apply on all children
	///param pU  The amount the texture should be moved horizontally (u direction). 
	///param pV  The amount the texture should be moved vertically (v direction). 
	void setTextureScroll(float pU, float pV);
	///brief Sets the scaling factor applied to texture coordinates and apply on all children 
	///param pU  The amount the texture should be scalled horizontally (u direction). 
	///param pV  The amount the texture should be scalled vertically (v direction). 
	void setTextureScale(float pU, float pV);
	///brief Sets the anticlockwise rotation factor applied to texture coordinates. (in radian)
	///param pAngle  angle  The angle of rotation (anticlockwise).   
	void setTextureRotate(Ogre::Radian pAngle);
	///brief Sets the alpha value to be applied to this object and all it children. 
	///param pValue alpha value (between 0 - 1) 
    void setAlpha(float pValue);
	///brief Get the alpha value of the object
	///return the value of alpha
	float getAlpha();
	///brief Sets the scene blending type to be applied to this object and all it children. 
	///param pSceneBlendType scene blending type
    void setSceneBlendType(Ogre::SceneBlendType pSceneBlendType);
	///brief Get the scene blending type of the object
	///return the scene blending type
	Ogre::SceneBlendType getSceneBlendType();

	//brief Get Vertex number
	int getVertexCount() {return (int)mVertexCount;};
	//brief Get Tri number
	int getTriCount() {return (int)mTriangleCount;};
	//brief Get Mesh Size
	Vector3 getMeshSize() {return mSize;};
	//brief Get primitives count
	int getPrimitivesCount() {return mPrimitivesCount;};

	/// Show or Hide the bounding box of this object and for all its child
	///	 param pValue TRUE for show bounding box, FALSE for hide it.
	void showBoundingBox(bool pValue);
	///brief Get if the bounding box is show or not.
	///return True is bounding box is shown, and false is it is hiden.
	bool getShowBoundingBox();
	///brief Update the size of the bounding box.
	void updateBoundingBox();

	/// brief Apply an action to modifiy the object
	bool apply( Command command, Real p1 );
	bool apply( Command command, Real p1, Real p2, Real p3 );

	///brief Add a command to the list of commands and update the list of points
	///param pCommand : command to add
	bool addCommand(TCommand &pTCommand, Command &pOldCommand, bool pForSave = false );
	///brief Remove the last command added and re-apply all the others in the right order
	bool undo();

	///brief Restore the second buffer by the first one
	///param pBufNew : the new reference buffer 
	///param pBufOld : the old buffer to be restored
	bool restoreBuffer( Buffer* pBufNew, Buffer* pBufOld );
	///brief Restore the second vertex buffer by the first one
	///param pBufNew : the new reference buffer 
	///param pBufOld : the old buffer to be restored
	bool restoreBufferVertex( Buffer* pBufNew, Buffer* pBufOld );
	///brief Restore the second index buffer by the first one
	///param pBufNew : the new reference buffer 
	///param pBufOld : the old buffer to be restored
	bool restoreBufferIndex( Buffer* pBufNew, Buffer* pBufOld );
	///brief Restore the backup buffer by the current one
	void updateBackup() { restoreBuffer( mBufCurrent, mBufBackup ); }
	///brief Resize the both verex & index hardware buffers
	void resizeBuffers( realvector &vertexData, size_t vertexCount, uintvector &indexData, size_t indexCount = 0 );


private:
	///brief Get the global size and the max & min corner sizes of the object
	void getSize(Vector3 &size, Vector3 &min, Vector3 &max); 
	///brief Copy the hardware vertex data to a vector Vertex
	void getDataFromBuffer(vector<Vector3>* pVertex, vector<Face>* pFace);
	///brief Change the location of an existing point in the point list
	void setPoint(unsigned int index, const Vector3 &value);
	///brief Return the location of an existing point in the point list
	Vector3 getPoint(unsigned int index);
	///brief Return the total number of points in the point list
	size_t getNumPoints(void);
	///brief Return the face from the index faces
	void getFace(unsigned int index, Face &face);
	///brief Return the total number of faces
	size_t getNumFaces(void);


	///brief Rotate from another object
	void rotateFromParent( float pValueX, float pValueY, float pValueZ );

	///brief Call this to update the hardware buffer after making changes.  
	void update();
	///brief Update only the hardware vertex buffer from the current vretex buffer
	void updateBufferVertex();

	/// Add child to the list mChildren
	///	 param child Object3D to add
	void addChild(Object3D* child);
	/// Remove child to the list mChildren. The object is not deleted.
	///	param child Object3D to remove
	void removeChild(Object3D* child);
	/// Set parent : just update mParent
	void setParent(Object3D* object);


/*
	/// Put mCentreRotation and mCentreObject on their correct position
	///		mCentreRotation on the centre of selection
	///		mCentreObject on its position after rotation
	void findRotationPosition(float pValueX, float pValueY, float pValueZ, Vector3 pCentreSelection,
							 SceneNode * pCentreRotation, SceneNode* pCentreObject);
*/

protected:
	Entity* mEntity;							/// brief The associate 3D entity
	SceneNode* mNode;							/// brief The node where the object / entity is attached
	Vector3 mSize;								/// brief The object size
	Vector3 mCornerMin;							/// brief ...
	Vector3 mCornerMax;							/// brief ...
	vector<Object3D*>* mChildren;				/// brief The list of the sub objects / children
	Object3D* mParent;							/// brief The parent

    EntityUID mEntityUID;                       /// brief ...
	Ogre::String mName;							/// brief ...
	Type mType;									/// brief ...
 
	Real mTranslateX;							///brief ...
	Real mTranslateY;							///brief ...
	Real mTranslateZ;							///brief ...
	Real mRotateX;								///brief ...
	Real mRotateY;								///brief ...
	Real mRotateZ;								///brief ...
	Real mScaleX;								///brief ...
	Real mScaleY;								///brief ...
	Real mScaleZ;								///brief ...
	Real mTaperX;								/// brief ...
	Real mTaperY;								/// brief ...
    Real mPathCutBegin;							/// brief ...
	Real mPathCutEnd;							/// brief ...
	Real mDimpleBegin;							/// brief ...
	Real mDimpleEnd;							/// brief ...
	Real mHoleSizeX;							/// brief ...
	Real mHoleSizeY;							/// brief ...
	Shape mHollowShape;							/// brief ...
	Real mTwistBegin;							/// brief ...
	Real mTwistEnd;								/// brief ...
	Real mTopShearX;							/// brief ...
	Real mTopShearY;							/// brief ...
	Real mSkew;									/// brief ...
	unsigned int mRevolutions;					/// brief ...
	Real mRadiusDelta;							/// brief ...

	/// The creator property
	Ogre::String mCreatorName;							
	/// The owner property
	Ogre::String mOwnerName;							
	/// The group property
	Ogre::String mGroupName;							
	/// The description property
	Ogre::String mDesc;							
	/// The Tags property
	Ogre::String mTags;							
	/// The modification property
	bool		 mCanBeModified;
	/// The copy property
	bool		 mCanBeCopied;

	/// The collision property
	bool		 mEnableCollision;
	/// The gravity property
	bool		 mEnableGravity;
	/// The primitives count 
	/// TODO : icrement this value when you link an object
	int			mPrimitivesCount;

	/// The manager for all material of this object :
	ModifiedMaterialManager * mModifiedMaterialManager ;


private:
	VertexData* mVertexData;					/// brief ...

	realvector mVertex;							/// brief ...
	size_t mVertexCount;						/// brief ...
	size_t mVertexDecl;							/// brief ...

	uintvector mIndex;							/// brief ...
	size_t mIndexCount;							/// brief ...
	size_t mTriangleCount;						/// brief ...

public:
	Buffer *mBufPrim;							/// brief ...
	Buffer *mBufBackup;							/// brief ...
	Buffer *mBufCurrent;						/// brief ...

	Vector3 mCentreSelection;					/// brief ...
	SceneNode *mCentreRotation;					/// brief ...
	SceneNode *mCentreObject;					/// brief ...

	Command	mCommandLast;						/// brief The last command used 
	std::list<TCommand> mCommandList;			/// brief The commands history list 
};

static Object3D::Type objectStringToType(Ogre::String &toFind)
{
	int i = 0;
	for (i = 0 ; i < PRIMCOUNT ; i++)
	{
		if (toFind == SOLTYPESTRING[i])
			return (Object3D::Type) i;
	}
	return Object3D::OTHER;
}

// ---------------------------------------------------------------------------------
class Object3DPlane : public Object3D
{
public:
	Object3DPlane(const EntityUID& pEntityUID, const String& pName, SceneNode* pNode) : Object3D(pEntityUID, pName, pNode)
	{
        mType = PLANE;
	}
};

// ---------------------------------------------------------------------------------
class Object3DBox : public Object3D
{
public:
	Object3DBox(const EntityUID& pEntityUID, const String& pName, SceneNode* pNode) : Object3D(pEntityUID, pName, pNode)
	{
        mType = BOX;
	}
};

// ---------------------------------------------------------------------------------
class Object3DCorner : public Object3D
{
public:
	Object3DCorner(const EntityUID& pEntityUID, const String& pName, SceneNode* pNode) : Object3D(pEntityUID, pName, pNode)
	{
        mType = CORNER;

		apply( Object3D::TAPERX, 1);
		restoreBufferVertex( mBufCurrent, mBufBackup );
		apply( Object3D::TOP_SHEARX, 0.5);
		restoreBufferVertex( mBufCurrent, mBufBackup );
		restoreBufferVertex( mBufCurrent, mBufPrim );

		setTaperX( 0 );
		setTopShearX( 0 );

		// Init the last command 
		mCommandLast = NONE;
	}
};

// ---------------------------------------------------------------------------------
class Object3DPyramid : public Object3D
{
public:
	Object3DPyramid(const EntityUID& pEntityUID, const String& pName, SceneNode* pNode) : Object3D(pEntityUID, pName, pNode)
	{
        mType = PYRAMID;

		apply( Object3D::TAPERX, 1);
		apply( Object3D::TAPERY, 1);
		restoreBufferVertex( mBufCurrent, mBufBackup );
		restoreBufferVertex( mBufCurrent, mBufPrim );

		setTaperX( 0 );
		setTaperY( 0 );

		// Init the last command 
		mCommandLast = NONE;
	}
};

// ---------------------------------------------------------------------------------
class Object3DPrism : public Object3D
{
public:
	Object3DPrism(const EntityUID& pEntityUID, const String& pName, SceneNode* pNode) : Object3D(pEntityUID, pName, pNode)
	{
        mType = PRISM;

		apply( Object3D::TAPERX, 1);
		apply( Object3D::TAPERY, 1);
		restoreBufferVertex( mBufCurrent, mBufBackup );
		restoreBufferVertex( mBufCurrent, mBufPrim );

		setTaperX( 0 );
		setTaperY( 0 );

		// Init the last command 
		mCommandLast = NONE;
	}
};

// ---------------------------------------------------------------------------------
class Object3DCylinder : public Object3D
{
public:
	Object3DCylinder(const EntityUID& pEntityUID, const String& pName, SceneNode* pNode) : Object3D(pEntityUID, pName, pNode)
	{
        mType = CYLINDER;
	}
};

// ---------------------------------------------------------------------------------
class Object3DHalfCylinder : public Object3D
{
public:
	Object3DHalfCylinder(const EntityUID& pEntityUID, const String& pName, SceneNode* pNode) : Object3D(pEntityUID, pName, pNode)
	{
        mType = HALF_CYLINDER;

		apply( Object3D::PATH_CUT_BEGIN, 0.25);
		apply( Object3D::PATH_CUT_END, 0.75 );
		//restoreBuffer( mBufCurrent, mBufBackup );
		//restoreBuffer( mBufCurrent, mBufPrim );

		setPathCutBegin( 0 );
		setPathCutEnd( 1 );

		// Init the last command 
		mCommandLast = NONE;
	}
};

// ---------------------------------------------------------------------------------
class Object3DCone : public Object3D
{
public:
	Object3DCone(const EntityUID& pEntityUID, const String& pName, SceneNode* pNode) : Object3D(pEntityUID, pName, pNode)
	{
        mType = CONE;
		mTaperX = 1;
		mTaperY = 1;

		apply( Object3D::TAPERX, 1);
		apply( Object3D::TAPERY, 1);
		restoreBufferVertex( mBufCurrent, mBufBackup );
		restoreBufferVertex( mBufCurrent, mBufPrim );

		setTaperX( 0 );
		setTaperY( 0 );

		// Init the last command 
		mCommandLast = NONE;
	}
};

// ---------------------------------------------------------------------------------
class Object3DHalfCone : public Object3D
{
public:
	Object3DHalfCone(const EntityUID& pEntityUID, const String& pName, SceneNode* pNode) : Object3D(pEntityUID, pName, pNode)
	{
        mType = HALF_CONE;

		apply( Object3D::TAPERX, 1);
		apply( Object3D::TAPERY, 1);
		restoreBufferVertex( mBufCurrent, mBufBackup );
		apply( Object3D::PATH_CUT_BEGIN, 0.25);
		apply( Object3D::PATH_CUT_END, 0.75 );
		//restoreBuffer( mBufCurrent, mBufBackup );
		//restoreBuffer( mBufCurrent, mBufPrim );

		setTaperX( 0 );
		setTaperY( 0 );
		setPathCutBegin( 0 );
		setPathCutEnd( 1 );

		// Init the last command 
		mCommandLast = NONE;
	}
};

// ---------------------------------------------------------------------------------
class Object3DSphere : public Object3D
{
public:
	Object3DSphere(const EntityUID& pEntityUID, const String& pName, SceneNode* pNode) : Object3D(pEntityUID, pName, pNode)
	{
        mType = SPHERE;
	}
};

// ---------------------------------------------------------------------------------
class Object3DHalfSphere : public Object3D
{
public:
	Object3DHalfSphere(const EntityUID& pEntityUID, const String& pName, SceneNode* pNode) : Object3D(pEntityUID, pName, pNode)
	{
        mType = HALF_SPHERE;

		//apply( Object3D::PATH_CUT_BEGIN, 0.0 );
		//apply( Object3D::PATH_CUT_END, 0.5 );
		apply( Object3D::PATH_CUT_BEGIN, 0.25);
		apply( Object3D::PATH_CUT_END, 0.75 );
		//restoreBuffer( mBufCurrent, mBufBackup );
		//restoreBuffer( mBufCurrent, mBufPrim );

		setPathCutBegin( 0 );
		setPathCutEnd( 1 );

		// Init the last command 
		mCommandLast = NONE;
	}
};

// ---------------------------------------------------------------------------------
class Object3DTorus : public Object3D
{
public:
	Object3DTorus(const EntityUID& pEntityUID, const String& pName, SceneNode* pNode) : Object3D(pEntityUID, pName, pNode)
	{
        mType = TORUS;
		mHoleSizeX = 1;
		mHoleSizeY = 0.25;

		setHoleSizeX( 0 );
		setHoleSizeY( 0 );

		// Init the last command 
		mCommandLast = NONE;
	}
};

// ---------------------------------------------------------------------------------
class Object3DTube : public Object3D
{
public:
	Object3DTube(const EntityUID& pEntityUID, const String& pName, SceneNode* pNode) : Object3D(pEntityUID, pName, pNode)
	{
        mType = TUBE;
		mHoleSizeX = 1;
		mHoleSizeY = 0.25;
		mHollowShape = CIRCLE;

		setHoleSizeX( 0 );
		setHoleSizeY( 0 );

		// Init the last command 
		mCommandLast = NONE;
	}
};

// ---------------------------------------------------------------------------------
class Object3DRing: public Object3D
{
public:
	Object3DRing(const EntityUID& pEntityUID, const String& pName, SceneNode* pNode) : Object3D(pEntityUID, pName, pNode)
	{
        mType = RING;
		mHoleSizeX = 1;
		mHoleSizeY = 0.25;

		setHoleSizeX( 0 );
		setHoleSizeY( 0 );

		// Init the last command 
		mCommandLast = NONE;
	}
};

// ---------------------------------------------------------------------------------
class Object3DOther: public Object3D
{
public:
	Object3DOther(const EntityUID& pEntityUID, const String& pName, SceneNode* pNode) : Object3D(pEntityUID, pName, pNode)
	{
        mType = OTHER;
	}
};

} //namespace

// ---------------------------------------------------------------------------------

#endif //__Object3D_h__
