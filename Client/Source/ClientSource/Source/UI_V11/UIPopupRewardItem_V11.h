#pragma once

#include "../UI/UIPopupBase.h"

struct MCardInfo;

class UIPopupRewardItem_V11 : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupRewardItem_V11, UIPopupBase);

	enum{	MAX_REWARD = 4,	};

private:
	INT32 m_nItemIDForCardSetComplete[MAX_REWARD];		//	최종 보상 아이템 ID
	class UIImgSetCtrl*		m_ImgBox[MAX_REWARD];

public:
	UIPopupRewardItem_V11();
	virtual ~UIPopupRewardItem_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;
	/*
	param[in] pArg1 const MCardInfo*
	param[in] pArg2 bool*
	*/
	virtual bool	OnEntranceStart( void * pArg1 = nullptr, void * pArg2 = nullptr) override;
	virtual void	OnLoadAllScenes() override;
	virtual void	OnUnloadAllScenes() override;

	void			OpenPopupCardComplate( void);
	INT32			_GetRewardMedalByCardSet( INT32 cardSetType);
	void			OpenPopupFinalPrize( INT32 cardSetType);

	bool			OnEntranceStartRewardItemForCardSetComplete( void);

	void			_InitUI(void);
	void			_SetBasicInfo(const MCardInfo* pCardInfo, INT32, INT32);
	void			_SetPrizeInfo(const MCardInfo* pCardInfo, INT32 cardSetType, bool bIsFinalReward);
	void			_SetFinalMedal(bool);

	void			Confirm(void);
	virtual bool	OnKey_Enter() override { Confirm(); return true; }
	virtual bool	OnKey_Escape() override { Confirm(); return true; }
};
