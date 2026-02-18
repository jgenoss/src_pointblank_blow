#if !defined( __CROSSHAIR_H)
#define __CROSSHAIR_H

class CWeaponInfo;

class Crosshair : public i3GameObjBase
{
	I3_CLASS_DEFINE( Crosshair, i3GameObjBase);

protected:
	i3AttrSet *			m_pSg = i3AttrSet::new_object();

	VAR_LR(UINT8, 94)	m_CR_R;
	VAR_LR(UINT8, 95)	m_CR_G;
	VAR_LR(UINT8, 96)	m_CR_B;
	VAR_LR(UINT8, 97)	m_CR_A;

	VAR_LR(UINT8, 98)	m_CR_Org_R;
	VAR_LR(UINT8, 99)	m_CR_Org_G;
	VAR_LR(UINT8, 100)	m_CR_Org_B;
	VAR_LR(UINT8, 101)	m_CR_Org_A;

	VAR_LR(REAL32, 102)	m_fHitEffectSize = 0.0f;
	VAR_LR(BOOL, 103)		m_bHitEffect = false;
	VAR_LR(REAL32, 104)	m_CrossSizeOffset = 0.0f;

// Hit Effect
	i3Sprite2D	*		m_pHitSprite = nullptr;
	i3Texture	*		m_pHitTexture = nullptr;

	CGameCharaBase*			m_pTargetChar;

protected:
	CGameCharaBase *	_GetCurrentChara( void);

	// 현재 설정된 캐릭터의 현재무기를 반환한다.
	WeaponBase *		_GetCurrentWeapon( void);

	CWeaponInfo *		_GetCurrentWeaponInfo( void);

public:
	Crosshair(void);
	virtual ~Crosshair(void);

	virtual void	Create( void);

	virtual	void	SetEnable( bool bTrue);
	void			SetEnableHitEffect( bool bEnable);
	void			ResetHitEffect();

	void			SetCrosshairColor( UINT8 r, UINT8 g, UINT8 b)		{	m_CR_R = r; m_CR_G = g; m_CR_B = b;		}
	void			SetCrosshairColor()									{	m_CR_R = m_CR_Org_R; m_CR_G = m_CR_Org_G; m_CR_B = m_CR_Org_B;		}
	void			SetCrosshairAlpha( UINT8 a)							{	m_CR_A = a;	}
	void			SetCrossSizeOffset(REAL32 fm)						{	m_CrossSizeOffset = fm;		}
	REAL32			GetCrossSizeOffset()								{	return m_CrossSizeOffset;	}
	void			CalcRminLen(REAL32 &rmin, REAL32& len, REAL32 scrCY, CWeaponInfo* pWeaponInfo);

	virtual void	ResetCrosshair() {}

	virtual void	OnUpdate( REAL32 tm) override;
	void			setTargetChara(CGameCharaBase* pChara);
	const CGameCharaBase* getTargetChara() const{ return m_pTargetChar;}
};

#endif
