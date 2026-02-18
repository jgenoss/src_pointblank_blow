#include "pch.h"
#include "LinkBase.h"

I3_CLASS_INSTANCE( CLinkBase, i3Thread );

CLinkBase::CLinkBase()
{
	m_pRingLinkIn		= NULL;
	m_pRingLinkOut		= NULL;
	m_i32LogThreadIdx	= LOG_THREAD_INVALID;
}

CLinkBase::~CLinkBase()
{
	OnDestroy();
}

BOOL CLinkBase::OnCreate( CRingBuffer* pRingLinkIn, CRingBuffer* pRingLinkOut )
{
	m_pRingLinkIn	= pRingLinkIn;
	m_pRingLinkOut	= pRingLinkOut;

	m_i32LogThreadIdx		= g_pLog->InsertLogBuffer();
	if( LOG_THREAD_INVALID == m_i32LogThreadIdx )											return FALSE;

	return TRUE;
}

void CLinkBase::OnDestroy()
{
}
