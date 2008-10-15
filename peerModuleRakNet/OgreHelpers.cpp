/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author JAN Gregory

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

#include "OgreHelpers.h"
#include <CTSystem.h>
#include <CTStringHelpers.h>

using namespace Ogre;

namespace Solipsis {

OgreHelpers* OgreHelpers::mSingleton = 0;

//-------------------------------------------------------------------------------------
bool OgreHelpers::_initialize()
{
    mRoot = Ogre::Root::getSingletonPtr();
    mRootAllocated = false;
    if (mRoot != 0)
    {
        // Ogre Root instance already allocated so we are embedded in the Navigator application
        // we will just create the MeshSerializer, the HardwareBufferManager was created by
        // the loaded RenderSystem plugin
        mMeshSerializer = new MeshSerializer();
        return true;
    }

    // Here we are in standalone config, so we have to create the minimal Ogre instances
    // the MeshSerializer and the DefaultHardwareBufferManager to deal with vertices/indices
    mRoot = new Root("plugins.cfg", "ogre.cfg", "Ogre_" + CommonTools::StringHelpers::toString(CommonTools::System::getPID()) + "_"  + CommonTools::System::getDateTimeYYYYMMDDHHMMSS() + ".log");
    if (mRoot == 0)
        return false;
    mRootAllocated = true;
    mMeshSerializer = new MeshSerializer();
    mHWBufferManager = new DefaultHardwareBufferManager(); // needed because we don't have a rendersystem

    addResourceLocations();

    return true;
}

//-------------------------------------------------------------------------------------
void OgreHelpers::_shutdown()
{
    delete mMeshSerializer;

    if (mRootAllocated)
    {
//        removeResourceLocations();
//        ResourceGroupManager::getSingleton().destroyResourceGroup("General");
        delete mRoot;
    }

    delete mHWBufferManager;
}

//-------------------------------------------------------------------------------------
bool OgreHelpers::initialize()
{
    assert(mSingleton == 0);
    mSingleton = new OgreHelpers();
    return mSingleton->_initialize();
}

//-------------------------------------------------------------------------------------
void OgreHelpers::shutdown()
{
    assert(mSingleton != 0);
    mSingleton->_shutdown();
}

//-------------------------------------------------------------------------------------
Mesh* OgreHelpers::loadMesh(const String& filename)
{
	DataStreamPtr dataStream = ResourceGroupManager::getSingleton().openResource(filename);
	if (!dataStream->size())
        return 0;

    Mesh* mesh = new Mesh(Root::getSingleton().getMeshManager(), filename, 0, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    DataStreamPtr stream(dataStream);
    mMeshSerializer->importMesh(stream, mesh);

    return mesh;
}

//-------------------------------------------------------------------------------------
void OgreHelpers::getMeshInformation(Mesh* mesh,
                                     size_t &vertex_count,
                                     Vector3*& vertices,
                                     Vector2*& texCoords,
                                     size_t &index_count,
                                     unsigned long*& indices,
                                     const Vector3& position,
                                     const Quaternion& orientation,
                                     const Vector3& scale)
{
    bool added_shared = false;
    size_t current_offset = 0;
    size_t shared_offset = 0;
    size_t next_offset = 0;
    size_t index_offset = 0;

    vertex_count = index_count = 0;

    // Calculate how many vertices and indices we're going to need
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        SubMesh* submesh = mesh->getSubMesh(i);

        // We only need to add the shared vertices once
        if (submesh->useSharedVertices)
        {
            if (!added_shared)
            {
                vertex_count += mesh->sharedVertexData->vertexCount;
                added_shared = true;
            }
        }
        else
        {
            vertex_count += submesh->vertexData->vertexCount;
        }

        // Add the indices
        index_count += submesh->indexData->indexCount;
    }


    // Allocate space for the vertices, texture coordinates and indices
    vertices = new Vector3[vertex_count];
    texCoords = new Vector2[vertex_count];
    indices = new unsigned long[index_count];

    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for (unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
    {
        SubMesh* submesh = mesh->getSubMesh(i);

        VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

        if ((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared))
        {
            if (submesh->useSharedVertices)
            {
                added_shared = true;
                shared_offset = current_offset;
            }

            const VertexElement* posElem = vertex_data->vertexDeclaration->findElementBySemantic(VES_POSITION);
            const VertexElement* texCoordsElem = vertex_data->vertexDeclaration->findElementBySemantic(VES_TEXTURE_COORDINATES);
            HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());
            unsigned char* vertex = static_cast<unsigned char*>(vbuf->lock(HardwareBuffer::HBL_READ_ONLY));

            // There is _no_ baseVertexPointerToElement() which takes an Real or a double
            //  as second argument. So make it float, to avoid trouble when Real will
            //  be comiled/typedefed as double:
            //      Real* pReal;
            float* pReal;
            for (size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
            {
                posElem->baseVertexPointerToElement(vertex, &pReal);
                Vector3 pt(pReal[0], pReal[1], pReal[2]);
                vertices[current_offset + j] = (orientation*(pt*scale)) + position;
                if (texCoordsElem != NULL)
                {
                    texCoordsElem->baseVertexPointerToElement(vertex, &pReal);
                    Vector2 tc(pReal[0], pReal[1]);
                    texCoords[current_offset + j] = tc;
                }
                else
                    texCoords[current_offset + j] = Vector2::ZERO;
            }

            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
        }


        IndexData* index_data = submesh->indexData;
        size_t numTris = index_data->indexCount/3;
        HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

        bool use32bitindexes = (ibuf->getType() == HardwareIndexBuffer::IT_32BIT);

        unsigned long*  pLong = static_cast<unsigned long*>(ibuf->lock(HardwareBuffer::HBL_READ_ONLY));
        unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);

        size_t offset = (submesh->useSharedVertices) ? shared_offset : current_offset;
        if ( use32bitindexes )
        {
            for (size_t k = 0; k < numTris*3; ++k)
                indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
        }
        else
        {
            for (size_t k = 0; k < numTris*3; ++k)
                indices[index_offset++] = static_cast<unsigned long>(pShort[k]) + static_cast<unsigned long>(offset);
        }

        ibuf->unlock();
        current_offset = next_offset;
    }
}

//-------------------------------------------------------------------------------------
void OgreHelpers::addResourceLocations()
{
    // Load resource paths from config file
    ConfigFile cf;
    cf.load("resources.cfg");

    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
}

//-------------------------------------------------------------------------------------
void OgreHelpers::removeResourceLocations()
{
    // Load resource paths from config file
    ConfigFile cf;
    cf.load("resources.cfg");

    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            ResourceGroupManager::getSingleton().removeResourceLocation(
                archName, secName);
        }
    }
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
