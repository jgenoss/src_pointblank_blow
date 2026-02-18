#if !defined( __UI_NEWANDHOT_BANNER_H__)
#define __UI_NEWANDHOT_BANNER_H__

#include "UIFloatingBase.h"

class UIImgSetCtrl;

class UINewAndHotBanner : public UIFloatingBase
{
	I3_CLASS_DEFINE( UINewAndHotBanner, UIFloatingBase);

	enum 
	{ 
		GOODS_CHANGE_TIME = 5,
		GOODS_ANIMATION_TIME = 1
	};

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene) override;
	virtual void	_ClearAtUnload( i3UIScene * pScene) override;

public:
	UINewAndHotBanner();
	virtual ~UINewAndHotBanner();

	virtual void	OnCreate( i3GameNode * pParent) override;
	virtual void	OnLoadAllScenes() override;
	virtual void	OnUnloadAllScenes() override;
	virtual bool	OnEntranceStart( void* pArg1 = nullptr, void* pArg2 = nullptr) override;
	virtual bool	OnExitStart(void) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void	ProcessGameEvent( GAME_EVENT evt, INT32 arg, const i3::user_data &UserData) override;

	void			MakeNewAndHotGoodsList( void);

	INT32			SelectGoodsInList();		
	void			ChangeItemView(bool bNextItem);
	void			SetGoodsType(SHOP_GOODS* pGoods);

	inline void		SetMakeList( bool bMake) { m_bMakeGoodsList = bMake;}	
	inline bool		GetMakeList() { return m_bMakeGoodsList;}

	INT32			GetCurGoodsType() const { return m_nCurGoodsType; }

	inline void		SetNewAndHotClicked( bool bClicked) { m_bNewAndHotClicked = bClicked;}
	inline bool		GetNewAndHotClicked() { return m_bNewAndHotClicked;}
private:
	bool			m_bMakeGoodsList;
	bool			m_bNewAndHotClicked;

	INT32			m_nCurGoodsIndex;
	INT32			m_nCurGoodsType;
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
