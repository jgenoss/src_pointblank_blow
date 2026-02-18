require "Script.uiLua.uiCaption"

class 'uiFrameWnd' (uiCaption)

function uiFrameWnd:__init(id)
	uiCaption.__init(self, id)
end

function uiFrameWnd:PopupWindow()
	uiDoModal(self.ctrlID)
end

function uiFrameWnd:CloseWindow( MODALRESULT)
	uiEndModal(self.ctrlID, self.ctrlID, MODALRESULT)
end

function uiFrameWnd:setUpdateFocusWhenEnable( enable)
	uiFrameWnd_setUpdateFocusWhenEnable( self.ctrlID, enable)
end