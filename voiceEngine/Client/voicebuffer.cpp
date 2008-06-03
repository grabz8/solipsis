/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author RealXTend, updated by JAN Gregory

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

#include "voicebuffer.h"
// GREG BEGIN
//#include <fmodex/fmod.hpp>
#include <fmod.hpp>
// GREG END


VoiceBuffer::VoiceBuffer(FMOD::Sound* sound)
    : mSound(sound)
    , mLastPosition(0)
    , mChannel(0)
{
}

VoiceBuffer::~VoiceBuffer()
{
    if (mSound)
    {
        mSound->release();
        mSound = 0;
    }

    mChannel = 0;
}

FMOD::Sound* VoiceBuffer::getSound() const
{
    return mSound;
}

FMOD::Channel* VoiceBuffer::getChannel()
{
    return mChannel;
}

void VoiceBuffer::setLastPosition(unsigned int pos)
{
    mLastPosition = pos;
}

unsigned int VoiceBuffer::getLastPosition() const
{
    return mLastPosition;
}

void VoiceBuffer::play(FMOD::System* system)
{
    system->playSound(FMOD_CHANNEL_FREE, mSound, false, &mChannel);
}

void VoiceBuffer::stop()
{
    mLastPosition = 0;

    if (mChannel)
    {
        mChannel->stop();
        mChannel = 0;
    }
}

bool VoiceBuffer::isPlaying() const
{
    bool playing = false;
    if (mChannel)
        mChannel->isPlaying(&playing);

    return playing;
}
