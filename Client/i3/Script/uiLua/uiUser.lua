require "Script.uiLua.uiControl"

class 'uiUser' (uiControl)

function uiUser:__init(id)
	uiControl.__init(self,id)
end

function uiUser:OnCommand( action)
	uiCtrl_OnCommand( self.ctrlID, action)
end

function uiUser:OnClicked()

end

function uiUser:OnEnterMouse()

end

function uiUser:SetCurrentShape(idx)
    uiUser_SetCurrentShape( self.ctrlID, idx)
end


--[[-------------------------------------------------
void setImageIndex( i3UIUserControl* pCtrl, string szData)
: ListView에서 Cell의 내용을 업데이트하기 위해 호출됩니다.
: 이 함수를 재정의하여 Cell 안에서의 Template의 동작을 정의합니다.

- pCtrl: Cell의 Child Control
- szData: Cell의 Data( LVITEM_DATA 참고)

ex)
function imgList_Rank:setImageIndex( pCtrl, i)
	uiCtrl_setVisibleShape( pCtrl, 0, -1)	-- set all shapes invisible
	uiCtrl_setVisibleShape( pCtrl, 1, i)	-- set single shape visible
end
---------------------------------------------------]]
function uiUser:setImageIndex( pCtrl, i)
    uiCtrl_setVisibleShape( pCtrl, 0, -1)	-- set all shapes invisible
	uiCtrl_setVisibleShape( pCtrl, 1, i)	-- set single shape visible
end


class "User_Color" (uiUser)


function User_Color:__init(id)
    uiUser.__init(self,id)
end

class "User_Crosshair" (uiUser)

function User_Crosshair:__init(id)
    uiUser.__init(self,id)
end

class "User_Filtering" (uiUser)

function User_Filtering:__init(id)
	uiUser.__init(self, id)
end

