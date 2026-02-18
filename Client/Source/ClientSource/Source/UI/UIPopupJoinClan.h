#if !defined( __UI_POPUP_JOIN_CLAN_H__)
#define __UI_POPUP_JOIN_CLAN_H__

#include "UIPopupBase.h"
#include "UIFlowEditBox.h"

class UIPopupJoinClan : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupJoinClan, UIPopupBase);

private:
	i3UIImageBoxEx * m_pClanMark;
	i3UIStaticText * m_pClanName;

	CStrFilterPtr	 m_StringFilter;
	CStrFilterPtr	 m_StringFilter2;

	i3::rc_wstring	 m_wstrText; //[FONT_COUNT_512];

	UIFlowEditBox	 m_FlowEditBox;	

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload(i3UIScene * pScene) override;

	void			_InitClanData( void);
	void			_SubscriptionSendJoin(void);

public:
	UIPopupJoinClan(void);
	~UIPopupJoinClan(void);

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;
	virtual bool	OnKey_Enter( void) override { JoinSend(); return true; }
	virtual bool	OnKey_Escape( void) override { Close(); return true; }
	virtual bool	OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;

	void			JoinSend( void);
	void			Close( void);

	static void		cbQuaryJoinClan(void* pThis, INT32 nParam);

private:
	void			ReceiveGameEvent_Join_Request( INT32 arg, const i3::user_data&);
	

};
#endif