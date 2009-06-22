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

#ifndef __XmlLogin_h__
#define __XmlLogin_h__

#include "XmlDatas.h"

namespace Solipsis 
{
    class XMLDATAS_EXPORT XmlLogin : public XmlData
    {
    protected:
        static Pool mPool;

    protected:
        std::string mUsername;
        std::string mWorldHost;
        unsigned short mWorldPort;
        NodeId mNodeId;

    public:
        XmlLogin()
        {}
        XmlLogin(const std::string& username, const std::string& worldHost, unsigned short worldPort, const NodeId& nodeId) :
            mUsername(username),
            mWorldHost(worldHost),
            mWorldPort(worldPort),
            mNodeId(nodeId)
        {}

        static Pool& getStaticPool();
        virtual Pool& getPool() const;
        virtual void clear() {
            mUsername.clear();
            mWorldHost.clear();
            mWorldPort = 0;
            mNodeId.clear();
        }

        virtual std::string toXmlString() const;
        virtual bool toXmlElt(TiXmlElement& xmlElt) const;
        virtual bool fromXmlElt(TiXmlElement* xmlElt);

        void setUsername(const std::string& username) { mUsername = username; }
        const std::string& getUsername() { return mUsername; }

        void setWorldHost(const std::string& worldHost) { mWorldHost = worldHost; }
        const std::string& getWorldHost() { return mWorldHost; }

        void setWorldPort(unsigned short worldPort) { mWorldPort = worldPort; }
        unsigned short getWorldPort() { return mWorldPort; }

        void setNodeId(const NodeId& nodeId) { mNodeId = nodeId; }
        const NodeId& getNodeId() { return mNodeId; }
    };
 
    RefCntPoolPtr<XmlLogin> RefCntPoolPtr<XmlLogin>::nullPtr((XmlLogin*)0);
} // namespace Solipsis

#endif // #ifndef __XmlLogin_h__
