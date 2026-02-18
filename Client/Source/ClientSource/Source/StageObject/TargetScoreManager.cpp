#include "pch.h"
#include "TargetScoreManager.h"

I3_CLASS_INSTANCE( TargetScoreManager);//, i3GameNode);

TargetScoreManager::TargetScoreManager(void)
{
}

TargetScoreManager::~TargetScoreManager(void)
{
}

bool TargetScoreManager::Create( INT32 cnt)
{
	INT32 i;

	for( i = 0; i < cnt; i++)
	{
	}

	return true;
}

void TargetScoreManager::EmitScore( VEC3D * pPos, INT32 score)
{
}
