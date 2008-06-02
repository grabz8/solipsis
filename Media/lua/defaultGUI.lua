-- uichat listener
function uichatListener(eventType, naviName, naviDataName, param)
	if eventType == "Data" then
		if naviDataName == "pageLoaded" then
			naviShow(naviName)
		end
		if naviDataName == "sendMessage" then
			-- Reset input
			naviEvaluateJS(naviName, "$('inputChat').value = ''")
			-- Send the message
			navigator:sendMessage(param["msg"])
		end
	end
end

-- uictxtavatar listener
function uictxtavatarListener(eventType, naviName, naviDataName, param)
	if eventType == "Data" then
		if naviDataName == "pageLoaded" then
			naviShow(naviName)
		end
		if naviDataName == "contextItemSelected" then
			-- Perform action associated to item selected
			logMessage("contextItemSelected")
			navigator:contextItemSelected(param["item"])
		end
	end
end

-- uictxtwww listener
function uictxtwwwListener(eventType, naviName, naviDataName, param)
	if eventType == "Data" then
		if naviDataName == "pageLoaded" then
		    local ctxtNaviName = param["ctxtNaviName"]
		    if naviCanNavigateBack(ctxtNaviName) then
		        naviEvaluateJS(naviName, "$('mTbIconBack').addClass('mozToolbarBackActive')")
		    end
		    if naviCanNavigateForward(ctxtNaviName) then
		        naviEvaluateJS(naviName, "$('mTbIconFwd').addClass('mozToolbarFwdActive')")
		    end
		    local url = naviGetCurrentLocation(ctxtNaviName)
		    naviEvaluateJS(naviName, "$('inputUrl').value = '" .. url .. "'")
			naviShow(naviName)
		elseif naviDataName == "navCommand" then
		    local ctxtNaviName = param["ctxtNaviName"]
		    local cmd = param["cmd"]
		    if cmd == "go" then
    		    local url = naviEvaluateJS(naviName, "$('inputUrl').value")
		        naviNavigateTo(ctxtNaviName, url)
		    elseif cmd == "back" then
		        naviNavigateBack(ctxtNaviName)
		    elseif cmd == "fwd" then
    		    naviNavigateForward(ctxtNaviName)
		    elseif cmd == "refresh" then
    		    naviEvaluateJS(ctxtNaviName, "window.location.reload(false)")
		    elseif cmd == "stop" then
    		    naviNavigateStop(ctxtNaviName)
		    elseif cmd == "home" then
		        naviNavigateTo(ctxtNaviName, "http://www.solipsis.org")
		    end
			navigator:hideNavi(naviName)
		end
	end
end

-- uictxtvlc listener
function uictxtvlcListener(eventType, naviName, naviDataName, param)
	logMessage(string.format("uictxtvlcListener(%s, %s, %s)", eventType, naviName, naviDataName))
	if eventType == "Data" then
		if naviDataName == "pageLoaded" then
		    local ctxtVLCName = param["ctxtVLCName"]
	        local mrl = navigator:extTextSrcExHandleEvt("vlc", ctxtVLCName, "getmrl")
	        local mrlJS = string.gsub(mrl, "[\\]", "\\\\");
		    naviEvaluateJS(naviName, "$('inputMrl').value = '" .. mrlJS .. "'")
	        local mute = navigator:extTextSrcExHandleEvt("vlc", ctxtVLCName, "getmute")
	        if mute == "true" then
		        naviEvaluateJS(naviName, "$('mTbIconVolume').addClass('vlcToolbarVolumeOff')")
		    else
		        naviEvaluateJS(naviName, "$('mTbIconVolume').removeClass('vlcToolbarVolumeOff')")
		    end
			naviShow(naviName)
		elseif naviDataName == "vlcCommand" then
		    local ctxtVLCName = param["ctxtVLCName"]
		    local cmd = param["cmd"]
		    if cmd == "setmrl" then
		        local mrl = naviEvaluateJS(naviName, "$('inputMrl').value")
	            navigator:extTextSrcExHandleEvt("vlc", ctxtVLCName, "setmrl?" .. mrl)
		    else
	            navigator:extTextSrcExHandleEvt("vlc", ctxtVLCName, cmd)
	        end
			navigator:hideNavi(naviName)
		end
	end
end

-- clamping computation
function clampNaviOnScreen(x, y, w, h)
	local cx = x - w/2
	local cy = y - h/2
	if cx < 0 then cx = 0 end
	if cy < 0 then cy = 0 end
	local scrWidth, scrHeight = navigator:getRenderWinMetrics()
	if cx > (scrWidth - w) then cx = (scrWidth - w) end
	if cy > (scrHeight - h) then cy = (scrHeight - h) end
	return cx, cy
end

-- GUI creation
function NavigatorLua:createGUI(guiName, ...)
	logMessage(string.format("NavigatorLua:createGUI(%s)", guiName))

	if guiName == "uichat" then
		-- Create Navi UI chat
		naviMgrCreateNavi("uichat", "local://uichat.html", "TopLeft", 0, 0, 512, 64, true, false)
		naviSetMask("uichat", "uichat.png")
		naviSetOpacity("uichat", 0.75)
		naviAddEventListener("uichat", "uichatListener")
		return true
	elseif guiName == "uictxtavatar" then
		-- Create Navi UI context about avatar
		local args = { ... }
		local x, y, items = args[1], args[2], args[3]
		logMessage(string.format("x, y, items = %d, %d, %s", x, y, items))
		local itemW, itemH = 50, 50
		local naviW, naviH = 256, 256
		x, y = clampNaviOnScreen(x, y, naviW, naviH)
		if not naviMgrIsNaviExists(guiName) then
		    naviMgrCreateNavi(guiName, "", x, y, naviW, naviH, false, false)
		    naviSetColorKey(guiName, "#010203", 0, "#000000")
		    naviSetOpacity(guiName, 0.75)
		    naviAddEventListener(guiName, guiName .. "Listener")
		else
		    naviSetPosition(guiName, x, y)
		end
		local naviDatas = {}
		naviDatas["naviDataName"] = guiName .. "Datas"
		naviDatas["items"] = items
		naviDatas["itemWidth"] = itemW
		naviDatas["itemHeight"] = itemH
		naviNavigateTo(guiName, "local://" .. guiName .. ".html", naviDatas)
		return true
	elseif guiName == "uictxtwww" then
		-- Create Navi UI context about WWW Navi material
		local args = { ... }
		local x, y, ctxtNaviName = args[1], args[2], args[3]
		logMessage(string.format("x, y, ctxtNaviName = %d, %d, %s", x, y, ctxtNaviName))
		local naviW, naviH = 256, 48
		x, y = clampNaviOnScreen(x, y, naviW, naviH)
		if not naviMgrIsNaviExists(guiName) then
		    naviMgrCreateNavi(guiName, "", x, y, naviW, naviH, false, false)
		    naviSetOpacity(guiName, 0.8)
		    naviSetMaxUpdatesPerSec(guiName, 8)
		    naviSetForceMaxUpdate(guiName, false)
		    naviSetAutoUpdateOnFocus(guiName, true)
		    naviAddEventListener(guiName, guiName .. "Listener")
		else
		    naviSetPosition(guiName, x, y)
		end
		local naviDatas = {}
		naviDatas["naviDataName"] = guiName .. "Datas"
		naviDatas["ctxtNaviName"] = ctxtNaviName
		naviNavigateTo(guiName, "local://" .. guiName .. ".html", naviDatas)
		return true
	elseif guiName == "uictxtvlc" then
		-- Create Navi UI context about VLC material
		local args = { ... }
		local x, y, ctxtVLCName = args[1], args[2], args[3]
		logMessage(string.format("x, y, ctxtVLCName = %d, %d, %s", x, y, ctxtVLCName))
		local naviW, naviH = 256, 64
		x, y = clampNaviOnScreen(x, y, naviW, naviH)
		if not naviMgrIsNaviExists(guiName) then
		    naviMgrCreateNavi(guiName, "", x, y, naviW, naviH, false, false)
		    naviSetOpacity(guiName, 0.8)
		    naviSetMaxUpdatesPerSec(guiName, 8)
		    naviSetForceMaxUpdate(guiName, false)
		    naviSetAutoUpdateOnFocus(guiName, true)
		    naviAddEventListener(guiName, guiName .. "Listener")
		else
		    naviSetPosition(guiName, x, y)
		end
		local naviDatas = {}
		naviDatas["naviDataName"] = guiName .. "Datas"
		naviDatas["ctxtVLCName"] = ctxtVLCName
		naviNavigateTo(guiName, "local://" .. guiName .. ".html", naviDatas)
		return true
	end
	return false
end
