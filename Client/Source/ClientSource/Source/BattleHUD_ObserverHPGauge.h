#pragma once

class CHud;

class CHUD_ObserverHPGauge : public i3GameObjBase
{
	I3_CLASS_DEFINE( CHUD_ObserverHPGauge, i3GameObjBase );

public:
	CHUD_ObserverHPGauge();
	~CHUD_ObserverHPGauge();

public:
	static CHUD_ObserverHPGauge* instance();

public:
	void	Create( CHud * pHud);

	void	SetEnableGauge( bool bFlag);    
	void	OnUpdate( REAL32 rDeltaSeconds );
	void	UpdateGauge( REAL32 rDeltaSeconds);	

	INT32	getChatOffset(void)				{ return m_nOffsetHeight;			}

private:
	CHud*	m_pHud;
	i3Sprite2D *	m_pSprite;
	i3TextNodeDX2 *	m_pTextTitle[2];
	i3TextNodeDX2 *	m_pTextName[SLOT_MAX_COUNT];
	i3TextNodeDX2 *	m_pTextCount[SLOT_MAX_COUNT];
	i3TextNodeDX2 *	m_pTextWeapon[SLOT_MAX_COUNT];

	i3Sprite2D *	m_pSprite2;

	bool			m_bEnableHPGauge;
	INT32			m_nOffsetHeight;

	// 처음 GameContext의 쓰레기 값으로 인해 제대로 된값이 업데이트 되지 않을수 있으므로, 초기 실행시 약간 딜레이된후 실행하도록 합니다.
	bool			m_bFirstIn;
	REAL32			m_fDelayTime;


private:
	void CreateGMObserverTeamGuageSpr();
};
