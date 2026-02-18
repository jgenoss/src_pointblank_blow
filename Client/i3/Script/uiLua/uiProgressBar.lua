require "Script.uiLua.uiCaption"

class 'uiProgressBar' (uiCaption)

function uiProgressBar:__init(id)
	uiCaption.__init(self, id)

	self.pbarPos = 0;
end

function uiProgressBar:SetValue( fVal)
	uiPBar_setValue( self.ctrlID, fVal)
end

function uiProgressBar:GetValue( )
	return uiPBar_getValue( self.ctrlID)
end

-- nonzero: true, zero: false
function uiProgressBar:SetReadOnly( nReadOnly)
	uiPBar_setReadOnly( self.ctrlID, nReadOnly)
end

-- 0: false, 1: true
function uiProgressBar:GetReadOnly( )
	return uiPBar_getReadOnly( self.ctrlID)
end

function uiProgressBar:recordInitialState()
	uiCaption.recordInitialState( self)

	self.pbarPos = self:GetValue()
end

function uiProgressBar:restoreInitialState()
	uiCaption.restoreInitialState( self)

	self:SetValue( self.pbarPos)
end
