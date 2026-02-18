#if !defined( __GAME_CHARA_BONE_DINO_CTX_H__)
#define __GAME_CHARA_BONE_DINO_CTX_H__

#include "../GameCharaBoneContext.h"

class CGameDinoBoneContext : public CGameCharaBoneContext
{
	I3_CLASS_DEFINE( CGameDinoBoneContext, CGameCharaBoneContext);

protected:
	virtual void	__InitBone( void) override;
	virtual void	_DoUpdateRhoBone1PV( MATRIX * pMod) override;
	virtual void	_DoUpdateRhoBone3PV( MATRIX * pMod) override;
	virtual void	_CalcAimBone( REAL32 rDeltatime) override;

	virtual void	_ProcessFire( void) override {}

public:
	virtual REAL32	getViewRho( void) override;

	virtual void	CalcDeviation( REAL32 rDeltatime) override {}
	
	virtual INT32	GetRootBone( void) override;
	virtual INT32	GetSpine3Bone( void) override;
};

#endif
