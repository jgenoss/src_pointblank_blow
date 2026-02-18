require "Script.uiLua.uiFrameWnd"

UI_MODALRESULT_YES = 0
UI_MODALRESULT_NO = 1
UI_MODALRESULT_CANCEL = 2

MSGBOX_TYPE_TIMEOUT = -1 --대기중, 사용자 입력 없음.
MSGBOX_TYPE_OK  = 0
MSGBOX_TYPE_YESNO = 1
MSGBOX_TYPE_YESNOCANCEL = 2

class 'MESSAGEBOX_INFO'

function MESSAGEBOX_INFO:__init()
	self._Type = MSGBOX_TYPE_OK
	self._title = "MessageBox"
	self._isHilight = false
	self._text = "Type String Here"
	self._btn1_text = "Yes"
	self._btn2_text = "No"
	self._btn3_text = "Cancel"
	self._eventReceiver = nil
end

class 'uiMessageBox' (uiFrameWnd)

function uiMessageBox:__init(id)
	uiFrameWnd.__init(self, id)
	
	self.initialized = false
	
	--Hide
	self:setEnable( 0)
end

function uiMessageBox:restoreInitialState()
	uiFrameWnd.restoreInitialState(self)

	uiMB_Clear( self.ctrlID)
end

function uiMessageBox:initialize( mbInfo)
	self.info = mbInfo
	
	self:setText( self.info._title);
	self:setTextAlign(1,0)	-- caption text align
	--self:setTextSpaceY(18)
	self:setTextColor(200, 200, 200, 255)
	
	uiMB_initialize( self.ctrlID, self.info._Type, self.info._text, self.info._eventReceiver, self.info._btn1_text, self.info._btn2_text, self.info._btn3_text)
	
	self. initialized = true
end

function uiMessageBox:Domodal( )
	if self.initialized then
		self:PopupWindow()
	else
		uiTrace("messageBox :"..self:getName().." was not initialized")
	end
end
