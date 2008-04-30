#ifndef __TextureIterator_h__
#define __TextureIterator_h__

#include <Ogre.h>

using namespace Ogre;

typedef std::map<String,TexturePtr> TextureMap;
typedef MapIterator<TextureMap> TextureMapIterator;

typedef std::vector<TexturePtr> TextureVector;
typedef VectorIterator<TextureVector> TextureVectorIterator;

#endif
