#include "pch.h"
#include "UITabMailBox.h"

#include "UIMainFrame.h"
#include "UITopMenu_V15.h"
#include "UIPopupReceiveMail.h"
#include "MailBoxContext.h"

#include "UIUtil.h"

#include "i3Base/string/ext/format_string.h"
#include "i3Base/string/ext/mb_to_utf16.h"


I3_CLASS_INSTANCE( UITabMailBox);

UITabMailBox::UITabMailBox() : m_pListView( nullptr ), m_hdc(nullptr),
m_eSortType(MAIL_SORT_INDEX), m_bIsAscending(true), m_i32SelectIdx(-1)
{
	m_NoteList.reserve( 100 );
}

UITabMailBox::~UITabMailBox()
{
}

void UITabMailBox::__BuildNoteDataList(void)
{
	m_NoteList.clear();

	_Note note;

	const INT32 i32NoteCnt = _GetNoteDataCount();

	for( int idx = 0; idx < i32NoteCnt; ++idx )
	{
		note.idx	= idx;
		note.pNote	= _FindNoteData(idx);

		m_NoteList.push_back( note );
	}

	std::sort( m_NoteList.begin(), m_NoteList.end(), _Note::comp_idx );
}

void UITabMailBox::__UpdateMailList( bool bMailStateOnly )
{
	_ClearMailList();

	__SetUnReadMailNotice();

	for(INT32 idx = 0; idx < GetNoteCount() ; ++idx)
	{
		NOTE_RECEIVE * pNote = m_NoteList.at(idx).pNote;

		_SetMailData(idx, pNote, bMailStateOnly);
		_SetMailTextColor(idx, pNote);
	}
}

void UITabMailBox::__SortMailList(INT32 i32SortType)
{
	I3_BOUNDCHK( i32SortType, MAIL_SORT_MAX );

	if(m_eSortType != i32SortType)
		m_bIsAscending = true;
	else
		m_bIsAscending = !m_bIsAscending;

	m_eSortType = static_cast<MAIL_SORT_TYPE>(i32SortType);

	switch(m_eSortType)
	{
	case MAIL_SORT_INDEX :
		{
			std::sort( m_NoteList.begin(), m_NoteList.end(), _Note::comp_idx );
		}
		break;
	case MAIL_SORT_SENDER :
		{
			if(m_bIsAscending == true)
				std::sort( m_NoteList.begin(), m_NoteList.end(), _Note::comp_sender );
			else
				std::sort( m_NoteList.begin(), m_NoteList.end(), _Note::comp_sender_rev );	
		}
		break;
	case MAIL_SORT_PERIOD :
		{
			if(m_bIsAscending == true)
				std::sort( m_NoteList.begin(), m_NoteList.end(), _Note::comp_period );
			else
				std::sort( m_NoteList.begin(), m_NoteList.end(), _Note::comp_period_rev );
		}
		break;
	case MAIL_SORT_RECEIVE_ITEM :
		{
			if(m_bIsAscending == true)
				std::sort( m_NoteList.begin(), m_NoteList.end(), _Note::comp_gift_state );
			else
				std::sort( m_NoteList.begin(), m_NoteList.end(), _Note::comp_gift_state_rev );
		}
		break;
	default : 
		{
			std::sort( m_NoteList.begin(), m_NoteList.end(), _Note::comp_idx );
		}
		break;
	}
}

void UITabMailBox::__SetUnReadMailNotice(void)
{
	size_t nNoteCnt = m_NoteList.size();

	INT32 iReadMsgCount = 0;

	for( UINT32 idx = 0; idx < nNoteCnt; ++idx )
	{
		const NOTE_RECEIVE* pNote = m_NoteList.at(idx).pNote;
		if( pNote->header._state == NS_UNREAD)
			iReadMsgCount++;
	}

	i3::stack_wstring wstrNotice;
	i3::sprintf(wstrNotice, GAME_RCSTRING("STR_POPUP_MESSAGEBOX_READCOUNT"), iReadMsgCount, nNoteCnt);

	I3ASSERT(__GetMailBox() != nullptr);
	__GetMailBox()->SetNoticeMessage(wstrNotice);
}

/*virtual*/ void UITabMailBox::_ClearAtUnload(i3UIScene * pScene)
{
	if( nullptr != m_hdc )
		::ReleaseDC(g_hWnd, m_hdc);

	m_hdc		= nullptr;
	m_pListView = nullptr;

	UITabBase::_ClearAtUnload(pScene);
}


void UITabMailBox::_CalculateEllipseText( i3::wstring & wstrMsg, INT32 i32TextWidth, INT32 i32EllipseWidth )
{
	if( wstrMsg.empty() == true )
		return;

	size_t _pos = wstrMsg.find( L'\n' );

	if( std::string::npos != _pos )
	{
		wstrMsg = wstrMsg.substr( 0 , _pos );
	}

	if( MAX_NOTE_LENGTH < wstrMsg.length() )
	{
		wstrMsg = wstrMsg.substr( 0, MAX_NOTE_LENGTH );
	}

	int nLen = wstrMsg.length();
	int nElipseLen = 0;

	SIZE sizeEffective;

	::GetTextExtentExPointW( m_hdc, wstrMsg.c_str(), nLen, i32TextWidth, &nElipseLen, NULL, &sizeEffective );

	if( nElipseLen < nLen ) 
	{
		// 글자가 많은 경우...Ellipse길이를 빼고 다시 처리한다..
		::GetTextExtentExPointW( m_hdc, wstrMsg.c_str(), wstrMsg.length(), i32TextWidth - i32EllipseWidth, &nElipseLen, NULL, &sizeEffective );

		wstrMsg = wstrMsg.substr( 0, nElipseLen );
		wstrMsg += L"...";
	}
}

/*virtual*/ void UITabMailBox::OnCreate( i3GameNode * pParent)
{
	UITabBase::OnCreate( pParent);

	I3ASSERT(__GetMailBox() != nullptr);
	__GetMailBox()->GetSlide()->AddSyncSlide(GetSlide());
}

/*virtual*/ bool UITabMailBox::OnEntranceStart(void * pArg1 /* = nullptr */, void * pArg2 /* = nullptr */)
{
	if( UITabBase::OnEntranceStart(pArg1, pArg2) == false)
		return false;

	__BuildNoteDataList();

	m_eSortType = MAIL_SORT_INDEX;
	m_bIsAscending = true;
	m_i32SelectIdx = -1;

	m_slide.ForceEnd();
	return true;
}

/*virtual*/ void UITabMailBox::OnEntranceEnd(void)
{
	UITabBase::OnEntranceEnd();
	__UpdateMailList( false );
}

/*virtual*/ bool UITabMailBox::OnExitStart(void)
{
	UITabBase::OnExitStart();
	m_slide.ForceEnd();

	return true;
}

/*virtual*/ void UITabMailBox::OnExitEnd(void)
{
	UITabBase::OnExitEnd();	

	I3ASSERT(__GetMailBox() != nullptr);
	__GetMailBox()->ProcessNextTab();
}

/*virtual*/ void UITabMailBox::OnUpdate( REAL32 rDeltaSeconds )
{
	UITabBase::OnUpdate( rDeltaSeconds );
	
	if( MailBoxContext::i()->IsNoteNew() )
	{	
		MailBoxContext::i()->ResetNoteNewFlag();

		__BuildNoteDataList();
		__UpdateMailList( false );
	}

	if( MailBoxContext::i()->IsNoteRefresh() )
	{
		MailBoxContext::i()->ResetNoteRefreshFlag();

		if( GetMainFrame()->GetTopMenu_V15() )
			GetMainFrame()->GetTopMenu_V15()->_CheckUnReadMail();

		__UpdateMailList( true );
	}
}

/*virtual*/ void UITabMailBox::ProcessGameEvent( GAME_EVENT event, INT32 Arg , const i3::user_data& UserData)
{
	UITabBase::ProcessGameEvent(event, Arg, UserData );

	switch(event)
	{
	case EVENT_DELETE_NOTE		:
		{
			__BuildNoteDataList();
			__UpdateMailList( false );
		}
		break;
	case EVENT_AUTH_GIFT_GOODS	:
		{
			if( !MailBoxContext::i()->IsWaitResult() )
			{
				switch( MailBoxContext::i()->GetGiftReqType() )
				{
				case TYPE_SHOP_GIFT_READ :
					{
						__UpdateMailList( true );
					}
					break;
				case TYPE_SHOP_GIFT_AUTH :
					{
						__UpdateMailList( true );
						GameUI::MsgBox(MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_SHOP_INFORM_SUCCESS_GIFT"));
					}
					break;
				case TYPE_SHOP_GIFT_DELETE :
					{
						__BuildNoteDataList();
						__UpdateMailList( false );

						INT32 i32Count =  MailBoxContext::i()->GetGiftReqSuccessCount();
						i3::rc_wstring wstrMsg = i3::format_string( GAME_RCSTRING("STR_GIFT_DELETEALL"), i32Count);

						GameUI::MsgBox( MSG_TYPE_GAME_OK, wstrMsg );
					}
					break;
				}		

				i3UI::setDisableUIInput(false);
				MailBoxContext::i()->ResetGiftReqList();
			}
		}
		break;
	default:					
		break;
	}
}

void UITabMailBox::SortMailList(INT32 i32SortType)
{
	__SortMailList( i32SortType );
	__UpdateMailList( false );
}


void UITabMailBox::SetSelectAll( bool bSelect )
{
	_SetSelectAll(bSelect);
}

void UITabMailBox::SendMail(void)
{
	UIPopupReceiveMail::OPEN_DATA openData;
	
	openData.mailType	= UIPopupReceiveMail::SEND;	
	openData.wstrNickName.clear();	

	if( true == _IsSafeNoteIdx( m_i32SelectIdx) )
	{
		_Note& note = m_NoteList[ m_i32SelectIdx ];

		openData.wstrNickName = note.pNote->body._sender;
		openData.noteIdx = note.pNote->header._id;
	}
	
	GetMainFrame()->OpenPopup( UPW_MAILREV, &openData );
}

void UITabMailBox::ReadMail(INT32 i32Idx)
{
	if( false == _IsSafeNoteIdx( i32Idx ) )
		return;

	_Note& note = m_NoteList[ i32Idx ];

	UIPopupReceiveMail::OPEN_DATA openData;

	openData.mailType = static_cast<UIPopupReceiveMail::MAILTYPE>( _GetMailType( *(note.pNote)));

	_GetAttachedItemInfo(note.pNote, openData.wstrAttachedItem);

	openData.noteIdx		= note.pNote->header._id;
	openData.noteAppendData = (int)note.pNote->header._appendingData;
	openData.wstrNickName	= note.pNote->body._sender;
	openData.wstrMessage	= note.pNote->body._message;

	GetMainFrame()->OpenPopup( UPW_MAILREV, &openData );

	MailBoxContext::i()->SetNoteRefreshFlag();
}

void UITabMailBox::DeleteMail(void)
{	
	int idx = 0;
	int nSelCnt = 0;
	int nSelList[ MAX_MAIL_COUNT ];

	for each( const _Note & note in m_NoteList )
	{
		const NOTE_RECEIVE* pNote = note.pNote;

		if( false == m_pListView->isCheckedData( idx++ , UITabMailBox::LISTVIEW_CELL_CHECKBOX ) )
			continue;

		if( nullptr == pNote )
			continue;

		nSelList[ nSelCnt++ ] = _GetNoteIdx(pNote);
	}

	_SendEvent(ACTION_DELETE, nSelList, nSelCnt );
}

void UITabMailBox::ReceiveItem(void)
{
	int idx = 0;
	int nSelCnt = 0;
	int nSelList[ MAX_MAIL_COUNT ];

	for each( const _Note & note in m_NoteList )
	{
		const NOTE_RECEIVE* pNote = note.pNote;

		if( false == m_pListView->isCheckedData( idx++ , UITabMailBox::LISTVIEW_CELL_CHECKBOX ) )
			continue;

		if( nullptr == pNote )
			continue;

		const GIFT_BUFFER * pGift = reinterpret_cast<GIFT_BUFFER*>(pNote->body._user);

		if(nullptr == pGift)
			continue;

		if( pGift->_eDeliver == TYPE_GIFT_DELIVER_RECEIVED)
			continue;

		nSelList[ nSelCnt++ ] = _GetNoteIdx(pNote);
	}

	_SendEvent(ACTION_RECEIVE, nSelList, nSelCnt );
}

void UITabMailBox::ReceiveItem(INT32 i32Idx)
{	
	if( false == _IsSafeNoteIdx( i32Idx ) )
		return;

	m_i32SelectIdx = i32Idx;

	_Note& note = m_NoteList[ m_i32SelectIdx ];

	const NOTE_RECEIVE* pNote = note.pNote;
	I3ASSERT(pNote != nullptr);

	const GIFT_BUFFER * pGift = reinterpret_cast<GIFT_BUFFER*>(pNote->body._user);
	I3ASSERT(pGift != nullptr);

	if( pGift->_eDeliver == TYPE_GIFT_DELIVER_RECEIVED)
		return;

	int nSel = _GetNoteIdx(pNote);
	int nCnt = 1;

	_SendEvent(ACTION_RECEIVE, &nSel, nCnt);
}

void UITabMailBox::SetReadedMail(void)
{
	int idx = 0;
	int nSelCnt = 0;
	int nSelList[ MAX_MAIL_COUNT ];

	for each( const _Note & note in m_NoteList )
	{
		const NOTE_RECEIVE* pNote = note.pNote;

		if( false == m_pListView->isCheckedData( idx++ , UITabMailBox::LISTVIEW_CELL_CHECKBOX ) )
			continue;

		if( nullptr == pNote )
			continue;

		if(NS_READED == pNote->header._state)
			continue;

		nSelList[ nSelCnt++ ] = _GetNoteIdx(pNote);

		_SetNoteState(*(const_cast<_Note&>(note).pNote));
	}

	_SendEvent(ACTION_READ, nSelList, nSelCnt );
}

bool UITabMailBox::IsCheckedNote(void)
{
	INT32 count = m_pListView->getDataCount();

	for( INT32 idx = 0 ; idx < count ; ++idx)
	{
		if( m_pListView->isCheckedData( idx , UITabMailBox::LISTVIEW_CELL_CHECKBOX ) )
			return true;
	}
	
	return false;
}