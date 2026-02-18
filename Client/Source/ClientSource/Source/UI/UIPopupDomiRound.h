#pragma once

#include "UIPopupBase.h"

namespace Domination_Util
{
	struct RoundItem
	{
		INVEN_BUFFER* item;
		INT32 round;
		RoundItem() {}
		RoundItem( const INVEN_BUFFER* i, INT32 r)
		{
			item = const_cast<INVEN_BUFFER*>(i);
			round = r;
		}
	};

	RoundItem FindDomiRoundItem( INT32 start_round);
}
namespace dui = Domination_Util;


class UIImgSetCtrl;
class UIPopupDomiRound : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupDomiRound, UIPopupBase);
	
	UIImgSetCtrl* m_img_ctrl;
	INT32 m_cur_sel;
	void			RefreshFrame( INT32 cur_sel);

public:
	UIPopupDomiRound();
	virtual ~UIPopupDomiRound();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;
	
	virtual void	OnLoadAllScenes() override;
	virtual void	OnUnloadAllScenes() override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnEntranceEnd() override;
	virtual bool	OnExitStart(void) override;

	virtual bool	OnKey_Enter() override;
	virtual bool	OnKey_Escape() override;

	virtual void	ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override;

	void ChangeRound();
	void Confirm();
};
