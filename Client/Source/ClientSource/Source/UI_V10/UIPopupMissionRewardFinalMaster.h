#pragma once

#include "../UI/UIPopupBase.h"

struct MCardInfo;

class UIPopupMissionRewardFinalMaster : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupMissionRewardFinalMaster, UIPopupBase);

public:
	UIPopupMissionRewardFinalMaster();
	virtual ~UIPopupMissionRewardFinalMaster();

public:
	virtual void	OnCreate( i3GameNode * pParent) override;
	/*
	param[in] pArg1 const MCardInfo*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	virtual bool	OnExitStart( void) override;

public:
	void			Confirm();

private:
	void			_SetInfo(const MCardInfo* pCardInfo, INT32 cardSetType);
	void			_SetDefaultIcon();
	void			_NextCardSet();
};