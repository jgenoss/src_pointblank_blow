#if !defined( __UI_TOOL_TIP_MAPINFO_H__)
#define __UI_TOOL_TIP_MAPINFO_H__

#include "UIToolTipBase.h"

class UIToolTipLobbyMapInfo: public UIToolTipBase
{
	I3_CLASS_DEFINE( UIToolTipLobbyMapInfo, UIToolTipBase);
private:

private:
	void	_SetMapInfo( const ROOM_INFO_BASIC * pInfo, const ROOM_INFO_ADD * pAddInfo);
	void	_GetBalanceText(i3::rc_wstring& out, UINT8 ui8Balance) const;

public:
	UIToolTipLobbyMapInfo();
	virtual ~UIToolTipLobbyMapInfo();

	virtual void	OnCreate( i3GameNode * pParent) override;
	/*
	param[in] pArg1 ROOM_INFO_BASIC*
	param[in] pArg2 ROOM_INFO_ADD*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArv2 = nullptr) override;
	virtual bool	OnExitStart(void) override;

	void	EnableToolTip( const ROOM_INFO_BASIC * pArg1 = nullptr, const ROOM_INFO_ADD * pArg2 = nullptr);
	void	DisableToolTip(void);

	void	ChangeToolTip( const ROOM_INFO_BASIC * pInfo, const ROOM_INFO_ADD * pAddInfo);
};

#endif	// __UI_TOOL_TIP_MAPINFO_H__
