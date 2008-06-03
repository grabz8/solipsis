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

#ifndef VOICESOURCE_H
#define VOICESOURCE_H

class VoiceSource
{
public:
    VoiceSource()
    {
        mPosition[0] = 0; mPosition[1] = 0; mPosition[2] = 0;
        mDirection[0] = 0; mDirection[1] = 0; mDirection[2] = 0;
        mVelocity[0] = 0; mVelocity[1] = 0; mVelocity[2] = 0;
    }

    void setPosition(float x, float y, float z) { mPosition[0] = x; mPosition[1] = y; mPosition[2] = z; }
    void setDirection(float x, float y, float z) { mDirection[0] = x; mDirection[1] = y; mDirection[2] = z; }
    void setVelocity(float x, float y, float z) { mVelocity[0] = x; mVelocity[1] = y; mVelocity[2] = z; }

    const float* getPosition() const { return &mPosition[0]; }
    void getPosition(float* x, float* y, float* z) const { *x = mPosition[0]; *y = mPosition[1]; *z = mPosition[2]; }

    const float* getDirection() const { return &mDirection[0]; }
    void getDirection(float* x, float* y, float* z) const { *x = mDirection[0]; *y = mDirection[1]; *z = mDirection[2]; }

    const float* getVelocity() const { return &mVelocity[0]; }
    void getVelocity(float* x, float* y, float* z) const { *x = mVelocity[0]; *y = mVelocity[1]; *z = mVelocity[2]; }

private:
    float mPosition[3];
    float mDirection[3];
    float mVelocity[3];

};	//	class VoiceSource

#endif	//	VOICESOURCE_H
