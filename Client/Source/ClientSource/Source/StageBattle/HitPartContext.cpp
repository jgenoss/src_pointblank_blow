#include "pch.h"
#include "HitPartContext.h"

void HitPartContext::reset(void)
{
	for( int i = 0; i < CHARA_HIT_MAXCOUNT; i++ )
	{
		HitPartIndex[i] = 0;
	}
}

void HitPartContext::SetHitPart(INT32 Count,UINT8 Index)
{
	HitPartIndex[Count] = Index;
}

UINT8 HitPartContext::EncryptHitPart( CHARA_HIT_PART part)
{
	for( int i = 0; i < CHARA_HIT_MAXCOUNT; i++ )
	{
		if (HitPartIndex[i] == part)
		{
			return UINT8(i);
		}
	}
	return 0;

}
CHARA_HIT_PART HitPartContext::DecryptHitPart(INT32 ArrayIndex)
{
	return (CHARA_HIT_PART) HitPartIndex[ArrayIndex];
}

