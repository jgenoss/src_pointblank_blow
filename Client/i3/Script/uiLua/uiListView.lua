
--[[-------------------------------------------

-외부에서 호출하면 안되는 함수들은
uiListView_Box.lua에 숨겨놓고
외부에 보여지는 함수들은 여기에 정의한다.

-여기에 있는 함수들은
직접 Lua Glue함수를 호출하거나
uiListView_Box.lua에 있는 루아함수를 통해
Lua Glue함수를 호출한다.

---------------------------------------------]]

require "Script.uiLua.uiListView_Box"


--[[-------------------------------------------------
void AddData( string szData0, ...)
---------------------------------------------------]]
function uiListView_Box:AddData( ...)
	local pItem = self:_NewItem()
	self:_SetCells( pItem, ...)
	self:_AddItem( pItem)
end


--[[-------------------------------------------------
void InsertData( int nIdx, string szData0, ...)
---------------------------------------------------]]
function uiListView_Box:InsertData( nIdx, ...)
	local pItem = self:_NewItem()
	self:_SetCells( pItem, ...)
	self:_InsertItem( pItem, nIdx)
end


--[[-------------------------------------------------
void DeleteData( int nIdx)
---------------------------------------------------]]
function uiListView_Box:DeleteData( nIdx)
	uiLV_DeleteItem( self.ctrlID, nIdx)
end

--[[-------------------------------------------------
void DeleteAllDatas( void)
---------------------------------------------------]]
function uiListView_Box:DeleteAllDatas()
	uiLV_DeleteAllItems(self.ctrlID)
end

--[[-------------------------------------------------
szData0, ... GetData( int nIdx)
---------------------------------------------------]]
function uiListView_Box:GetData( nIdx)
	return uiLV_GetData( self.ctrlID, nIdx)
end


--[[-------------------------------------------------
void SetCell( int nDataIdx, int nCellidx, string szData)
---------------------------------------------------]]
function uiListView_Box:SetCell( nDataIdx, nCellIdx, szData)
	if type( nDataIdx) ~= "number" then
		error( "SetCell: First parameter should be a number(data index)", 2)
	end

	if type( nCellIdx) ~= "number" then
		error( "SetCell: Second parameter should be a number(cell index)", 2)
	end

	--[[if type( szData) ~= "string" then
		error( "SetCell: Third parameter should be a string(data)", 2)
	end--]]

	uiLV_SetCell( self.ctrlID, nDataIdx, nCellIdx, szData)
end


--[[-------------------------------------------------
void SetCellDataGroup( int nDataIdx, int nCellidx, string szData)
---------------------------------------------------]]
function uiListView_Box:SetCellDataTable( nDataIdx, nCellIdx, szTable)
	if type( nDataIdx) ~= "number" then
		error( "SetCell: First parameter should be a number(data index)", 2)
	end

	if type( nCellIdx) ~= "number" then
		error( "SetCell: Second parameter should be a number(cell index)", 2)
	end

	uiLV_SetCellDataTable( self.ctrlID, nDataIdx, nCellIdx, szTable)
end


--[[-------------------------------------------------
void SetCellDCStyle( int nDataIdx, int nCellidx, int nControlIdx, int style)
---------------------------------------------------]]
function uiListView_Box:SetCellDCStyle( nDataIdx, nCellIdx, nControlIdx, nStyle)
	uiLV_SetCellDCStyle( self.ctrlID, nDataIdx, nCellIdx, nControlIdx, nStyle)
end


--[[-------------------------------------------------
void SetCellDCPos( int nDataIdx, int nCellidx, int nControlIdx, int x, int y)
---------------------------------------------------]]
function uiListView_Box:SetCellDCPos( nDataIdx, nCellIdx, nControlIdx, x, y)
	uiLV_SetCellDCPos( self.ctrlID, nDataIdx, nCellIdx, nControlIdx, x, y)
end


--[[-------------------------------------------------
void SetCells( int nDataIdx, ...)
---------------------------------------------------]]
function uiListView_Box:SetData( nDataIdx, ...)
	local numOfArgs = select( "#", ...)

	local result, msg = uiLV_SetData( self.ctrlID, nDataIdx, numOfArgs, ...)

	if result ~= true then
		error(msg, 2)
	end
end


--[[-------------------------------------------------
int GetDatacount( void)
---------------------------------------------------]]
function uiListView_Box:GetDataCount()
	return uiLV_GetDataCount( self.ctrlID)
end



--[[-------------------------------------------------
int GetItemCount( void)
---------------------------------------------------]]
function uiListView_Box:GetItemCount()
	return uiLV_GetItemCount( self.ctrlID)
end

function uiListView_Box:GetItem(idx)
    return uiLV_GetItem( self.ctrlID, idx)
end



--[[-------------------------------------------------
int GetCellCount( void)
---------------------------------------------------]]
function uiListView_Box:GetCellCount()
	local count = uiLV_GetCellCount( self.ctrlID)
	if count < 0 then
		error( "CellCount < 0(This error may occur due to 'No Binded Item'", 2)
	end
	return count
end



--[[-------------------------------------------------
table GetCurSel( void)
: 현재 선택되어 있는 Data의 인덱스들을 테이블로 반환한다.
: 아무런 아이템도 선택되어 있지 않다면 {nil}을 반환한다.
---------------------------------------------------]]
function uiListView_Box:GetCurSel()
	local t = {uiLV_GetCurSel( self.ctrlID)}
	return t
end



--[[-------------------------------------------------
void SetCurSel( int idx, bool bEnable)
: enable인자를 추가했으며, 넣지 않으면 true로 작동한다. (11.07.17.수빈)
---------------------------------------------------]]

function uiListView_Box:SetCurSel( idx, bEnable)
	uiLV_SetCurSel( self.ctrlID, idx, bEnable)
end

--[[-------------------------------------------------
void UnsetCurSel( int idx, bool bEnable)
: 현재 선택된 셀을 리셋한다. (사실상, 모든 셀을 리셋하는 것과 같으므로 함수명을 이렇게 처리) (11.07.17.수빈)
---------------------------------------------------]]

function uiListView_Box:UnsetCurSel()
	uiLV_UnsetCurSel(self.ctrlID)
end

--[[-------------------------------------------------
void ScrollTo( int idx)
: idx번째 Data가 Box의 맨 위로 오도록 스크롤
: UpdateToPreviewMode() 이후에 호출해야 합니다.
---------------------------------------------------]]
function uiListView_Box:ScrollTo( idx)
	uiLV_ScrollToIndex( self.ctrlID, idx)
end



--[[-------------------------------------------------
void SetTextAlign( string szXAlign, string szYAlign, string szScope, int or string param1, int param2)
: 특정 Cell들의 Text Align을 설정합니다.
szXAlign = "Left", "Center", "Right"
szYAlign = "Top", "Middle", "Bottom"
szScope = "All", "Header", "Item", "Row", "Column", "Cell", "Data", "Box"
	All: 	"Header" + "Item"
	Header: 헤더만
	Item: 	현재 박스에 추가되어 있는 모든 data
	Row: 	선택한 행( param1 = row index)
	Column: 선택한 열( param1 = col index)
	Cell: 	선택한 셀( param1 = row index, param2 = col index)
	Data: 	선택한 data. (Ex: param1로 "ABC"를 주면 "ABC"라는 데이터들에만 적용된다. )
	Box: Box의 Default Align으로 설정하여 이후에 추가되는 아이템들에도 적용된다.
param1 = optional value depends on szScope
param2 = optional value depends on szScope

: Example
---------------------------------------------------]]
function uiListView_Box:SetTextAlign( szXAlign, szYAlign, szScope, param1, param2)
	local nXAlign = _StrToEnum_TextAlignX( szXAlign)
	if type( nXAlign) == "string" then
		error( "First" .. nXAlign, 2)
	end

	local nYAlign = _StrToEnum_TextAlignY( szYAlign)
	if type( nYAlign) == "string" then
		error( "Second" .. nYAlign, 2)
	end


	local nScope = _StrToEnum_CommandScope( szScope)
	if type( nScope) == "string" then
		error( "Third" .. nScope, 2)
	end

	uiLV_SetTextAlign( self.ctrlID, nXAlign, nYAlign, nScope, param1, param2)
end



--[[-------------------------------------------------
void SetTextColor( int r, int g, int b, string szScope, int param1, int param2)
: 특정 Cell들의 Text Color를 설정합니다.
r,g,b = integer between 0 and 255
szScope = [See SetTextAlign()]
param1 = optional value depends on szScope
param2 = optional value depends on szScope

: Example
---------------------------------------------------]]
function uiListView_Box:SetTextColor( r, g, b, szScope, param1, param2)

	if type( r) ~= "number" or r < 0 or r > 255 then
		error( "First parameter should be a number between 0 and 255", 2)
	end

	if type( g) ~= "number" or g < 0 or g > 255 then
		error( "Second parameter should be a number between 0 and 255", 2)
	end

	if type( b) ~= "number" or b < 0 or b > 255 then
		error( "Third parameter should be a number between 0 and 255", 2)
	end

	local nScope = _StrToEnum_CommandScope( szScope)
	if type( nScope) == "string" then
		error( "Fourth" .. nScope, 2)
	end

	uiLV_SetTextColor( self.ctrlID, r, g, b, nScope, param1, param2)
end

function uiListView_Box:SetTextColor4Byte( rgba, szScope, param1, param2)

	local nScope = _StrToEnum_CommandScope( szScope)

	if type( nScope) == "string" then
		error( "Fourth" .. nScope, 2)
	end

	uiLV_SetTextColor4Byte( self.ctrlID, rgba, nScope, param1, param2 )

end

function uiListView_Box:RestoreTextColor(szScope, param1, param2)

	local nScope = _StrToEnum_CommandScope( szScope)

	if type( nScope) == "string" then
		error( "Fourth" .. nScope, 2)
	end

	uiLV_RestoreTextColor( self.ctrlID, nScope, param1, param2)
	
end

--[[-------------------------------------------------

---------------------------------------------------]]
function uiListView_Box:SetHeader( ...)
	local count = self:GetCellCount()
	local arg = {...}
	arg.n = select( "#", ...)

	if arg.n < count then
		error( "Not enough arguments: There are "..count.." Cells", 2)
	elseif arg.n > count then
		error( "Too many arguments: There are "..count.." Cells", 2)
	end

	uiLV_SetHeader( self.ctrlID, ...)
end



--[[-------------------------------------------------
void SetChildRect( int colIdx, int l, int t, int w, int h)
: 특정 열에 있는 모든 Cell들에 대하여 그 안에 배치될 이미지, 템플릿의 Rect를 설정합니다.
: CellType이 Text인 경우 적용되지 않습니다. SetTextAlign 사용
l = left
t = top
w = width
h = height
---------------------------------------------------]]
function uiListView_Box:SetChildRect( colIdx, l, t, w, h)
	uiLV_SetChildRect( self.ctrlID, colIdx, l, t, w, h)
end



--[[-------------------------------------------------
i3UIControl* GetControl( int nItemIdx, int nCellIdx)
: returns a pointer to the child control which belongs to the specified cell
: returns NULL if nRow and nCol values are out of range
: returns NULL if specified cell's type is not user template
외부로 노출하지 않는게 좋을 듯..
---------------------------------------------------]]
function uiListView_Box:GetControl( nItemIdx, nCellIdx)
	return uiLV_GetInstancePointer( self.ctrlID, nItemIdx, nCellIdx)
end



--[[-------------------------------------------------

				 Event Handler

---------------------------------------------------]]

--[[-------------------------------------------------
void OnButtonClicked( int row, int col)
---------------------------------------------------]]
function uiListView_Box:OnButtonClicked( row, col)
	uiPlaySound("Sound\\UI_Button_Click.wav")
end



--[[-------------------------------------------------
void OnItemClicked( int row)
---------------------------------------------------]]
function uiListView_Box:OnItemClicked( row)
	uiPlaySound("Sound\\UI_Mouse_Over.wav")
end



--[[-------------------------------------------------
void UpdateToPreviewMode( void)
: ListView가 ListView에 추가된 실제 Data들을 로딩할 수 있도록
: 반드시 호출해 주어야 합니다.
: 이 함수를 통해 스크롤바가 동적으로 추가됩니다.
---------------------------------------------------]]
function uiListView_Box:UpdateToPreviewMode()
	local result = uiLV_UpdateToPreviewMode( self.ctrlID)
	if result == false then
		error( "No Binded Item or Inconsistent Cell Type", 2)
	end
	--
end

function uiListView_Box:UpdateToEditMode()
    local result = uiLV_UpdateToEditMode( self.ctrlID)
    if result == false then
        Error( "No binded item or inconsistent cell type", 2)
    end
end


--[[-------------------------------------------------
void setTemplateName( int nCol, string szName)
: ListView에서 특정 열의 CellType이 Template인 경우
: 사용할 Template의 이름을 지정합니다.
---------------------------------------------------]]
function uiListView_Box:setTemplateName( nCol, szName)
	uiLV_SetTemplateName( self.ctrlID, nCol, szName)
end

--[[-------------------------------------------------
void setTemplateResource( int nCol, table t)
: ListView에서 특정 열의 CellType이 Template인 경우
: 경우 사용할 여러개의 Template을 지정합니다.
---------------------------------------------------]]
function uiListView_Box:setTemplateResource( nCol, szTemplates)
  uiLV_SetTemplateResource( self.ctrlID, nCol, szTemplates)
end


--[[-------------------------------------------------
void setUserData( int nRow, int nCol, arglist)
: User Template인 경우 ...으로 넘어온 data를 lua의 해당 인스턴스에 설정??
: 안될것같은데..
---------------------------------------------------]]
function uiListView_Box:setUserData( nRow, nCol, ...)

end

function uiListView_Box:IsMouseOverItem( )
	return uiLV_GetIsMouseOver( self.ctrlID)
end

function uiListView_Box:SetMouseOverItem( bOver)
	uiLV_SetMouseOver( self.ctrlID, bOver)
end

function uiListView_Box:ResetButtonState( bOver)
	uiLV_ResetButtonState( self.ctrlID)
end

function uiListView_Box:GetShapeValueIdx()
	return uiLV_GetShapeValueIdx( self.ctrlID)
end

function uiListView_Box:GetShapeValueString()
	return uiLV_GetShapeValueString( self.ctrlID)
end

function uiListView_Box:SetDataEnable( nIdx, bEnable)
	uiLV_SetDataEnable( self.ctrlID, nIdx, bEnable)
end

function uiListView_Box:SetCellCallback( nIdx, nCellIdx, func)
    uiLV_SetCellCallback( self.ctrlID, nIdx, nCellIdx, func)
end

function uiListView_Box:SetCellToolTip( nIdx, nCellIdx, tooltip)
    uiLV_SetToolTip( self.ctrlID, nIdx, nCellIdx, tooltip)
end

function uiListView_Box:SetUserCallback(func)
	uiLV_SetUserCallback( self.ctrlID, func)
end
