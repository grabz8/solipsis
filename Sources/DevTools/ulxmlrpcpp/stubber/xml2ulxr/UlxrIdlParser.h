/***************************************************************************
             UlxrIdlParser.h  -  parse Ulxr idl files
                             -------------------
    begin                : Sun May 20 2007
    copyright            : (C) 2002-2007 by Ewald Arnold
    email                : ulxmlrpcpp@ewald-arnold.de

    $Id: UlxrIdlParser.h 1007 2007-07-21 13:54:34Z ewald-arnold $

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

#ifndef ULXR_IDL_PARSER_H
#define ULXR_IDL_PARSER_H

#include "xmlparser.h"
#include "UlxrIdlClass.h"


/** An xml parser for Ulxr idl files
  */
class UlxrIdlParser : public XmlParser
{
 public:

 /** Constructs a parser.
   */
   UlxrIdlParser();

 /** Destroys the parser.
   */
   virtual ~UlxrIdlParser();

   unsigned numClasses() const;

   UlxrIdlClass getClass(unsigned i) const;

   std::vector<Method> getFunctions() const;

 protected:

  enum IdlState
  {
    eIdlParserFirst = XmlParserBase::eXmlParserLast,
    eUlxrIdl,
    eSource,
    eInclude,
    eClass,
    eFunction,
    eName,
    eLinkScope,
    eSuper,
    eMethod,
    eConstructor,
    eType,
    eArg,
    eIdlParserLast
  };

 /** Parses the current opening XML tag.
   * Used ONLY internally as callback from expat.
   * @param  name  the name of the current tag
   * @param  pointer to the current attributs (unused in XML-RPC)
   */
   virtual void startElement(const XML_Char *name,
                             const XML_Char **atts);

 /** Parses the current closing XML tag.
   * Used ONLY internally as callback from expat.
   * @param  name  the name of the current tag
   * @param  pointer to the current attributs (unused in XML-RPC)
   */
   virtual void endElement(const XML_Char* name);

 /** Tests if the current opening tag is to be parsed by this
   * inheritance level or by the parent.
   * Used ONLY internally.
   * @param  name  the name of the current tag
   * @param  atts  pointer to the current attributs (unused in XML-RPC)
   */
   bool testStartElement(const XML_Char *name,
                         const XML_Char **atts);

 /** Tests if the current closing tag is to be parsed by this
   * inheritance level or by the parent.
   * Used ONLY internally.
   * @param  name  the name of the current tag
   * @param  pointer to the current attributs
   */
   bool testEndElement(const XML_Char *name);

 private:

   bool                 in_arg;
   bool                 in_method;
   bool                 in_function;

   UlxrIdlClass         theClass;
   std::vector<Method>  functions;
   Method               method;
   Argument             arg;
   Type                 type;

   std::vector<UlxrIdlClass>  classList;
};


#endif // ULXR_IDL_PARSER_H
