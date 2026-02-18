#include "pch.h"
#include "LinkManager.h"
#include "LinkBrazil.h"

I3_CLASS_INSTANCE( CLinkManager, i3ElementBase );

CLinkManager::CLinkManager()
{
	//m_i32LogThreadIdx	= LOG_THREAD_INVALID;
	m_pLink = NULL;
}

CLinkManager::~CLinkManager()
{
	OnDestroy();
}

BOOL CLinkManager::OnCreate( CRingBuffer* pRingLinkIn, CRingBuffer* pRingLinkOut )
{
	m_pLink = CLinkBrazil::NewObject();
	if( NULL == m_pLink )											return FALSE;
	if( FALSE == m_pLink->OnCreate( pRingLinkIn, pRingLinkOut ) )	return FALSE;
	//m_i32LogThreadIdx		= g_pLog->InsertLogBuffer();
	//if( LOG_THREAD_INVALID == m_i32LogThreadIdx )											return FALSE;

	return TRUE;
}

void CLinkManager::OnDestroy()
{
	I3_SAFE_RELEASE( m_pLink );
}
