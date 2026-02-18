require "Script.uiLua.uiCaption"

class 'uiEditBox' (uiCaption)

function uiEditBox:__init(id)
	uiCaption.__init(self, id)
end


function uiEditBox:setEditEnable(bable)
	uiEdit_setReadOnly( self.ctrlID, bable)
end

function uiEditBox:setAlphaNumericOnly(bOnly)
	uiEdit_setAlphaNumericOnly(self.ctrlID,bOnly)
end

function uiEditBox:setPasswordmodeEnable(bEnable)
	uiEdit_setPasswordmodeEnable(self.ctrlID,bEnable)
end

function uiEditBox:getPassword()
	return uiEdit_getPassword(self.ctrlID)
end

function uiEditBox:setChatEnable(bEnable)
	uiEdit_setChatEnable(self.ctrlID, bEnable)
end

function uiEditBox:setChatViewEnable(bEnable)
	uiEdit_setChatViewEnable(self.ctrlID, bEnable)
end


function uiEditBox:getImeTextLen()
	return uiEdit_getImeTextLen( self.ctrlID )
end