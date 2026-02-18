#if !defined( __TARGET_IN_TUTORIAL_H)
#define __TARGET_IN_TUTORIAL_H

#include "GameControledObject.h"

enum TARGET_ANIM_STATE
{
	TAS_IDLE,
	TAS_DOWN,
	TAS_DOWN_IDLE,
	TAS_RETURN,
	TAS_COUNT
};

class CTargetInTutorial : public CGameControledObject
{
	I3_CLASS_DEFINE( CTargetInTutorial, CGameControledObject);
private:
	i3Animation* m_pAnim[TAS_COUNT];
	TARGET_ANIM_STATE m_eTargetAnimState;
	REAL32 m_rStateTime;
public:
	CTargetInTutorial();
	virtual ~CTargetInTutorial();

	virtual void	Create( i3Framework * pFramework, i3GameResObject * pRes) override;
	virtual void	Damage( REAL32 dmg, i3GameObj * pEventSender, i3PhysixShapeSet * pHitShapeSet) override;
	virtual void	OnUpdate( REAL32 tm) override;
	void			SetTargetState(TARGET_ANIM_STATE eState);
	TARGET_ANIM_STATE GetTargetState() { return m_eTargetAnimState; }
	void			FirstTick();
	void			ResetObject();
	void			ResetObjectOnInit();
	void			InitObject() {}
	void			EnableObject()	{}
	void			ProcessObject( REAL32 rDeltaSeconds) {}
	void			SetControledObjectState( INT32 state) {}
};

#endif // __TARGET_IN_TUTORIAL_H7