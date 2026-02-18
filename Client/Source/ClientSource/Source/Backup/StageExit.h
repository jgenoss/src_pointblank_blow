#ifndef _StageExit_h
#define _StageExit_h

#if JuuL_0


class CStageExit: public i3Stage
{
	I3_CLASS_DEFINE(CStageExit, i3Stage);
public:
	CStageExit();
	virtual ~CStageExit();

	virtual void OnCreate(void);
	virtual void OnUpdate(RT_REAL32 rDeltaSeconds);

private:
};

#endif //_StageExit_h

#endif