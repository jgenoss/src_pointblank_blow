#if !defined( _GAME_CHARA_DINO_H__)
#define _GAME_CHARA_DINO_H__



class CGameCharaBase;
class WeaponDino;

#define DINO_WEAPON_COUNT 2

enum EDinoAttackType
{
	DAT_RAPTOR_ATTACK_A,	//랩터 할퀴기
	DAT_RAPTOR_ATTACK_B,	//랩터	 물기
	DAT_REX_ATTACK1,		//티라노 물기
	DAT_REX_ATTACK2,		//티라노하울링
	DAT_STING_ATTACK1,		//스팅 침뱉기
	DAT_STING_ATTACK2,		//스팅 독안개
	DAT_TANK_BUTT,			//탱크 박치기
	DAT_TANK_DASH,			//탱크   돌진
	DAT_ACID_BITE,			//에시드 물기
	DAT_ACID_EXPLOSION,		//에시드 폭팔
	DAT_ELITE_SCRATCH,		//엘리트 햘퀴기
	DAT_ELITE_HOWL,			//엘리트하울링
	RAT_COUNT
};

/**
 * \class	CGameCharaDino
 *
 * \brief	Game chara dino. 
 */
class CGameCharaDino : public CGameCharaBase
{
    I3_ABSTRACT_CLASS_DEFINE( CGameCharaDino, CGameCharaBase);

protected:
	/** \brief 상체 Bone Count */
	virtual INT32		_GetUpperBoneCount( void) const override { return CHARA_UPPER_DINO_BONE_COUNT;}

	/** \brief 하체 Bone Count */
	virtual INT32		_GetLowerBoneCount( void) const override { return CHARA_LOWER_DINO_BONE_COUNT; }

	virtual void		_PlayDownfallSound( void) override;

public:
	CGameCharaDino();
	virtual ~CGameCharaDino();

	virtual void				Create( GLOBAL_CHARA_INFO * pCharaInfo, const CCharaInfo * pDBInfo) override;
	virtual void				CreateFirst( const CCharaInfo * pDBInfo, bool bInLobby = false ) override;
	virtual void				ResetChara() override;
	virtual void				OnBindResource( void) override;

	// 공격
protected:
	EDinoAttackType				m_eNowDinoAttackType = RAT_COUNT;
	
	WeaponBase*					m_pCurDinoWeapon = nullptr;
	WeaponDino*					m_arDinoWeapon[DINO_WEAPON_COUNT];	// [initialize at constructor]

	i3Transform *				m_pFXDummy = nullptr;

private:
	bool						m_bNowAttackFirstFrame = false;
	REAL32						m_rElapsedBreathTime;				// [initialize at constructor]

protected:
	virtual void				_PlayBreathSnd(void) {}

	void						_PrepareLowerAI( void);
	void						_PrepareUpperAI( void);

	virtual void				_CreateDino( void);

	void						_PlayDinoAnimUpper_Fire(WEAPON::FIRE_ORDER fireOrder, ID_UPPER_DINO_AI DinoAI);
	virtual void				_PlayDinoSound_Fire(WEAPON::FIRE_ORDER fireOrder) {};

	ID_UPPER_DINO_AI			_FindDinoUpperAttackAI(WEAPON::FIRE_ORDER fireOrder);

public:
	void						ChangeDinoWeapon( WEAPON::FIRE_ORDER order);

	virtual ID_UPPER_DINO_AI	DinoAttack( WEAPON::FIRE_ORDER fireOrder) = 0;
	virtual void				OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm) = 0;

	/** \brief 컬리젼시 히트 이펙트
		\note Tank, Elite 공룡만 사용 */
	virtual void			DoFireHitEffect( void) {}

	WeaponBase*				GetDinoWeapon() const	{ return m_pCurDinoWeapon; }
	WeaponBase*				GetDinoWeapon(const WEAPON::FIRE_ORDER order) const;

	virtual MATRIX *		getFXMatrix(void)
	{
		if( m_pFXDummy == nullptr)	return nullptr;
		return m_pFXDummy->GetCacheMatrix();
	}

	void						SetNowAttackFirstFrame(bool bFlag) { m_bNowAttackFirstFrame = bFlag; }
	bool						GetNowAttackFirstFrame() { return m_bNowAttackFirstFrame; }
	virtual	WEAPON::FIRE_ORDER	GetFireOrder() = 0;

	EDinoAttackType				GetNowDinoAttackType() { return m_eNowDinoAttackType; }
	
	virtual WeaponBase*			GetCharaWeapon(INT32 iIdx)	const { return GetDinoWeapon(); }
	virtual WeaponBase*			GetCurrentCharaWeapon()		const override { return GetDinoWeapon(); }

	void						SwapWeaponDino();

	virtual void				OnUpdate( REAL32 rDeltaSeconds) override;

	// 피격..
	virtual bool				NET_HitForHost( CHARA_DAMAGEINFO * pDamageInfo, REAL32 rDamage) override;

	virtual void				SetDinoFireStartPos(VEC3D * pSrc);
	void						PlayDinoCrossHair(void);

	virtual REAL32				GetDinoSkillDuration(INT32 idx) const override;
	virtual REAL32				GetDinoSkillCoolTime(INT32 idx) const override;
	virtual REAL32				GetDinoSkillMovementRate(INT32 idx) const override;

	// 
	bool						isRun(INT32 idx);
	virtual bool				isWalk(INT32 idx);
	virtual bool				isSkipAttack(INT32 idx)			{ return false; }
	virtual EDinoAttackType		GetDinoAttackType(INT32 idx)	{ return m_eNowDinoAttackType; }
};

namespace DinoSkill
{
	struct BuffInfo
	{
		CGameCharaDino* owner = nullptr;
		INT32			effectIdx = 0;
		REAL32			cooldown = 0.0f;
		REAL32			buffSustain = 0.0f;
		REAL32			speedRate = 0.0f;
	};

	class Buff
	{
	public:
		Buff(const BuffInfo& info);
		~Buff();

		void	Start();
		void	Stop();
		void	Update(REAL32 deltaSeconds);
		REAL32	CooldownRate();	// 구HUD에서 참조하기 때문에 인터페이스로 빼둔다.
		bool	IsStarted() const { return m_started; }

	private:
		void	_UpdateGuage();

	private:
		BuffInfo	m_info;
		bool		m_started;
		REAL32		m_timeCurrent;
	};
}

#endif // __GAME_CHARA_DINO_H__
