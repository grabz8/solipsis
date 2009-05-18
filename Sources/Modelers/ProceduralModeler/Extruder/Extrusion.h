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

#ifndef EXTRUSION_H
#define EXTRUSION_H

/*!
 * \file Extrusion.h
 * \brief Header file describing the OgreExtrusion class.
 * 
 * \author Jonathan Merlet
 * \date 25 juin 2008 
 * 
 */

#include <vector>
extern "C" {
	#include "gle.h"
}
#include <Ogre.h>
#include <exception>

#include <OgreDefaultHardwareBufferManager.h>

#include <iostream>
#include <stack>
#include <map>
#include <string>

class texInfo;


/*!
 * 
 * \class ExtruderException
 * \brief Class to handle exception thrown by Extrusion.
 * 
 * This exception can be thrown for different reasons :
 * - choice of an ID to extrude a submesh that is already taken
 * - choice of an ID to create a mesh that is already taken
 * - trying to create a submesh which geometry hasn't been extruded
 * - trying to export a mesh which hasn't been created
 *
 */

class ExtruderException : public std::exception{
	public :
		ExtruderException(const char* msg) : _msg(msg){}

		virtual const char* what() const throw(){
			return _msg;
		}

	protected :
		const char* _msg;	
};


/*!
 * 
 * \class Extrusion
 * \brief Class used to generate .mesh files based on geometry 
		  given by an adaptation of the gle library.
 * 
 * The Extrusion class is compouned of two main parts :
 * - an adaptation of the gle library, used to extrude geometry 
 * (vertices, faces, normals and texture coordinates) from informations passed to the class, 
 * like an extrusion path, a contour...see extrudeGeometry for more details.
 * - a mesh creator, using Ogre managers (for hardware buffers, material and meshes) 
 * and previously extruded geometry to generate .mesh files for the Ogre engine.
 * 
 */		  

class Extrusion {
    public:
    	
    	/*!
    	 * 
    	 * \brief The constructor.
    	 * 
    	 * The constructor handles the creation of the Ogre root object, 
    	 * the restoration of choice of the display configuration, 
    	 * the initialisation of the root object and the initialisation 
    	 * of the hardware buffer manager.
    	 * 
    	 */

        Extrusion(); 

        
        /*!
         * 
         * \brief The destructor.
         * 
         * The destructor simply handles the destruction of Ogre root object.
         * 
         */
        
        ~Extrusion();

        /*!
         * 
         *  \brief Given some parameters, use gle to generate geometry informations
         * 
         * This method will call a function in the modified gle library to extrude a model,
         * with the extrusion path, the contour of the extrusion, the normals of this contour,
         * and the tranformations to apply on each piece of extrusion, passed as parameters.
         * The ID is here to give an easy way of saving extrusion informations using maps.
         * The name is here in case of multipleMaterials.
         * The method will first update the pointer on the active geometry data storage to have
         * a simple way to know where to store data when gle calls back the class with the data 
         * he calculated.
         * Then the texture style an join style gle should use are set.
         * Finally, we call the gle function with previously given parameters to let him
         * handle his extruding work.
         * A geometry can be considered as a submesh for an Ogre model.
         * 
         * \param geometryID The ID to use for this geometry.
         * \param geometryName The name to give to the geometry, in case of multiple materials.
         * \param ncp Number of contour points, i.e the length of contour and norm_contour.
         * \param contour Array of 2D points describing the contour we want to extrude, 
         * managed as a two dimensional array.
         * \param norm_contour Array of 2D points describing the normals of the contour 
         * we want to extrude, managed as a two dimensional array.
         * \param up_vector A 3D vector giving the up direction of the contour 
         * in 3 dimensional space, managed as a simple array.
         * \param npp The number of points in the extrusion path, 
         * i.e the length of path and xform.
         * \param path A serie of 3D points describing the extrusion path,
         * managed as a two dimensional array.
         * \param xform A serie of 2*3 matrices, used to apply transformations on the contour
         * at each point of the extrusion path. There must be one transformation per extrusion
         * path point. This parameter is managed as a three dimensional array.
         * 
         */
        
        void extrudeGeometry(int geometryID, const char* geometryName, int ncp, gleDouble** contour, gleDouble** norm_contour, gleDouble up_vector[3], int npp, gleDouble** path, gleDouble*** xform, texInfo* geometryTexInformations);
	
        
        /*!
         * 
         * \brief Creates a mesh with a list of submeshs' IDs given in parameters
         * 
         * Given a serie of submeshes' IDs, this method will handle the creation
         * using Ogre managers of one mesh.
         * 
         * \param subMeshIDs A simple array containing the IDs of the submeshes to use
         * to create the mesh.
         * \param nbOfSubMeshes The number of submeshes used to create the mesh,
         * i.e the length of subMeshIDs.
         * \param meshID An ID for the generated mesh.
         * \param meshName A name for the generated mesh.
         * 
         */
    
        
		Ogre::MeshPtr createMesh(int* subMeshIDs, int nbOfSubMeshes, int meshID, const char* materialName);

		/*!
		 * 
		 * \brief Export a given mesh in a .mesh file.
		 * 
		 * Using the MeshSerializer class of Ogre, this method will save the mesh
		 * which ID is given in parameter in a .mesh file.
		 * 
		 * \param meshID The ID of the mesh we want to export.
		 * 
		 */
	
		
		void exportMesh(int meshID);
		
		/*!
		 * 
		 * \brief Notification from gle that we are starting a new TriangleStrip.
		 * 
		 * As gle uses triangle strips to generate vertices, it must notify to the class
		 * (callback) when he starts a new one to correctly manage faces of the model.
		 * 
		 */
	
		
        void beginTriangleStripOgre();
		
		/*!
		 * 
		 * \brief Notification from gle that we are ending the current TriangleStrip.
		 * 
		 */

        
        void endTriangleStripOgre();
        
        /*!
         * 
         * \brief Add a vertex in the current vertex data storage.
         * 
         * This method adds a vertex given by gle in the currently selected data storage.
         * It also manage to add faces when it's needed, and apply transformation matrix
         * gle as notified at the start of this segment extrusion.
         * 
         * \param x The x coordinates of the vertex.
         * \param y The y coordinates of the vertex.
         * \param z The z coordinates of the vertex.
         * 
         */

        
        void addVertexOgre(double x, double y, double z);

        /*! 
         * 
         * \brief Add a normal in the current normal data storage.
         * 
         * This methods adds a normal given by gle in the curreently selected data storage.
         * It is associated with the vertex that will be added just after.
         * 
         * 
         * \param x The x coordinates of the normal.
         * \param y The y coordinates of the normal.
         * \param z The z coordinates of the normal.
         * 
         */
        
        void addNormalOgre(double x, double y, double z);

        
        /*!
         * 
         * \brief Add texture coordinates in the current t-coordinates data storage.
         * 
         * This texture coordinates are associated with the faces of the model,
         * and they are two dimensional coordinates.
         * 
         * \param u the x coordinates of the texture coordinates.
         * \param v the y coordinates of the texture coordinates.
         * 
         */
        
        void addTexCoordOgre(double u, double v);

        
        /*!
         * 
         * \brief Push an identity transformation matrix on the matrix stack.
         * 
         * As gle gives the coordinates of the vertex in a coordinate system local 
         * to the segment he is extruding, we must use a transformation matrix to
         * obtain the coordinates of the vertex in the entire model coordinates system.
         * So we push the identity matrix on the stack to let gle give us a 
         * multiplication matrix.
         * 
         */
        
        void pushMatrixOgre();

        
        /*!
         * 
         * \brief Pop the matrix on the top of the stack.
         * 
         */
        
        void popMatrixOgre();

        
        /*!
         * 
         * \brief Push a matrix on the top of the stack.
         * 
         * \param mat the 4*4 matrix to push.
         * 
         */
        
        void loadMatrixOgre(gleDouble mat[4][4]);

        
        /*!
         * 
         * \brief Multiply the top matrix of the stack  by a matrix given by gle.
         * 
         * \param mat The 4*4 matrix to be multiplied with the top matrix.
         * 
         */
        
        void multMatrixOgre(gleDouble mat[4][4]);

        
        /*!
         * 
         * \brief Tells gle what kind of join he should use.
         * 
         * This method tells gle what kind of join he should use for the extrusion, 
         * even if we are only using the angle style extrusion join.
         * 
         * \param flag The flag associated with the join style.
         * 
         */
        
        void setJoinStyleOgre(int flag);

        
        /*!
         * 
         * \brief Gets the current join style used by gle.
         * 
         * This method is pretty useless by now as we only use the angle join style,
         * but it is here in case of a will to reintegrate other join styles in
         * the library.
         * 
         */
        
        int getJoinStyleOgre();

        
        /*!
         * 
         * \brief Tells gle what kind of texture style he should use.
         * 
         * \param flag The flag associated with the texture style.
         * 
         */
        
        void setTextureStyleOgre(int flag);

        
        /*!
         * 
         * \brief Get the vertices data storage associated to a mesh ID.
         * 
         * \param meshID The ID of the mesh we want the vertices.
         * \return The vector of vertices.
         * 
         */
        
        std::vector<Ogre::Vector3>& getVertices(int meshID);

        
        /*!
         * 
         * \brief Get the faces data storage associated to a mesh ID.
         * 
         * \param meshID The ID of the mesh we want the faces.
         * \return The vector of faces.
         * 
         */
        
        std::vector<Ogre::Vector3>& getFaces(int meshID);
        
        
        /*!
         * 
         * \brief Get the normals data storage associated to a mesh ID.
         * 
         * \param meshID The ID of the mesh we want the normals.
         * \return The vector of normals.
         * 
         */

        std::vector<Ogre::Vector3>& getNormals(int meshID);
        
        
        /*!
         * 
         * \brief Get the texture coordinates data storage associated to a mesh ID.
         * 
         * \param meshID The ID of the mesh we want the texture coordinates.
         * \return The vector of texture coordinates.
         * 
         */

        std::vector<Ogre::Vector2>& getTCoordinates(int meshID);
        
        
        /*!
         * 
         * \brief Get the number of vertices associated to a mesh ID.
         * 
         * \param meshID The ID of the mesh we want the number of vertices.
         * \return The total number of vertices in the mesh.
         * 
         */

        unsigned int getNumberOfVertices(int meshID);

		
		/*!
		 * 
		 * \brief Set a flag to know if we are using one material per submesh or one per texture.
		 * 
		 * \param materialName The name of the material, used to retrieve in Ogre database.
		 * 
		 */
		
		void setMaterialsPerSubmeshesFlag(bool materialsPerSubmeshes);	
		
    	
    	/*!
    	 * 
    	 * \brief Create a default material associated to a texture.
    	 * 
    	 * This function is used to create a default material associated to a texture,
    	 * that will only contain backface culling information and the texture to use.
    	 * The name of the material will be based on the texture.
    	 * 
    	 * \param filename The filename of the texture.
    	 * 
    	 */ 
    	
		void createMaterial(std::string filename);


    protected:
		
    	
    	/*!
    	 * 
    	 * \brief Create a submesh for an Ogre model file.
    	 * 
    	 * In Ogre, meshes are compouned of submeshes. Here we are going to
    	 * create a submesh with the geometry that will be retrieved with the submeshID,
    	 * and use the vertices to update the axis aligned bounding box of the parent mesh
    	 * given in parameter. We will also associate a material name to this submesh,
    	 * that may have been defined by the parent mesh, or that will defined by the submesh
    	 * itself in case of multiple materials.
    	 * 
    	 * \param mesh An Ogre-pointer on the parent mesh
    	 * \param submeshID The id of the submesh we want to get the geometry.
    	 * \param aabox the bounding box of the parent mesh.
    	 * \param materialName The name of the material to associate to the mesh (may be NULL to let the submesh define it)
    	 * 
    	 */
    	
    	void createSubmesh(Ogre::MeshPtr& mesh, int submeshID, Ogre::AxisAlignedBox& aabox);

    	
    	/*!
    	 * 
    	 * \brief Create a default material for a submesh.
    	 * 
    	 * This function is used to create a default material for a submesh,
    	 * that will only contain backface culling information.
    	 * The name of the material will be based on the submesh.
    	 * 
    	 * \param ID The ID of the submesh that requested the material.
    	 * 
    	 */ 
    	
		void createMaterial(int ID);

		
		/*!
		 * 
		 * \brief Export a material to a file.
		 * 
		 * \param materialName The name of the material, used to retrieve in Ogre database.
		 * 
		 */
		
		void exportMaterial(const char* materialName);	

    	std::map<int, std::string> _meshNames; /*!< A map containing the names of the meshes, indexed by their ID. */
    	std::map<int, std::string> _geometryNames; /*!< A map containing the name of the geometry/submeshes, indexed by their ID. */
		std::map<int, std::vector<Ogre::Vector3> > _vertices; /*!< A map containing the vertices of the geometries, indexed by their ID. */
        std::map<int, std::vector<Ogre::Vector3> > _normals; /*!< A map containing the normals of the geometries, indexed by their ID. */
        std::map<int, std::vector<Ogre::Vector2> > _tcoordinates; /*!< A map containing the texture coordinates of the geometries, indexed by their ID. */
        std::map<int, std::vector<Ogre::Vector3> > _faces; /*!< A map containing the faces of the geometries, indexed by their ID. */
        std::map<int, unsigned int> _nbVertices; /*!< A map containing the number of vertices of the geometries, indexed by their ID. */
        std::map<int, float> _contourLength; /*!< A map containing the length of the contour of the geometries, indexed by their ID. */
        std::map<int, texInfo*> _texInformations; /*!< A map containing texture informations on the texturing of each geometry, indexed by their IDs. */
        std::map<std::string, Ogre::MaterialPtr> _materials;
        		        
        std::stack<Ogre::Matrix4> _matrixStack; /*!< A stack to keep track of the matrix sent by gle. */	
        
        bool _materialsPerSubmeshes;

        unsigned int _nbVerticesStrip; /*!< The number of vertices in the current triangle strip. */
		unsigned int _indexDebut; /*!< An index to know where a new triangle strip starts in the flow of the submesh vertices. */
        bool _triangleStrip; /*!< A boolean to know if we are drawing a triangle strip or a polygon. 
        						  It's useless as we don't draw the end cap of the mesh (which are defined using polygons)
        						  but we kept it in case of a will to extend. */
        bool _clockwise; /*!< A boolean used to know if the next triangle will be clockwise or counter clockwise.
        					  Due to the triangle strip, gle sends us vertices that will form once a clockwise triangle,
        					  and the next one will we counter clockwise, then clockwise, etc... */
		
        float _activeContourLength;
		int* _activeGeometry; /*!< A pointer to keep track of the ID of the geometry we are currently extruding. */
		unsigned int* _activeNbVertices; /*!< A pointer to keep track of the number of vertices of the geometry we are currently extruding. */
		texInfo* _activeTexInformations; /*!< A pointer to keep track of the texture informations of the geometry we are currently extruding. */
		std::vector<Ogre::Vector3>* _activeVertices; /*!< A pointer to keep track of where to store the vertices of the geometry we are currently extruding. */
		std::vector<Ogre::Vector3>* _activeNormals; /*!< A pointer to keep track of where to normal the vertices of the geometry we are currently extruding. */
		std::vector<Ogre::Vector2>* _activeTCoordinates; /*!< A pointer to keep track of where to store the texture coordinates of the geometry we are currently extruding. */
		std::vector<Ogre::Vector3>* _activeFaces; /*!< A pointer to keep track of where to store the faces of the geometry we are currently extruding. */
		
		Ogre::Root* _root; /*!< The Ogre root object we are using to gain access to Ogre MeshManager, MeshSerializer, and stuff. */
		Ogre::DefaultHardwareBufferManager* _hardwareBufferManager; /*!< A pointer to the ogre hardware buffer manager we have initialized in order to send the vertices to the sumeshes. */
};

#endif
