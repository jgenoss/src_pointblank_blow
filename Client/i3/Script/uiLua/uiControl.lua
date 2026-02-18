-- 컨트롤 기본
class 'uiControl'

function uiControl:__init(id)
	self.ctrlID = id
	self.x = 0.0
	self.y = 0.0
	self.z = 0.0
	self.width = 0.0
	self.height = 0.0
	self.volatile = false
	self.bEnable = false
end

function uiControl:setVolatile( bTrue)
	self.volatile = bTrue
end

function uiControl:onUpdate(deltaSeconds)

end

function uiControl:recordInitialState()

	self.x, self.y, self.z = self:getPosition()

	self.width, self.height = self:getSize()

	self.bEnable = self:getEnable()
end

function uiControl:restoreInitialState()
	self:setPosition( self.x, self.y, self.z)

	self:setSize(self.width, self.height)

--[[	local nEnable = 0
	if self.bEnable == true then
		nEnable = 1
	end--]]

	--self:setEnable( self.bEnable)

	self:setColor( 255, 255, 255, 255)

	self:SetTTMessage( nil)
end

function uiControl:setID( id)
	self.ctrlID = id
end

--notify handler
function uiControl:registerNotify( evt, handler)
	assert( handler ~= nil)
	assert( type(handler) == "function")

	self:RegisterNotifyHandler( evt, handler)

	if( type(evt) == "number") then
		if evt >= 0 and evt < UI_EVT_MAXCOUNT then
		uiCtrl_RegNotifyHandler(self.ctrlID, evt, 1)
		end
	end
end

function uiControl:RegisterNotifyHandler( evt, handler)
	self.notifyhandlers = self.notifyhandlers or {}

	self.notifyhandlers[evt] = handler
end

function uiControl:UnRegisterNotifyHandler(evt)
	if self.notifyhandlers ~= nil then
		if self.notifyhandlers[evt] ~= nil then
			self.notifyhandlers[evt] = nil
			if( type(evt) == "number") then
				if evt >= 0 and evt < UI_EVT_MAXCOUNT then
				uiCtrl_RegNotifyHandler(self.ctrlID, evt, 0)
				end
			end
		end
	end
end

function uiControl:OnNotify(evt, ...)
	if self.notifyhandlers == nil then do return end end
	if self.notifyhandlers[evt] == nil then do return end end

	self.notifyhandlers[evt](...)
end

-- OnParentNotify
function uiControl:OnParentNotify( evt, ...)
end

-- enableParentNotify
function uiControl:enableParentNotify( evt)
	if type(evt) == "number" then
		if evt >= 0 and evt < UI_EVT_MAXCOUNT then
			uiCtrl_RegNotifyHandler(self.ctrlID, evt, 1)
		end
	end
end

-- enableUIEvent
function uiControl:enableUIEvent( evt)
	if type(evt) == "number" then
		if evt >= 0 and evt < UI_EVT_MAXCOUNT then
			uiCtrl_RegEventHandler( self.ctrlID, evt, 1)
		end
	else

	end
end

--event handler
function uiControl:registerEvent( evt, handler)
	assert( handler ~= nil)
	assert( type(handler) == "function")

	self:RegisterEventHandler( evt, handler)

	if( type(evt) == "number") then
		if evt >= 0 and evt < UI_EVT_MAXCOUNT then
		uiCtrl_RegEventHandler(self.ctrlID, evt, 1)
		end
	end
end

function uiControl:RegisterEventHandler( evt, handler)
	self.handlers = self.handlers or {}

	self.handlers[evt] = handler
end

function uiControl:UnRegisterEventHandler( evt)
	if self.handlers ~= nil then
		if self.handlers[evt] ~= nil then
			self.handlers[evt] = nil

			if( type(evt) == "number") then
				if evt >= 0 and evt < UI_EVT_MAXCOUNT then
				uiCtrl_RegEventHandler(self.ctrlID, evt, 0)
				end
			end
		end
	end
end

function uiControl:OnEvent(evt, ...)
	if self.handlers == nil then do return end end
	if self.handlers[evt] == nil then do return end end

	self.handlers[evt](...)
end

--control control
function uiControl:setName (name)
	uiCtrl_setName( self.ctrlID, name)
end

function uiControl:getName ()
	uiCtrl_getName( self.ctrlID)
end

function uiControl:getClass ()
	local class = uiCtrl_getClass( self.ctrlID)
	return class
end

function uiControl:getPosition()
	local x,y,z = uiCtrl_getPosition( self.ctrlID)
	return x,y,z
end

function uiControl:getPositionAcc()
	local x,y,z = uiCtrl_getPositionAcc( self.ctrlID)
	return x,y,z
end

function uiControl:setPosition(x,y,z)
	uiCtrl_setPosition(self.ctrlID, x, y, z)
end

function uiControl:getSize()
	local h, w = uiCtrl_getSize(self.ctrlID)
	return h, w
end

function uiControl:setSize(width, height)
	return uiCtrl_setSize( self.ctrlID, width, height)
end

function uiControl:getTemplate()
	return uiCtrl_getTemplate(self.ctrlID)
end

function uiControl:setTemplate(template)
    return uiCtrl_setTemplate(self.ctrlID, template)
end

function uiControl:getEnable()
	return uiCtrl_getEnable(self.ctrlID)
end

function uiControl:setEnable(enable)
	uiCtrl_setEnable(self.ctrlID, enable)
end

function uiControl:isActivated()
	return uiCtrl_isActivate(self.ctrlID)
end

function uiControl:setActivate( activate)
	return uiCtrl_setActivate(self.ctrlID, activate)
end

function uiControl:isSelected()
	return uiCtrl_isSelected(self.ctrlID)
end

function uiControl:setSelected( select)
    return uiCtrl_setSelected(self.ctrlID, select)
end

function uiControl:isVisible()
	return uiCtrl_isVisible(self.ctrlID)
end

function uiControl:SetTTMessage( pszString)
	uiCtrl_setToolTipMsg( self.ctrlID, pszString)
end

function uiControl:GetTTMessage( )
	return uiCtrl_getToolTipMsg( self.ctrlID)
end

-- r,g,b,a : 0~255
function uiControl:setColor( r, g, b, a)
	uiCtrl_setColor( self.ctrlID, r, g, b, a)
end

function uiControl:getColor( )
	local r,g,b,a = uiCtrl_getColor( self.ctrlID)

	return r,g,b,a
end

function uiControl:getMousePosition( )
	local x,y = uiCtrl_getMousePosition( self.ctrlID)
	return x,y
end

function uiControl:getMousePositionAcc( )
	local x,y = uiCtrl_getMousePositionAcc( self.ctrlID)
	return x,y
end

function uiControl:setMousePosition( x, y)
	uiCtrl_setMousePosition( self.ctrlID, x, y)
end

function uiControl:setMousePositionAcc( x, y)
	uiCtrl_setMousePositionAcc( self.ctrlID, x, y)
end

function uiControl:setFocus()
	uiCtrl_setFocus( self.ctrlID)
end

function uiControl:killFocus()
	uiCtrl_killFocus( self.ctrlID)
end

function uiControl:setInputDisable(disable)
    uiCtrl_setInputDisable(self.ctrlID, disable)
end

function uiControl:setVisible(visible)
	uiCtrl_setVisible(self.ctrlID, visible)
end
--[[
function uiControl (id)
	local self = { ctrlID = id}

	local new 	= function (o)
					local o = o or {}
					setmetatable(o,self)
					self.__index = self
					return o
				end

	local setID	= function(id)
					self.ctrlID = id
				end

	local setName = function( name)
					uiCtrl_setName( self.ctrlID, name)
				end

	local getName = function()
					local name = uiCtrl_getName( self.ctrlID)
					return name
				end

	local getClass = function()
					local class = uiCtrl_getClass( self.ctrlID)
					return class
				end

	return { new = new,
				setID = setID,
				getName = getName,
				setName = setName,
				getClass = getClass
				}
end

--]]
