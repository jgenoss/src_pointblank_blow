#if !defined( __CROSSHAIRMGR_H)
#define __CROSSHAIRMGR_H

//
// StageBattle에서 크로스헤어 분리작업..
//
#include "tinst.h"

enum CROSSHAIR_MODE
{
	CH_MODE_INVALID = -1,
	CH_MODE_NORMAL = 0,
	CH_MODE_SCOPE,
	CH_MODE_RAPTOR,
	CH_MODE_TREX,
	CH_MODE_STING,
	CH_MODE_ELITE,
	CH_MODE_OBSERVER,
	CH_MODE_COUNT
};

class Crosshair;


class CrosshairMgr 
{
public:
	static CrosshairMgr*	i() { return tinst<CrosshairMgr>(); }

	Crosshair *				getCrosshair( void)	const { return m_pCrosshair;	}
	void					setCrosshairType(CROSSHAIR_MODE mode);
	void					enableCrossHair(bool enable);
	bool					isEnableCrossHair() const; 

	void					ProcessCreateStage(i3Stage* parent);
	void					ProcessDestroyStage();
	
	void					ApplyEnvSetCrosshair();
	void BackupEnable(bool force_hide = true);
	void RestoreEnable(bool force_show = false);

	void					setUserScopeTypes(INT32 idx, UINT8 type) { m_UserScopeTypes[idx] = type; }
	void					ApplyTargetCrosshair(CGameCharaBase* pChara);
	void					setTargetChara(CGameCharaBase* pChara);
private:
	Crosshair			*	m_pCrosshair;		//내거

	Crosshair			*	m_apCrosshairs[CH_MODE_COUNT];
	i3Stage*				m_parent;
	bool m_BackupEnableCrosshair;

	UINT8					m_UserScopeTypes[SLOT_MAX_COUNT];

	void					_ProcessCreateDinoCH();
	void					_ProcessCreateDinoCH_Detail(CROSSHAIR_MODE mode);
	void					_ProcessCreateScopeCH();

	
	CrosshairMgr();
	~CrosshairMgr();
	friend tinst_creator<CrosshairMgr>;
};

tinst<CrosshairMgr>;





#endif
