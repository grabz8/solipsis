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

#include "Node.h"
#include <CTLog.h>
#include <CTIO.h>

using namespace CommonTools;

namespace Solipsis {

//-------------------------------------------------------------------------------------
Node::Node(const std::string& type) :
    mNodeId(""),
    mType(type)
{
}

//-------------------------------------------------------------------------------------
Node::~Node()
{
}

//-------------------------------------------------------------------------------------
bool Node::loadFromElt(TiXmlElement* nodeElt)
{
    return true;
}

//-------------------------------------------------------------------------------------
TiXmlElement* Node::getSavedElt()
{
    // root node
    TiXmlElement* nodeElt = new TiXmlElement("node");

    return nodeElt;
}
//-------------------------------------------------------------------------------------
bool Node::loadNode(const std::string& mediaCachePath)
{
    if (mNodeId.empty())
        return false;

    std::string nodeIdFilename = mediaCachePath + IO::getPathSeparator() + mNodeId + ".xml";
    TiXmlDocument xmlNodeIdFileDoc(nodeIdFilename.c_str());
    if (!xmlNodeIdFileDoc.LoadFile())
        return false;

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Node::loadNode() loading node with nodeId:%s from %s", mNodeId.c_str(), nodeIdFilename.c_str());

    TiXmlElement* nodeElt = xmlNodeIdFileDoc.FirstChildElement("node");
    if (nodeElt == 0)
        return false;

    return loadFromElt(nodeElt);
}

//-------------------------------------------------------------------------------------
bool Node::saveNode(const std::string& mediaCachePath)
{
    std::string nodeIdFilename = mediaCachePath + IO::getPathSeparator() + mNodeId + ".xml";
    TiXmlDocument xmlNodeIdFileDoc(nodeIdFilename.c_str());

    LOGHANDLER_LOGF(LogHandler::VL_DEBUG, "Node::saveNode() saving node with nodeId:%s into %s", mNodeId.c_str(), nodeIdFilename.c_str());

    TiXmlElement* nodeElt = getSavedElt();
    if (nodeElt == 0)
        return false;
    xmlNodeIdFileDoc.LinkEndChild(nodeElt); 
    xmlNodeIdFileDoc.SaveFile();

    return true;
}

//-------------------------------------------------------------------------------------

} // namespace Solipsis
