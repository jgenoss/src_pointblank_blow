require "Script.uiLua.uiCaption"

class 'uiListBox' (uiCaption)

function uiListBox:__init(id)
	uiCaption.__init(self, id)

	uiLB_init( self.ctrlID)
end

function uiListBox:restoreInitialState()
	uiCaption.restoreInitialState(self)

	uiLB_Clear( self.ctrlID)
end


function uiListBox:AddItem( pszItem)
	return uiLB_AddItem( self.ctrlID, pszItem)
end

function uiListBox:InsertItem( nIdx, pszItem)
	return uiLB_InsertItem( self.ctrlID, nIdx, pszItem)
end

function uiListBox:DeleteItem( nIdx)
	return uiLB_DeleteItem( self.ctrlID, nIdx)
end

function uiListBox:RemoveItem( pszItem)
	return uiLB_RemoveItem( self.ctrlID, pszItem)
end

function uiListBox:GetItem( nIdx)
	return uiLB_GetItem( self.ctrlID, nIdx)
end

function uiListBox:SetItem( nIdx, pszItem)
	uiLB_SetItem( self.ctrlID, nIdx, pszItem)
end

function uiListBox:GetCurSel()
	return uiLB_GetCurSel( self.ctrlID)
end

function uiListBox:SetCurSel( nIdx)
	uiLB_SetCurSel( self.ctrlID, nIdx)
end

function uiListBox:GetItemCount()
	return uiLB_GetItemCount( self.ctrlID)
end
