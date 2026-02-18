#if !defined( __UI_PHASE_CLANMEMBER_H__)
#define __UI_PHASE_CLANMEMBER_H__

#include "UIPhaseClanBase.h"

#include "UISlide.h"

#define MEMBER_LIST_ROW_MAX 12

enum UITabClanType
{
	UTC_NONE = -1,
	UTC_INFO,		//0
	UTC_MEMBER,		//1
	UTC_RESULT,		//2
	UTC_RANKING,	//3
	UTC_MGT,		//4
	UTC_MAX,		//5
};

enum ADMISSION_LIMIT_TYPE
{
	ADMISSION_LIMIT_RANK,
	ADMISSION_LIMIT_AGE1,
	ADMISSION_LIMIT_AGE2,
};

class CClanContext;
class UITabClan;
class UITabClanMgt_Main;

class UIPhaseClanMember : public UIPhaseClanBase
{
	I3_CLASS_DEFINE( UIPhaseClanMember, UIPhaseClanBase);

private:
	
	i3UIImageBoxEx *	m_pClanMark;

	// 시즌 전적 정보
	// KEYWORD : SEASON_INFO_INDICATION
	i3UIFrameWnd *		m_pSeasonInfo;

	UITabClanType		m_eCurrnetTab;
	UITabClanType		m_eNextTab;

	UITabClan *			m_pClanTab[UTC_MAX];

protected:
	virtual void		_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void		_ClearAtUnload(i3UIScene * pScene) override;

	void				_InitClanMemberPhase( UITabClanType eType);
	void				_UpdateClanDetailInfo( void);

public:
	UIPhaseClanMember();
	virtual ~UIPhaseClanMember();

	virtual void		OnCreate( i3GameNode * pParent) override;
	virtual bool		OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual bool		OnExitStart( void) override;
	virtual void		OnExitEnd() override;
	virtual bool		OnEvent(UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code /* = I3_EVT_CODE_ACTIVATE */) override;
	virtual bool		OnKey_Enter() override;
	virtual void		OnUpdate(REAL32 rDeltaSeconds) override;

	virtual void		OnLoadAllScenes() override;
		
	virtual void		Enter( void) override;
	virtual void		Leave( void) override;

	virtual bool		OnPlayClickSound(i3UIControl * pCtrl) override;

	virtual void		ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) override;

	void				ProcessNextTab( void);
	void				ChangeTab( UITabClanType tab);
	void				ClickButton( void);
	void				OnBackPhase( void);

	// 시즌 전적 정보
	// KEYWORD : SEASON_INFO_INDICATION
	void				ToggleFrame( void);

	UITabClan*			GetCurrTab( void) const			{ return m_pClanTab[m_eCurrnetTab]; }
	UITabClanType		GetCurrTabType( void) const		{ return m_eCurrnetTab; }
	UITabClan*			GetTab( UITabClanType eType) { return m_pClanTab[eType]; }

private:
	typedef void		(UIPhaseClanMember::*call_func)(INT32,const i3::user_data&);
	i3::unordered_map< GAME_EVENT, call_func >	m_EventFuncMap;

	void				RegisterGameEventFunc( GAME_EVENT evt, call_func f);

	void				ReceiveGameEvent_MemberInfo( INT32 arg, const i3::user_data&);
	void				ReceiveGameEvent_DetailInfo( INT32 arg, const i3::user_data&);
	void				ReceiveGameEvent_CommisionMaster( INT32 arg, const i3::user_data&);
	void				ReceiveGameEvent_ChangeGrade( INT32 arg, const i3::user_data&);
	
	

};

#endif
