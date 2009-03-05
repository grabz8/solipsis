/*
This source file is part of Solipsis
    (Solipsis is an opensource decentralized Metaverse platform)
For the latest info, see http://www.solipsis.org/

Copyright (C) 2006-2008 ANR-RIAM (IRISA, Archivideo, Artefacto, Rennes 2 University, Orange Labs)
Author 

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

#include "Prerequisites.h"
#include "LuaBinding.h"

//-------------------------------------------------------------------------------------
int LuaPushArgs(lua_State *L, const char *fmt, va_list argp)
{
    int nparams = 0;
    for (;;) {
        const char *e = strchr(fmt, '%');
        if (e == NULL) break;
        switch (*(e+1))
        {
        case 's':
            {
                char *s = va_arg(argp, char*);
                lua_pushstring(L, s);
                break;
            }
        case 'c':
            {
                char s[2];
                s[0] = (char)va_arg(argp, int);
                s[1] = '\0';
                lua_pushstring(L, s);
                break;
            }
        case 'd':
            {
                int i = va_arg(argp, int);
                lua_pushinteger(L, i);
                break;
            }
        case 'f':
            {
                double d = va_arg(argp, double);
                lua_pushnumber(L, d);
                break;
            }
        default:
            return -1;
        }
        nparams++;
        fmt = e + 2;
    }
    return nparams;
}

//-------------------------------------------------------------------------------------
