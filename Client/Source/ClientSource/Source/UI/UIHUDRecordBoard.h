#if !defined( __UI_HUD_RECORD_BOARD_H__)
#define __UI_HUD_RECORD_BOARD_H__

//----------------------------------------------------------------------------------//
//	Creator : Dongik.Ha
//	Date :	2012.01.09
//	Desc : HUD Respwan Board
//----------------------------------------------------------------------------------//

#include "UIHUDBase.h"
#include "BattleHUD_Define.h"

class UIImgSetCtrl;

class UIHUDRecordBoard : public UIHUDBase
{
	I3_CLASS_DEFINE( UIHUDRecordBoard, UIHUDBase);
	typedef REVENGE_ARRAY REVENGE_ARRAY_UI;

public:
	UIHUDRecordBoard();
	virtual ~UIHUDRecordBoard();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnExitEnd( void) override;
	virtual void	ClearData() override;

	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;

	void			UIRecordRevenge(INT32, INT32);

	void			SetRedTargetName(const i3::rc_wstring& );
	void			SetBlueTargetName(const i3::rc_wstring& );
	void			SetRedTargetCount(const i3::wliteral_range& wTextRng);
	void			SetRedTargetOnColor();
	void			SetBlueTargetCount(const i3::wliteral_range& wTextRng);
	void			SetBlueTargetOnColor();
	void			SetWeaponName(const i3::rc_wstring& strName);
	void			SetWeaponFrame(bool);
	void			DeleteRecordRevenge(INT32);


private:
	REVENGE_ARRAY_UI	m_Revenge[SLOT_MAX_COUNT];
	bool				m_bStartRecordBoard;
	REAL32				t_Record;
	INT32				m_index;
	//	char *				m_pszTarget;
	UIImgSetCtrl*		m_ItemSetCtrl;
	COLOR				m_pColor;

	i3UIStaticText*		m_pLeftText;
	i3UIStaticText*		m_pRightText;

};

#endif
