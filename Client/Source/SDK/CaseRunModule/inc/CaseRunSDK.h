#pragma once

#include "CaseRunDefine.h"
#include "KeyDefines.h"
#include "Interface.h"
#include "Nodes.h"
#include "Job.h"
#include "TLineLexer.h"

namespace CaseRun
{
	extern bool Init( Interface & itf);
	extern bool	Close(void);
	extern bool Tick(void);
	extern void TestClick(const wchar_t * pszCtrlPath);

	extern bool	AddJob( Job * pJob);
	extern void	RemoveJob( Job * pJob);

	extern bool StartCapture(void);
	extern bool EndCapture( const vector<NodeBase*> & pCapturedList);
}
