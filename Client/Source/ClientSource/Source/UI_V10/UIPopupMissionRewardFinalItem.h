#pragma once

#include "../UI/UIPopupBase.h"

struct MCardInfo;

class UIPopupMissionRewardFinalItem : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupMissionRewardFinalItem, UIPopupBase);

public:
	UIPopupMissionRewardFinalItem();
	virtual ~UIPopupMissionRewardFinalItem();

public:
	virtual void	OnCreate( i3GameNode * pParent) override;
	/*
	param[in] pArg1 const MCardInfo*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;

	virtual bool	OnExitStart( void) override;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

public:
	void			Confirm();

private:
	void			_SetInfo(const MCardInfo* pCardInfo);
	void			_NextCardSet();

private:
	class UIImgSetCtrl* m_ImageBox;
	INT32				m_nItemIDForCardSetComplete;
};