#include "pch.h"
#include "S2MOBase.h"

S2MOBase::S2MOBase()
{
	m_pS2MORSA = NULL;
}

S2MOBase::~S2MOBase()
{
	Destory();
}

BOOL S2MOBase::Create()
{
	m_pS2MORSA = new S2MORSA;
	if( NULL == m_pS2MORSA )				return FALSE;
	if( FALSE == m_pS2MORSA->Create() )		return FALSE;

	return TRUE;
}

void S2MOBase::Destory()
{
	if( m_pS2MORSA )
	{
		delete m_pS2MORSA;
		m_pS2MORSA = NULL;
	}
}