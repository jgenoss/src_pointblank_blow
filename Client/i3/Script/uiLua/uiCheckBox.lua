-- 체크박스
require "Script.uiLua.uiCaption"

class 'uiCheckBox' (uiCaption)

function uiCheckBox:__init(id)
	uiCaption.__init(self, id)
end

function uiCheckBox:OnClicked()
	uiPlaySound("Sound\\UI_Button_Click.wav")
end

function uiCheckBox:getChecked()
	return uiCheck_getChecked(self.ctrlID)
end

function uiCheckBox:setChecked(sel)
	return uiCheck_setChecked(self.ctrlID, sel)
end