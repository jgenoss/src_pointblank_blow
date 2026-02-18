#if !defined( __UI_PHASE_CREATE_CHARA_H__)
#define __UI_PHASE_CREATE_CHARA_H__

#include "UIPhaseBase.h"
#include "UIFloatCharacterView.h"
#include "UIFlowEditBox.h"

#define CREATECHARA_MAX_COUNT		4		// 총 4칸의 슬롯을 갖는다.

enum CreateCharaState
{
	CREATECHARA_STATE_CLASS = 0,
	CREATECHARA_STATE_CHARA,
	CREATECHARA_STATE_MAX,
};

#include "NEW_CREATE_CHARA.h"

class UIPhaseCreateChara : public UIPhaseBase
{
	I3_CLASS_DEFINE( UIPhaseCreateChara, UIPhaseBase);

private:				//Struct Data

	//Member variable;
	CreateCharaState		m_PageState;
	i3UIFrameWnd*			m_pFrameClass;
	i3UIFrameWnd*			m_pFrameChara;

	i3UIButton *			m_pCharaSlot[ CREATECHARA_MAX_COUNT];

	UIFlowEditBox			m_FlowEditBox;

	NEW_CREATE_CHARA		m_CharaInfo;
	NEW_CREATE_CHARA		m_TempInfo;

	// 선택할 수 있는 캐릭터 ItemID
	T_ItemID					m_nItemId[ CREATECHARA_MAX_COUNT];
	INT32					m_UsedSlotCount;

	// 현재 선택된 캐릭터 Index
	INT32					m_CurrentCreateIndex;

	CHARA_CLASS	m_CurrentClass;

	VEC3D			m_vFirstBtnPos;
	bool			m_bSendCreate;

	//Member Function
protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;
	void			SetStateButten();
	void			_SetButtonInfo( void);
	void			_SetDetailInfo( T_ItemID ItemID);
//	void			_SetFlowEditBox( i3::string & strIn);
	void			_SetFlowEditBox( const i3::rc_wstring & wstrIn);

	INT32			_FindParentButton( i3UIControl * pCtrl);

protected:			//Member variable;
	UIMessageBox*			m_pmbCreateCharaWait;

public:
	UIPhaseCreateChara(void);
	virtual ~UIPhaseCreateChara(void);

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	OnEntranceEnd() override;

	virtual bool	OnEnterMouse( i3UIControl * pControl) override;
	virtual void	OnLeaveMouse( i3UIControl * pControl) override;
	virtual bool	OnClick( i3UIControl * pControl) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;


	// 퇴장 완료
	virtual void	OnExitEnd( void) override;
	virtual void	OnUpdate(REAL32 rDeltaSeconds) override;
	static void		CbSameNick(void* pThis,INT32 nParam);

	void			Click_Next();
	void			Click_Back();
	void			Click_Chara(INT32 iIdx);

	void			Click_Class( CHARA_CLASS iIdx);


private:
	typedef void	(UIPhaseCreateChara::*Func_UIPhaseCreateChara)(INT32,const i3::user_data&);
	i3::unordered_map< GAME_EVENT, Func_UIPhaseCreateChara >	m_EventFuncMap;

	void			RegisterGameEventFunc( GAME_EVENT evt, Func_UIPhaseCreateChara f);

	void			ReceiveGameEvent_Create_Character( INT32 arg, const i3::user_data&);
	void			ReceiveGameEvent_Create_NickName( INT32 arg, const i3::user_data&);
	

};


#endif