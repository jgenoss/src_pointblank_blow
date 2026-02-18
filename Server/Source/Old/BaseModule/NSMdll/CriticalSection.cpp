#include "stdafx.h"
#include "CriticalSection.h"

NSM_cCriticalSection::NSM_cCriticalSection()
{
	::InitializeCriticalSection( &m_CriticalSection );
	return;
}

NSM_cCriticalSection::~NSM_cCriticalSection()
{
	::DeleteCriticalSection( &m_CriticalSection );
	return; 
}

void NSM_cCriticalSection::Enter(void)
{
	::EnterCriticalSection( &m_CriticalSection );
	return; 
}

void NSM_cCriticalSection::Leave(void)
{
	::LeaveCriticalSection( &m_CriticalSection );
	return; 
}
