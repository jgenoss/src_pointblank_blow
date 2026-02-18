#pragma once

#include "UIPopupBase.h"

class UIPopupSkillOpenResult : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupSkillOpenResult, UIPopupBase);

public:
	UIPopupSkillOpenResult();
	virtual ~UIPopupSkillOpenResult();

public:
	virtual void	OnCreate( i3GameNode * pParent) override;

	/*
	param[in] pArg1 T_ItemID*
	param[in] pArg2 INT8*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual bool	OnExitStart( void) override;

	virtual bool	OnKey_Enter() override;
	virtual bool	OnKey_Escape(void) override;

private:
	void SetInfo();

	T_ItemID m_item_id;
	INT8 chara_idx;
};