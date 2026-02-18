#include "stdafx.h"
#include "i3LevelIcon.h"

I3_CLASS_INSTANCE( i3LevelIcon);

void i3LevelIcon::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3LevelControl::CopyTo( pDest, method);

	I3ASSERT( pDest != nullptr);
	I3ASSERT( i3::kind_of<i3LevelIcon*>(pDest));
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 Level_ICON
	{
		UINT8		m_ID[4] = { 'T', 'I', 'C', '1' };
		UINT8		m_Reserved[64] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3LevelIcon::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::Level_ICON Info;

	Result = i3LevelElement3D::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc  = pStream->Write( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LevelIcon::OnSave()", "Could not write data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32	i3LevelIcon::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::Level_ICON Info;

	Result = i3LevelElement3D::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Rc  = pStream->Read( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3LevelIcon::OnLoad()", "Could not read data.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}
