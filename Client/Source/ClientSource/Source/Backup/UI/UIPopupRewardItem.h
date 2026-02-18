#if !defined( __UI_POPUP_REWARDITEM_H__)
#define __UI_POPUP_REWARDITEM_H__

#include "UIPopupBase.h"
#include "ItemImageMgr.h"

enum POPUP_REWARD_TYPE
{
	PRT_NONE = -1,
	PRT_READYBOX = 0,
	PRT_CARD_COMPLATE,
	PRT_REWARD_CARDSET_COMPLATE,
	PRT_FINAL_PRIZE,
};

class CGameCharaBase;
class CMessageBox;

class UIPopupRewardItem : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupRewardItem, UIPopupBase);

private:
	INT32 m_nItemIDForCardSetComplete[4];		//	최종 보상 아이템 ID
	class UIImgSetCtrl*		m_LeftImgBox;
	class UIImgSetCtrl*		m_RightImgBox;

public:
	UIPopupRewardItem();
	virtual ~UIPopupRewardItem();

	virtual void	OnCreate( i3GameNode * pParent);
	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void	OnLoadAllScenes();
	virtual void	OnUnloadAllScenes();

	void			OpenPopupCardComplate( void);
	INT32			_GetRewardMedalByCardSet( UINT32 cardSetId);
	void			OpenPopupFinalPrize( CARDSET_ID CardsetID);

	bool			OnEntranceStartRewardItemForCardSetComplete( void);

	void			_InitUI(void);
	void			_SetPopupType(POPUP_REWARD_TYPE);
	void			_SetBasicInfo(INT32,INT32);
	void			_SetPrizeInfo(CARDSET_ID cardSetId);
	void			_SetFinalMedal(bool);

	void			Confirm(void);
	virtual bool	OnKey_Enter() { Confirm(); return true; }
	virtual bool	OnKey_Escape() { Confirm(); return true; }
};

#endif

