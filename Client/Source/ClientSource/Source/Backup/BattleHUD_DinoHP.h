#ifndef _BATTLE_HUD_DINOHP_H__
#define _BATTLE_HUD_DINOHP_H__

#if 지워질_것들

enum DINOHP_SPR
{
	DINOHP_SPR_TREX = 0,
	DINOHP_SPR_BODY_LEFT,
	DINOHP_SPR_BODY_CENTER,
	DINOHP_SPR_BODY_RIGHT,
	DINOHP_SPR_GAGE,

	DINOHP_SPR_COUNT,
};

class CHud;

class CHUD_DinoHP :	public i3GameObjBase
{
	I3_CLASS_DEFINE( CHUD_DinoHP, i3GameObjBase );

protected:
	CHud*			m_pHud;

	i3Sprite2D *	m_pSprite;

	REAL32			m_fLocalTime;
	REAL32			m_fGageLength;
	INT32			m_nPosX;
	INT32			m_nPosY;
	REAL32			m_fClanOffsetY;
	BOOL			m_bUpdateGage;

public:
	CHUD_DinoHP(void);
	~CHUD_DinoHP(void);

	// Create()는 반드시 호출되어야 합니다.
	void Create( CHud * pHud);

	void OnUpdate( REAL32 rDeltaSeconds );
//	void SetRidingMark(EUH60_CONTROL_MARK eMark);
	void UpdateHP(REAL32 fHp);
};

#endif

#endif //_BATTLE_HUD_HELICOPTER_H__