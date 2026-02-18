#pragma once

enum RIDE_HELI_SPR
{
	RIDE_HELI_SPR_HELICOPTER = 0,
	RIDE_HELI_SPR_BODY_LEFT,
	RIDE_HELI_SPR_BODY_CENTER,
	RIDE_HELI_SPR_BODY_RIGHT,
	RIDE_HELI_SPR_GAGE,

	RIDE_HELI_SPR_COUNT,
};

enum EUH60_CONTROL_MARK
{
	ECM_FLY_AND_CONTROL_M197 = 0,
	ECM_CONTROL_M197,
	ECM_UNCONTROL_M197,
	ECM_NONE,
	ECM_COUNT,
};

class CHud;

class CHud_Helicopter :	public i3GameObjBase
{
	I3_CLASS_DEFINE( CHud_Helicopter, i3GameObjBase );

public:
	CHud_Helicopter(void);
	~CHud_Helicopter(void);

public:
	static CHud_Helicopter* instance();

public:
	void Create(CHud * pHud);

	void OnUpdate( REAL32 rDeltaSeconds );
	void SetRidingMark(EUH60_CONTROL_MARK eMark);
	void UpdateWarningNotice();

private:
	CHud*	m_pHud;
};
