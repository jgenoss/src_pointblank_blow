#if !defined( __UI_HUD_LOADING_BG_H__)
#define __UI_HUD_LOADING_BG_H__


#include "UIHUDBase.h"
#include "UIHudDefine.h"
#include "iHudBase.h"

enum MAP_STRING
{
	MAP_STRING_NAME		= 0,
	MAP_STRING_MISSION ,
	MAP_STRING_TEAM ,
	MAP_STRING_DESCRIPTION ,
};
 
class UIHUDLoadingBG : public UIBase
{
	I3_CLASS_DEFINE( UIHUDLoadingBG, UIBase);
	
	bool m_visible;
	i3UIFrameWnd *		m_pFrame;
	i3UIImageBoxEx *	m_pLoading;
	i3UIProgressBar *	m_pProgressbar;
	i3UIImageBoxEx *	m_pBottomImg;
	i3UIFrameWnd *		m_pFrameMsg;
	i3UIStaticText *	m_pMsg1;
	i3UIStaticText *	m_pMsg2;
	VEC2D				m_OrgSize;

	INT32				m_iTeam;
	STAGE_MODE			m_StageType;
	i3::rc_wstring		m_sTeam;


protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;

public:
	UIHUDLoadingBG();
	virtual ~UIHUDLoadingBG();

	static UIHUDLoadingBG* instance();

	virtual bool	OnKeyInput( i3InputDeviceManager * pInputMng) override;

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd( void) override;
	virtual void	OnExitEnd() override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	void			SetVisible(bool visible);
	void			SetTexture(void);
	virtual void	_Resize(void);

	void			GetStageMapName( i3::rc_wstring& out_wstrMapName);
	void			GetStageTip( i3::rc_wstring& out_wstrMapName);
	void			GetStageMission( i3::rc_wstring& out_wstrMapName);

	void			GetMapString(i3::rc_wstring& _outstr, char* _str);
	void			GetMapLabel(MAP_STRING idx, i3::rc_wstring& _outstr);

	void			UpdateProgress(REAL32 rValue);
};


#endif

