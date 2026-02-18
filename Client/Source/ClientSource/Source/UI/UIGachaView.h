
#if !defined( __UI_LOTTERY_VIEW_H__)
#define __UI_LOTTERY_VIEW_H__

#include "UIGachaDef.h"
#include "UISlide.h"

class GachaObject;

class UIGachaView : public i3GameNode
{
	I3_CLASS_DEFINE( UIGachaView, i3GameNode);

private:
	REAL32				m_rCharaChangePoseTime;

	REAL32				m_LightTheta;
	REAL32				m_LightRho;
	REAL32				m_LightDist;
	REAL32				m_timeLocal;

	i3UI3DViewBox *		m_p3DView;
	GachaObject *		m_pGacha;
	i3UIFrameWnd *		m_pParentFrm;
	i3UIFrameWnd *		m_pParentFrm2;

	i3Light *			m_pLight;

	bool				m_bStartMoveFrame;
	GACHA_ANIM_STATE	m_AniState;

	REAL32				weaponIn_time;
	REAL32				weaponOut_time;
	bool				m_bComboStart1;
	REAL32				m_rComboElapsedTime1;
	bool				m_bComboStart2;
	REAL32				m_rComboElapsedTime2;
	GACHA_ITEM_INFO *	m_pSelectedItem;
	UI::tmSlide			m_Slide;

public:
	bool				m_bObjectAniStart;
	REAL32				m_rObjectDurationTime;
	REAL32				m_rObjectElapseTime;

protected:

	void			_AddLight( void);
	void			_ReleaseLight( void);


public:
	UIGachaView();
	virtual ~UIGachaView();

	GachaObject *			getRouletteObject( void)			{ return m_pGacha; }

	i3UI3DViewBox *			get3DViewBox()						{ return m_p3DView;}

	void			OnIdleStart();

	void			Create3DViewBox( i3UIControl * pParent, i3UIScene * pScene);
	void			SetPos3DViewBox(void);

	void			Delete3DViewBox( void);

	void			CreateViewGacha();

	void			ProcessGameEvent(GAME_EVENT evt,INT32 arg, const i3::user_data& UserData);

//	void			UpdateParentSize();

	void			setItemInfo(const GACHA_ITEM_INFO * pInfo);

	void			setAnimationState(GACHA_ANIM_STATE pState)	{ m_AniState = pState;	}

	void			setBuyAnimationStart();
	
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	void			setFrame2(i3UIFrameWnd * pfrm)		{ m_pParentFrm2 = pfrm;}

	void			AddGachaItem(GACHA_WEAPON_POS _posIdx, T_ItemID _itemID);

public: 
//	void 			HDIObject1();
//	void 			HDIObject2(REAL32 rDeltaSeconds);

	void BeginGachaPlayingAni();
	bool UpdateGachaPlayingAni(REAL32 rDeltaSeconds);

};

#endif
