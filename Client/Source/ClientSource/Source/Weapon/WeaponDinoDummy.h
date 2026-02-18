#if !defined( _WEAPON_DINO_DUMMY_H__)
#define _WEAPON_DINO_DUMMY_H__

#include "weaponbase.h"

class i3Quad;

enum DUMMY_BEHAVIOR
{
	DUMMY_DESTROY,
	DUMMY_IDLE,
	DUMMY_NONE,
	DUMMY_BEHAVIOR_MAX,
};

class WeaponDinoDummy :	public WeaponBase
{
	I3_CLASS_DEFINE( WeaponDinoDummy, WeaponBase);
public:
	WeaponDinoDummy(void);
	virtual ~WeaponDinoDummy(void);
	
	virtual void	Create(CGameCharaBase* owner, T_ItemID itemID, bool bCreateInWeapon) override;
	virtual void	Reset() override;
	virtual void	InstallToWorld(i3Node* pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pPos, bool bToWorld, MATRIX* pMat, REAL32 theta = 0.0f) override;
	virtual	void	ReturnInstance() override;
	virtual void	OnBindResource() override;
	virtual void	OnUpdate(REAL32 tm) override;
	virtual void	_OnCheckCollisionByExplosion() override;

	void	OnNone(i3AIContext* ctx, REAL32 tm);
	void	OnIdle(i3AIContext* ctx, REAL32 tm);
	void	OnDestroy(i3AIContext* ctx, REAL32 tm);

	void	Uninstall();
	void	SetHP(INT16 hp);
	INT16	GetHP() const { return m_hp; }
	REAL32	GetHPRatio() const;

	CGameCharaBase* GetInstaller() const { return m_installer; }

protected:
	virtual void	_InitCollideeGroup( void) override;
	void			_OnCollision(CGameCharaBase * pChara, i3CollideeLine * pLine, I3_PHYSIX_HIT_RESULT * pResult);

private:
	i3Node*			m_Dummy;
	i3AttrSet*		m_pAttrSet;
	i3Quad*			m_pWeaponTag;
	INT16			m_hp;
	INT16			m_max_hp;
	CGameCharaBase*	m_installer;
};


#endif