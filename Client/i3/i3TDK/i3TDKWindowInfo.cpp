#include "stdafx.h"
#include "i3TDKWindowInfo.h"

I3_CLASS_INSTANCE( i3TDKWindowInfo);

i3TDKWindowInfo::~i3TDKWindowInfo(void)
{
	INT32 i;

	for( i = 0; i < 2; i++)
	{
		I3_SAFE_RELEASE( m_pPane[i]);
	}
}

void i3TDKWindowInfo::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	INT32 i;

	for( i = 0; i < 2; i++)
	{
		if( m_pPane[i] != nullptr)
		{
			m_pPane[i]->OnBuildObjectList( List);
		}
	}

	i3PersistantElement::OnBuildObjectList( List);
}

#if defined(I3_COMPILER_VC)
#pragma pack(push, 8)					// [test required] 패킹사이즈 누락으로 추가함. 2017.02.14. soon9
#endif

namespace pack
{
	struct WINDOW_INFO
	{
		OBJREF		m_pParent = 0;
		OBJREF		m_pPane[2] = { 0, 0 };

		INT32	m_X = 0;
		INT32	m_Y = 0;
		INT32	m_Width = 0;
		INT32	m_Height = 0;
		INT32	m_Size = 0;

	};
}

#if defined(I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32 i3TDKWindowInfo::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	INT32 i;
	i3Stream * pStream = pResFile->GetStream();
	pack::WINDOW_INFO		Info;

	Info.m_pParent = (OBJREF) pResFile->GetObjectPersistID( GetParent());
	
	for( i = 0; i < 2; i++)
	{
		Info.m_pPane[i] = (OBJREF) pResFile->GetObjectPersistID( GetPane( i));
	}

	Info.m_X = GetX();
	Info.m_Y = GetY();
	Info.m_Width = GetWidth();
	Info.m_Height = GetHeight();
	Info.m_Size = GetSize();

	Rc = pStream->Write( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TDKWindowInfo::OnSave()", "Could not write window information.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3TDKWindowInfo::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::WINDOW_INFO Info;
	INT32 i;

	Rc = pStream->Read( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TDKWindowInfo::OnLoad()", "Could not read window information.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( Info.m_pParent != 0)
	{
		m_pParent = (i3TDKWindowInfo *) pResFile->FindObjectByID( Info.m_pParent);
		I3ASSERT( m_pParent);
	}
	
	for( i = 0; i < 2; i++)
	{
		if( Info.m_pPane[i] != 0)
		{
			m_pPane[i] = (i3TDKWindowInfo *) pResFile->FindObjectByID( Info.m_pPane[i]);
			I3_MUST_ADDREF(m_pPane[i]);
		}
	}

	SetX( Info.m_X);
	SetY( Info.m_Y);
	SetWidth( Info.m_Width);
	SetHeight( Info.m_Height);
	SetSize( Info.m_Size);

	return Result;
}
