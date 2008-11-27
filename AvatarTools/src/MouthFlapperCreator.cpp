#include "MouthFlapperCreator.h"
#include "MouthFlapper.h"

using namespace Solipsis;

MouthFlapperCreator::MouthFlapperCreator( const std::string & jawBoneName, const Ogre::Quaternion & closedJawOrientation, const Ogre::Quaternion & openJawOrientation )
	: mJawBoneName( jawBoneName )
	, mClosedJawOrientation( closedJawOrientation )
	, mOpenJawOrientation( openJawOrientation )
{

}

MouthFlapperCreator::~MouthFlapperCreator()
{
}

IFaceController* MouthFlapperCreator::createFaceController( CharacterInstance* pFaceControllerOwner )
{
	return new MouthFlapper( pFaceControllerOwner, this );
}
