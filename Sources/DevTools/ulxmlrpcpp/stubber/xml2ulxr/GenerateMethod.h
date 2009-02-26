/***************************************************************************
            GenerateMethod.h  -  generate class with mock methods
                             -------------------
    begin                : Sun June 03 2007
    copyright            : (C) 2002-2007 by Ewald Arnold
    email                : ulxmlrpcpp@ewald-arnold.de

    $Id: GenerateMethod.h 1020 2007-07-23 09:09:26Z ewald-arnold $

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

#ifndef GENERATEMETHOD_H
#define GENERATEMETHOD_H

#include <string>
#include <vector>
#include <iosfwd>

#include "UlxrIdlClass.h"


class GenerateMethod
{
  public:

    virtual ~GenerateMethod();

    virtual void generate(const std::string &destdir, const std::string &name) = 0;

    static void generateHeaderHead(std::ostream &h_file,
                                   const std::string &name);

    static void generateHeaderTail(std::ostream & h_file,
                                   const std::string &name);

    static void generateSourceHead(std::ostream & cpp_file,
                                   const std::string &h_name);

    static void generateUserSourceHead(std::ostream & cpp_file,
                                       const std::string &h_name);

    static void generate_NameDefines(const std::string &destdir,
                                     const std::string &name,
                                     const std::vector<Method> &theMethods,
                                     const std::string &suffix = "");

    static std::string dateString();

    static void setDateOutput(bool output);

  private:

    static bool date_output;
};

#endif // GENERATEMETHOD_H
