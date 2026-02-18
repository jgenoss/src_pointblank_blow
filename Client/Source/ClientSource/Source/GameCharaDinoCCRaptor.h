#ifndef GAMECHARA_DINO_CCRAPTOR_H_
#define GAMECHARA_DINO_CCRAPTOR_H_

#include "GameCharaDinoRaptor.h"

class CGameCharaDinoCCRaptor : public CGameCharaDinoRaptor
{
	I3_CLASS_DEFINE( CGameCharaDinoCCRaptor, CGameCharaDinoRaptor);

private:
	DinoSkill::Buff* m_buff = nullptr;

protected:
	virtual void		_CreateDino( void) override;

public:
	virtual ~CGameCharaDinoCCRaptor();

	virtual void				Create(GLOBAL_CHARA_INFO* pCharaInfo, const CCharaInfo* pDBInfo) override;
	virtual void				Destroy( void) override;
	virtual void				OnUpdate( REAL32 rDeltaSeconds) override;

public:
	ID_UPPER_DINO_AI			DinoAttack(WEAPON::FIRE_ORDER fireOrder);
	void						OnDinoAttackRun(i3AIContext * pCtx, REAL32 tm);

	virtual void				ResetChara() override;

	DinoSkill::Buff*			GetBuff() const { return m_buff; }

};

#endif

