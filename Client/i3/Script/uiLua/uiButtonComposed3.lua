-- 4스테이트 3조각 버튼
require "Script.uiLua.uiCaption"

class 'uiButtonComposed3' (uiCaption)

function uiButtonComposed3:__init(id)
	uiCaption.__init(self, id)
end

function uiButtonComposed3:OnCommand( action)
	uiCtrl_OnCommand( self.ctrlID, action)
end

function uiButtonComposed3:OnClicked()
	--uiPlaySound("Sound\\UI_Button_Click.wav")
end

function uiButtonComposed3:OnEnterMouse()
	--uiPlaySound("Sound\\UI_Mouse_Over.wav")
end

function uiButtonComposed3:skipUnselect( bEnable)
	uiButtonComposed3_skipUnselect(self.ctrlID, bEnable)
end

function uiButtonComposed3:isSkipUnselect()
	return uiButtonComposed3_isSkipUnselect(self.ctrlID)
end

function uiButtonComposed3:GetButtonTextColor( btnColorType )
	return uiButtonComposed3_GetButttonTextColor( self.ctrlID, btnColorType )
end

function uiButtonComposed3:SetButtonTextColor( btnColorType, r, g, b, a )
	uiButtonComposed3_SetButttonTextColor( self.ctrlID, btnColorType, r, g, b, a )
end

function uiButtonComposed3:SetButtonTextColor4Byte( btnColorType, rgba )
	uiButtonComposed3_SetButttonTextColor4Byte( self.ctrlID, btnColorType, rgba )
end