#pragma once

enum ANIMATION_SEX_TYPE
{
	ANIMATION_MALE = 0,
	ANIMATION_FEMALE,

	ANIMATION_SEX_MAX,
};

enum ANIMATION_PERSON_TYPE
{
	ANIMATION_FIRST_PERSON = 0,
	ANIMATION_THIRD_PERSON,

	ANIMATION_PERSON_MAX,
};

class WeaponNunchaku : public WeaponKnife
{
	I3_CLASS_DEFINE( WeaponNunchaku, WeaponKnife);

public:
	WeaponNunchaku();
	virtual ~WeaponNunchaku();

	virtual MainWeapon*	CreateMainWeapon() override;

public:
	virtual void	OnBindResource( void) override;

	// 나눠지는 Animation들
	virtual void	_OnPlayWeaponIdleAnim(void) override;
	virtual void	_OnPlayWeaponFireAnim(void) override;
	virtual void	_OnPlayWeaponSecondFireAnim( void) override;
	
	virtual void	PlayAnim_UI_IdleA( void) override;
	virtual void	PlayAnim_UI_IdleB( void) override;

	/** \breif 무기 교체 애니메이션을 플레이합니다. */
	virtual void	OnPlayChangeAniImpl( REAL32 rTimeScale = 1.f) override;

	virtual void	OnEndFire(i3AIContext * pCtx, REAL32 tm) override;

private:
	INT8	m_ai_Change[ANIMATION_SEX_MAX][ANIMATION_PERSON_MAX];
	INT8	m_ai_Idle[ANIMATION_SEX_MAX][ANIMATION_PERSON_MAX];

	INT8	m_ai_AttackA[ANIMATION_SEX_MAX][ANIMATION_PERSON_MAX];
	INT8	m_ai_AttackB[ANIMATION_SEX_MAX][ANIMATION_PERSON_MAX];

	INT8	m_ai_IdleA[ANIMATION_SEX_MAX];
	INT8	m_ai_IdleB[ANIMATION_SEX_MAX];
};