require "Script.uiLua.uiControl"

class 'uiSpin' (uiControl)

function uiSpin:__init(id)
	uiControl.__init(self, id)

	uiSpin_init( self.ctrlID)
end

function uiSpin:GetCurPos()
	return uiSpin_getCurPos( self.ctrlID)
end
