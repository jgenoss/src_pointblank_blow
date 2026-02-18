#ifndef _StageExit_h
#define _StageExit_h

class CStageExit: public i3Stage
{
	I3_CLASS_DEFINE(CStageExit, i3Stage);
public:
	CStageExit();
	virtual ~CStageExit();

	virtual void OnCreate(void) override;
	virtual void OnUpdate(REAL32 rDeltaSeconds) override;

private:
};

#endif //_StageExit_h
