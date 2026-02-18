#if !defined( __UI_POPUP_USEITEM_DISGUISERANK_H__)
#define __UI_POPUP_USEITEM_DISGUISERANK_H__

#include "UIPopupUseItemBase.h"

#define	MAX_DISGUISE_RANK_COUNT 51
#define BOUNDS_DISGUISE_RANK 10

class UIPopupUseItemDisguiseRank : public UIPopupUseItemBase
{
	I3_CLASS_DEFINE( UIPopupUseItemDisguiseRank, UIPopupUseItemBase);
private:
	INT32 m_SelectedDisguiseRank;
	INT32 m_CurRank;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual LuaState * _SetItemInfo( void) override;

public:
	UIPopupUseItemDisguiseRank();
	virtual ~UIPopupUseItemDisguiseRank();

	virtual void	OnCreate( i3GameNode * pParent) override;

	virtual void	OnEntranceEnd( void) override;
	virtual bool	OnExitStart( void) override;

	virtual bool	OnOKButton( void) override;

	virtual bool	OnKey_Escape() override;


	void Init();

	void ClosePopup();
	void SelectRank( INT32 idx);
	void GetRankPos(INT32 rankIdx, VEC2D* pPos);
	void GetRankName(INT32 rankidx, char* name);

	void SetAbleToSelectRank(INT32 idx, INT32 bounds);
};

#endif
