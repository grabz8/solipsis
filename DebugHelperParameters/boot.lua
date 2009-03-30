--[[
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
]]

logMessage("Booting Lua ...")

-- Events handler
function NavigatorLua:handleEvent(evt, ...)
	logMessage("NavigatorLua:handleEvent()")
	local args = { ... }

	logMessage(string.format("evt = '%s'", evt))
	
	return true
end

-- Create the NavigatorLua instance
navigator = NavigatorLua()
navigator:bind()
navigator:setNameValueVariable("PeerAddress", "localhost:8880")
navigator:setNameValueVariable("LocalWorldAddress", "r-wxpl-goeb7326:8558")
navigator:setNameValueVariable("WorldsServerAddress", "r-wxpl-goeb7326:8550")
navigator:setNameValueVariable("WorldsServerTimeout", "8")
navigator:setNameValueVariable("Login", "motherfucker")
navigator:setNameValueVariable("Password", "motherfucker")
navigator:setNameValueVariable("FacebookApiKey", "8d81e4c64ac0039b209c4a53b21ba220");
navigator:setNameValueVariable("FacebookServer", "api.facebook.com/restserver.php");
navigator:setNameValueVariable("FacebookLoginUrl", "http://api.facebook.com/login.php");
navigator:setNameValueVariable("FixedNodeId", "")
navigator:setNameValueVariable("MediaCachePath", "")
navigator:setNameValueVariable("VoIPServerAddress", "localhost:30000")
navigator:setNameValueVariable("VoIPSilenceLevel", "5.0")
navigator:setNameValueVariable("VoIPSilenceLatency", "5")

-- some usefull URLs
-- http://mdeverdelhan.developpez.com/tutoriel/lua
-- http://www.tecgraf.puc-rio.br/~celes/lua++/