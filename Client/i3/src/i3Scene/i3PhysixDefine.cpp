#include "i3SceneDef.h"
#include "i3PhysixDefine.h"
#include "i3PhysixContext.h"

void i3Physix::ContactReportCopyTo( I3_PHYSIX_CONTACT_REPORT * pDest, I3_PHYSIX_CONTACT_REPORT * pSrc )
{
	pDest->m_Events			= pSrc->m_Events;
	pDest->m_pOwnShapeSet	= pSrc->m_pOwnShapeSet;
	pDest->m_pDestShapeSet	= pSrc->m_pDestShapeSet;

	for( size_t i = 0; i < pSrc->m_ContactList.size(); i++ )
	{
		I3_PHYSIX_CONTACT_INFO * pSrcInfo = pSrc->m_ContactList.at(i);

		I3_PHYSIX_CONTACT_INFO * pNewInfo = g_pPhysixCtx->AllocContactPoint();
		pSrcInfo->CopyTo( pNewInfo);

		pDest->m_ContactList.push_back( pNewInfo);
	}
}

void i3Physix::ContactReportClear( I3_PHYSIX_CONTACT_REPORT * pSrc )
{
	pSrc->m_Events		= 0;

	for( size_t i = 0; i < pSrc->m_ContactList.size(); i++ )
	{
		I3_PHYSIX_CONTACT_INFO * pInfo = pSrc->m_ContactList.at(i);
		pInfo->Reset();

		g_pPhysixCtx->FreeContactPoint( pInfo);
	}
	pSrc->m_ContactList.clear();

	pSrc->m_pDestShapeSet	= nullptr;
	pSrc->m_pOwnShapeSet	= nullptr;
}

I3_PHYSIX_CONTACT_REPORT::~I3_PHYSIX_CONTACT_REPORT()
{
	const size_t num = m_ContactList.size();
	for (size_t i = 0 ; i < num ; ++i)
	{
		m_ContactList[i]->Reset();
		g_pPhysixCtx->FreeContactPoint(m_ContactList[i]);
	}
}
