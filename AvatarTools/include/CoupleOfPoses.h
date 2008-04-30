#ifndef __CoupleOfPoses_h__
#define __CoupleOfPoses_h__

#include <Ogre.h>

/// brief This is the class representing a couple of poses. The two poses of a the couple represents two opposites states. For example the first pose could be a thin noze and the second pose a big noze. The CoupleOfPoses class contains a position between these two states (a number between 0 and 1). If the position is 0, the first state, pose (or the left one because it is one the left on the screen) has a an influence 0 and the second state has an influence of 1 (see ogre poses documentations for more informations).
/// file CoupleOfPoses.h
/// author François FOURNEL
/// date 2007.06.25

using namespace Ogre;
namespace Solipsis {

class BodyPart;

class CoupleOfPoses
{
public:
	///brief Constructor
	///param name Name of the CoupleOfPoses. It could be "Size" for example if the BodyPart to which the couple of poses is attached is named "Noze".
	///param leftStateName Name of the left state (associated to the left pose). For a couple of poses representing the larger of the noze, it could be a thin noze for example.
	///param leftPoseIndex Index of the left pose in the avatar's mesh.
	///param rightStateName Name of the right state (associated to the right pose). For a couple of poses representing the larger of the noze, it could be a thin noze for example.
	///param rightPoseIndex Index of the right pose in the avatar's mesh.
	///param owner BodyPart to which the couple of poses is attached.
	CoupleOfPoses(const String& name, const String& leftStateName, ushort leftPoseIndex, const String& rightStateName, ushort rightPoseIndex, BodyPart* owner);



	///brief Method which returns the BodyPart to which the couple of poses is attached.
	///return BodyPart to which the couple of poses is attached.
	BodyPart* getOwner();

	///brief Method which returns the name of the CoupleOfPoses. It could be "Size" for example if the BodyPart to which the couple of poses is attached is named "Noze".
	///return Name of the CoupleOfPoses. It could be "Size" for example if the BodyPart to which the couple of poses is attached is named "Noze"
	const String& getName();

	///brief Method which returns the position of the couple of poses. It's the the influence of the right pose. So the influence of the left pose is 1-mPosition.
	///return Position of the couple of poses. It's the the influence of the right pose. So the influence of the left pose is 1-mPosition.
	float getPosition();

	///brief Method which returns the name of the left state (associated to the left pose). For a couple of poses representing the larger of the noze, it could be a thin noze for example.
	///return Name of the left state.
	const String& getLeftStateName();

	///brief Method which returns the index of the left pose in the avatar's mesh.
	///return Index of the left pose in the avatar's mesh.
	ushort getLeftPoseIndex();

	///brief Method which returns the ImageSet containing the CEGUI picture for the left pose (It should be a "photo" of the avatar in the left state, so with a thin noze if we take our precedent example).
	///return ImageSet containing the CEGUI picture for the left pose (It should be a "photo" of the avatar in the left state, so with a thin noze if we take our precedent example).
//	CEGUI::Imageset* getLeftPoseImageSet();
	Image* getLeftPoseImageSet();

	///brief Method which returns the name of the right state (associated to the right pose). For a couple of poses representing the larger of the noze, it could be a large noze for example.
	///return Name of the right state.
	const String& getRightStateName();

	///brief Method which returns the index of the right pose in the avatar's mesh.
	///return Index of the right pose in the avatar's mesh.
	ushort getRightPoseIndex();

	///brief Method which returns the ImageSet containing the CEGUI picture for the right pose (It should be a "photo" of the avatar in the right state, so with a large noze if we take our precedent example).
	///return ImageSet containing the CEGUI picture for the left pose.
//	CEGUI::Imageset* getRightPoseImageSet();
	Image* getRightPoseImageSet();



	///brief Method which modify the position of the couple of states.
	///param position New position to set. Must be between 0 and 1.
	void setPosition(float position);
	
	///brief Method which modify the LeftPose ImageSet of the couple of states.
	///param imageSet New LeftPose ImageSet to set.
//	void setLeftPoseImageSet(CEGUI::Imageset* imageSet);
	void setLeftPoseImageSet(Image* imageSet);

	///brief Method which modify the RightPose ImageSet of the couple of states.
	///param imageSet New RightPose ImageSet to set.
//	void setRightPoseImageSet(CEGUI::Imageset* imageSet);
	void setRightPoseImageSet(Image* imageSet);

private:
	BodyPart* mOwner;						///brief BodyPart to which the couple of poses is attached.

	String mName;							///brief Name of the CoupleOfPoses. It could be "Size" for example if the BodyPart to which the couple of poses is attached is named "Noze"

	float mPosition;						///brief Position of the couple of poses. It's the the influence of the right pose. So the influence of the left pose is 1-mPosition. It is between 0 and 1.

	String mLeftStateName;					///brief Name of the left state (associated to the left pose). For a couple of poses representing the larger of the noze, it could be a thin noze for example.
	ushort mLeftPoseIndex;					///brief Index of the left pose in the avatar's mesh.
//	CEGUI::Imageset* mLeftPoseImageSet;		///brief ImageSet containing the CEGUI picture for the left pose (It should be a "photo" of the avatar in the left state, so with a thin noze if we take our precedent example).
	Image* mLeftPoseImageSet;				///brief ImageSet containing the CEGUI picture for the left pose (It should be a "photo" of the avatar in the left state, so with a thin noze if we take our precedent example).

	String mRightStateName;					///brief Name of the right state (associated to the right pose). For a couple of poses representing the larger of the noze, it could be a large noze for example.
	ushort mRightPoseIndex;					///brief Index of the right pose in the avatar's mesh.
//	CEGUI::Imageset* mRightPoseImageSet;	///brief ImageSet containing the CEGUI picture for the right pose (It should be a "photo" of the avatar in the right state, so with a large noze if we take our precedent example).
	Image* mRightPoseImageSet;				///brief ImageSet containing the CEGUI picture for the right pose (It should be a "photo" of the avatar in the right state, so with a large noze if we take our precedent example).
};

}

#endif

