#include "i3SceneDef.h"
#include "i3AmbientAttr.h"

I3_CLASS_INSTANCE( i3AmbientAttr);

i3AmbientAttr::i3AmbientAttr(void)
{
	SetID( I3_ATTRID_AMBIENT);
}

void i3AmbientAttr::Apply( i3RenderContext * pContext)
{
	pContext->setAmbient( &m_Color);
}

void i3AmbientAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	((i3AmbientAttr *)pObj)->Set( Get());
}

bool i3AmbientAttr::IsSame( i3RenderAttr * pAttr)
{
	return i3Color::IsSame( &m_Color, ((i3AmbientAttr *)pAttr)->Get());
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4	AMBIENT_ATTR
	{
		INT8				m_ID[4] = { 'A', 'M', 'A', '1' };
		i3::pack::COLORREAL	m_Color;
		UINT32				pad[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3AmbientAttr::OnSave( i3ResourceFile * pFile)
{
	pack::AMBIENT_ATTR data;
	UINT32 Rc, Result;

	Result = i3RenderAttr::OnSave( pFile);
	I3_CHKIO( Result);
		
	data.m_Color.r = m_Color.r;
	data.m_Color.g = m_Color.g;
	data.m_Color.b = m_Color.b;
	data.m_Color.a = m_Color.a;

	Rc = pFile->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Rc;
}

UINT32 i3AmbientAttr::OnLoad( i3ResourceFile * pFile)
{
	pack::AMBIENT_ATTR data;
	UINT32 Rc, Result;

	Result = i3RenderAttr::OnLoad( pFile);
	I3_CHKIO( Result);

	Rc = pFile->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	m_Color.r = data.m_Color.r;
	m_Color.g = data.m_Color.g;
	m_Color.b = data.m_Color.b;
	m_Color.a = data.m_Color.a;

	return Rc;
}

#if defined( I3_DEBUG)
void i3AmbientAttr::Dump(void)
{
	I3TRACE4( "i3AmbientAttr : %x, %x, %x, %x\n", 
		i3Color::GetR( &m_Color),
		i3Color::GetG( &m_Color),
		i3Color::GetB( &m_Color),
		i3Color::GetA( &m_Color));
}
#endif
