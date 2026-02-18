require "Script.uiLua.uiCaption"

class 'uiComboBox' (uiCaption)

function uiComboBox:__init(id)
	uiCaption.__init(self, id)
end

function uiComboBox:restoreInitialState()
	uiControl.restoreInitialState(self)

	uiCB_Clear( self.ctrlID)
end

-- static-like functions
function uiComboBox:SetItems( pszItems)
	uiCB_SetItems( self.ctrlID, pszItems)
end

function uiComboBox:GetItems()
	return uiCB_GetItems( self.ctrlID)
end

function uiComboBox:SetCurSel(idx)
	uiCB_SetCurSel( self.ctrlID, idx)
end

function uiComboBox:GetCurSel()
	return uiCB_GetCurSel( self.ctrlID)
end

function uiComboBox:GetCurSelString()
	return uiCB_GetCurSelString( self.ctrlID)
end

function uiComboBox:SetEditTextColor(r, g, b)
	uiCB_SetEditTextColor( self.ctrlID, r, g, b)
end

function uiComboBox:SetListTextColor( r, g, b)
	uiCB_SetListTextColor( self.ctrlID, r, g, b)
end

function uiComboBox:SetEditBoxReadOnly(readonly)
    uiCB_SetEditBoxReadOnly( self.ctrlID, readonly)
end

function uiComboBox:isListBoxUnfolded()
	return uiCB_isListBoxUnfolded(self.ctrlID)
end

function uiComboBox:FoldListBox()
	return uiCB_FoldListBox(self.ctrlID)
end

function uiComboBox:UnfoldListBox()	
	return uiCB_UnfoldListBox(self.ctrlID)
end

function uiComboBox:SetEditBoxText( str )	
	uiCB_SetEditBoxText( self.ctrlID, str )
end

