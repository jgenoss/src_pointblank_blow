#if !defined( __UI_TAB_MAILBOX_NOTE_H__)
#define __UI_TAB_MAILBOX_NOTE_H__

#include "UITabMailBox.h"

class UITabMailBox_Gift : public UITabMailBox
{
	I3_CLASS_DEFINE( UITabMailBox_Gift, UITabMailBox);

private:
	enum GIFT_STATE
	{
		GIFT_UNREAD,	// 선물 미확인
		GIFT_READED,	// 선물 확인 / 미수취
		GIFT_RECEIVE	// 선물 수취
	};

private:
	INT32					m_i32SubjectWidth;
	INT32					m_i32EllipseWidth;

public:
	UITabMailBox_Gift();
	virtual ~UITabMailBox_Gift();

	virtual void			OnCreate( i3GameNode * pParent) override;

protected:
	virtual void			_InitializeAtLoad( i3UIScene * pScene) override;

	virtual void			_ClearMailList(void) override;
	virtual void			_SetMailData(INT32 idx, const NOTE_RECEIVE * pNote, bool bMailStateOnly) override;
	virtual void			_SetMailTextColor(INT32 idx, const NOTE_RECEIVE * pNote) override;
	virtual void			_SetSelectAll(bool bSelect) override;

	virtual NOTE_RECEIVE *	_FindNoteData(INT32 idx) override;
	virtual INT32			_GetNoteDataCount(void) override;
	virtual INT32			_GetMailType(NOTE_RECEIVE & Note) override;
	virtual void			_GetAttachedItemInfo(const NOTE_RECEIVE* pNote, i3::rc_wstring & wstrItemInfo) override;

	virtual void			_SetNoteState(NOTE_RECEIVE & Note) override {}

	virtual INT32			_GetNoteIdx(const NOTE_RECEIVE * pNote) override;
	virtual void			_SendEvent(int action, int * list, int count) override;
	virtual GAME_EVENT		_GetEvent(int action) override;

protected:
	INT32					_GetReceiveState(const INT32 i32Deliver);
	void 					_GetExpirePeriod(i3::stack_wstring & wstrOut, const UINT32 ui32Expire);
	UINT8					_GetGiftAuthType(int action);
};

#endif	// __UI_TAB_MAILBOX_NOTE_H__
