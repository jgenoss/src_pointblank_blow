#include "i3MathType.h"
#include "i3RectCoordSource.h"

I3_CLASS_INSTANCE( i3RectCoordSource);

void i3RectCoordSource::GetRect( i3::pack::RECT * pRect)
{
	pRect->left = i3Vector::GetX( &m_vStart);
	pRect->top = i3Vector::GetY( &m_vStart);
	pRect->right = pRect->left + m_Width;
	pRect->bottom = pRect->top + m_Height;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 RECT_COORD_SOURCE
	{
		REAL32 m_X = 0.0f;
		REAL32 m_Y = 0.0f;
		REAL32 m_Width = 0.0f;
		REAL32 m_Height = 0.0f;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3RectCoordSource::OnSave( i3ResourceFile * pResFile)
{
	pack::RECT_COORD_SOURCE data;
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	
	data.m_X = i3Vector::GetX( &m_vStart);
	data.m_Y = i3Vector::GetY( &m_vStart);
	data.m_Width = m_Width;
	data.m_Height = m_Height;

	Rc = pStream->Write( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		I3TRACE( "i3RectCoordSource::OnSave() - Could not write data\n");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3RectCoordSource::OnLoad( i3ResourceFile * pResFile)
{
	pack::RECT_COORD_SOURCE data;
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	Rc = pStream->Read( &data, sizeof(data));
	if( Rc == STREAM_ERR)
	{
		I3TRACE( "i3RectCoordSource::OnLoad() - Could not read data\n");
		return STREAM_ERR;
	}
	Result += Rc;

	SetX( data.m_X);
	SetY( data.m_Y);
	SetWidth( data.m_Width);
	SetHeight( data.m_Height);

	return Result;
}

