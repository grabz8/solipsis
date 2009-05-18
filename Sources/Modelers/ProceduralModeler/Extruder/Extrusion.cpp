/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author MERLET Jonathan

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

/*!
 * \file OgreExtrusion.cpp
 * \brief Source file implementing the OgreExtrusion class.
 * 
 * \author Jonathan Merlet
 * \date 25 juin 2008 
 * 
 */

#include "Extrusion.h"
#include "wrapperobj.h"

#include <sstream>

#include "texInfo.h"

using namespace std;
using namespace Ogre;

Extrusion::Extrusion()
: _root( NULL )
{
	//warning : as this library is stand alone by now, it is launching 
	//ogre root object by itself, so don't forget to delete the following
	//lines to avoid problems (except for the wrapper linking)

	//Create an Ogre root object
	_root = new Root( "plugins.cfg","ogre.cfg", "ogre.log" ); //Root::getSingletonPtr();
   
	//First try to restore the Ogre config, and if there is no ogre.cfg file, show the config dialog.
    if (!(_root->restoreConfig() || _root->showConfigDialog()))
        throw Exception(52, "User canceled the config dialog!", "Application::setupRenderSystem()");

    //Initialise the Ogre root object, telling him we don't want to have an auto generated window
    _root->initialise(false);
    
    //Create an hardware buffer manager
    _hardwareBufferManager = new DefaultHardwareBufferManager();
    
    //Tells gle to use this instance to send his informations. Because of that we can't have multiple extruders at a time.
    wrapperobjOgre = this;
}


Extrusion::~Extrusion()
{
    wrapperobjOgre = NULL;
    
    //Delete the Ogre root object
//    delete _root;
}


void Extrusion::extrudeGeometry(int geometryID, const char* geometryName, int ncp, gleDouble** contour, gleDouble** norm_contour, gleDouble up_vector[3], int npp, gleDouble** path, gleDouble*** xform, texInfo* geometryTexInfos)
{
	//Check if there isn't already a geometry using this ID 
	if (_nbVertices.find(geometryID) != _nbVertices.end()){
		throw ExtruderException("Une geometrie (mesh/submesh) possedant cette ID a deja ete extrude !");
	}
	
	//Set the pointers to know where to send the data
	_activeGeometry = &geometryID;
	string sname(geometryName);
	_geometryNames[geometryID] = sname;
	_activeNbVertices = &(_nbVertices[geometryID]);
	_activeVertices = &(_vertices[geometryID]);
	_activeFaces = &(_faces[geometryID]);
	_activeTCoordinates = &(_tcoordinates[geometryID]);
	_activeNormals = &(_normals[geometryID]);
	float contourLength = 0;
	int i;
	for(i = 0 ; i < ncp - 1 ; ++i){	
		contourLength += Vector2(contour[i+1][0] - contour[i][0], 
								 contour[i+1][1] - contour[i][1]).length();
	}
	_texInformations[geometryID] = geometryTexInfos;
	_contourLength[geometryID] = contourLength;
	_activeContourLength = contourLength;
	
	//Set join style and texture style
	setTextureStyleOgre(GLE_TEXTURE_VERTEX_FLAT);
	setJoinStyleOgre(TUBE_JN_ANGLE | TUBE_NORM_PATH_EDGE);
	
	//Calls the gle function to start the extrusion
	gleSuperExtrusionOgre(ncp, contour, norm_contour, up_vector, npp, path, NULL, xform);
	
	//Unset the pointers
	_activeGeometry = NULL;
	_activeNbVertices = NULL;
	_activeVertices = NULL;
	_activeFaces = NULL;
	_activeTCoordinates = NULL;
	_activeNormals = NULL;
}

MeshPtr Extrusion::createMesh(int* submeshesIDs, int nbOfSubmeshes, int meshID, const char* meshName){
	//Check if there isn't already a mesh with this ID
	if(_meshNames.find(meshID) != _meshNames.end()){
		throw ExtruderException("Il existe deja un mesh avec cette ID !");
	}
	
	//Store the name of the mesh
	string sname(meshName);
	_meshNames[meshID] = sname;
	
	//Ask Ogre to create a pointer for us
    MeshPtr mesh = MeshManager::getSingleton().createManual(_meshNames[meshID].c_str(),"RoadGen");
    
    //A bounding box for culling
    AxisAlignedBox aabox;
	int i;
	
	//For each geometry int the submeshesIDs list, create the submesh
	for(i = 0 ; i < nbOfSubmeshes ; ++i){
		createSubmesh(mesh, submeshesIDs[i], aabox);
	}
	
    //We must indicate the bounding box
    mesh->_setBounds(aabox);
    mesh->_setBoundingSphereRadius((aabox.getMaximum()-aabox.getMinimum()).length()/2.0);
    
    //And we load the mesh
    mesh->load();
    
    //return MeshPtr
    return mesh;
}

void Extrusion::createSubmesh(MeshPtr& mesh, int submeshGeometryID, AxisAlignedBox& aabox){
	//Check if he geometry of the submesh we want to create has been extruded
	if (_nbVertices.find(submeshGeometryID) == _nbVertices.end()){
		throw ExtruderException("Vous essayez de creer un mesh dont vous n'avez pas encore extrude un des submesh !");
	}
	
	//Keep track of the data storages to use
	_activeContourLength = _contourLength[submeshGeometryID];
	_activeGeometry = &submeshGeometryID;
	_activeNbVertices = &_nbVertices[submeshGeometryID];
	_activeVertices = &_vertices[submeshGeometryID];
	_activeFaces = &_faces[submeshGeometryID];
	_activeTCoordinates = &_tcoordinates[submeshGeometryID];
	_activeNormals = &_normals[submeshGeometryID];
	_activeTexInformations = _texInformations[submeshGeometryID];
	unsigned int i;
	
	//Set some values 
	unsigned int nbFaces = _activeFaces->size();
	unsigned int nbOfVerticesPerFace = 3;
	
    //We create a submesh associated to the parent mesh given in parameter
    SubMesh* submesh = mesh->createSubMesh(_geometryNames[submeshGeometryID]);
    
    //We must create the vertex data, indicating how many vertices there will be
	unsigned int nbVertices = *(_activeNbVertices);
	
	//Submeshes won't share informations between them
    submesh->useSharedVertices = false;
    
    //Create the Vertex data
    submesh->vertexData = new VertexData();
    submesh->vertexData->vertexStart = 0;
    submesh->vertexData->vertexCount = nbVertices;
    
    //We must now declare what the vertex data contains
    VertexDeclaration* declaration = submesh->vertexData->vertexDeclaration;
    static const unsigned short source = 0;
    size_t offset = 0;
    
    //Tell we want to have a position, a normal and texture coordinates for each vertex
    offset += declaration->addElement(source,offset,VET_FLOAT3,VES_POSITION).getSize();
    offset += declaration->addElement(source,offset,VET_FLOAT3,VES_NORMAL).getSize();
    offset += declaration->addElement(source,offset,VET_FLOAT2,VES_TEXTURE_COORDINATES).getSize();
    
    //Create a vertex buffer
    HardwareVertexBufferSharedPtr vbuffer =
    HardwareBufferManager::getSingletonPtr()->createVertexBuffer(declaration->getVertexSize(source), // == offset
                                 submesh->vertexData->vertexCount,   // == nbVertices
                                 HardwareBuffer::HBU_STATIC_WRITE_ONLY,
                                 true);
    
    //Lock the buffer for writing
    float* vdata = static_cast<float*>(vbuffer->lock(HardwareBuffer::HBL_DISCARD));

    // No we get access to the buffer to fill it.  During so we record the bounding box.
    for (i=0;i<nbVertices;++i)
    {
        // Position
        Vector3 position = (*_activeVertices)[i];
        *vdata++ = position.x;
        *vdata++ = position.y;
        *vdata++ = position.z;
        //Update the bounding box
        aabox.merge(position);
        // Normal
        *vdata++ =  ((*_activeNormals)[i]).x;
        *vdata++ =  ((*_activeNormals)[i]).y;
        *vdata++ =  ((*_activeNormals)[i]).z;
        // Texture coordinate
        //We assume the contour is centered around zero, so we must translate the t-coordinates and normalize them on x
        *vdata++ =  (((*_activeTCoordinates)[i]).x + _activeContourLength/2.0)/_activeContourLength;
        *vdata++ =  ((*_activeTCoordinates)[i]).y / _activeTexInformations->length;
    }
    
    //Release the buffer
    vbuffer->unlock();
    
    //Set the buffer binding
    submesh->vertexData->vertexBufferBinding->setBinding(source,vbuffer);
    
    // Creates the index data
    submesh->indexData->indexStart = 0;
    submesh->indexData->indexCount = nbFaces*nbOfVerticesPerFace;
    submesh->indexData->indexBuffer =
    	HardwareBufferManager::getSingletonPtr()->createIndexBuffer(HardwareIndexBuffer::IT_32BIT,
                                submesh->indexData->indexCount,
                                HardwareBuffer::HBU_STATIC_WRITE_ONLY);
    
    //Lock the buffer
    uint32* idata = static_cast<uint32*>(submesh->indexData->indexBuffer->lock(HardwareBuffer::HBL_DISCARD));
    
    //Fill it
    for (i=0;i<nbFaces;++i)
    {
        *idata++ = (int)((*_activeFaces)[i].x);
        *idata++ = (int)((*_activeFaces)[i].y);
        *idata++ = (int)((*_activeFaces)[i].z);
    }
    
    //And unlock it
    submesh->indexData->indexBuffer->unlock();
    
    //Create a material for the mesh
    if(_materialsPerSubmeshes){
        createMaterial(submeshGeometryID);
        exportMaterial(_geometryNames[submeshGeometryID].c_str());
        submesh->setMaterialName(_geometryNames[submeshGeometryID].c_str());
    } else{
    	submesh->setMaterialName(_activeTexInformations->filename);
    }
    
	_activeGeometry = NULL;
	_activeNbVertices = NULL;
	_activeVertices = NULL;
	_activeFaces = NULL;
	_activeTCoordinates = NULL;
	_activeNormals = NULL;
	_activeTexInformations = NULL;
}

void Extrusion::exportMesh(int meshID){
	//Get the name of the mesh
	const char* meshName = (_meshNames[meshID]).c_str();
	
	//Retrieve the associated Ogre mesh
	MeshPtr mesh = MeshManager::getSingleton().getByName(meshName);
	
	//Has this mesh been created ?
	if(mesh.isNull())
		throw ExtruderException("Vous essayez d'exporter un mesh qui n'a pas ete extrude !");
	
	//Create an exporter
	MeshSerializer serializer;
	
	//Add the .mesh extension to the name
	ostringstream oss;
	oss << meshName << ".mesh";
	
	//Finally save the mesh
	serializer.exportMesh(mesh.get(), oss.str().c_str());
}

void Extrusion::createMaterial(int ID){
	const char* materialName;
	materialName = (_geometryNames[ID]).c_str();
	
	//Ask Ogre to create a material
	MaterialPtr material = MaterialManager::getSingleton().create(materialName,"RoadGen");
	
	//Start with a fresh one
	material->removeAllTechniques();
	
	//Create a technique
	Technique* tech = material->createTechnique();
	
	//Create a pass
	tech->removeAllPasses();
	Pass* pass = tech->createPass();

	//No backface culling for this pass
	pass->setCullingMode(CULL_NONE);
	pass->setManualCullingMode(MANUAL_CULL_NONE);
	
	//Add a texture unit
	pass->createTextureUnitState(_texInformations[ID]->filename.c_str());
}

void Extrusion::createMaterial(string filename){
	if(_materials.find(filename) == _materials.end()){
		//Check the materialType parameter if it's a mesh or a submesh that requested the material
		const char* materialName = filename.c_str();
		
		//Ask Ogre to create a material
		MaterialPtr	material = MaterialManager::getSingleton().create( materialName, "ROGE" );
		
		//Start with a fresh one
		material->removeAllTechniques();
		
		//Create a technique
		Technique* tech = material->createTechnique();
		
		//Create a pass
		tech->removeAllPasses();
		Pass* pass = tech->createPass();
	
		//No backface culling for this pass
		pass->setCullingMode(CULL_NONE);
		pass->setManualCullingMode(MANUAL_CULL_NONE);
		
		//Add a texture unit
		pass->createTextureUnitState(filename.c_str());
		
		//Store this material in the map to use it later
		_materials[filename] = material;
		
		//Finally export it
		exportMaterial(filename.c_str());
	}
}

void Extrusion::exportMaterial(const char* materialName){
	//Retrieve the material with his name
	MaterialPtr material = MaterialManager::getSingleton().getByName(materialName);
	
	//Prepare the exporter
	MaterialSerializer serializer;
	
	//Add the extension to the name
	ostringstream oss;
	oss << materialName << ".material";
	
	//Finally save the material
	serializer.exportMaterial(material, oss.str().c_str());
}

void Extrusion::setMaterialsPerSubmeshesFlag(bool materialsPerSubmeshes){
	_materialsPerSubmeshes = materialsPerSubmeshes;
}

void Extrusion::beginTriangleStripOgre()
{
	//As we are starting a new triangle strip, there are no vertices in it
    _nbVerticesStrip=0;
    _triangleStrip = true;
    
    //the first triangle will be counter clockwise
    _clockwise = false;
    
    //Store from where we are starting depending on the already stored vertices
    _indexDebut = _activeVertices->size();
}


void Extrusion::endTriangleStripOgre()
{
	//Store the number of vertices created for this strip
    *(_activeNbVertices) += _nbVerticesStrip;
}

void Extrusion::addVertexOgre(double x, double y, double z)
{
	//Get the stack's top matrix
    Ogre::Matrix4 topMatrix = _matrixStack.top();
    
    //apply it to the vertex given by gle
    Ogre::Vector3 added = topMatrix*Ogre::Vector3(x,y,z);
    
	//We push back the vertex into the vector
	_activeVertices->push_back(topMatrix*Ogre::Vector3(x,y,z));
	_nbVerticesStrip++;
	
	//Due to the trianglestrip, if there have been more than 3 vertices added in the vertices vector for this strip,
	//this new vertex defines a new triangle with his two predecessors and we must add it to the face list.
	if(_triangleStrip && _nbVerticesStrip >= 3){
		//Depending on the clockwise flag, we may have to invert the order of the vertices
	    if(_clockwise){
		_activeFaces->push_back(Ogre::Vector3(_indexDebut+_nbVerticesStrip-3,
                                       _indexDebut+_nbVerticesStrip-2,
                                       _indexDebut+_nbVerticesStrip-1));
	    }else{
		_activeFaces->push_back(Ogre::Vector3(_indexDebut+_nbVerticesStrip-1,
                                       _indexDebut+_nbVerticesStrip-2,
                                       _indexDebut+_nbVerticesStrip-3));
	    }
        _clockwise = !_clockwise;
    }
}


void Extrusion::addNormalOgre(double x, double y, double z)
{
    Ogre::Vector3 normal = Ogre::Vector3(x,y,z);
    
    //Normalise the normal
    normal.normalise();
    
    //Add it into the current normal storage
    _activeNormals->push_back(normal);
}


void Extrusion::addTexCoordOgre(double u, double v)
{
	//Simply add the texture coordinates in the current tcoordinates storage
    _activeTCoordinates->push_back(Ogre::Vector2(u, v));
}


void Extrusion::pushMatrixOgre(){
	//Push an identity transformation matrix
    _matrixStack.push(Ogre::Matrix4(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1));
}


void Extrusion::popMatrixOgre(){
	//Pop the top matrix of the stack
    _matrixStack.pop();
}


void Extrusion::loadMatrixOgre(gleDouble mat[4][4]){
	//Load a matrix at the top of the stack
    _matrixStack.pop();
    _matrixStack.push(Ogre::Matrix4(mat[0][0], mat[1][0], mat[2][0], mat[3][0],
                                    mat[0][1], mat[1][1], mat[2][1], mat[3][1],
                                    mat[0][2], mat[1][2], mat[2][2], mat[3][2],
                                    mat[0][3], mat[1][3], mat[2][3], mat[3][3]));
}


void Extrusion::multMatrixOgre(gleDouble mat[4][4]){
	//Multiply the top matrix by the one given in parameter
    Ogre::Matrix4 topMatrix = _matrixStack.top();
    topMatrix = Ogre::Matrix4(mat[0][0], mat[1][0], mat[2][0], mat[3][0],
                                    mat[0][1], mat[1][1], mat[2][1], mat[3][1],
                                    mat[0][2], mat[1][2], mat[2][2], mat[3][2],
                                    mat[0][3], mat[1][3], mat[2][3], mat[3][3]) * topMatrix;
    _matrixStack.pop();
    _matrixStack.push(topMatrix);
}


vector<Ogre::Vector3>& Extrusion::getVertices(int meshID){
	return _vertices[meshID];
}


vector<Ogre::Vector3>& Extrusion::getFaces(int meshID){
	return _faces[meshID];
}


vector<Ogre::Vector3>& Extrusion::getNormals(int meshID){
	return _normals[meshID];
}


vector<Ogre::Vector2>& Extrusion::getTCoordinates(int meshID){
	return _tcoordinates[meshID];
}


unsigned int Extrusion::getNumberOfVertices(int meshID){
	return _nbVertices[meshID];
}


void Extrusion::setJoinStyleOgre(int flag)
{
    gleSetJoinStyleOgre(flag);
}


int Extrusion::getJoinStyleOgre()
{
    return gleGetJoinStyleOgre();
}


void Extrusion::setTextureStyleOgre(int flag)
{
    gleTextureModeOgre(GLE_TEXTURE_ENABLE|flag);
}
