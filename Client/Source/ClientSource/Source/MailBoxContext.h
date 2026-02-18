#pragma once

// 'C'접두어 안씀...



struct NOTE_RECEIVE
{
	NOTE_HEADER	header;
	NOTE_BODY body;
};

enum GIFT_SENDER_TYPE { GST_USER = 0, GST_GM };

struct NOTE_GIFT
{
	GIFT_SENDER_TYPE	sendertype;
	NOTE_RECEIVE		note;
	GIFT_BUFFER			gift;
};

struct GIFT_REQ_DATA
{
	INT32	_idx = 0;
	bool	_success = false;

	GIFT_REQ_DATA(INT32 idx)
	{
		_idx = idx;
		_success = false;
	}
};

enum MAIL_SORT_TYPE
{
	MAIL_SORT_INDEX = 0,
	MAIL_SORT_SENDER,
	MAIL_SORT_PERIOD,
	MAIL_SORT_RECEIVE_ITEM,

	MAIL_SORT_MAX
};

struct _Note
{
	int				idx = 0;
	NOTE_RECEIVE*	pNote = nullptr;

	static bool	comp_idx( const _Note& lhs, const _Note& rhs );
	static bool comp_sender( const _Note& lhs, const _Note& rhs );
	static bool comp_sender_rev( const _Note& lhs, const _Note& rhs );
	static bool comp_period(const _Note& lhs, const _Note& rhs );
	static bool comp_period_rev(const _Note& lhs, const _Note& rhs );
	static bool comp_gift_state(const _Note& lhs, const _Note& rhs );
	static bool comp_gift_state_rev(const _Note& lhs, const _Note& rhs );
};

class MailBoxContext : public i3::shared_ginst<MailBoxContext>
{
public:
	~MailBoxContext();

	INT32			GetNoteReceiveCount() const						{return (INT32)m_NoteReceiveList.size();}
	const NOTE_RECEIVE *	GetReceiveNote( UINT32 Index) const		{ if( Index < m_NoteReceiveList.size()) { return m_NoteReceiveList[Index];} return nullptr;}
	NOTE_RECEIVE *	SetReceiveNote( UINT32 Index)					{ if( Index < m_NoteReceiveList.size()) { return m_NoteReceiveList[Index];} return nullptr;}
	NOTE_RECEIVE *	SetReceiveNoteID( INT32 id)
	{ 
		for(size_t i = 0; i < m_NoteReceiveList.size();i++)
		{
			NOTE_RECEIVE* note = m_NoteReceiveList[i];
			if( note->header._id == id)
				return m_NoteReceiveList[i];	
		}
		return nullptr;
	}
	void			ReceiveNoteFreeAll()							{ i3::cu::for_each_SafeFree_clear(m_NoteReceiveList);}
	void			AddReceiveNote(NOTE_RECEIVE * pNote)			
	{ 
		m_NoteReceiveList.push_back(pNote);
	}

	bool			AddNote( const NOTE_RECEIVE * pNew, bool bAlram = false);
	bool			DeleteNote( INT32 noteId);	

	bool		IsNoteRefresh() const	{ return m_bNoteRefreshFlag; }
	bool		IsNoteNew() const		{ return m_bNoteNewFlag; }
	void		SetNoteRefreshFlag()	{ m_bNoteRefreshFlag = true; }
	void		SetNoteNewFlag()		{ m_bNoteNewFlag = true; }
	void		ResetNoteRefreshFlag()	{ m_bNoteRefreshFlag = false; }
	void		ResetNoteNewFlag()		{ m_bNoteNewFlag = false; }
	
	void		CreateClanMessageHandle();	
	void		UpdateClanMessage();
	void		UpdateClanMessage( NOTE_RECEIVE* pNote );	// 클랜 메세지를 추가한다.

	INT32		GetNoteGiftCount() const						{ return (INT32)m_NoteGiftList.size();		}
	const NOTE_GIFT*	GetNoteGift( INT32 nIndex ) const		{ return m_NoteGiftList[nIndex];	}
	const NOTE_RECEIVE*	GetNoteGiftNote( INT32 nIndex ) const	{ return (NOTE_RECEIVE*)&( GetNoteGift( nIndex )->note );	}
	const GIFT_BUFFER*	GetNoteGiftGift( INT32 nIndex )	const	{ return (GIFT_BUFFER* )&( GetNoteGift( nIndex )->gift );	}
	const GIFT_BUFFER*	GetNoteGiftGiftID( INT32 id )	const	
	{ 
		for(size_t i = 0; i < m_NoteGiftList.size();i++)
		{
			const NOTE_GIFT* pGift = GetNoteGift( i );
			if( pGift->note.header._id == id)
				return (GIFT_BUFFER* )&( pGift->gift );	
		}
		return nullptr;
		
	}
	void		NoteGiftFreeAll()								{ i3::cu::for_each_SafeFree_clear(m_NoteGiftList); }

	bool		AddNoteGift( NOTE_GIFT* pnoteNew, bool bAlram = false);
	bool		DeleteNoteGift( T_GiftDBIdx GiftDBIdx );
	void		UpdateNoteGift( AUTH_GIFT * pAuthGift);

	void		DeleteOldNoteGift(void);

	void		AddGift( const GIFT_BUFFER & Gift, const UINT8 nSenderNickSize, const UINT8 nMessageSize );
	void		ConvertEventGiftMessage(NOTE_BODY & pOut, const i3::wliteral_range& wMsgRng);

	bool		IsEnableGift( void) const			{ return m_bEnableGift; }
	void		setEnableGift( bool bEnable)		{ m_bEnableGift = bEnable; }

	void		ResetGiftReqList(void);
	void		BuildGiftReqList(UINT8 type, INT32 * list, INT32 count);
	void		SetGiftReqResult(INT32 type, AUTH_GIFT * pAuthGift);
	INT32		GetGiftReqSuccessCount(void);
	
	void		IncreaseResultCount(INT32 count)	{ m_i32ResultCount += count; }
	bool		IsWaitResult(void)					{ return ( (UINT32)m_i32ResultCount < m_GiftReqList.size() ); }		
	UINT8		GetGiftReqType(void)				{ return m_ui8GiftReqType; }

	bool		IsRecvLoginEvent()					{ return m_bRecv_LoginEvent; }
	void		setRecvLoginEvent(bool _enable)		{ m_bRecv_LoginEvent = _enable; }

private:
	i3::vector<NOTE_RECEIVE*>		m_NoteReceiveList;
	i3::vector<NOTE_GIFT*>			m_NoteGiftList;
	
	INT32							m_nTypeMessage[NOTE_MESSAGE_MAX] = { 0 };
	
	bool							m_bNoteNewFlag = false;
	bool							m_bNoteRefreshFlag = false;
	bool							m_bEnableGift = false;

	// Gift State Request 
	i3::vector<GIFT_REQ_DATA>		m_GiftReqList;

	INT32							m_i32ResultCount = 0;
	UINT8							m_ui8GiftReqType = 0;

	bool							m_bRecv_LoginEvent = false;
};


