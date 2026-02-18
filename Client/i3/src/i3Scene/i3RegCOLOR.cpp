#include "i3SceneDef.h"
#include "i3RegCOLOR.h"

I3_CLASS_INSTANCE( i3RegCOLOR);

i3RegCOLOR::i3RegCOLOR(void)
{
	m_nDataType = I3REG_TYPE_COLOR;
}


void i3RegCOLOR::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3RegData::CopyTo( pDest, method);

	I3ASSERT( i3::kind_of<i3RegCOLOR*>(pDest));

	i3RegCOLOR * pDstColor = ( i3RegCOLOR*)pDest;

	pDstColor->set( *this);
}

void	i3RegCOLOR::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	i3RegData::OnBuildObjectList( List);
}

#if defined ( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED COLOR
	{
		UINT8 Col[4] = { 0 };
	};
}

#if defined ( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3RegCOLOR::OnSave( i3ResourceFile * pResFile)
{
	i3Stream * pStream = pResFile->GetStream();
	UINT32	Result, Rc = 0;
	pack::COLOR	data;

	Result = i3RegData::OnSave( pResFile);
	I3_CHKIO( Result);

	{
		data.Col[0] = m_Value.r;
		data.Col[1] = m_Value.g;
		data.Col[2] = m_Value.b;
		data.Col[3] = m_Value.a;
	}

	Rc = pStream->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	I3TRACE("OnSave::i3RegValue\n");
	return Result;
}

UINT32	i3RegCOLOR::OnLoad( i3ResourceFile * pResFile)
{
	pack::COLOR data;
	UINT32				Result, Rc = 0;
	i3Stream	*		pStream = pResFile->GetStream();

	Result = i3RegData::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc	= pStream->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	i3Color::Set( &m_Value, data.Col[0], data.Col[1], data.Col[2], data.Col[3]); 

	return Result;
}

void i3RegCOLOR::GetValueByString( i3::wstring& out )
{
	i3::sprintf(out, L"%d, %d, %d, %d", m_Value.r, m_Value.g, m_Value.b, m_Value.a);
}
