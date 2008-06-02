#include "CTStringHelpers.h"

using namespace std;

namespace CommonTools {

//-------------------------------------------------------------------------------------
void StringHelpers::tokenize(const string& str, const string& delimiter, vector<string>& tokens)
{
    tokens.clear();
    size_t p0 = 0, p1 = string::npos;
    while (p0 != string::npos)
    {
        p1 = str.find_first_of(delimiter, p0);
        if (p1 != p0)
        {
            string token = str.substr(p0, p1 - p0);
            tokens.push_back(token);
        }
        p0 = str.find_first_not_of(delimiter, p1);
    }
}

//-------------------------------------------------------------------------------------

} // namespace CommonTools