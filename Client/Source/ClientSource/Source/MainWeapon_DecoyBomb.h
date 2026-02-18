#pragma once

#include "MainWeapon_Grenade.h"

class WeaponDecoySndInterval;

class MainWeapon_DecoyBomb : public MainWeapon_Grenade
{
protected:

	// DecoySndIntervalGenerator =============================================
	struct DecoySndIntervalGenerator
	{
		DecoySndIntervalGenerator(const WeaponDecoySndInterval* intervalData);
		~DecoySndIntervalGenerator();

		// -1 : 종료
		// 0  : 진행중
		// 1  : 다음 Interval로 바뀜
		INT32 Update(REAL32 dSecond);
		void Reset(bool loop=false);

		INT32								m_indexInterval;
		REAL32								m_elapsed;
		REAL32								m_lastIntervalChange;
		INT32								m_curIndexInterval;

		const WeaponDecoySndInterval*		m_intervalData;
	};
	// DecoySndIntervalGenerator =============================================

public:

	MainWeapon_DecoyBomb(WeaponBase* p);
	virtual ~MainWeapon_DecoyBomb();
	
	virtual void	Reset() override;

	virtual bool	UpdateDecoying(REAL32 dSecond); // dSecond == -1.0f 이면 디코잉 강제종료
protected:
	
private:
	
	virtual void	Explosion( void) override;

private:
	
	bool						m_isSetted;
	bool						m_isDecoying;

	REAL32						m_decoyLifeTime;
	REAL32						m_decoyTime;

	DecoySndIntervalGenerator*	m_intervalGenerator;

	bool						m_oneTimeDecoySndCreate;
	i3SoundPlayInfo*			m_decoySndPlayInfo;
	i3SoundPlayState*			m_decoySndPlayState;
	MATRIX						m_decoySndTM;

	INT32						m_decoyEffectIndex; // 점멸이펙트
	i3TimeStamp*				m_decoyEffect;

private :

	// virtual function
	virtual const WeaponDecoySndInterval *	_GetSndInterval(void);
	virtual i3SoundPlayInfo	*				_GetSoundPlayInfo(void);

	void OnSettled();
	void ResetDecoyStatus();
	WeaponBase* GetOwnerWeapon( WEAPON_SLOT_TYPE slot=WEAPON_SLOT_PRIMARY);

protected:
	i3SoundPlayInfo* GetDefaultDecoySound();

};