/***************************************************************************
             UlxrIdlClass.h  -  parse Ulxr idl files
                             -------------------
    begin                : Sun May 20 2007
    copyright            : (C) 2002-2007 by Ewald Arnold
    email                : ulxmlrpcpp@ewald-arnold.de

    $Id: UlxrIdlClass.h 1016 2007-07-22 15:03:44Z ewald-arnold $

 ***************************************************************************/

/**************************************************************************
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 ***************************************************************************/

#ifndef ULXR_IDL_CLASS_H
#define ULXR_IDL_CLASS_H

#include <string>
#include <vector>


//  <TYPE  qleft="const" qright="*">QString</TYPE>
struct Type
{
    Type();

    void setProperty(const std::string &in_left, const std::string &in_right);

    void setName(const std::string &in_name);

    void clear();

    std::string getCppString() const;

    std::string getRpcName() const;

    std::string getProxyType() const;

    std::string getTypeAdapter() const;

    std::string getInversTypeAdapter() const;

    std::string getTypeDereference() const;

    std::string getTypeAccessor() const;

    std::string getRpcAccessor() const;

    std::string getLeft() const;

    std::string getRight() const;

    bool isReference() const;

    bool isPointer() const;

    std::string getName() const;

  private:

    std::string   name;
    std::string   simplename;
    std::string   left;
    std::string   right;
};


//   <ARG><TYPE>int</TYPE><NAME>num</NAME></ARG>
struct Argument
{
    Argument();

    void setName(const std::string &in_name);

    std::string getName() const;

    static std::string getConstCastedName(const std::string &name,
                                          const Type &type,
                                          const std::string &prefix = "",
                                          const std::string &suffix = "");

    std::string getConstCastedName() const;

    void setType(const Type &in_type);

    Type getType() const;

    void clear();

    std::string getCppString() const;

  private:

    std::string   name;
    Type          type;
};


/*
  <METHOD virtual='1'>
    <TYPE>DCOPRef</TYPE>
    <NAME>getObject</NAME>
    <ARG><TYPE>int</TYPE><NAME>num</NAME></ARG>
  </METHOD>
*/
struct Method
{
    Method();

    void setProperty(bool constructor,
                     bool in_virtual,
                     bool in_const);

    void setFunction(bool isfunc);

    void setName(const std::string &in_classname, const std::string &in_name);

    void setName(const std::string &in_name);

    std::string getName(bool fully = false) const;

    std::string extractNamespace();

    void setOverloadName(const std::string &in_name);

    std::string getOverloadName(bool fully = false,
                                const std::string &suffix = "",
                                const std::string &binder = "::") const;

    void setType(const Type &in_type);

    Type getType() const;

    void addArgument(const Argument &arg);

    std::string getCppString(unsigned indent,
                             bool impl,
                             const std::string &suffix) const;

    std::string getCppString(const std::string &name,
                             unsigned indent,
                             bool impl,
                             const std::string &suffix) const;

    std::string getSignature() const;

    std::string getTemplateSignature() const;

    void clear();

    bool isVirtual() const;

    bool isConstructor() const;

    unsigned numArgs() const;

    Argument getArg(unsigned i) const;

    bool operator<(const Method &rhs) const;

    static void resolveOverloaded(std::vector<Method> &methods);

    static void OverloadRename(Method *method, unsigned ctr);

    void setSource(const std::string &name);

    std::string getSource() const;

private:

    std::vector<Argument>  args;

    Type         type;
    bool         isvirtual;
    bool         isconst;
    bool         isconstructor;
    bool         isfunction;

    std::string  name;
    std::string  overloadname;
    std::string  classname;
    std::string  sourcefile;
};


/** An c++ class for Ulxr idl files
  */
class UlxrIdlClass
{
 public:

  /** Constructs a parser.
    */
    UlxrIdlClass();

    void setName(const std::string &in_name);

    std::string getName() const;

    std::string getBaseName() const;

    std::string getNamespace() const;

    void addMethod(const Method &method);

    void clear();

    unsigned numMethods() const;

    Method getMethod(unsigned i) const;

    const std::vector<Method> & getAllMethods() const;

    unsigned numCtors() const;

    Method getCtor(unsigned i) const;

    std::vector<unsigned> getArgsCount() const;

    void resolveOverloaded();

    void OverloadRename(Method *method, unsigned ctr);

    void setSource(const std::string &name);

    std::string getSource() const;

  private:

    std::string            name;
    std::vector<Method>    methods;
    std::vector<Method>    ctors;
    std::vector<unsigned>  argsCnt;
    std::string            sourcefile;
};


#endif // ULXR_IDL_CLASS_H
