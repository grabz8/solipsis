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

#ifndef __CTMaths_h__
#define __CTMaths_h__

namespace CommonTools {

/** This static class contains several helper methods for maths.
 */
class Maths
{
public:
    template<class type>
    static inline type min(type a, type b)
    {
        if (a <= b)
            return a;
        return b;
    }

    template<class type>
    static inline type max(type a, type b)
    {
        if (a >= b)
            return a;
        return b;
    }

    template<class type>
    static inline void limit(type& value, type min, type max)
    {
        if (value < min)
            value = min;
        else if (value > max)
            value = max;
    }
};

} // namespace CommonTools

#endif // #ifndef __CTMaths_h__
