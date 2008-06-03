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

#ifndef __CTException_h__
#define __CTException_h__

#include <string>
#include <exception>

using std::string;

namespace CommonTools {

// Assertion macro
#define CTAssert(a, b) if (!(a)) CT_EXCEPT(CommonTools::Exception::ERR_RT_ASSERTION_FAILED, (b), __FUNC__ )

/** When thrown, provides information about an error that has occurred.
    @remarks
        Ideally application never uses return values to indicate errors. Instead, if an
        error occurs, an exception is thrown, and this is the object that
        encapsulates the detail of the problem. The application using
        this Exception class should always ensure that the exceptions are caught, so all
        functions should occur within a
        try{} catch(CommonTools::Exception& e) {} block.
*/
class Exception : public std::exception
{
protected:
    long line;
    int number;
	string typeName;
    string description;
    string source;
    string file;
	mutable string fullDesc;

public:
    /** Static definitions of error codes.
    */
    enum ExceptionCodes {
        ERR_INVALID_PARAMS,
        ERR_INVALID_STATE,
        ERR_INTERNAL_ERROR,
        ERR_RT_ASSERTION_FAILED, 
		ERR_NOT_IMPLEMENTED
    };

    /** Default constructor.
    */
    Exception(int number, const string& description, const string& source);

    /** Advanced constructor.
    */
    Exception(int number, const string& description, const string& source, const char* type, const char* file, long line);

    /** Copy constructor.
    */
    Exception(const Exception& rhs);

	/// Needed for  compatibility with std::exception
	~Exception() throw() {}

    /** Assignment operator.
    */
    void operator=(const Exception& rhs);

    /** Returns a string with the full description of this error.
        @remarks
            The description contains the error number, the description
            supplied by the thrower, what routine threw the exception,
            and will also supply extra platform-specific information
            where applicable. For example - in the case of a rendering
            library error, the description of the error will include both
            the place in which OGRE found the problem, and a text
            description from the 3D rendering library, if available.
    */
    virtual const string& getFullDescription(void) const;

    /** Gets the error code.
    */
    virtual int getNumber(void) const throw();

    /** Gets the source function.
    */
    virtual const string& getSource() const { return source; }

    /** Gets source file name.
    */
    virtual const string& getFile() const { return file; }

    /** Gets line number.
    */
    virtual long getLine() const { return line; }

	/** Returns a string with only the 'description' field of this exception. Use 
		getFullDescriptionto get a full description of the error including line number,
		error number and what function threw the exception.
    */
	virtual const string& getDescription(void) const { return description; }

	/// Override std::exception::what
	const char* what() const throw() { return getFullDescription().c_str(); }
    
};

#ifndef CT_EXCEPT
#define CT_EXCEPT(num, desc, src, type) throw Exception(num, desc, src, type, __FILE__, __LINE__)
#endif

} // namespace CommonTools

#endif // #ifndef __CTException_h__
