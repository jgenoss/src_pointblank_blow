#include "stdafx.h"
#include "i3WindowInfo.h"

I3_CLASS_INSTANCE( i3WindowInfo, i3PersistantElement);

i3WindowInfo::i3WindowInfo(void)
{
	m_pParent = NULL;

	m_pPane[0] = NULL;
	m_pPane[1] = NULL;

	m_X = 0;
	m_Y = 0;
	m_Width = 0;
	m_Height = 0;

	m_Size = 0;
}

i3WindowInfo::~i3WindowInfo(void)
{
	INT32 i;

	for( i = 0; i < 2; i++)
	{
		I3_SAFE_RELEASE( m_pPane[i]);
	}
}

void i3WindowInfo::OnBuildObjectList( i3List * pList)
{
	INT32 i;

	for( i = 0; i < 2; i++)
	{
		if( m_pPane[i] != NULL)
		{
			m_pPane[i]->OnBuildObjectList( pList);
		}
	}

	i3PersistantElement::OnBuildObjectList( pList);
}

typedef struct _tagi3WindowInfo
{
	OBJREF		m_pParent;
	OBJREF		m_pPane[2];

	INT32	m_X;
	INT32	m_Y;
	INT32	m_Width;
	INT32	m_Height;
	INT32	m_Size;

} I3_PERSIST_WINDOW_INFO;

UINT32 i3WindowInfo::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	INT32 i;
	i3Stream * pStream = pResFile->GetStream();
	I3_PERSIST_WINDOW_INFO		Info;

	Info.m_pParent = (OBJREF) GetParent();
	
	for( i = 0; i < 2; i++)
	{
		Info.m_pPane[i] = (OBJREF) GetPane( i);
	}

	Info.m_X = GetX();
	Info.m_Y = GetY();
	Info.m_Width = GetWidth();
	Info.m_Height = GetHeight();
	Info.m_Size = GetSize();

	Rc = pStream->Write( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3WindowInfo::OnSave()", "Could not write window information.");
		return STREAM_ERR;
	}
	Result += Rc;

	return Result;
}

UINT32 i3WindowInfo::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	I3_PERSIST_WINDOW_INFO Info;
	INT32 i;

	Rc = pStream->Read( &Info, sizeof(Info));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3WindowInfo::OnLoad()", "Could not read window information.");
		return STREAM_ERR;
	}
	Result += Rc;

	if( Info.m_pParent != NULL)
	{
		m_pParent = (i3WindowInfo *) pResFile->FindObjectByID( Info.m_pParent);
		I3ASSERT( m_pParent);
	}
	else
	{
		I3TRACE( "sdf\n");
	}
	
	for( i = 0; i < 2; i++)
	{
		if( Info.m_pPane[i] != NULL)
		{
			m_pPane[i] = (i3WindowInfo *) pResFile->FindObjectByID( Info.m_pPane[i]);
			I3ASSERT( m_pPane[i]);

			m_pPane[i]->AddRef();
		}
	}

	SetX( Info.m_X);
	SetY( Info.m_Y);
	SetWidth( Info.m_Width);
	SetHeight( Info.m_Height);
	SetSize( Info.m_Size);

	return Result;
}
