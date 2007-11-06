-- uichat listener
function uichatListener(eventType, naviName, naviDataName, param)
	if eventType == "Data" then
		if naviDataName == "pageLoaded" then
			showNavi(naviName)
		end
		if naviDataName == "sendMessage" then
			-- Reset input
			naviEvaluateJS(naviName, "$('inputChat').value = ''")
			-- Send the message
			navigator:sendMessage(param["msg"])
		end
	end
end

-- uicontext listener
function uicontextListener(eventType, naviName, naviDataName, param)
	if eventType == "Data" then
		if naviDataName == "pageLoaded" then
			showNavi(naviName)
		end
		if naviDataName == "contextItemSelected" then
			-- Perform action associated to item selected
			logMessage("contextItemSelected")
			navigator:contextItemSelected(param["item"])
		end
	end
end

-- GUI creation
function NavigatorLua:createGUI(guiName, ...)
	logMessage(string.format("NavigatorLua:createGUI(%s)", guiName))

	if guiName == "uichat" then
		-- Create Navi UI chat
		createNavi("uichat", "local://uichat.html", "TopLeft", 0, 0, 512, 64, true, false)
		setNaviMask("uichat", "uichat.png")
		setNaviOpacity("uichat", 0.75)
		addNaviEventListener("uichat", "uichatListener")
		return true
	elseif guiName == "uicontext" then
		-- Create Navi UI context
		local args = { ... }
		local x, y, items = args[1], args[2], args[3]
		logMessage(string.format("x, y, items = %d, %d, %s", x, y, items))
		local naviW, naviH = 256, 256
		local itemW, itemH = 50, 50
		local scrWidth, scrHeight = navigator:getRenderWinMetrics()
		x = x - naviW/2
		y = y - naviH/2
		if x < 0 then x = 0 end
		if y < 0 then y = 0 end
		if x > (scrWidth - naviW) then x = (scrWidth - naviW) end
		if y > (scrHeight - naviH) then y = (scrHeight - naviH) end
		createNavi("uicontext", "", x, y, naviW, naviH, false, false)
		setNaviColorKey("uicontext", "#010203", 0, "#000000")
		setNaviOpacity("uicontext", 0.75)
		addNaviEventListener("uicontext", "uicontextListener")
		local naviDatas = {}
		naviDatas["naviDataName"] = "uicontextDatas"
		naviDatas["items"] = items
		naviDatas["itemWidth"] = itemW
		naviDatas["itemHeight"] = itemH
		navigateNaviTo("uicontext", "local://uicontext.html", naviDatas)
		return true
	end
	return false
end
