#pragma once

#include "../UI/UIPopupBase.h"

struct MCardInfo;

class UIPopupMissionReward : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupMissionReward, UIPopupBase);

public:
	UIPopupMissionReward();
	virtual ~UIPopupMissionReward();

	virtual void	OnCreate( i3GameNode * pParent) override;
	/*
	param[in] pArg1 MCardInfo*
	param[in] pArg2 bool*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	virtual bool	OnExitStart( void) override;

public:
	void			Confirm();

private:
	void			_SetInfo();
	INT32			_GetRewardMedalByCardSet( INT32 cardSetType);

private:
	bool			isFinal;
	MCardInfo*		m_pCardInfo;
};