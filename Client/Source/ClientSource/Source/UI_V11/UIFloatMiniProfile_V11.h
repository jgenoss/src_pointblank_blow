#if !defined( __UI_FLOAT_MINIPROFILE_V11_H__)
#define __UI_FLOAT_MINIPROFILE_V11_H__

#include "UI/UIFloatingBase.h"

class UIFloatMiniProfile_V11 : public UIFloatingBase
{
	I3_CLASS_DEFINE( UIFloatMiniProfile_V11, UIFloatingBase);

private:
	REAL32			m_rUpdateTime;

	i3UIImageBoxEx*	m_pImgBox;
	i3UIStaticText* m_pClanName;
	i3UIButtonImageSet*	m_pNationMark;
	
	UINT8			m_ui8NationMarkShapeIdx;

	INT32			__GetExpMax( INT32 Rank);

public:
	void			UpdateUserInfo( void);

public:
	UIFloatMiniProfile_V11();
	virtual ~UIFloatMiniProfile_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	
	virtual void	OnLoadAllScenes() override;
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArv2 = nullptr) override;
	virtual void	_InitializeAtLoad(i3UIScene * pScene) override;

	virtual bool	OnExitStart( void) override;

	virtual void	ProcessGameEvent(GAME_EVENT evt, INT32 arg, const i3::user_data& UserData) override;

	void			_SetClanMark(UINT32);
	void			_SetClanMarkPosition();

	void			AttendanceOpenPopup(INT32 EvtType);
};

#endif
