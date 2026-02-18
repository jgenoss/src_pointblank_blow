#ifndef _GAME_CHARA_FULLBODY_H_
#define _GAME_CHARA_FULLBODY_H_

#include "FullbodyFSM.h"
#include "../../GameCharaBase.h"

#define FULLBODY_AI_MAX 40

class CGameCharaFullbody : public CGameCharaBase
{
	I3_ABSTRACT_CLASS_DEFINE(CGameCharaFullbody, CGameCharaBase);

public:
	virtual ~CGameCharaFullbody();

	virtual void			Create(GLOBAL_CHARA_INFO*, const CCharaInfo*) override;
	virtual void			CreateFirst(const CCharaInfo*, bool bInLobby = false) override {}
	virtual void			CreateLater(const CCharaInfo*, bool bLoadWeapon = true) override {}
	virtual void			ResetChara() override;
	virtual void			OnBindResource() override;
	virtual void			OnUpdate(REAL32) override;

public:
	void					PlayFullbody(INT32 id);
	void					OnFullbodyRun(INT32 id, REAL32 tm);
	const fb_fsm::StateMgr*	GetFBStateMgr() const;
	fb_fsm::StateMgr*		GetFBStateMgr();
	i3AIContext*			GetAiContext() const;

protected:
	// 풀바디에서 상하체 본은 필요없다.
	virtual INT32			_GetUpperBoneCount() const override { return 0; }
	virtual INT32			_GetLowerBoneCount() const override { return 0; }

protected:
	i3AIContext*		m_ai = nullptr;
	fb_fsm::StateMgr	m_smgr;
	static const char*	s_fullbodyName[FULLBODY_AI_MAX];
};

#endif