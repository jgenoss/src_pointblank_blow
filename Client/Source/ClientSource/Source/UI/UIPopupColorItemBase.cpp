#include "pch.h"
#include "UIPopupColorItemBase.h"

#include "UIUtil.h"
#include "ItemConsumer.h"

#define SELECT_CROSSHAIR_COLOR_BOX_COUNT		10	//	변경 색상 샘플 갯수

//I3_ABSTRACT_CLASS_INSTANCE( UIPopupColorItemBase, UIPopupUseItemBase);
I3_CLASS_INSTANCE( UIPopupColorItemBase);

UIPopupColorItemBase::UIPopupColorItemBase()
{
	m_UIStyle |= UI_STYLE_INCLUDE_OKBUTTON;
}

UIPopupColorItemBase::~UIPopupColorItemBase()
{
}

/*virtual*/ void UIPopupColorItemBase::OnEntranceEnd( void)
{
	UIPopupUseItemBase::OnEntranceEnd();
	ItemConsumer::instance()->m_nSelected = -1;
}


/*virtual*/ bool UIPopupColorItemBase::OnOKButton( void)
{
	if( ItemConsumer::instance()->m_nSelected == -1)
	{
		/*변경할 색상을 선택해 주십시오.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK,  GAME_RCSTRING("STR_POPUP_MESSAGE_CHOOSE_MODIFY_COLOR"));
	}
	else
	{
		SetInputDisable( true);

		UINT8 ui8Size = sizeof(INT32);
		INT32 idx = ItemConsumer::instance()->m_nSelected + 1;	//	GUI는 -1 베이스이며, 서버는 0 베이스이다. 주의할것!
		GameEventSender::i()->SetEvent(EVENT_ITEM_AUTH_DATA, &m_AuthWareDBIndex, &ui8Size, &idx);
	}

	// 서버 event를 받으면 닫는다.
	return false;
}


I3COLOR * UIPopupColorItemBase::GetColor( INT32 idx)
{
	I3ASSERT( idx >= -1 && idx < SELECT_CROSSHAIR_COLOR_BOX_COUNT);	//	-1번은 흰색, 0~9번까지 색상

	I3COLOR * pSelectColor = nullptr;

	ItemConsumer::instance()->m_nSelected = idx;

	if( idx == -1)
	{
		//	선택되지 않은 상태 또는 기본 상태	기본 컬러
		pSelectColor = (I3COLOR*)GetColorCashItem(0);
	}
	else
	{
		//	선택된 컬러 박스의 위치로 선택 커서를 맞춘다.
		INT32 idxColor = idx + 1;	//	idxColor는 1 base이다.

		//	선택 컬러
		pSelectColor = (I3COLOR*)GetColorCashItem((UINT8)idxColor);
	}

	return pSelectColor;
}
