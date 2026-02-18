#pragma once

#include "BattleHUD_Define.h"

class CHUD_Revenge  : public i3GameObjBase
{
	I3_CLASS_DEFINE(CHUD_Revenge, i3GameObjBase);

public:
	CHUD_Revenge(void);
	~CHUD_Revenge(void);

public:
	static CHUD_Revenge* instance();

public:
	void CreateHUD(i3AttrSet* pHudRoot);
	void OnUpdate( REAL32 rDeltaSeconds );

	void	RecordRevenge(INT32 index, INT32 nPoint);
	void	DeleteRecordRevenge(INT32 index);
	void	UpdateRevenge(REAL32 rDeltaSeconds);
	void	SetRevenge(bool bTrue);

private:
	REVENGE_ARRAY m_Revenge[16];

	i3Texture* m_pTexture;

	i3Sprite2D* m_pRevengeInfo;
	i3TextNodeDX2* m_pRevengeTextBlue;
	i3TextNodeDX2* m_pRevengeTextRed;
	i3TextNodeDX2* m_pRevengeTextBlueCount;
	i3TextNodeDX2* m_pRevengeTextRedCount;

	REAL32 m_fRevengeLocalTime;
	bool m_bEnableRevengeUpdate;
};
