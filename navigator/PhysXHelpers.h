#ifndef __PhysXHelpers_h__
#define __PhysXHelpers_h__

#ifdef PHYSX

#include "Ogre.h"
#include "NxPhysics.h"
#include "NxStream.h"

class UserStream : public NxStream
	{
	public:
								UserStream(const char* filename, bool load);
	virtual						~UserStream();

	virtual		NxU8			readByte()								const;
	virtual		NxU16			readWord()								const;
	virtual		NxU32			readDword()								const;
	virtual		float			readFloat()								const;
	virtual		double			readDouble()							const;
	virtual		void			readBuffer(void* buffer, NxU32 size)	const;

	virtual		NxStream&		storeByte(NxU8 b);
	virtual		NxStream&		storeWord(NxU16 w);
	virtual		NxStream&		storeDword(NxU32 d);
	virtual		NxStream&		storeFloat(NxReal f);
	virtual		NxStream&		storeDouble(NxF64 f);
	virtual		NxStream&		storeBuffer(const void* buffer, NxU32 size);

				FILE*			fp;
	};

class MemoryWriteBuffer : public NxStream
	{
	public:
								MemoryWriteBuffer();
	virtual						~MemoryWriteBuffer();
				void			clear();

	virtual		NxU8			readByte()								const	{ NX_ASSERT(0);	return 0;	}
	virtual		NxU16			readWord()								const	{ NX_ASSERT(0);	return 0;	}
	virtual		NxU32			readDword()								const	{ NX_ASSERT(0);	return 0;	}
	virtual		float			readFloat()								const	{ NX_ASSERT(0);	return 0.0f;}
	virtual		double			readDouble()							const	{ NX_ASSERT(0);	return 0.0;	}
	virtual		void			readBuffer(void* buffer, NxU32 size)	const	{ NX_ASSERT(0);				}

	virtual		NxStream&		storeByte(NxU8 b);
	virtual		NxStream&		storeWord(NxU16 w);
	virtual		NxStream&		storeDword(NxU32 d);
	virtual		NxStream&		storeFloat(NxReal f);
	virtual		NxStream&		storeDouble(NxF64 f);
	virtual		NxStream&		storeBuffer(const void* buffer, NxU32 size);

				NxU32			currentSize;
				NxU32			maxSize;
				NxU8*			data;
	};

class MemoryReadBuffer : public NxStream
	{
	public:
								MemoryReadBuffer(const NxU8* data);
	virtual						~MemoryReadBuffer();

	virtual		NxU8			readByte()								const;
	virtual		NxU16			readWord()								const;
	virtual		NxU32			readDword()								const;
	virtual		float			readFloat()								const;
	virtual		double			readDouble()							const;
	virtual		void			readBuffer(void* buffer, NxU32 size)	const;

	virtual		NxStream&		storeByte(NxU8 b)							{ NX_ASSERT(0);	return *this;	}
	virtual		NxStream&		storeWord(NxU16 w)							{ NX_ASSERT(0);	return *this;	}
	virtual		NxStream&		storeDword(NxU32 d)							{ NX_ASSERT(0);	return *this;	}
	virtual		NxStream&		storeFloat(NxReal f)						{ NX_ASSERT(0);	return *this;	}
	virtual		NxStream&		storeDouble(NxF64 f)						{ NX_ASSERT(0);	return *this;	}
	virtual		NxStream&		storeBuffer(const void* buffer, NxU32 size)	{ NX_ASSERT(0);	return *this;	}

	mutable		const NxU8*		buffer;
	};

using namespace Ogre;

namespace Solipsis {

/** This static class contains several helper methods above Ageia PhysX physics engine.
 */
class PhysXHelpers
{
private:
    // No instanciation
    PhysXHelpers() {}

public:
    // Init
    static bool init();
    // Shutdown
    static bool shutdown();

    // Get/Set
    static NxPhysicsSDK* getPhysicsSDK() { return mPhysicsSDK; }

    // Cook 1 mesh into 1 NxTriangleMesh
    static NxTriangleMesh* cookMesh(const MeshPtr mesh,
        const Vector3& position,
        const Quaternion& orient,
        const Vector3& scale);

protected:
    static NxPhysicsSDK* mPhysicsSDK;
};

} // namespace Solipsis

#endif

#endif // #ifndef __PhysXHelpers_h__