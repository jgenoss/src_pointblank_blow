#include "pch.h"
#include "UITabMailBox_Note.h"

#include "UIPopupMailBox.h"
#include "UIPopupReceiveMail.h"
#include "MailBoxContext.h"
#include "UIUtil.h"
#include "i3Base/string/ext/mb_to_utf16.h"

I3_CLASS_INSTANCE( UITabMailBox_Note);

static UITabMailBox_Note * GetThis()
{
	UIPopupMailBox * pPopup = static_cast<UIPopupMailBox*>(GameUI::GetPopup(UPW_MAILBOX));
	if( nullptr != pPopup)
	{
		return static_cast<UITabMailBox_Note*>(pPopup->GetTab(UIPopupMailBox::TAB_MESSAGE_LIST));
	}

	return nullptr;
}
//------------------------------------------------------------------------------//
//								Lua Glue Functions								//
//------------------------------------------------------------------------------//
extern "C" {

	int UITabMailBox_Note_SelectMail( LuaState * L)
	{
		INT32 i32Idx = i3Lua::GetIntegerArg( L, 1, -1 );		
		
		UITabMailBox_Note * pThis = GetThis();
		if(nullptr != pThis)
			pThis->SetSelectIdx(i32Idx);

		return 0;
	}

	int UITabMailBox_Note_ReadMail( LuaState * L)
	{
		INT32 nIndex = i3Lua::GetIntegerArg( L, 1, -1 );

		UITabMailBox_Note * pThis = GetThis();
		if(nullptr != pThis)
			pThis->ReadMail(nIndex);

		return 0;
	}

	int UITabMailBox_Note_SortMailList( LuaState * L )
	{
		INT32 i32SortType = i3Lua::GetIntegerArg( L, 1);

		UITabMailBox_Note * pThis = GetThis();
		if(nullptr != pThis)
			pThis->SortMailList(i32SortType);

		return 0;
	}
}

LuaFuncReg UITabMailBox_Note_Glue[] = {
	{"SelectMail",				UITabMailBox_Note_SelectMail},
	{"ReadMail",				UITabMailBox_Note_ReadMail},
	{"SortMailList",			UITabMailBox_Note_SortMailList},
	{nullptr,						nullptr}
};

//------------------------------------------------------------------------------//


UITabMailBox_Note::UITabMailBox_Note() : m_i32SubjectWidth(0), m_i32EllipseWidth(0)
{
}

UITabMailBox_Note::~UITabMailBox_Note()
{
}

/*virtual*/ void UITabMailBox_Note::_InitializeAtLoad(i3UIScene * pScene)
{
	UITabMailBox::_InitializeAtLoad(pScene);

	if( nullptr == pScene )
		return;

	m_pListView = (i3UIListView_Box*) pScene->FindChildByName("ListView_Note");

	if( nullptr != m_pListView )
	{
		i3UIListView_Item* pItem = m_pListView->getBoundItem();
		i3UIListView_Cell* pCell = nullptr;

		if( nullptr != pItem )
			pCell = pItem->getCell( UITabMailBox::LISTVIEW_CELL_SUBJECT );

		if( nullptr != pCell )
		{
			i3UIFont* font	= pCell->ObtainUIFont();
			m_i32SubjectWidth = INT32(pCell->getWidth() ) - pCell->getTextSpaceX() * 2;	

			if( nullptr == font )
				return;

			if( nullptr == m_hdc )
				::ReleaseDC(g_hWnd, m_hdc);

			SIZE sizeExt;

			m_hdc = ::GetDC( g_hWnd );

			::SelectObject( m_hdc, font->getFontHandle() );
			::GetTextExtentPoint32A( m_hdc, "....", 3, &sizeExt );

			m_i32EllipseWidth= sizeExt.cx;
		}
	}
}

/*virtual*/ void UITabMailBox_Note::_ClearMailList(void)
{
	LuaState *  L = _CallLuaFunction( "ClearListViewBox" );
	if(nullptr != L )
	{
		INT32 i32NoteCnt = GetNoteCount();

		i3Lua::PushInteger( L, i32NoteCnt );
		_EndLuaFunction( L, 1 );
	}
}

/*virtual*/ void UITabMailBox_Note::_SetMailData(INT32 idx, const NOTE_RECEIVE * pNote, bool bMailStateOnly)
{
	if( pNote == nullptr)
		return;

	LuaState * L = nullptr;

	INT32 i32MailState = pNote->header._state == NS_UNREAD ? 0 : 1;

	if( false == bMailStateOnly )
	{
		i3::wstring wstrMessage;

		if( 0 < pNote->body._messageSize )
		{
			wstrMessage = pNote->body._message;
			_CalculateEllipseText( wstrMessage, m_i32SubjectWidth, m_i32EllipseWidth );
		}

		L = _CallLuaFunction( "SetMailDataAll");
		if( nullptr != L )
		{
			i3Lua::PushInteger( L, idx );
			i3Lua::PushInteger( L, 0 );
			i3Lua::PushInteger( L, i32MailState );
			i3Lua::PushStringUTF16To8( L, pNote->body._sender);	
			i3Lua::PushStringUTF16To8( L, wstrMessage );
			_EndLuaFunction( L, 5 );
		}
	}
	else
	{
		L = _CallLuaFunction( "SetMailState");
		if( nullptr != L )
		{
			i3Lua::PushInteger( L, idx );
			i3Lua::PushInteger( L, i32MailState );
			_EndLuaFunction( L, 2 );
		}
	}
}

/*virtual*/ void UITabMailBox_Note::_SetMailTextColor(INT32 idx, const NOTE_RECEIVE * pNote)
{
	COLOR Color;
	
	if(pNote->header._state == NS_UNREAD)
		i3Color::Set(&Color, (UINT8)255, (UINT8)255, (UINT8)255, (UINT8)255);
	else
		i3Color::Set(&Color, (UINT8)136, (UINT8)136, (UINT8)139, (UINT8)255);

	LuaState * L = _CallLuaFunction("SetMailTextColor");

	if( nullptr != L )
	{
		i3Lua::PushInteger(L, idx);
		i3Lua::PushColor(L , &Color);
		_EndLuaFunction(L, 5);
	}
}

/*virtual*/ void UITabMailBox_Note::_SetSelectAll(bool bSelect)
{
	LuaState * L = _CallLuaFunction( "SetSelectAll");

	if( nullptr != L )
	{
		i3Lua::PushBoolean( L, bSelect ? true : false );
		_EndLuaFunction( L, 1 );
	}
}

/*virtual*/ NOTE_RECEIVE * UITabMailBox_Note::_FindNoteData(INT32 idx)
{
	return const_cast<NOTE_RECEIVE*>(MailBoxContext::i()->GetReceiveNote( idx ));
}

/*virtual*/ INT32 UITabMailBox_Note::_GetNoteDataCount(void)
{
	return MailBoxContext::i()->GetNoteReceiveCount();
}

/*virtual*/ INT32 UITabMailBox_Note::_GetMailType(NOTE_RECEIVE & Note)
{
	INT32 i32MailType = UIPopupReceiveMail::READ;

	if(Note.header._type == NT_CLAN_ASK)
	{
		if(Note.header._state == NS_UNREAD)
			i32MailType = UIPopupReceiveMail::CLAN_READ_ASK;
		else
			i32MailType = UIPopupReceiveMail::CLAN_READ;
	}
	else
	{
		if(Note.header._type == NT_CLAN)
			i32MailType = UIPopupReceiveMail::CLAN_READ;
		else
			i32MailType = UIPopupReceiveMail::READ;

		_SendReadEventByOne(Note);
	}

	return i32MailType;
}

/*virtual*/ void UITabMailBox_Note::_SetNoteState(NOTE_RECEIVE & Note)
{
	if(Note.header._state != NS_READED)
	{
		Note.header._state = NS_READED;	// 읽음상태로 변경합니다.
		if(Note.header._type != NT_GIFT)
			Note.header._remainDay = 7;		// 읽은 상태의 쪽지는 남은기간을 7일로 설정합니다.
	}
}

/*virtual*/ INT32 UITabMailBox_Note::_GetNoteIdx(const NOTE_RECEIVE * pNote)
{
	return pNote->header._id;
}

/*virtual*/ void UITabMailBox_Note::_SendEvent(int action, int * list, int count)
{
	GAME_EVENT evt = _GetEvent(action);

	if(count > 0 && evt > EVENT_NOTHING)
		GameEventSender::i()->SetEvent( evt, &count, list );
}

/*virtual*/ GAME_EVENT UITabMailBox_Note::_GetEvent(int action)
{
	switch(action)
	{
	case ACTION_READ	:	return EVENT_CHECK_READED_NOTE;
	case ACTION_DELETE	:	return EVENT_DELETE_NOTE;
	}

	return EVENT_NOTHING;
}

void UITabMailBox_Note::_SendReadEventByOne(NOTE_RECEIVE & Note)
{
	if(Note.header._state != NS_READED)
	{
		_SetNoteState(Note);

		int nCnt = 1;
		int nSel = Note.header._id;
		_SendEvent(ACTION_READ, &nSel, nCnt);
	}
}

/*virtual*/ void UITabMailBox_Note::OnCreate( i3GameNode * pParent)
{
	UITabMailBox::OnCreate( pParent);
	AddScene( "Scene/Popup/PBRe_PopUp_MessageBox_Tab_Note.i3UIs", UITabMailBox_Note_Glue);
}