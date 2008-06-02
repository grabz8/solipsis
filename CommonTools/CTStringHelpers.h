#ifndef __CTStringHelpers_h__
#define __CTStringHelpers_h__

#include <string>
#include <vector>

namespace CommonTools {

/** This static class contains several helper methods above system.
 */
class StringHelpers
{
public:
    /// Tokenize function
    static void tokenize(const std::string& str, const std::string& delimiter, std::vector<std::string>& tokens);
};

} // namespace CommonTools

#endif // #ifndef __CTStringHelpers_h__