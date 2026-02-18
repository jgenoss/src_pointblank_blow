
require "Script.uiLua.UIDefine"
require "Script.uiLua.uiControl"
require "Script.uiLua.uiCaption"
require "Script.uiLua.uiFrameWnd"
require "Script.uiLua.uiButton"
require "Script.uiLua.uiStaticText"
require "Script.uiLua.uiCheckBox"
require "Script.uiLua.uiComboBox"
require "Script.uiLua.uiEditBox"
require "Script.uiLua.uiHBar"
require "Script.uiLua.uiImageBox"
require "Script.uiLua.uiListBox"
require "Script.uiLua.uiProgressBar"
require "Script.uiLua.uiRadio"
require "Script.uiLua.uiScrollBar"
require "Script.uiLua.uiSocketBox"
require "Script.uiLua.uiSpin"
require "Script.uiLua.uiTab"
require "Script.uiLua.uiToolTip"
require "Script.uiLua.uiListView"
require "Script.uiLua.uiMessageBox"
require "Script.uiLua.uiUser"
require "Script.uiLua.uiButtonComposed3"
require "Script.uiLua.uiButtonImageSet"
require "Script.uiLua.eventHandler"

global_bUpdateUI = false

local controltable = {
}

function BindClass( class, id)
	local control = nil
	if class == "i3UIButton" then
		control = uiButton(id)
	elseif class == "i3UIFrameWnd" then
		control = uiFrameWnd(id)
	elseif class == "i3UIStaticText" then
		control = uiStaticText(id)
	elseif class == "i3UICheckBox" then
		control = uiCheckBox(id)
	elseif class == "i3UIComboBox" then
		control = uiComboBox(id)
	elseif class == "i3UIButtonComboBox" then
		control = uiComboBox(id)
	elseif class == "i3UIComboBox_NoEditBox" then
		control = uiComboBox(id)		
	elseif class == "i3UIEditBox" then
		control = uiEditBox(id)
	elseif class == "i3UIHBar" then
		control = uiHBar(id)
	elseif class == "i3UIImageBox" then
		control = uiImageBox(id)
	elseif class == "i3UIListBox" then
		control = uiListBox(id)
	elseif class == "i3UIProgressBar" then
		control = uiProgressBar(id)
	elseif class == "i3UIRadio" then
		control = uiRadio(id)
	elseif class == "i3UIScrollBar" then
		control = uiScrollBar(id)
	elseif class == "i3UISocketBox" then
		control = uiSocketBox(id)
	elseif class == "i3UISpin" then
		control = uiSpin(id)
	elseif class == "i3UITab" then
		control = uiTab(id)
	elseif class == "i3UIToolTip" then
		control = uiToolTip(id)
	elseif class == "i3UIMessageBox" then
		control = uiMessageBox(id)
	elseif class == "i3UIListView_Box" then
		control = uiListView_Box(id)
	elseif class == "i3UIUserControl" then -- uiUser는 template name그대로의 class를 만들어 둔후 그 instance를 만들어 사용한다.
		usetemplatename = uiCtrl_getTemplate(id)
		control = linkUserCtrol( usetemplatename, id)
	elseif class == "i3UIButtonComposed3" then
		control = uiButtonComposed3(id)
	elseif class == "i3UIButtonImageSet" then
		control = uiButtonImageSet(id)
	else
		control = uiControl(id)
	end
	
	return control
end

function Use(name)
	local id = uiGetObject(name)		-- pointer

	if id == nil then
		error( "Cannot Find Control Named ".. name, 2)
	end

	local class = uiCtrl_getClass(id)	-- string
	local control = BindClass( class, id)
	
	--luaTrace(name)
	if( controltable[name] ~= nil) then
		error( "redefinition of the name: "..name, 2)
	end
	--assert( controltable[name] == nil, name.. " already defined in the table:controltable")

	controltable[name] = control

	control:recordInitialState()

	assert( control ~= nil)
	assert( _G[name] == nil)

	_G[name] = control
	return control
end

local tempTable = {
}

function linkUserCtrol( template, id)
	ctrlID = id	-- id는 local이어서 사용할 수 없심..
	
	f = loadstring("newcontrol = "..template.."(ctrlID)")
	
	f()
	
	control = newcontrol
	
	assert( control ~= nil)
	
	return control
end

function CreateVolatile(template, parent)	-- lua에서 정의된 class를 상속받는 컨트롤을 템플릿의 이름을 이용하여 client program에 생성 시킨다. 
	id = uiCreate( template, parent)
	
	f = loadstring("newcontrol = "..template.."(id)")

	f()

	control = newcontrol
		
	control:setName( tostring(id))
	control:setVolatile(true)
		
	table.insert( tempTable, control)
	
	assert(control ~= nil)
	
	return control
end
--[[
function allocInstance( template, parent)	-- 일반 컨트롤을 템플릿의 이름을 이용하여 client program에 생성 시킨다. 
	id = uiCreate( template, parent)
	
	local class = uiCtrl_getClass(id)	-- string
	local control = nil

	control = BindClass( class, id)
	
	table.insert(tempTable, control)
	
	return control
end
--]]

function UpdateUI(deltaSeconds)
	if( global_bUpdateUI == true)	then
		for k in pairs(controltable)	do
							controltable[k]:onUpdate(deltaSeconds)
							end
	end
end

function ResetUI()
	for k in pairs(controltable)	do
		if controltable[k].volatile == false then
			controltable[k]:restoreInitialState()
		end
	end
	
	for k in pairs(tempTable)	do
		uiDelete( tempTable[k].ctrlID)
	end
end
