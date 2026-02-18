#if !defined( __UI_POPUP_QUICK_MEDAL_REWARD_H__)
#define __UI_POPUP_QUICK_MEDAL_REWARD_H__

#include "UIPopupBase.h"

struct MEDAL_INFO;
class UILoadingIcon;

class UIPopupQuickMedaiReward : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupQuickMedaiReward, UIPopupBase);

private:
	INT32			m_i32CurrentPage;	
	UILoadingIcon * m_pWaitMedalResult;
	UIMessageBox *	m_pInviteMsg;

private:
	void			__SetQuestName(i3::stack_wstring & out, const char * szQuestName, const INT32 level);
	void			__SetQuestDesc(i3::stack_wstring & out, const MEDAL_INFO * pInfo, const INT32 level);
	void			__SetQuestReward(i3::stack_wstring & out, const MQF_REWARD_INFO * pInfo);

protected:
	void			_SetRewordList(void);
	void			_SetRewardData(INT32 idx);
	void			_SetEmptyData(INT32 idx);
	void			_SetPageButton(void);

	void			_SendRewardMedalList(void);
public:
	UIPopupQuickMedaiReward();
	virtual ~UIPopupQuickMedaiReward();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnExitEnd(void) override;
	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;
	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 arg , const i3::user_data& UserData) override;

	virtual bool	OnOKButton( void) override;

public:
	void			MovePage(INT32 i32Value);
	static void		Callback_Close(void* pThis,INT32 nParam);
	void			SetInviteMsg(UIMessageBox * msg)  {m_pInviteMsg = msg;}
	

};

#endif // __UI_POPUP_QUICK_MEDAL_REWARD_H__
