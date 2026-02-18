#if !defined( __GAME_CHARA_DINO_RAPTOR_H__)
#define __GAME_CHARA_DINO_RAPTOR_H__

#include "GameCharaDino.h"

class CGameCharaDinoRaptor : public CGameCharaDino
{
    I3_CLASS_DEFINE( CGameCharaDinoRaptor, CGameCharaDino);

protected:
	INT32				m_idxRaptorBuffEffect = 0;

protected:
	/** \brief 상체 Bone Count */
	virtual INT32		_GetUpperBoneCount( void) const override { return CHARA_UPPER_RAPTOR_BONE_COUNT;}

	virtual void		_CreateDino( void) override;

	virtual void		_PlayDinoSound_Fire(WEAPON::FIRE_ORDER fireOrder) override;
	virtual void		_PlayDownfallSound( void) override;

public:
	CGameCharaDinoRaptor();
	
	// 공격	
	
public:

	/**
	 * \fn	ID_UPPER_DINO_AI CGameCharaDinoRaptor::DinoAttack(WEAPON::FIRE_ORDER fireOrder);
	 *
	 * \brief	CGameCharaInputControlLocal::ProcessDinoAttack에서 해당 함수를 호출함
	 * 			공룡 애니메이션과 사운드를 설정함
	 *
	 * \param	fireOrder	The fire order.
	 *
	 * \return	.
	 */

	ID_UPPER_DINO_AI			DinoAttack( WEAPON::FIRE_ORDER fireOrder);

	/**
	 * \fn	void CGameCharaDinoRaptor::OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm);
	 *
	 * \brief	공룡이 공격 애니메이션 플레이 중일 때 계속 호출 됨\n
	 * 			공격 판정이 나는 시점이 되면 FireCollisionDino함수를 호출함\n
	 * 			(FireCollisionDino함수에서 충돌과 데미지 관련 처리를 함)
	 *
	 * \param [in,out]	pCtx	If non-null, the context.
	 * \param	tm				The time.
	 */

	void						OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm);
	virtual	WEAPON::FIRE_ORDER	GetFireOrder() override;

	virtual void				_PlayBreathSnd(void) override;

	virtual void				ResetChara() override;

	virtual void				SetDinoFireStartPos(VEC3D * pSrc) override;

	virtual EDinoAttackType		GetDinoAttackType(INT32 idx) override;
};

#endif //__GAME_CHARA_DINO_RAPTOR_H__
