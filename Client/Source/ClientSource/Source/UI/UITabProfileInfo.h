#if !defined( __UI_TAB_PROFILE_INFO_H__)
#define __UI_TAB_PROFILE_INFO_H__

#include "UITabProfileBase.h"
#include "UIImgSetCtrl.h"

class UILoadingIcon;

namespace
{
	class UIClanMark
	{
	private:
		i3UIImageBoxEx *	m_pClanMark;
		i3UIImageBox *		m_pDefaultClanMark;

	public:
		UIClanMark(void);
		~UIClanMark(void);

		void SetClanMarkCtrl( i3UIImageBoxEx * pClanMark, i3UIImageBox * pDefClanMark );
		void SetClanMarkFlag( UINT32 ui32MarkFlag );
	};
};

class UITabProfileInfo : public UITabProfileBase
{
	I3_CLASS_DEFINE( UITabProfileInfo, UITabProfileBase);

public:
	enum
	{
		MAX_UICHARA_SLOT = 3,
	};

	class CCharaSlot
	{
	public:
		CCharaSlot();
		~CCharaSlot();

	public:
		bool	Init( i3UIFrameWnd* wnd );

		void	Clear();

	public:
		const WCHAR16*		GetNickName() const
		{
			return m_name ? m_name->getText() : L"";
		}

	public:
		void	SetEnable( bool bEnable );

		void	SetCreateDate( UINT32 uDate );
		void	SetNickName( const TTCHAR* name );

		//void	SetPlayType( eCharaState _type );

		//void	SetRemainPointRatio( float ratio );
		//void	SetPoint( int nPoint );
		void	SetFocus( bool );
		bool	GetFocus(void);
		void	SetCharaImageID( int id );
		void	ChangeCharaImageTemplate( T_ItemID itemId );
		void	SetCharaSkillID( int id );\
		void	SetAbility(int idx);
		void	SetAbilityFrameWnd(bool val);
		void	SetCheckBoxEnable( bool );
		void	SetClose( bool );

		void	EnableEditNickName( bool bEdit );

	public:
		i3UIFrameWnd*			m_wnd;
		i3UIButton*				m_img;
		i3UIButtonImageSet*		m_skill;
		i3UIEditBox*			m_name;
		i3UIStaticText*			m_date;
		i3UICheckBox*			m_editName;
		i3UIFrameWnd*			m_focus;
		i3UIFrameWnd*			m_close;
		i3UIStaticText*			m_Damage;
		i3UIStaticText*			m_Defence;
		i3UIStaticText*			m_Movement;
		i3UIFrameWnd*			m_Ability;
	};


private:
	CCharaSlot				m_CharaSlot[ MAX_UICHARA_SLOT ];
	UIClanMark				m_ClanMark;

	UIMessageBox*			m_pWaitMessageBox;
	i3UIScrollBar*			m_pCharaScroll;
	UILoadingIcon *			m_pInfoWaitIcon;
	i3UIFrameWnd*			m_pMedalInfoBoard;
	UIImgSetCtrl*			m_pLastGameImage;

	//i3UIButton*				m_pMedalInfoBoardBtn;
	
	INT32	m_iCurrentEditCharaSlot;

	bool	m_bChangeTab;
//	bool	m_bMedalBtn;
//	bool	m_bMedalBtnisOn;

public:
	void	PlayBtnClick( int nSel );

	void	EditNickName( int nSel, int nKey );
	void	EnableEditNickName( int nSel, bool bEnable );

	void	SetChangeCharaNick( int arg );

	void	UpdateWheel( int nDelta );
	void	UpdateScroll();
	void	UpdateCharaList();
	void	UpdateProperty(INT32);
	void	SetCharaFocus(INT32);
	void	SetMyProperty(INT32);
	void	SetUserProperty(INT32);

	void	SelectCharacter(INT32);

public:
	virtual bool	RequestChangeTab() override;
	virtual bool	RequestExitTab() override;
	

private:
	void	_SetBasicData( const USER_INFO_BASIC* info, const USER_INFO_INVITEM_DATA* invData );
	void	_SetKillDeathData( const USER_INFO_RECORD* info );
	void	_SetBattleRecordData( const USER_INFO_RECORD* info );

	void	_SetMyLastGameRecord();
	void	_SetUserLastGameRecord();
	void	_SetFocus_ScrollBar();

	void	_SetMedalNumberOfHoldings();

	bool	_SendEvent_ChangeCharaNick();

	void	_InitMyInfoData(void);
	void	_InitUserInfoData(void);

	void	_UpdateMyCharaList(void);
	void	_UpdateUserCharaList(void);
	void	_UpdateUserCharaList_EditingName(void);						// 캐릭터 슬롯 중 이름 변경 중인 놈들을 모두 변경중이던 이름으로 갱신하는 함수.
	

public:
	virtual void		OnCreate( i3GameNode * pParent ) override;
	virtual bool		OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param2, I3_EVT_CODE code = I3_EVT_CODE_ACTIVATE) override;
	
	virtual bool		OnEntranceStart(void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void		OnEntranceEnd() override;

	virtual bool		OnExitStart() override;
	virtual void		OnUpdate(REAL32 rDeltaSeconds) override;

	virtual void		OnUnloadAllScenes() override;

protected:
	virtual void		_InitializeAtLoad( i3UIScene * pScene ) override;

public:
	UITabProfileInfo();
	virtual ~UITabProfileInfo();
};

#endif
