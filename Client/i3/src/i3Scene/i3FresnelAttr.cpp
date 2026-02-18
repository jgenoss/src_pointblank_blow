#include "i3SceneDef.h"
#include "i3FresnelAttr.h"

I3_CLASS_INSTANCE( i3FresnelAttr);

i3FresnelAttr::i3FresnelAttr(void)
{
	SetID( I3_ATTRID_FRESNEL);
}

void i3FresnelAttr::Apply( i3RenderContext * pCtx)
{
	pCtx->SetFresnelEnable( m_bEnable);
	pCtx->SetFresnelIndexOfReflect( m_IOR);
	pCtx->SetFresnelColor( &m_Color);
}

void i3FresnelAttr::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	I3ASSERT( pDest != nullptr);

	i3RenderAttr::CopyTo( pDest, method);

	I3ASSERT( i3::same_of<i3FresnelAttr*>(pDest));

	i3FresnelAttr * pFresnel = (i3FresnelAttr *) pDest;

	pFresnel->setEnable( isEnable());
	pFresnel->setIOR( getIOR());
	pFresnel->setColor( getColor());
}

bool i3FresnelAttr::IsSame( i3RenderAttr * pAttr)
{
	I3ASSERT( pAttr != nullptr);
	I3ASSERT( i3::same_of<i3FresnelAttr*>(pAttr));

	i3FresnelAttr * pFresnel = (i3FresnelAttr *) pAttr;

	return (pFresnel->isEnable() == isEnable()) && (pFresnel->getIOR() == getIOR());
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 FRESNEL_ATTR
	{
		UINT8				m_ID[4] = { 'F', 'R', 'N', '1' };
		INT32				m_bEnable = 0;
		REAL32				m_IOR = 0.0f;
		i3::pack::COLORREAL	m_Color;
		UINT32				pad[28] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3FresnelAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::FRESNEL_ATTR data;

	result = i3RenderAttr::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_bEnable		= isEnable() ? 1 : 0;
	data.m_IOR			= getIOR();
	data.m_Color.r		= m_Color.r;
	data.m_Color.g		= m_Color.g;
	data.m_Color.b		= m_Color.b;
	data.m_Color.a		= m_Color.a;

	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32 i3FresnelAttr::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::FRESNEL_ATTR data;

	result = i3RenderAttr::OnLoad( pResFile);
	I3_CHKIO( result);

	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	setEnable( data.m_bEnable != 0);
	setIOR( data.m_IOR);

	setColor( data.m_Color.r, data.m_Color.g, data.m_Color.b, data.m_Color.a);

	return result;
}

#if defined( I3_DEBUG)
void i3FresnelAttr::Dump(void)
{
	I3TRACE( "i3FresnelAttr : %d, %f\n", isEnable(), getIOR());
}
#endif
