/***************************************************************************
        GenServerMethod.h  -  generate class with server methods
                             -------------------
    begin                : Sun May 28 2007
    copyright            : (C) 2002-2007 by Ewald Arnold
    email                : ulxmlrpcpp@ewald-arnold.de

    $Id: GenServerMethod.h 978 2007-07-11 17:05:16Z ewald-arnold $

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

#ifndef GENSERVERMETHOD_H
#define GENSERVERMETHOD_H

#include <string>

#include "GenerateMethod.h"


class GenerateServerMethods : public GenerateMethod
{
  public:

    GenerateServerMethods(const UlxrIdlClass &theClass);

    void generate(const std::string &destdir, const std::string &name);

  private:

    void generate_H(const std::string &destdir, const std::string &name);

    void generate_CPP(const std::string &destdir, const std::string &name);

    void generate_CPP_USER(const std::string &destdir, const std::string &name);

    void generateHeaderMethods(std::ostream & h_file);

    void generateHeaderVariables(std::ostream & h_file);

    void generateSourceCtors(std::ostream & cpp_file,
                             const std::string &name);

    void generateSourceMethods(std::ostream & cpp_file);

    void generateHeaderClassHead(std::ostream &h_file,
                                 const std::string &name);
  private:

    UlxrIdlClass   theClass;
};


#endif // GENSERVERMETHOD_H

