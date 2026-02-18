#include "i3SceneDef.h"
#include "i3ColorAttr.h"

I3_CLASS_INSTANCE( i3ColorAttr);

i3ColorAttr::i3ColorAttr(void)
{
	SetID( I3_ATTRID_COLOR);
}

void i3ColorAttr::Apply( i3RenderContext * pContext)
{
	pContext->SetColor( &m_Color);
}

void i3ColorAttr::CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method)
{
	i3RenderAttr::CopyTo( pObj, method);

	((i3ColorAttr *)pObj)->Set( Get());
}

bool i3ColorAttr::IsSame( i3RenderAttr * pAttr)
{
	return i3Color::IsSame( &m_Color, ((i3ColorAttr *)pAttr)->Get());
}

UINT32 i3ColorAttr::OnSave( i3ResourceFile * pResFile)
{
	i3::pack::COLORREAL col;	
	UINT32 Rc;

	col.r = i3Color::GetR( &m_Color);
	col.g = i3Color::GetG( &m_Color);
	col.b = i3Color::GetB( &m_Color);
	col.a = i3Color::GetA( &m_Color);

	Rc = pResFile->GetStream()->Write( &col, sizeof(i3::pack::COLORREAL));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ColorAttr::OnSave()", "Could not write color value.");
		return STREAM_ERR;
	}

	return Rc;
}

UINT32 i3ColorAttr::OnLoad( i3ResourceFile * pResFile)
{
#ifdef TRACE_I3_SCENE
	I3TRACE( "UINT32 i3ColorAttr::OnLoad( i3ResourceFile * pResFile)\n" );
#endif
	i3::pack::COLORREAL col;
	UINT32 Rc;

	Rc = pResFile->GetStream()->Read( &col, sizeof(i3::pack::COLORREAL));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3ColorAttr::OnLoad()", "Could not read color value.");
		return STREAM_ERR;
	}

	i3Color::Set( &m_Color, col.r, col.g, col.b, col.a);

	return Rc;
}

#if defined( I3_DEBUG)
void i3ColorAttr::Dump(void)
{
	I3TRACE4( "i3ColorAttr : %x, %x, %x, %x\n", 
		i3Color::GetR( &m_Color),
		i3Color::GetG( &m_Color),
		i3Color::GetB( &m_Color),
		i3Color::GetA( &m_Color));
}
#endif
