#if !defined( _CGAME_OBJECT_WITH_WEAPON_H__)
#define _CGAME_OBJECT_WITH_WEAPON_H__

#include "GameControledObject.h"

class WeaponOnObject;
class CGameCharaBase;

class CGameObjectWithWeapon : public CGameControledObject
{
private:
	INT32	m_iEnterCharaIdx;
	REAL32	m_rEnableTime;
	bool	m_bHalfDestroyed;

protected:
	WeaponOnObject* m_pWeaponOnObject;

public:
	CGameObjectWithWeapon();
	virtual ~CGameObjectWithWeapon();

	void					InitEnterCharaIdx();
	virtual void			ProcessObject( REAL32 rDeltaSeconds) override;

	virtual void			ResetObject() override;
	virtual void			ProcessIntrude( const GAMEINFO_OBJECT_NETDATA * pNetData) override;

	void					EnterObject(CGameCharaBase* pChara);
	void					LeaveObject();
	void					LeaveObject(INT32 iIdx);

	VEC3D*					GetMuzzlePos();
	WeaponOnObject*			GetWeaponOnObject() { I3ASSERT(m_pWeaponOnObject); return m_pWeaponOnObject; }
	INT32					GetEnterCharaIdx() { return m_iEnterCharaIdx; }

	bool					IsEnableEnter(CGameCharaBase* pChara);
	bool					IsEnableEnter(CGameCharaBase* pChara, bool bTakeObject);
	bool					EnableOnOffObject(UINT64 keyStroke);
	bool					IsEnableTime();

	void					ExplosionObject( i3GameObj * pSender);
	// revision 62269 오브젝트 폭파 컬리전 체크 수정 및 파괴미션 헬기 버그 수정
	void					SetHalfDestroyed(bool isHalf)		{m_bHalfDestroyed = isHalf;}
	bool					GetHalfDestroyed()					{return m_bHalfDestroyed;}
};

#endif // _CGAME_OBJECT_WITH_WEAPON_H__