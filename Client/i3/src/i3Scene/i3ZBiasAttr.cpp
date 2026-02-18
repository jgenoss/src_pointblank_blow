#include "i3SceneDef.h"
#include "i3ZBiasAttr.h"

I3_CLASS_INSTANCE( i3ZBiasAttr);

i3ZBiasAttr::i3ZBiasAttr(void)
{
	SetID( I3_ATTRID_Z_BIAS);
}

void	i3ZBiasAttr::Apply( i3RenderContext * pCtx)
{
	pCtx->SetZBias( m_fSlope, m_fBias);
}

void	i3ZBiasAttr::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pDest, method);

	I3ASSERT( i3::same_of<i3ZBiasAttr*>(pDest));

	i3ZBiasAttr * pAttr = (i3ZBiasAttr *) pDest;

	pAttr->setSlope( getSlope());
	pAttr->setBias( getBias());
}

bool	i3ZBiasAttr::IsSame( i3RenderAttr * pAttr)
{
	I3ASSERT( i3::same_of<i3ZBiasAttr*>(pAttr));
	
	i3ZBiasAttr * pBiasAttr = (i3ZBiasAttr *) pAttr;

	return (pBiasAttr->getSlope() == getSlope()) && (pBiasAttr->getBias() == getBias());
}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(push, 8)			// [test required] 패킹 사이즈 누락으로 추가합니다. 2017.02.24. soon9
#endif

namespace pack
{
	struct ZBIAS_ATTR
	{
		REAL32		m_fSlope = 0;
		REAL32		m_fBias = 0;
		UINT32		pad[4] = { 0 };
	};
}
#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(pop)
#endif

UINT32	i3ZBiasAttr::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	
	Result = i3RenderAttr::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	pack::ZBIAS_ATTR data;
	data.m_fSlope = getSlope();
	data.m_fBias = getBias();

	Rc = pStream->Write( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32	i3ZBiasAttr::OnLoad( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::ZBIAS_ATTR data;

	Result = i3RenderAttr::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc = pStream->Read( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Could not read data.");
		return STREAM_ERR;
	}
	Result += Rc;

	setSlope( data.m_fSlope);
	setBias( data.m_fBias);

	return Result;
}

#if defined( I3_DEBUG)
void i3ZBiasAttr::Dump(void)
{
	I3TRACE( "i3ZBiasAttr : SLOPE:%f  BIAS:%f\n", getSlope(), getBias());
}
#endif
