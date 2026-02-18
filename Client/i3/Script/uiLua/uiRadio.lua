-- ¶óµð¿À
require "Script.uiLua.uiCaption"

class 'uiRadio' (uiCaption)

function uiRadio:__init(id)
	uiCaption.__init(self, id)
end

function uiRadio:setCheck(bCheck,bNotifyEnable)
	uiRadio_setCheck(self.ctrlID, bCheck, bNotifyEnable)
end

function uiRadio:isChecked()
	return uiRadio_isChecked(self.ctrlID)
end
