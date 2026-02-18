#include "pch.h"
#if JuuL_0
#include "StageExit.h"
#include "GlobalVariables.h"

I3_CLASS_INSTANCE(CStageExit);//, i3Stage);

CStageExit::CStageExit()
{
}

CStageExit::~CStageExit()
{
}

void CStageExit::OnCreate(void)
{
	i3Stage::OnCreate();
	
	g_pViewer->AddState( I3_VIEWER_STATE_EXIT);
	::PostMessage( g_pViewer->GetWindowHandle(), WM_DESTROY, 0,0);
}

void CStageExit::OnUpdate(RT_REAL32 rDeltaSeconds)
{
	i3Stage::OnUpdate(rDeltaSeconds);
}
#endif