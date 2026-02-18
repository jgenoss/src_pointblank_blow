require "Script.uiLua.uiCaption"

class 'uiToolTip' (uiCaption)

function uiToolTip:__init(id)
	uiCaption.__init(self, id)

	--시작시 툴팁은 안보임
	self:setEnable(0)
end

function uiToolTip:restoreInitialState()
	uiCaption.restoreInitialState( self)

	self:setEnable(0)
end
