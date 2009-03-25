// Plugin_skp.cpp : Defines the exported functions for the DLL application.

#include "plugin_skp.h"
#include   <comutil.h>   
//#include   <stdio.h>   
#pragma   comment(lib,   "comsuppw.lib")   
#pragma   comment(lib,   "kernel32.lib") 
#include < fstream >

#include "CTIO.h"

using namespace CommonTools;

const String sPluginName = "Plugin_skp";

//------------------------------------------------------
Plugin_skp::Plugin_skp()
{
}

Plugin_skp::~Plugin_skp()
{
}

//------------------------------------------------------
const Ogre::String& Plugin_skp::getName() const
{
	return sPluginName;
}
//------------------------------------------------------
    
void Plugin_skp::install()
{
}
//------------------------------------------------------
void Plugin_skp::initialise()
{
}
//------------------------------------------------------
void Plugin_skp::shutdown()
{
}
//------------------------------------------------------
void Plugin_skp::uninstall()
{
}



static std::set<long> textureHandles;




void Plugin_skp::WriteTextureFiles(SketchUp::ISkpDocumentPtr pDoc,String szMeshPrefix, SketchUp::ISkpTextureWriter2Ptr m_pTextureWriter)
{
    Ogre::String sFullPrefix(szMeshPrefix);
	Ogre::String sTextureFloder;
	size_t lastSlash = sFullPrefix.find_last_of("\\");
	if (lastSlash != Ogre::String::npos)
		sTextureFloder = sFullPrefix.substr(0,lastSlash+1)+"texture";

	
	SketchUp::ISkpApplicationPtr pApp;
	pApp = pDoc->GetApplication();

	m_pTextureWriter = pApp->CreateTextureWriter();


	LoadTexturesFromEntities(pDoc,m_pTextureWriter);
    
	long textureCount;
	textureCount = m_pTextureWriter->GetCount();

    _bstr_t textDirBstr(sTextureFloder.c_str());

    if(!IO::FolderExist(sTextureFloder))
		CreateDirectoryW(textDirBstr,NULL);

	m_pTextureWriter->WriteAllTextures(textDirBstr,0);

}

void Plugin_skp::LoadTexturesFromEntities(SketchUp::ISkpEntityProviderPtr pEntProvider,SketchUp::ISkpTextureWriter2Ptr m_pTextureWriter)
{
    long nElements, i;

    //Load all the textures applied to component instances
    SketchUp::ISkpComponentInstancesPtr pInstances;
	pInstances = pEntProvider->GetComponentInstances();
	nElements = pInstances->GetCount();

    for(i=0; i<nElements; i++)
    {
        SketchUp::ISkpComponentInstancePtr pInstance;
		pInstance = pInstances->GetItem(i);

        SketchUp::ISkpEntityPtr pEnt;
        pEnt = pInstance;
        long entityID;
		entityID = pEnt->GetId();

        long handle;
        handle  = m_pTextureWriter->LoadComponentInstance(pInstance);
		if (handle > 0)
        {
			textureHandles.insert(handle);
        }

        SketchUp::ISkpComponentDefinitionPtr pDef;
		pDef = pInstance->GetComponentDefinition();

		LoadTexturesFromEntities(pDef,m_pTextureWriter);
    }

    //Load all the textures applied to groups
    SketchUp::ISkpGroupsPtr pGroups;
	pGroups = pEntProvider->GetGroups();
	nElements = pGroups->GetCount();

    for(i=0; i<nElements; i++)
    {
        SketchUp::ISkpGroupPtr pGroup;
		pGroup = pGroups->GetItem(i);

        long handle;
        handle = m_pTextureWriter->LoadGroup(pGroup);

		if (handle > 0)
        {
            textureHandles.insert(handle);
        }

		LoadTexturesFromEntities(pGroup,m_pTextureWriter);
    }

    //Load all the textures applied to images
    SketchUp::ISkpImagesPtr pImages;
	pImages = pEntProvider->GetImages();
	nElements = pImages->GetCount();

    for(i=0; i<nElements; i++)
    {
        SketchUp::ISkpImagePtr pImage;
		pImage = pImages->GetItem(i);

        long handle;
        handle = m_pTextureWriter->LoadImage(pImage);

		if (handle > 0)
        {
            textureHandles.insert(handle);
        }


        LoadTexturesFromEntities(pImage,m_pTextureWriter);
    }

    //Load all the textures applied to faces
    SketchUp::ISkpFacesPtr pFaces;
	pFaces = pEntProvider->GetFaces();
	nElements = pFaces->GetCount();

    for(i=0; i<nElements; i++)
    {
        SketchUp::ISkpFacePtr pFace;
		pFace = pFaces->GetItem(i);

        long handle;
        handle = m_pTextureWriter->LoadFace(pFace, true);
	
		if (handle > 0)
        {
            textureHandles.insert(handle);
        }


        handle=0;
        handle = m_pTextureWriter->LoadFace(pFace, false);

		if (handle > 0)
        {
            textureHandles.insert(handle);
        }

    }
}


void Plugin_skp::WriteMaterials(SketchUp::ISkpDocumentPtr pDoc,String szMeshPrefix)
{
	
	Ogre::MaterialManager* pMatMgr = Ogre::MaterialManager::getSingletonPtr();
	assert(pMatMgr != NULL);
	Ogre::MaterialSerializer matSer;
	Ogre::String sFullPrefix(szMeshPrefix);
	Ogre::String sMatPrefix;
	size_t lastSlash = sFullPrefix.find_last_of("\\");
	if (lastSlash != Ogre::String::npos)
		sMatPrefix = sFullPrefix.substr(lastSlash+1)+"/";


	SketchUp::ISkpMaterialsPtr pMats;
	pMats = pDoc->GetMaterials();
    long count;
	count = pMats->GetCount();
    
	for(long i=0; i<count; i++)
    {
        SketchUp::ISkpMaterialPtr pMat;
		pMat = pMats->GetItem(i);

		pMatMgr->unload(String(pMat->Name));
		pMatMgr->remove(String(pMat->Name));

        Ogre::MaterialPtr ogremat = pMatMgr->create(String(pMat->Name), 
            Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);


		if(pMat->IsColor)
		{
			COLORREF skpcolor = (COLORREF)(pMat->Color);
			
			float r = float(GetRValue(skpcolor))/255.0;
			float g = float(GetGValue(skpcolor))/255.0;
			float b = float(GetBValue(skpcolor))/255.0;

			Ogre::ColourValue color;

			if(pMat->UsesAlpha)
				color = Ogre::ColourValue(r,g,b,pMat->Alpha);
			else
				color = Ogre::ColourValue(r,g,b);

			ogremat->setAmbient(color);
			ogremat->setDiffuse(color);

		}

		ogremat->setCullingMode(Ogre::CULL_NONE);
		
		if(pMat->IsTexture)
		{
			Ogre::TextureUnitState *tu;
			Ogre::String textureSkpName(String(pMat->GetTexture()->Filename));
			Ogre::String textureName;
			size_t lastSlash = textureSkpName.find_last_of("\\");
			if(lastSlash != Ogre::String::npos)
				textureName = textureSkpName.substr(lastSlash+1);
			else
			{
				lastSlash = textureSkpName.find_last_of("/");
				if(lastSlash != Ogre::String::npos)
					textureName = textureSkpName.substr(lastSlash+1);
				else
					textureName = textureSkpName;
			}

			tu = ogremat->getTechnique(0)->getPass(0)->createTextureUnitState(textureName);
		}
		
		matSer.queueForExport(ogremat);
        
    }
	matSer.exportQueued(Ogre::String(szMeshPrefix)+".material");

}

void Plugin_skp::createSubMesh(Ogre::MeshPtr ogreMesh, 
							  SketchUp::ISkpEntityProviderPtr pEntProvider,
							  CTransform mTransform,
							  string sMaterialName,
							  SketchUp::ISkpTextureWriter2Ptr m_pTextureWriter,
							  Ogre::Vector3 &min,
							  Ogre::Vector3 &max,
							  Ogre::Real &maxSquaredRadius)
{
	long i = 0;
	long j = 0;

	SketchUp::ISkpFacesPtr pFaces = pEntProvider->GetFaces();
	SketchUp::ISkpVerticesPtr pVerts = pEntProvider->GetVertices();
	long VerticesCount = pVerts->GetCount();
	long FacesCount = pFaces->GetCount();
	long nElements = 0;


	if(FacesCount)
	{


		std::vector<string> MaterialNames;
		MaterialNames.push_back(sMaterialName);
		std::vector<std::vector<SketchUp::ISkpFacePtr>> FaceGroups;
		SketchUp::ISkpFacePtr pFace;
		
		for(j = 0; j< FacesCount; j++)
		{
			bool bMaterialExist = false;
			pFace = pFaces->GetItem(j);
			if(pFace->GetFrontMaterial())
			{
				for(unsigned long k = 0;k<MaterialNames.size();k++)
					bMaterialExist = bMaterialExist||(MaterialNames[k]==String(pFace->GetFrontMaterial()->GetName()));
				if(!bMaterialExist)
					MaterialNames.push_back(String(pFace->GetFrontMaterial()->GetName()));
			}
				
			else if(pFace->GetBackMaterial())
			{
				for(unsigned long k = 0;k<MaterialNames.size();k++)
					bMaterialExist = bMaterialExist||(MaterialNames[k]==String(pFace->GetBackMaterial()->GetName()));
				if(!bMaterialExist)
					MaterialNames.push_back(String(pFace->GetBackMaterial()->GetName()));
			}
				
		}
		FaceGroups.resize(MaterialNames.size());
		for(unsigned long k = 0; k<MaterialNames.size();k++)
		{
			for(j = 0; j< FacesCount; j++)
			{
				pFace = pFaces->GetItem(j);
				if(pFace->GetFrontMaterial())
				{
					if(String(pFace->GetFrontMaterial()->GetName())==MaterialNames[k])
						FaceGroups[k].push_back(pFace);
				}
				else if(pFace->GetBackMaterial())
				{
					if(String(pFace->GetBackMaterial()->GetName())==MaterialNames[k])
						FaceGroups[k].push_back(pFace);
				}
				else
						FaceGroups[0].push_back(pFace);
			}
		}
		

		Ogre::ManualObject mo("");

		
		for(unsigned long k = 0;k<FaceGroups.size();k++)
		{
			long offset  = 0;
			mo.begin(MaterialNames[k]);

			//For the complex face with one or more holes in it
			//we tesselate it into triangles using the polygon mesh class,
			//then export each triangle as a face.
			SketchUp::ISkpPolygonMeshPtr pMesh;

			for(j = 0; j< FaceGroups[k].size(); j++)
			{
				SketchUp::ISkpFacePtr pFace;
				pFace = FaceGroups[k][j];
				double xScale = 0.0, yScale = 0.0;
				if(pFace->GetFrontMaterial())
				{
					if(pFace->GetFrontMaterial()->GetIsTexture())
					{
					xScale = pFace->GetFrontMaterial()->GetTexture()->GetXScale();
					yScale = pFace->GetFrontMaterial()->GetTexture()->GetXScale();
					}
				}
				else if(pFace->GetBackMaterial())
				{
					if(pFace->GetBackMaterial()->GetIsTexture())
					{
					xScale = pFace->GetBackMaterial()->GetTexture()->GetXScale();
					yScale = pFace->GetBackMaterial()->GetTexture()->GetXScale();
					}
				}

				SketchUp::ISkpUVHelperPtr pUVHelper = NULL;

				//If the face has a texture(s) applied to it, then create a UVHelper class so we can output the uv
				//coordinance at each vertex.
				SketchUp::ISkpCorrectPerspectivePtr pCorrectPerspective = m_pTextureWriter;

				pUVHelper = pFace->GetUVHelper(true, true, pCorrectPerspective);

				pMesh = pFace->CreateMeshWithUVHelper(3, pUVHelper);
				long nPolys = pMesh->GetNumPolygons();
				long nPoint = pMesh->GetNumPoints();
				std::vector<Ogre::Vector3> vectices;
				std::vector<long> vecticesindex;
				std::vector<Ogre::Vector3> points;


				for(i=0;i<nPoint;i++)
				{
					SketchUp::ISkpPoint3dPtr skpPoint;
					SketchUp::ISkpVector3dPtr skpNormal;
					skpPoint = pMesh->GetPoint(i+1);
					skpNormal = pMesh->GetVertexNormal(i+1);

					CPoint3d point(skpPoint);
					CPoint3d worldPoint = mTransform * point;
					Ogre::Vector3 vectex = Ogre::Vector3(worldPoint.X(),worldPoint.Z(),-worldPoint.Y());
					mo.position(vectex);

					double u, v, q;
					pUVHelper->GetFrontUVQ(worldPoint.X(), worldPoint.Y(), worldPoint.Z(), &u, &v, &q);
					
					mo.textureCoord(u, -v);

				}


				for (i=0;i<nPolys;i++)
				{
					long nPolyPoints;

					//The mesh is 1 based
					nPolyPoints = pMesh->CountPolygonPoints(i+1);

					mo.triangle(abs(pMesh->GetPolygonPointIndex(i+1,1))-1+offset,
						abs(pMesh->GetPolygonPointIndex(i+1,2))-1+offset,
						abs(pMesh->GetPolygonPointIndex(i+1,3))-1+offset);
				}

				offset += nPoint;

			}
			mo.end();
		}

		

		if (!mo.getNumSections())
		{
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
				"No data defined to convert to a mesh.",
				"ManualObject::convertToMesh");
		}
		for ( i = 0; i< mo.getNumSections(); ++i)
		{
			Ogre::ManualObject::ManualObjectSection* sec =  mo.getSection(i);
			if (!sec->getRenderOperation()->useIndexes)
			{
				OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS,
					"Only indexed geometry may be converted to a mesh.",
					"ManualObject::convertToMesh");
			}
		}


		for ( i = 0; i< mo.getNumSections(); ++i)
		{
			Ogre::ManualObject::ManualObjectSection* sec = mo.getSection(i);
			Ogre::RenderOperation* rop = sec->getRenderOperation();
			Ogre::SubMesh* sm = ogreMesh->createSubMesh();
			sm->useSharedVertices = false;
			sm->operationType = rop->operationType;
			sm->setMaterialName(sec->getMaterialName());
			// Copy vertex data; replicate buffers too
			sm->vertexData = rop->vertexData->clone(true);
			// Copy index data; replicate buffers too; delete the default, old one to avoid memory leaks
			delete sm->indexData;
			sm->indexData = rop->indexData->clone(true);

		}
		
		
		//	 update bounds

		if(min==Ogre::Vector3(0,0,0))
			min = mo.getBoundingBox().getMinimum();
		if(max==Ogre::Vector3(0,0,0))
			max = mo.getBoundingBox().getMaximum();
		if(maxSquaredRadius==0.0)
			maxSquaredRadius = mo.getBoundingRadius();
		min.makeFloor(mo.getBoundingBox().getMinimum());
		max.makeCeil(mo.getBoundingBox().getMaximum());
		ogreMesh->_setBounds(Ogre::AxisAlignedBox(min, max), false);
		ogreMesh->_setBoundingSphereRadius(maxSquaredRadius);
	}

	//Recurse all the instances
	SketchUp::ISkpComponentInstancesPtr pInstances = pEntProvider->GetComponentInstances();
	nElements = pInstances->GetCount();

    for(i=0; i<nElements; i++)
    {
		SketchUp::ISkpComponentInstancePtr pInstance = pInstances->GetItem(i);

		SketchUp::ISkpDrawingElementPtr pDrawElement = pInstance;
		if(pDrawElement->HasMaterial)
			sMaterialName = pDrawElement->GetMaterial()->GetName();
		else
			sMaterialName = "";
		
		double m[16];
		pInstance->GetTransform()->GetData(m);

		CTransform enTransform;
		enTransform.SetMatrix(m);
		CTransform inTransform;
		inTransform = mTransform * enTransform;
		
		SketchUp::ISkpComponentDefinitionPtr pDef = pInstance->GetComponentDefinition();

		
		createSubMesh(ogreMesh, pDef,inTransform,sMaterialName,m_pTextureWriter, min, max, maxSquaredRadius);
    }

    //Recurse all the groups
    SketchUp::ISkpGroupsPtr pGroups = pEntProvider->GetGroups();
    nElements = pGroups->GetCount();

    for(i=0; i<nElements; i++)
    {
		SketchUp::ISkpGroupPtr pGroup = pGroups->GetItem(i);

		SketchUp::ISkpDrawingElementPtr pDrawElement = pGroup;
		if(pDrawElement->HasMaterial)
			sMaterialName = pDrawElement->GetMaterial()->GetName();
		//else
		//	sMaterialName = "";

		double m[16];
		pGroup->GetTransform()->GetData(m);

		CTransform enTransform;
		enTransform.SetMatrix(m);
		CTransform gTransform;
		gTransform = mTransform * enTransform;

        createSubMesh(ogreMesh, pGroup,gTransform,sMaterialName,m_pTextureWriter, min, max, maxSquaredRadius);
    }

    //Recurse all the images
    SketchUp::ISkpImagesPtr pImages = pEntProvider->GetImages();
    nElements = pImages->GetCount();

    for(i=0; i<nElements; i++)
    {
        SketchUp::ISkpImagePtr pImage = pImages->GetItem(i);
		SketchUp::ISkpDrawingElementPtr pDrawElement = pImage;
		if(pDrawElement->HasMaterial)
			sMaterialName = pDrawElement->GetMaterial()->GetName();
		else
			sMaterialName = "";

		double m[16];
		pImage->GetTransform()->GetData(m);

		CTransform enTransform;
		enTransform.SetMatrix(m);
		CTransform imTransform;
		imTransform = mTransform * enTransform;
        
		createSubMesh(ogreMesh, pImage,imTransform,sMaterialName,m_pTextureWriter, min, max, maxSquaredRadius);
    }
}
//-------------------------------------------------------------------------------------
bool Plugin_skp::convertskpToMesh(const Ogre::String & szskpFileName,
								 const Ogre::String & szMeshPrefix)
{
	Ogre::MeshManager* pMeshMgr = Ogre::MeshManager::getSingletonPtr();
	assert(pMeshMgr != NULL);

	pMeshMgr->unload(szMeshPrefix);
	pMeshMgr->remove(szMeshPrefix);
	Ogre::MeshPtr ogreMesh = pMeshMgr->createManual(Ogre::String(szMeshPrefix),
		Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    static HMODULE hModuleSketchUp = NULL;
    static SketchUp::ISkpApplicationPtr pApp = NULL;
    typedef HRESULT (*SkpAppFunc)(SketchUp::ISkpApplication** ppApplication) ;
    static SkpAppFunc lpAppFunc = NULL; 
    if (hModuleSketchUp == NULL)
    {
        hModuleSketchUp = LoadLibraryEx("SketchUpReader.dll", NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
        if(hModuleSketchUp == NULL)
        {
            return NULL;
        }

        lpAppFunc = (SkpAppFunc)GetProcAddress(hModuleSketchUp, "GetSketchUpSkpApplication");
        assert(lpAppFunc != NULL);
        if(lpAppFunc != NULL)
        {
            (*lpAppFunc)(&pApp);

            assert(pApp != NULL);
        }
        else
        {
            return NULL;
        }
    }
    if(lpAppFunc == NULL)
        return NULL;


	SketchUp::ISkpFileReaderPtr pFileReader = pApp;

    SketchUp::ISkpDocumentPtr pDoc = pFileReader->OpenFile(_bstr_t(szskpFileName.c_str()));

	SketchUp::ISkpTextureWriter2Ptr m_pTextureWriter;

	//---------------------------------------------------------------------------------------
	WriteTextureFiles(pDoc,szMeshPrefix,m_pTextureWriter);

	WriteMaterials(pDoc,szMeshPrefix);
	
	CTransform mTransform;
	mTransform.SetToIdenity();
	

	String sMaterialName = "";
	Ogre::Vector3 min = Ogre::Vector3(0,0,0);
	Ogre::Vector3 max = Ogre::Vector3(0,0,0);
    Ogre::Real maxSquaredRadius = 0;
	createSubMesh(ogreMesh,pDoc,mTransform,sMaterialName,m_pTextureWriter, min, max, maxSquaredRadius);

	//Write the mesh file
	Ogre::MeshSerializer meshSer;
	meshSer.exportMesh(ogreMesh.getPointer(), Ogre::String(szMeshPrefix)+".mesh");
	Ogre::MeshManager::getSingleton().remove(ogreMesh->getHandle());


	return true;
}

//-------------------------------------------------------------------------------------------------
Ogre::Entity* Plugin_skp::createEntityFromskp(const Ogre::String & entityName, 
											 const Ogre::String & szskpFileName,
									 		 Ogre::SceneManager* mSceneMgr)
{
	
	Ogre::String szMeshPrefix = szskpFileName.substr(0,szskpFileName.length()-4);

	size_t lastSlash = szskpFileName.find_last_of("\\");
	Ogre::String mResourceFloder = szskpFileName.substr(0,lastSlash+1);

	
	convertskpToMesh(szskpFileName,szMeshPrefix);

	// add the ressources location
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation(mResourceFloder,"FileSystem");
	if(IO::FolderExist( mResourceFloder+"texture"))
					ResourceGroupManager::getSingleton().addResourceLocation( mResourceFloder+"texture", "FileSystem");
	if(IO::FolderExist( mResourceFloder+"textures"))
					ResourceGroupManager::getSingleton().addResourceLocation( mResourceFloder+"textures", "FileSystem");

	lastSlash = szMeshPrefix.find_last_of("\\");
	Ogre::String mMeshFileName = szMeshPrefix.substr(lastSlash+1);

	Ogre::Entity* ent;
	ent = mSceneMgr->createEntity(entityName,mMeshFileName+".mesh");

	DeleteFileA((szMeshPrefix+".mesh").c_str());

	DeleteFileA((szMeshPrefix+".material").c_str());

	
	return ent;

}