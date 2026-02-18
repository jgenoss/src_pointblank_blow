class 'eventHandler'

function eventHandler:__init()
	self.events = self.events or {}
end

function eventHandler:registerEvent(evt)
	self.events = self.events or {}
	self.events[evt] = true
end

function eventHandler:registerEventHandler( evt, handler)
	assert( handler ~= nil)
	assert( type(handler) == "function")

	self.handlers = self.handlers or {}
	self.handlers[evt] = handler
end

function eventHandler:UnRegisterEventHandler( evt)
	if self.handlers ~= nil then
		if self.handlers[evt] ~= nil then
			self.handlers[evt] = nil
		end
	end
end

function eventHandler:OnEvent(event, ...)
	--if self.events ~= nil then
	--end


	--if self.events == nil then do return end end
	--if self.handlers == nil then do return end end
	--if self.handlers[evt] == nil then do return end end
	--if self.events[evt] == nil then do return end end
end


-- name of ToolTip Control in string
function eventHandler.SetToolTip( scene, pszToolTip)
	--헉.. scene에는 ctrlID가 없나요??
	uiS_setToolTip( scene.ctrlID, pszToolTip)
end
