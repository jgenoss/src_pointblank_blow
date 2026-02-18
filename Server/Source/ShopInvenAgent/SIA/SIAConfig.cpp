#include "pch.h"
#include "SIAConfig.h"


CSIAConfig* g_pSIAConfig = NULL;

CSIAConfig::CSIAConfig()
{
}


CSIAConfig::~CSIAConfig()
{
}

void CSIAConfig::InsertContents(ContentsInfo* pContents)
{
	for( INT32 i = 0; i < CONTENTS_COUNT; ++i )
		m_ContentsManager.InsertContents( static_cast<CONTENTS>(i), pContents[i].m_i32Version, pContents[i].m_bEnable );
}