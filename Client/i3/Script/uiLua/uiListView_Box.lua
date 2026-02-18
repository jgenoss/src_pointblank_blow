require "Script.uiLua.uiControl"

class 'uiListView_Box' (uiControl)

function uiListView_Box:__init(id)
	uiControl.__init(self, id)
end

function uiListView_Box:restoreInitialState()
	uiControl.restoreInitialState(self)

	uiLV_UpdateToEditMode( self.ctrlID)		--reset
end

--ListView를 다루는데 필요한 인덱스는 모두 0부터 시작!


--[[-------------------------------------------------
LVITEM_DATA* _NewItem( void)
---------------------------------------------------]]
function uiListView_Box:_NewItem()
	return uiLV_NewItem( self.ctrlID)
end



--[[-------------------------------------------------
void _AddItem( LVITEM_DATA* pItem)
---------------------------------------------------]]
function uiListView_Box:_AddItem( pItem)
	uiLV_AddItem( self.ctrlID, pItem)
end



--[[-------------------------------------------------
void _InsertItem( LVITEM_DATA* pItem, int nIdx)
---------------------------------------------------]]
function uiListView_Box:_InsertItem( pItem, nIdx)
	uiLV_InsertItem( self.ctrlID, nIdx, pItem)
end


--[[-------------------------------------------------
void _SetItemCell( LVITEM_DATA* pItem, int nCellIdx, string szData)
---------------------------------------------------]]
function uiListView_Box:_SetItemCell( pItem, nCellIdx, szData)
	local result, celltype = uiLV_SetItemCell( self.ctrlID, pItem, nCellIdx, szData)
	if( result == false) then
		error( "SetCell Failed At Index "..nCellIdx..": cannot find '"..szData.."' in type:"..celltype, 2)
	end
end



--[[-------------------------------------------------
void _SetCells( LVITEM_DATA* pItem, string szData0, ...)
: pItem의 모든 Cell들의 값을 설정한다.
---------------------------------------------------]]
function uiListView_Box:_SetCells( pItem, ...)
	local count = self:GetCellCount()
	local arg = {...}
	arg.n = select( "#", ...)

	if arg.n < count then
		error( "Not enough arguments: There are "..count.." Cells", 3)
	elseif arg.n > count then
		error( "Too many arguments: There are "..count.." Cells", 3)
	end

	local result, idx, celltype = uiLV_SetCells( self.ctrlID, pItem, ...)
	if( result == false) then
		error( "SetCell Failed At Index "..idx..": cannot find '"..select(idx+1,...).."' in type:"..celltype, 3)
	end
end



--[[-------------------------------------------------
int or string _StrToEnum_TextAlignX( string strTextAlign)
:
---------------------------------------------------]]
function _StrToEnum_TextAlignX( strTextAlign)
	assert( type(strTextAlign) == "string")

	if strTextAlign == "Left" then
		return 0
	elseif strTextAlign == "Center" then
		return 1
	elseif strTextAlign == "Right" then
		return 2
	else
		return " parameter should be one of the followings: 'Left', 'Center', or 'Right'"
	end
end



--[[-------------------------------------------------
int or string _StrToEnum_TextAlignY( string strTextAlign)
:
---------------------------------------------------]]
function _StrToEnum_TextAlignY( strTextAlign)
	assert( type(strTextAlign) == "string")

	if strTextAlign == "Top" then
		return 0
	elseif strTextAlign == "Middle" then
		return 1
	elseif strTextAlign == "Bottom" then
		return 2
	else
		return " parameter should be one of the followings: 'Top', 'Middle', or 'Bottom'"
	end
end



--[[-------------------------------------------------
int or string _StrToEnum_CommandScope( string strCommandScope)
:
---------------------------------------------------]]
function _StrToEnum_CommandScope( strCommandScope)
	assert( type(strCommandScope) == "string")	--여기서 assert를 거는게 의미가 있나?

	if strCommandScope == "All" then
		return 0	--ECOMMAND_SCOPE_ALL
	elseif strCommandScope == "Header" then
		return 1	--ECOMMAND_SCOPE_HEADER
	elseif strCommandScope == "Item" then
		return 2	--ECOMMAND_SCOPE_ITEM
	elseif strCommandScope == "Row" then
		return 3	--ECOMMAND_SCOPE_ROW
	elseif strCommandScope == "Column" then
		return 4	--ECOMMAND_SCOPE_COLUMN
	elseif strCommandScope == "Cell" then
		return 5	--ECOMMAND_SCOPE_CELL
	elseif strCommandScope == "Data" then
		return 6	--ECOMMAND_SCOPE_DATA
	elseif strCommandScope == "Box" then
		return 7	--ECOMMAND_SCOPE_BOX
	else
		return " parameter should be one of the followings: 'All', 'Header', 'Item', 'Row', 'Column', 'Cell', 'Data', 'Box'"
	end
end



-------------------------------
--[[-------------------------------------------------
---------------------------------------------------]]
function uiListView_Box:SetComboItems( nCol, pszItems)
	uiLV_SetComboItems( self.ctrlID, nCol, pszItems)
end



--[[-------------------------------------------------
---------------------------------------------------]]
function uiListView_Box:SetButtonName( nCol, pszName)
	uiLV_SetButtonName( self.ctrlID, nCol, pszName)
end
