#ifndef _GuiNotifyReceiver_h
#define _GuiNotifyReceiver_h

#if legacy_gui_function
#include "GuiDef.h"
#include "GameCharaDef.h"
#include "GameGUI.h"

// OnLinkControl Define
// 기존 Stage 들어낼때, _EX 부분 지울것

#define FINDCTRL_CRC

#if defined( FINDCTRL_CRC)

struct STRCINFO
{
	UINT32			crc;
	i3GameNode *	pCtrl;
};

template<> struct i3::less<STRCINFO*> 
{
	bool operator()( const STRCINFO * p1, const STRCINFO * p2) const { return p1->crc < p2->crc; }
	bool operator()( UINT32 crc1, const STRCINFO * p2) const { return crc1 < p2->crc; }
	bool operator()( const STRCINFO * p1, UINT crc2) const { return p1->crc < crc2; }
};

static i3GameNode * FindControl( const i3::vector_set<STRCINFO*>& List, const char * pszCtrlName)
{
	UINT32 CRC = CRC32( 0xFFFFFFFF, (UINT8 *) pszCtrlName, i3::generic_string_size( pszCtrlName));

	i3::vector_set<STRCINFO*>::const_iterator it = List.find_other(CRC);

	if( it == List.end() )
		return nullptr;
	else
	{
		STRCINFO * pCtl = *it; 
		return pCtl->pCtrl;
	}
}

//	press button 의 state에 따른 gui shape 번호
#define GUI_SHAPE_PRESS_BUTTON_NORMAL		GUI_SHAPE_OFFSET
#define GUI_SHAPE_PRESS_BUTTON_ONMOUSE		GUI_SHAPE_OFFSET + 1
#define GUI_SHAPE_PRESS_BUTTON_CLICKED		GUI_SHAPE_OFFSET + 2
#define GUI_SHAPE_PRESS_BUTTON_DISABLED		GUI_SHAPE_OFFSET + 3

//	stroke button 의 state에 따른 gui shape 번호
#define GUI_SHAPE_STROKE_BUTTON_NORMAL		GUI_SHAPE_OFFSET
#define GUI_SHAPE_STROKE_BUTTON_ONMOUSE		GUI_SHAPE_OFFSET + 1
#define GUI_SHAPE_STROKE_BUTTON_CLICKED		GUI_SHAPE_OFFSET + 2
#define GUI_SHAPE_STROKE_BUTTON_DISABLED	GUI_SHAPE_OFFSET + 2


//#define LINK_CONTROL_EX(str,control,class,id)	control = (class*)FindControl( plist, str); I3ASSERT( control != nullptr); control->SetControlID(id);
template<typename T> inline  void LinkControlEx(const i3::vector_set<STRCINFO*>& list, const char * str, T*& control,INT32 id)
{
	// control = dynamic_cast<i3GuiObjBase*>(FindControl( plist, str)); 
	control = (T*)(FindControl( list, str)); 
	if( control == nullptr)
	{
		char szTemp[256] = "";
		i3::snprintf( szTemp, 256, "Cannot find a Gui Control (%s)", str);
		I3PRINTLOG(I3LOG_FATAL,  szTemp);	// Control이 없을 경우 어차피 이걸 사용하는 부분에서 크래시나므로 여기서 미리 크래시시킴
	}
	I3ASSERT( control != nullptr);
	if(control != nullptr) {
		control->SetControlID(id);
	}
}
#endif

//	press button
#define INIT_BUTTON_EX(control)					{control->SetUserDefine( I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_PRESS_BUTTON_NORMAL); \
												 control->SetUserDefine( I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_PRESS_BUTTON_ONMOUSE); \
												 control->SetUserDefine( I3GUI_CONTROL_STATE_CLICKED, GUI_SHAPE_PRESS_BUTTON_CLICKED); \
												 control->SetUserDefine( I3GUI_CONTROL_STATE_DISABLED, GUI_SHAPE_PRESS_BUTTON_DISABLED);\
												 control->setInputMode(I3GUI_INPUT_MODE_PRESS);}

//	stroke button
#define INIT_PUSH_BUTTON(control)				{control->SetUserDefine( I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_STROKE_BUTTON_NORMAL); \
												 control->SetUserDefine( I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_STROKE_BUTTON_ONMOUSE); \
											     control->SetUserDefine( I3GUI_CONTROL_STATE_CLICKED, GUI_SHAPE_STROKE_BUTTON_CLICKED); \
												 control->SetUserDefine( I3GUI_CONTROL_STATE_DISABLED, GUI_SHAPE_STROKE_BUTTON_DISABLED); \
												 control->setInputMode(I3GUI_INPUT_MODE_PRESS);}

#define INIT_CARD_EX(control)					{control->SetDownClickAction(true);control->SetUserDefine( I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);control->SetUserDefine( I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET);control->SetUserDefine( I3GUI_CONTROL_STATE_DISABLED, GUI_SHAPE_OFFSET);control->setInputMode(I3GUI_INPUT_MODE_PRESS);}
#define INIT_COMBO_EX(control)					{control->SetDownClickAction(true);control->SetUserDefine( I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);control->SetUserDefine( I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);control->SetUserDefine( I3GUI_CONTROL_STATE_DISABLED, GUI_SHAPE_OFFSET + 2);control->setInputMode(I3GUI_INPUT_MODE_PRESS);}
#define INIT_REPEAT_COMBO(control)				{control->SetDownClickAction(true);control->SetUserDefine( I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);control->SetUserDefine( I3GUI_CONTROL_STATE_ONMOUSE, GUI_SHAPE_OFFSET + 1);control->SetUserDefine( I3GUI_CONTROL_STATE_DISABLED, GUI_SHAPE_OFFSET + 2);control->setInputMode(I3GUI_INPUT_MODE_REPEAT);}
inline	void	INIT_TAB_EX(i3GuiControl * control)
{
	control->setClickAction(true);
	control->SetUserDefine( I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);
	control->SetUserDefine( I3GUI_CONTROL_STATE_DISABLED, GUI_SHAPE_OFFSET + 1);
	control->setInputMode(I3GUI_INPUT_MODE_PRESS);
}
inline	void INIT_CAPTION(i3GuiControl * control, const char * caption)
{
	const i3TextFontSetInfo	* fonSetInfo = GetDefaultFontName();
	control->CreateCaption( fonSetInfo, FONT_COUNT_64, GAME_FONT_DEFAULT_SIZE);
	control->SetCaption(caption);
	control->SetCaptionColor(GameGUI::GetColor(GCT_VALUE));
}
#define INIT_CAPTION_TAB(control,caption)		{INIT_TAB_EX(control);INIT_CAPTION(control, caption);}
#define INIT_CAPTION_BUTTON(control,caption)	{INIT_BUTTON_EX(control);INIT_CAPTION(control, caption); }

inline void __INIT_WIN_CAPTION(i3GuiEditBox * control, const char * caption, COLOR * color)
{
#if defined(ENABLE_BOLD_CAPTION)
		control->ReCreateTextEx(GetDefaultFontName(), FONT_COUNT_DEFAULT, GAME_FONT_DEFAULT_CAPTION_SIZE, false, nullptr, FW_BOLD);

		control->SetTextSpace(g_pConfigEx->GetGUI().DefaultTextSpaceX, g_pConfigEx->GetGUI().DefaultTextSpaceY);
		control->SetOffsetPos(g_pConfigEx->GetGUI().DefaultTextOffsetX, g_pConfigEx->GetGUI().DefaultTextOffsetY);
#endif
		control->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);
		control->SetTextColor(color);
		control->SetText(caption);
}
#define INIT_WIN_CAPTION(control,caption)				__INIT_WIN_CAPTION(control,caption,GameGUI::GetColor(GCT_DEFAULT))
#define INIT_WIN_CAPTION2(control,caption,color)		__INIT_WIN_CAPTION(control,caption,color)
#define INIT_NORMAL_CAPTION(control,caption)		{control->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);control->SetTextColor(GameGUI::GetColor(GCT_DEFAULT));control->SetText(caption);}
#define INIT_STATIC_CAPTION(control,caption)	{control->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);control->SetTextColor(GameGUI::GetColor(GCT_DEFAULT));control->SetText(caption);}
#define INIT_STATIC_CAPTION2(control,caption)	{control->SetTextAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_MIDDLE);control->SetTextColor(GameGUI::GetColor(GCT_DEFAULT));control->SetText(caption);}
#define INIT_VALUE_CAPTION(control,caption)		{control->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);control->SetTextColor(GameGUI::GetColor(GCT_VALUE));control->SetText(caption);}
#define INIT_VALUE_CAPTION2(control,caption)	{control->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);control->SetTextColor(GameGUI::GetColor(GCT_VALUE));control->SetText(caption);}
#define INIT_SLOT(control)						{control->SetTextAlign(TEXT_ATTR_ALIGN_CENTER, TEXT_ATTR_ALIGN_MIDDLE);control->SetTextColor(GameGUI::GetColor(GCT_DEFAULT));control->setInputDisable(true);}
#define INIT_SLOT2(control)						{control->SetTextAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_MIDDLE);control->SetTextColor(GameGUI::GetColor(GCT_DEFAULT));control->setInputDisable(true);}
#define INIT_CHECK_EX(control)					{control->SetUserDefine(I3GUI_CONTROL_STATE_NORMAL, GUI_SHAPE_OFFSET);control->SetUserDefine( I3GUI_CONTROL_STATE_FOCUS, GUI_SHAPE_OFFSET + 1);control->SetUserDefine(I3GUI_CONTROL_STATE_CLICKED, GUI_SHAPE_OFFSET + 2);control->SetUserDefine( I3GUI_CONTROL_STATE_DISABLED, GUI_SHAPE_OFFSET + 3);}

#define DEACTIVATE_BUTTON(control)				{control->SetEnable(true);	control->OnUpdate(0.0f); control->setInputDisable(true); control->SetCaptionColor(GameGUI::GetColor(GCT_STATIC)); }
#define ACTIVATE_BUTTON(control)				{control->SetEnable(true);	control->OnUpdate(0.0f); control->setInputDisable(false); control->SetCaptionColor(GameGUI::GetColor(GCT_VALUE)); }
#define IS_ACTIVATE_BUTTON(control)				(!control->getInputDisable())

//#define FLUSH_CONTROL(control)					{control->m_bOnceUpdate = true; control->OnUpdate(0.0f);} 

template< class T >
void FLUSH_CONTROL(T & control)
{
	control->m_bOnceUpdate = true; 
	control->OnUpdate(0.0f);
};


// GUI 윈도우 등장/퇴장 상태
enum SlideState
{
	SS_IN = 0,		// 등장 완료
	SS_OUT,			// 퇴장 완료
	SS_SLIDE_IN,	// 등장 중
	SS_SLIDE_OUT	// 퇴장 중
};

enum MenuButtonType
{
	MBT_CLAN = 0,
	MBT_PROFILE,
	MBT_INVENTORY,
	MBT_SHOP,
	MBT_COMMUNITY,
	MBT_OPTION,

	MAX_MBT,
};

enum GuiSound
{
	GUI_SND_SCROLLIN = 0,
	GUI_SND_SCROLLOUT,
	GUI_SND_POPUP_OPEN,
	GUI_SND_POPUP_CLOSE,
	GUI_SND_ITEM_SELECTING,
	GUI_SND_ITEM_EQUIP,

	GUI_SND_MAX
};

class CGuiNotifyReceiver: public i3GameNode
{
	I3_ABSTRACT_CLASS_DEFINE(CGuiNotifyReceiver, i3GameNode);
public:
	CGuiNotifyReceiver();
	virtual ~CGuiNotifyReceiver();

	virtual void OnPreGuiNotify(void)	{}
	virtual void OnPostGuiNotify(void)	{}

	virtual void OnGameEvent(INT32 event,INT32 arg);
	virtual void OnUpdate(REAL32 rDeltaSeconds);

	virtual void OnGuiNotify(I3GUI_CONTROL_NOTIFY* pNotify) = 0;
	virtual bool OnLinkControl( const i3::vector_set<STRCINFO*>* plist, i3GameNode * pControl) = 0;
	virtual void OnInitControl(void);

	virtual void OnExitKey(void);
	virtual	void OnSubExitKey(void);
	virtual void OnWheelUpdate(INT32 scrollby);
	virtual void SetInputDisable(bool bDisable);
	virtual bool GetInputDisable(void);

	static void CbExitNormal(void* pThis,INT32 nParam);
	static void ExclusionPopup(i3GuiPopupList* pPopup);

	// GUI 윈도우 미끄러지며 등장/퇴장을 위한 공용 인터페이스
	void SlideIn(void);
	void SlideOut(void);
	virtual bool OnSliding(REAL32 rDeltaSeconds);
	REAL32 GetSlideTime(void) const;
	SlideState GetSlideState(void) const;
	REAL32 GetSlideX(void) const;
	REAL32 GetSlideY(void) const;	

	//	sound
	bool PlaySound(GuiSound type);

	//	키 입력
	bool InputKeyboardEnter(void);		

private:
	i3SoundPlayInfo * m_pGuiSound[GUI_SND_MAX];

	REAL32 m_slideTime;						// GUI 윈도우 등장/퇴장 시작 후 경과시간
	SlideState m_slideState;				// GUI 윈도우 등장/퇴장 상태
	REAL32 m_slideX;
	REAL32 m_slideY;
};
#endif

#endif //_GuiNotifyReceiver_h
