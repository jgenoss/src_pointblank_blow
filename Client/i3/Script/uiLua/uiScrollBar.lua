require "Script.uiLua.uiControl"

class 'uiScrollBar' (uiControl)

function uiScrollBar:__init(id)
	uiControl.__init(self, id)
end

function uiScrollBar:getScrollPos()
	return uiScroll_getScrollPos(self.ctrlID)
end

function uiScrollBar:setScrollPos( Value )
	uiScroll_setScrollPos(self.ctrlID, Value)
end

function uiScrollBar:setScrollRange( minValue, maxValue )
	uiScroll_setScrollRange(self.ctrlID, minValue, maxValue)
end

function uiScrollBar:setScrollTick( Value )
	uiScroll_setScrollTick(self.ctrlID, Value)
end

function uiScrollBar:setScrollPage( Value )
	uiScroll_setScrollPage(self.ctrlID, Value)
end
