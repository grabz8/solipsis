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
navigator:setConnectionParams("localhost", 8550, "me")

-- some usefull URLs
-- http://mdeverdelhan.developpez.com/tutoriel/lua
-- http://www.tecgraf.puc-rio.br/~celes/lua++/