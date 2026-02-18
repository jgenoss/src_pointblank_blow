#if !defined( __UI_NEWANDHOT_BANNER_V11_H__)
#define __UI_NEWANDHOT_BANNER_V11_H__

#include "UI/UIFloatingBase.h"

class UIImgSetCtrl;

class UINewAndHotBanner_V11 : public UIFloatingBase
{
	I3_CLASS_DEFINE( UINewAndHotBanner_V11, UIFloatingBase);

	enum 
	{ 
		GOODS_CHANGE_TIME = 5,
		GOODS_ANIMATION_TIME = 1
	};

public:
	UINewAndHotBanner_V11();
	virtual ~UINewAndHotBanner_V11();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	OnLoadAllScenes() override;
	virtual void	OnUnloadAllScenes() override;
	virtual bool	OnEntranceStart( void* pArg1 = nullptr, void* pArg2 = nullptr) override;
	virtual void	OnExitEnd() override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	void			MakeNewAndHotGoodsList( void);

	INT32			SelectGoodsInList();							

	inline void		SetMakeList( bool bMake) { m_bMakeGoodsList = bMake;}	
	inline bool		GetMakeList() { return m_bMakeGoodsList;}

	inline void		SetNewAndHotClicked( bool bClicked) { m_bNewAndHotClicked = bClicked;}
	inline bool		GetNewAndHotClicked() { return m_bNewAndHotClicked;}
private:
	bool			m_bMakeGoodsList;
	bool			m_bNewAndHotClicked;

	INT32			m_nCurGoodsIndex;
	REAL32			m_fChangeTime;

	bool			m_bOnAni;
	REAL32			m_fAniTime;

	i3::vector<const SHOP_GOODS_PACK*> m_RefGoodsList;


	UIImgSetCtrl*	m_goods_img_ctrl;
	UIImgSetCtrl*	m_prev_img_ctrl;

	void			_InitVariables( void);

	void			_SetBlankGoods( void);
	void			_MakeGroupList_ShopGoodsType( i3::vector<const SHOP_GOODS_PACK*>& refGoodsList, SHOP_GOODS_TYPE t);

	void			_StartAni( INT32 cur, INT32 prev );
	void			_EndAni( void);
	void			_AniUpdate( REAL32 rDeltaSeconds);

	INT32			_SelectIndexBySequence( INT32 nCurIndex, INT32 nRange); //¼øÂ÷
	INT32			_SelectIndexByRandom( INT32 nCurIndex, INT32 nRange);	//·£´ý
};

#endif
