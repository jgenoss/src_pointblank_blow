#if !defined( __UI_FLOAT_READYROOMINFO_V11_H__)
#define __UI_FLOAT_READYROOMINFO_V11_H__

#include "UI/UIFloatingBase.h"

enum READYROOM_INFO_TAB
{
	RIT_NONE = -1,
	RIT_MAP = 0,
	RIT_MISSION,

	RIT_MAX
};

class UIFloatReadyRoomMissionInfo_V11;

class UIFloatReadyRoomInfo_V11 : public UIFloatingBase
{
	I3_CLASS_DEFINE( UIFloatReadyRoomInfo_V11, UIFloatingBase);

private:
	// 임무카드 작업 후 넣어줘야 합니다. 20150520 김현우
	UIFloatReadyRoomMissionInfo_V11*	m_pMissionInfo;
	i3UIFrameWnd *						m_pMapInfoFrame;
	READYROOM_INFO_TAB					m_CurrentTab;
	bool								m_bNeedUpdataInfo;

protected:
	bool			_BeHost(void);

	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

public:
	UIFloatReadyRoomInfo_V11();
	virtual ~UIFloatReadyRoomInfo_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArv2 = nullptr) override;
	virtual bool	OnExitStart( void) override;

	virtual void	ProcessGameEvent(UINT32 event , INT32 arg);

	virtual void	SetInputDisable( bool bDisable) override;

	void			ChangeMapInfoTab( void);
	void			ChangeMissionInfoTab( void);
	void			ChangeRoomSetting();

	void			EnableRoomSet(bool bHost);

	void			UpdateMapInfo( void);

	void			SetReadyRoomNeedUpdateInfo(bool tr) {	m_bNeedUpdataInfo = tr;};
	bool			GetReadyRoomNeedUpdataInfo()	{ return m_bNeedUpdataInfo;};
};

#endif
