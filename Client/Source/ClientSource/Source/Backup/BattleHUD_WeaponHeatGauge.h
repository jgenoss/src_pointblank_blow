#if !defined( __BATTLEHUD_WEAPONHEATGAUGE_H__)
#define __BATTLEHUD_WEAPONHEATGAUGE_H__

#if 지워질_것들
class CHud;

class CHUD_WeaponHeatGauge : public i3GameObjBase
{
	I3_CLASS_DEFINE( CHUD_WeaponHeatGauge , i3GameObjBase);

protected:
	CHud*	m_pHud;
	i3Sprite2D *	m_pSprite;
	i3TextNodeDX2 *	m_pTextGage;

	REAL32			m_fLocalTime;
	REAL32			m_fGageLength;
	INT32			m_nPosX;
	INT32			m_nPosY;
	BOOL			m_bUpdateGage;

	bool			m_bShowIcon;

public:
	CHUD_WeaponHeatGauge();
	~CHUD_WeaponHeatGauge();

	// Create()는 반드시 호출되어야 합니다.
	void	Create( CHud * pHud, bool bShowIcon = false);

	void	OnUpdate( REAL32 rDeltaSeconds );
	void	SetRidingMark( BOOL bEnable);
	void	UpdateGauge( REAL32 fGaugeValue);
};

#endif

#endif
