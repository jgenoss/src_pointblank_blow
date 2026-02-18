--DoubleText
require "Script.uiLua.uiControl"

class 'uiDoubleText' (uiControl)

function uiDoubleText:__init(id)
	uiControl.__init(self, id)
end

function uiDoubleText:recordInitialState()
	uiControl.recordInitialState(self)
end

function uiDoubleText:restoreInitialState()
	uiControl.restoreInitialState(self)

end

function uiDoubleText:getText(slot)
	local str = uiDoubleText_getText( self.ctrlID, slot)
	return str
end

function uiDoubleText:setText( str1, str2)
	uiDoubleText_setText(self.ctrlID, str1, str2)
end

