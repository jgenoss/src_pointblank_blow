
-- ?좎뼵
--=======================================
-- file:  UIDefine.lua
-- author:    raja
-- created:   june 10 2010
-- descrip:   Define constant values for all scripts
--=======================================

UI_EVT_NORMAL = 0
UI_EVT_CREATE = 1
UI_EVT_DESTROY = 2
UI_EVT_SHOW = 3
UI_EVT_HIDE = 4
UI_EVT_ACTIVATE = 5
UI_EVT_INACTIVATE = 6
UI_EVT_ENABLE = 7
UI_EVT_DISABLE = 8
UI_EVT_SETFOCUS = 9
UI_EVT_KILLFOCUS = 10
UI_EVT_ENTERMOUSE = 11
UI_EVT_LEAVEMOUSE = 12

----------------------------------------------------
-- UI_EVT_CLICKED
--
-- Desc			Mouse LButton이 클릭되었을 때 전달된다.
--
-- Param1		Unused
-- Param2		Unused
UI_EVT_CLICKED = 13

----------------------------------------------------
-- UI_EVT_DBL_CLICKED
--
-- Desc			Mouse LButton이 Double Click되었을 때 전달된다.
--
-- Param1		Unused
-- Param2		Unused
UI_EVT_DBL_CLICKED = 14

----------------------------------------------------
-- UI_EVT_R_CLICKED
--
-- Desc			Mouse RButton이 Click 되었을 때 전달된다.
--
-- Param1		Unused
-- Param2		Unused
UI_EVT_R_CLICKED = 15

----------------------------------------------------
-- UI_EVT_R_DBL_CLICKED
--
-- Desc			Mouse RButton이 Double Click되었을 때 전달된다.
--
-- Param1		Unused
-- Param2		Unused
UI_EVT_R_DBL_CLICKED = 16

----------------------------------------------------
-- UI_EVT_M_CLICKED
--
-- Desc			Mouse MButton이 Click 되었을 때 전달된다.
--
-- Param1		Unused
-- Param2		Unused
UI_EVT_M_CLICKED = 17

----------------------------------------------------
-- UI_EVT_M_DBL_CLICKED
--
-- Desc			Mouse MButton이 Double Click되었을 때 전달된다.
--
-- Param1		Unused
-- Param2		Unused
UI_EVT_M_DBL_CLICKED = 18

----------------------------------------------------
-- UI_EVT_DRAGSTART UI_EVT_DRAGGING UI_EVT_DRAGEND
--
-- Desc			Dragging
-- Param1		x position of mouse
-- Param2		y position of mouse
UI_EVT_DRAGSTART = 19
UI_EVT_DRAGGING = 20
UI_EVT_DRAGEND = 21

UI_EVT_DRAGCANCEL = 22

UI_EVT_SIZING = 23

----------------------------------------------------
-- UI_EVT_SIZE
--
-- Desc			OnSize
-- Param1 cx
-- Param2 cy
----------------------------------------------------
UI_EVT_SIZE = 24

UI_EVT_MOVING = 25
UI_EVT_MOVE = 26

UI_EVT_BTN_PUSHED = 27

----------------------------------------------------
-- UI_EVT_IME
--
-- Desc			IME에서 상태가 변경되었을 때 Notify된다.
--
-- Param1		Event Code ( i3InputIME의 EDIT_STATE를 참조)
-- Param2		Unused
UI_EVT_IME = 28

----------------------------------------------------
-- UI_EVT_CHANGED
--
-- Desc			값이 변경되었을 때 전달된다.
--
-- Param1		Unused
-- Param2		Unused
UI_EVT_CHANGED = 29

----------------------------------------------------
-- UI_EVT_MODALRESULT
--
-- Desc			Modal Dialog의 동작 결과를 Owner Window에게 전달한다.
--
-- Param1		i3UIFrameWnd *
-- Param2		I3UI_MODALRESULT
UI_EVT_MODALRESULT = 30

------------------------------------------------------
-- UI_EVT_SCROLL
-- Desc				Scroll Position이 변경되었음을 알린다.
-- Param1			Current Position
-- Param2			Unused
UI_EVT_SCROLL = 31

------------------------------------------------------
-- UI_EVT_SELITEM
-- Desc				List Box의 아이템이 선택되었음을 알린다.
-- Param1			Item Idx
-- Param2			Unused
UI_EVT_SELITEM = 32


UI_EVT_NOTIFY = 33
UI_EVT_MOUSEWHEEL = 34
UI_EVT_UPDATE_STATE = 35
UI_EVT_TOOLTIP = 36
UI_EVT_KEYDOWN = 37
------------------------
UI_EVT_COMBOBOX_UNFOLDED = 38
UI_EVT_COMBOBOX_FOLDED = 39
UI_EVT_COMBOBOX_SELITEM = 40
UI_EVT_RADIO_CHECKED = 41
UI_EVT_RADIO_UNCHECKED = 42
UI_EVT_LVH_PUSHED = 43
------------------------
UI_EVT_RESERVED9 = 44

UI_EVT_RESERVED10 = 45
UI_EVT_RESERVED11 = 46
UI_EVT_RESERVED12 = 47
UI_EVT_RESERVED13 = 48
UI_EVT_RESERVED14 = 49
UI_EVT_RESERVED15 = 50
UI_EVT_RESERVED16 = 51
UI_EVT_RESERVED17 = 52
UI_EVT_RESERVED18 = 53
UI_EVT_RESERVED19 = 54


UI_EVT_MAXCOUNT = 55

-- 구 맥시멈값 49-->51(콤보박스2개추가)-->53(라디오버튼2개추가) -> 54(리스트뷰헤더 버튼누르기동작)  //임수빈.
