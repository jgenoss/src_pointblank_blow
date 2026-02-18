require "Script.uiLua.uiCaption"

class 'uiTab' (uiCaption)

function uiTab:__init(id)
	uiCaption.__init(self, id)
	uiTab_init( self.ctrlID)
end

function uiTab:setTabState(state)
    uiTab_setTabState( self.ctrlID, state)
end

function uiTab:setPush(enable)
    uiTab_selected( self.ctrlID, enable)
end
