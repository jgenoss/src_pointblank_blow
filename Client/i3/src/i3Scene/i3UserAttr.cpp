#include "i3SceneDef.h"
#include "i3UserAttr.h"

I3_CLASS_INSTANCE( i3UserAttr);

i3UserAttr::i3UserAttr(void)
{
	SetID( I3_ATTRID_USER );
}

void i3UserAttr::SetCallBack( I3USERATTRPROC pFunc, void *pParam )
{
	m_pFunc = pFunc;
	m_pParam = pParam;
}

void i3UserAttr::Apply( i3RenderContext * pContext)
{
	if( m_pFunc != nullptr )
	{
		m_pFunc( pContext, m_pParam );
	}
}

bool i3UserAttr::IsSame( i3RenderAttr * pAttr)
{
	i3UserAttr *pUserAttr = (i3UserAttr*)pAttr;
	if( ( pUserAttr->m_pFunc == m_pFunc ) && ( pUserAttr->m_pParam == m_pParam ) )
	{
		return true;
	}

	return false;
}

UINT32 i3UserAttr::OnSave( i3ResourceFile * pResFile)
{
	return i3RenderAttr::OnSave( pResFile );
}

UINT32 i3UserAttr::OnLoad( i3ResourceFile * pResFile)
{
	return i3RenderAttr::OnLoad( pResFile );
}

void i3UserAttr::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pDest, method);

	I3ASSERT( i3::kind_of<i3UserAttr*>(pDest));

	((i3UserAttr *)pDest)->SetCallBack( m_pFunc, m_pParam );
}


