#include <ctime>
#include <iostream>
#include <map>

#include <GetPot>

#include <Ogre.h>

#include "BSpline/BSpline.h"
#include "Extruder/Extrusion.h"

#include "RoadNetwork.h"
#include "ComputeFactory.h"

using namespace Ogre;
using namespace roge;

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 
#define WIN32_LEAN_AND_MEAN 
#include "windows.h" 
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT ) {
#else 
int main( int argc, char** argv ) {
	GetPot commandLine(argc, argv);

	srand(time(NULL));

	if (argc < 2 || argc > 2 || commandLine.search(2, "-h", "--help")) {
		std::cout << "Usage : " << argv[0] << " <xml_config_file>" << std::endl;
		exit(0);
	}

	std::vector<std::string> files = commandLine.nominus_vector();
#endif 

	OGRRegisterAll();

	std::cout << "ROGE (ROad GEnerator) | Copyright 2009 by ARCHIVIDEO"	<< std::endl;

	std::string xmlFileName( "C:/src/workspace/Generation_automatique_de_contenu_3D/Modelisation_procedurale/Axes_de_transport/roge/Debug/rgConfig.xml" );

	Config::instance( xmlFileName );

	std::cout << "Creating the roads network..." << std::endl;
	std::cout << "Creating roadways..." << std::flush;
	RoadNetwork network;

	std::cout << "Creating roads intersections..." << std::flush;
	network.createIntersections();

	if (!network.verifyNetwork()) {
		std::cerr << "Error in network" << std::endl;
		return false;
	}

	// extrusion
	bool extrudeFlag, interFlag, bridgeFlag, contourFlag;
	if (!Config::instance()->getFlags(extrudeFlag, interFlag, bridgeFlag, contourFlag)) {
		std::cout << "Error in config file" << std::endl;
		return 0;
	}

	std::cout << "Computing the geometry of the roads network..." << std::endl;
	
	//create the factory for geometry computing
	ComputeFactory factory;
	
	//create the extruder
	Extrusion extruder;
	extruder.setMaterialsPerSubmeshesFlag(false);
	
	//Prepare a mesh pointer
	MeshPtr roadNetworkMesh;
	if (extrudeFlag) {
		std::cout << "Computing the geometry of roadways..." << std::flush;
		roadNetworkMesh = factory.extrude(network, extruder);
		std::cout << std::endl;
	}

	// intersections & contours
	MeshPtr intersections;
	std::vector< OGRLineString* > contours;
	if (extrudeFlag && interFlag) {
		std::cout << "Computing the geometry of intersections..." << std::flush;
		intersections = factory.createIntersections(roadNetworkMesh, network,
				contourFlag, contours );
		std::cout << std::endl;
		ostringstream ossInter, ossRoad;
		ossInter << intersections->getName() << ".mesh";
		MeshSerializer serializer;
		serializer.exportMesh(intersections.get(), ossInter.str().c_str());
		ossRoad << roadNetworkMesh->getName() << ".mesh";
		serializer.exportMesh(roadNetworkMesh.get(), ossRoad.str().c_str());
		
	}
	
	// exports
	if( extrudeFlag && contourFlag ) {
		
////		const char *pszDriverName = "ESRI Shapefile";
////	    OGRSFDriver *poDriver;
////
////	    poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName( pszDriverName );
////	    if( poDriver == NULL ) {
////	        printf( "%s driver not available.\n", pszDriverName );
////	        exit( 1 );
////	    }
////
////		OGRDataSource *poDS;
////		poDS = poDriver->CreateDataSource( Config::instance()->getOutputFile().c_str(), NULL );
////	    if( poDS == NULL ) {
////	        printf( "Creation of output file failed.\n" );
////	        exit( 1 );
////	    }
////
////    OGRLayer *poLayer;
////
////    poLayer = poDS->CreateLayer( "point_out", NULL, wkbPoint, NULL );
////    if( poLayer == NULL )
////    {
////        printf( "Layer creation failed.\n" );
////        exit( 1 );
////    }
////
////    OGRFieldDefn oField( "Name", OFTString );
////
////    oField.SetWidth(32);
////
////    if( poLayer->CreateField( &oField ) != OGRERR_NONE )
////    {
////        printf( "Creating Name field failed.\n" );
////        exit( 1 );
////    }
////
////    double x, y;
////    char szName[33];
////
////    while( !feof(stdin) 
////           && fscanf( stdin, "%lf,%lf,%32s", &x, &y, szName ) == 3 )
////    {
////        OGRFeature *poFeature;
////
////        poFeature = new OGRFeature( poLayer->GetLayerDefn() );
////        poFeature->SetField( "Name", szName );
////
////        OGRPoint pt;
////        
////        pt.setX( x );
////        pt.setY( y );
//// 
////        poFeature->SetGeometry( &pt ); 
////
////        if( poLayer->CreateFeature( poFeature ) != OGRERR_NONE )
////        {
////           printf( "Failed to create feature in shapefile.\n" );
////           exit( 1 );
////        }
////
////         OGRFeature::DestroyFeature( poFeature );
////    }
////
////    OGRDataSource::DestroyDataSource( poDS );
////}
////
//		QString dxfFile;
//		if (config.getOutputDXFFile(dxfFile)) {
//			std::cout << "\tThe DXF bounding contours to: "
//			<< dxfFile.toLatin1().data() << std::endl;
//			arFbStorageFactory fac;
//			arFbStorage *dxf = fac.create("DXF");
//			if (dxf == NULL) {
//				cerr << "Could not create DXF plugin" << endl;
//				return false;
//			}
//			dxf->setLocation(dxfFile);
//			dxf->put(contours);
//			cout << endl;
//		}
	}
	return 0;
}

#ifdef __cplusplus
}
#endif
