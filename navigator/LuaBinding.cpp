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
