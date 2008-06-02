#include "CTException.h"
#include "CTLog.h"

#include <sstream>

namespace CommonTools {

//-------------------------------------------------------------------------------------
Exception::Exception(int num, const string& desc, const string& src) :
    line(0),
    number(num),
    description(desc),
    source(src)
{
    LogHandler::getLogHandler()->log(LogHandler::VL_CRITICAL, this->getFullDescription().c_str());
}

//-------------------------------------------------------------------------------------
Exception::Exception(int num, const string& desc, const string& src, 
	const char* typ, const char* fil, long lin) :
    line(lin),
    number(num),
	typeName(typ),
    description(desc),
    source(src),
    file(fil)
{
    LogHandler::getLogHandler()->log(LogHandler::VL_CRITICAL, this->getFullDescription().c_str());
}

//-------------------------------------------------------------------------------------
Exception::Exception(const Exception& rhs)
    : line(rhs.line), number(rhs.number), description(rhs.description), source(rhs.source), file(rhs.file)
{
}

//-------------------------------------------------------------------------------------
void Exception::operator=(const Exception& rhs)
{
    description = rhs.description;
    number = rhs.number;
    source = rhs.source;
    file = rhs.file;
    line = rhs.line;
	typeName = rhs.typeName;
}

//-------------------------------------------------------------------------------------
const string& Exception::getFullDescription(void) const
{
	if (fullDesc.empty())
	{
		std::stringstream desc;
		desc <<  "CT EXCEPTION(" << number << ":" << typeName << "): "
			<< description 
			<< " in " << source;
		if (line > 0)
		{
			desc << " at " << file << " (line " << line << ")";
		}
		fullDesc = desc.str();
	}

	return fullDesc;
}

//-------------------------------------------------------------------------------------
int Exception::getNumber(void) const throw()
{
    return number;
}

//-------------------------------------------------------------------------------------

} // namespace CommonTools
