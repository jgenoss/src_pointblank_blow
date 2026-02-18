#if !defined(__OBJ_EXPLOSION_H)
#define __OBJ_EXPLOSION_H

#include "tinst.h"

enum HIT_CHECK
{
	HIT_CHECK_HEAD = 0,
	HIT_CHECK_BODY,
	HIT_CHECK_COUNT,
};

class obj_explosion
{
public:
	static obj_explosion*	i() { return tinst<obj_explosion>(); }
	
	void	ProcessCreateStage();
	void	Process_InitObject(INT32 nStageObjCount);
	void	Process_ResetObjectExploded(CStageBattle * sb);
	void	ProcessDestroyStage();
	
	void	ProcessUpdate(CStageBattle* sb);

	/** \brief 오브젝트 폭발에 대한 처리
		\desc 다른 오브젝트 및 Character에 대한 Hit를 처리합니다. */
	void	call( i3GameObj * pObject, VEC3D * pPos, REAL32 rRange, REAL32 rForce, REAL32 rDamage, REAL32 rSlop );
	/** \brief 오브젝트 폭발에 대한 처리
		\desc DedicatedMode에서 오브젝트 Hit로 다음에 폭발시 미리 처리합니다.
			미리 처리된 패킷을 오브젝트 히트와 같이 보냅니다.
			폭발과 데미지에 대한 시점을 맞추기 위해서 입니다. */
	void	call_for_dedi( CGameCharaBase * pShooter,  CHARA_DAMAGEINFO& DamageInfo, i3Object * pObject);

	void	CollisionCalcHitByObject( UINT8 nCharaHitCheck[][HIT_CHECK_COUNT],
		CHARA_DAMAGEINFO& DamageInfo, CHARA_HIT_PART hitPart, i3CollideeLine& ColLine, REAL32& rLength,
		CGameCharaBase* pChara, CGameCharaBase* pShooter);

private:
	
	obj_explosion();
	~obj_explosion();

	friend tinst_creator<obj_explosion>;

	void			CalcHitByObject( UINT8 nCharaHitCheck[][HIT_CHECK_COUNT],
		CHARA_DAMAGEINFO& DamageInfo, i3CollideeLine& ColLine, REAL32& rLength,
		CGameCharaBase* pChara, CGameCharaBase* pShooter);

	// HIT_RESULT를 넘겨줍니다. CheckObjCollision 내부에서 호출합니다.
	REAL32			SetObjForce( I3_PHYSIX_HIT_RESULT * pResult, VEC3D * pStartPos, REAL32 rRange, REAL32 rForce, REAL32 rSlop );



	UINT8*	m_pObjectHitCheck;
	// for dedi
	REAL32*	m_pObjectAccDamage;	// Object 갯수 만큼의 배열 누적된 Damage
	UINT8*	m_pObjectExploded;	// Object 폭발의 계산 여부
};





#endif