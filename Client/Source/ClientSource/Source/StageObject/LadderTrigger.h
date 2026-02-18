#if !defined( __LADDER_TRIGGER_H__)
#define __LADDER_TRIGGER_H__


struct LadderStateInfo;

const VEC3D*	GetCurrLadderAccAxis(const LadderStateInfo* info);
const VEC3D*	GetCurrLadderRightAxis(const LadderStateInfo* info);
bool			IsJumpingOnLadder(const LadderStateInfo* info);
void			ResetLadderJump(const LadderStateInfo* info);



class CLadderTrigger : public i3Trigger
{
	I3_CLASS_DEFINE( CLadderTrigger, i3Trigger);

public:
	CLadderTrigger();
	virtual ~CLadderTrigger();

	virtual void	OnInitTrigger() override;

	virtual void	OnEnter(i3PhysixShapeSet* pTriggerShapeSet, i3ElementBase* pDest) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

private:

	void			UpdateTriggerImp();
	LadderStateInfo*	m_LadderInfo;
};

#endif
