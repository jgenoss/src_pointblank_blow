-- 버튼
require "Script.uiLua.uiCaption"

UIBUTTON_TEXTCOLOR_TYPE = 
{
	IDLE 		= 0,
	ONMOUSE 	= 1,
	SELECTED 	= 2,
	DISABLE		= 3,
}

class 'uiButton' (uiCaption)

function uiButton:__init(id)
	uiCaption.__init(self, id)
end

function uiButton:OnCommand( action)
	uiCtrl_OnCommand( self.ctrlID, action)
end

function uiButton:OnClicked()
	--uiPlaySound("Sound\\UI_Button_Click.wav")
end

function uiButton:OnEnterMouse()
	--uiPlaySound("Sound\\UI_Mouse_Over.wav")
end

function uiButton:skipUnselect( bEnable)
	uiButton_skipUnselect(self.ctrlID, bEnable)
end

function uiButton:isSkipUnselect()
	return uiButton_isSkipUnselect(self.ctrlID)
end

function uiButton:GetButtonTextColor( btnColorType )
	return uiButton_GetButttonTextColor( self.ctrlID, btnColorType )
end

function uiButton:SetButtonTextColor( btnColorType, r, g, b, a )
	uiButton_SetButttonTextColor( self.ctrlID, btnColorType, r, g, b, a )
end

function uiButton:SetButtonTextColor4Byte( btnColorType, rgba )
	uiButton_SetButttonTextColor4Byte( self.ctrlID, btnColorType, rgba )
end