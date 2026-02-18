#pragma once

#include "UIPopupBase.h"

class UIPopupRankup : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupRankup, UIPopupBase);
	
public:
	UIPopupRankup();
	virtual ~UIPopupRankup();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual bool	OnExitStart(void) override;

	virtual bool	OnKey_Enter() override { Confirm(); return true; }
	virtual bool	OnKey_Escape() override { Confirm(); return true; }

public:
	void	 Confirm();

private:
	void Set_UI();

private:
	UINT32 m_iItemCount;
	class UIImgSetCtrl* rank_ctrl;

	struct ItemContext
	{
		i3UIFrameWnd* frm;
		class UIImgSetCtrl* ctrl;
		i3UIStaticText* text;
	};

	ItemContext m_ItemContext[MAX_RANKUP_ITEM_COUNT];

	i3UIEditBox* gain_skill_buf_ctrl;
};
