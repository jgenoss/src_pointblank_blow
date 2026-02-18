#if !defined( __UI_TAB_MAILBOX_H__)
#define __UI_TAB_MAILBOX_H__

#include "UITabBase.h"
#include "MailBoxContext.h"

struct NOTE_RECEIVE;
class UIPopupMailBox;

class UITabMailBox : public UITabBase
{
	I3_ABSTRACT_CLASS_DEFINE( UITabMailBox, UITabBase);

public:
	enum LISTVIEW_CELL_TYPE
	{
		LISTVIEW_CELL_CHECKBOX = 0,
		LISTVIEW_CELL_MAILTYPE,
		LISTVIEW_CELL_SENDER,
		LISTVIEW_CELL_FRONT_PADDING,
		LISTVIEW_CELL_SUBJECT,
		LISTVIEW_CELL_BACK_PADDING,
		LISTVIEW_CELL_PERIOD,
		LISTVIEW_CELL_RECEIVE
	};

	enum
	{
		ACTION_READ = 0,
		ACTION_DELETE = 1,
		ACTION_RECEIVE = 2,

		MAX_MAIL_COUNT = 100,
	};

private:
	i3::vector< _Note >		m_NoteList;

protected:
	i3UIListView_Box *		m_pListView;
	HDC						m_hdc;

	MAIL_SORT_TYPE			m_eSortType;
	bool					m_bIsAscending;

	INT32					m_i32SelectIdx;

public:
	UITabMailBox(void);
	virtual ~UITabMailBox(void);

private:
	void					__BuildNoteDataList(void);
	void					__UpdateMailList(bool bMailStateOnly);
	void					__SortMailList(INT32 i32SortType);

	UIPopupMailBox *		__GetMailBox(void)	{ return i3::same_cast<UIPopupMailBox*>(getParent()); }
	void					__SetUnReadMailNotice(void);

protected:
	virtual void			_ClearAtUnload( i3UIScene * pScene) override;

	virtual void			_ClearMailList(void) = 0;
	virtual void			_SetMailData(INT32 idx, const NOTE_RECEIVE * pNote, bool bMailStateOnly) = 0;
	virtual void			_SetMailTextColor(INT32 idx, const NOTE_RECEIVE * pNote) = 0;
	virtual void			_SetSelectAll(bool bSelect) = 0;

	virtual NOTE_RECEIVE *	_FindNoteData(INT32 idx) = 0;
	virtual INT32			_GetNoteDataCount(void)	= 0;
	virtual INT32			_GetMailType(NOTE_RECEIVE & pNote) = 0;
	virtual void			_GetAttachedItemInfo(const NOTE_RECEIVE* pNote, i3::rc_wstring & wstrItemInfo) = 0;

	virtual void			_SetNoteState(NOTE_RECEIVE & Note) = 0;
	
	virtual INT32			_GetNoteIdx(const NOTE_RECEIVE * pNote) = 0;
	virtual void			_SendEvent(int action, int * list, int count) = 0;
	virtual GAME_EVENT		_GetEvent(int action) = 0;

protected:
	void					_CalculateEllipseText(i3::wstring & wstrMsg, INT32 i32TextWidth, INT32 i32EllipseWidth );

	bool					_IsSafeNoteIdx( INT32 idx ) const
	{
		return 0 <= idx && idx < static_cast<INT32>( m_NoteList.size() );
	}

public:
	virtual void			OnCreate( i3GameNode * pParent) override;

	virtual bool			OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void			OnEntranceEnd(void) override;
	virtual bool			OnExitStart(void) override;
	virtual void			OnExitEnd(void) override;

	virtual void			OnUpdate( REAL32 rDeltaSeconds ) override;

	virtual void			ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData ) override;

public:
	void					SortMailList(INT32 i32SortType);

	void					SetSelectAll( bool bSelect );
	void					SendMail(void);
	void					ReadMail(INT32 i32Idx);
	void					DeleteMail(void);
	void					ReceiveItem(void);
	void					ReceiveItem(INT32 i32Idx);
	void					SetReadedMail(void);
	bool					IsCheckedNote(void);
	void					SetSelectIdx(INT32 idx)		{ if( _IsSafeNoteIdx(idx) ) m_i32SelectIdx = idx; }

	inline INT32			GetNoteCount(void)			{ return static_cast<INT32>( m_NoteList.size()); }
};

#endif
