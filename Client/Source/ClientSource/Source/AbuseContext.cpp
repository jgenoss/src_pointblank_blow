#include "pch.h"
#include "AbuseContext.h"

void	AbuseContext::SetAbuseInfo(ABUSING_LEVEL AbusingLevel, UINT32 AbusingTime)
{
	if(AbusingLevel != ABUSING_LEVEL_NONE)
	{
		m_AbuseFlag = true;
		m_AbuseTime = AbusingTime;
		m_AbuseLevel = (UINT8)AbusingLevel;
	}
}



void	AbuseContext::ResetAbuse()
{
	m_AbuseFlag = false;
	m_AbuseTime = 0;
	m_AbuseLevel = ABUSING_LEVEL_NONE;
}
