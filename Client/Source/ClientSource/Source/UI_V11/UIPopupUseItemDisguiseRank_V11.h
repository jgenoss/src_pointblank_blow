#if !defined( __UI_POPUP_USEITEM_DISGUISERANK_H__)
#define __UI_POPUP_USEITEM_DISGUISERANK_H__

#include "UI/UIPopupUseItemBase.h"

#define	MAX_DISGUISE_RANK_COUNT 51
#define BOUNDS_DISGUISE_RANK 10

class UIPopupUseItemDisguiseRank_V11 : public UIPopupUseItemBase
{
	I3_CLASS_DEFINE( UIPopupUseItemDisguiseRank_V11, UIPopupUseItemBase);
private:
	INT32 m_SelectedDisguiseRank;
	INT32 m_CurRank;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene);
	virtual LuaState * _SetItemInfo( void);

public:
	UIPopupUseItemDisguiseRank_V11();
	virtual ~UIPopupUseItemDisguiseRank_V11();

	virtual void	OnCreate( i3GameNode * pParent);

	virtual void	OnEntranceEnd( void);
	virtual bool	OnExitStart( void);

	virtual bool	OnOKButton( void);

	void Init();

	void ClosePopup();
	void SelectRank( INT32 idx);
	void GetRankPos(INT32 rankIdx, VEC2D* pPos);
	void GetRankName(INT32 rankidx, char* name);

	void SetAbleToSelectRank(INT32 idx, INT32 bounds);
};

#endif
