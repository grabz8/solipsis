//#include <iostream>
//
//#include <GetPot>
//
//#include "Config.h"
//#include "GeomBuilder.h"
//#include "LSystem.h"
//#include "Utils.h"
//
//int main( int argc, char** argv ) 
//{	
//	GetPot cl(argc, argv);
//
//	if( argc < 2 || cl.search("-h" ) ) {
//		pm_utils::notify( pm_utils::notifyLevel() ) << "Usage : " << argv[0] << " <XML config file>" << std::endl;
//	    exit(0);
//	}
//
//    std::string xmlfilename = cl.nominus_vector()[0];
//	// build the instance for the XML Config manager
//	pm_utils::notify( pm_utils::notifyLevel() ) << "main -> INITIALIZING FROM CONFIG FILE: " <<  xmlfilename << std::endl;
//    pm_trax::Config::instance( xmlfilename );
//
//	// build the instance for the GeomBuilder
//	pm_trax::GeomBuilder::instance();
//
//	// create the LSystem and initialize it
//	pm_trax::LSystem lsystem;	
//	lsystem.init();	
//	
//	// set the notification level from Config
//	pm_trax::setNotifyLevel( pm_trax::Config::instance()->notifyLevel() );
//
//	pm_utils::notify( pm_utils::notifyLevel() ) << "main -> ITERATING..." << std::flush; 
//	pm_utils::notify( pm_utils::PM_DEBUG ) << "main -> LSYSTEM AXIOM: " << lsystem.asString( lsystem.axiom() ) << std::endl; 
//	// make the LSystem iterate 'depth' times
//	lsystem.iterate( pm_trax::Config::instance()->depth() );
//	pm_utils::notify( pm_utils::notifyLevel() ) << " DONE." << std::endl; 
//	pm_utils::notify( pm_utils::PM_DEBUG ) << "main -> LSYSTEM STATE #" << lsystem.states().size()-1 << ": " << lsystem.asString( lsystem.state( lsystem.states().size()-1 ) ) << std::endl;
//
//	// write geometries to datasource
//	pm_utils::notify( pm_utils::notifyLevel() ) << "main -> WROTING TRANSPORT AXES TO DATASOURCE: " <<  pm_trax::Config::instance()->output() << std::endl;
//	pm_trax::GeomBuilder::instance()->write( pm_trax::Config::instance()->output() );
//	
//	return 0;
//}
