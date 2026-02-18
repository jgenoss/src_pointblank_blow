#include "i3SceneDef.h"
#include "i3RegINT32.h"

I3_CLASS_INSTANCE( i3RegINT32);

i3RegINT32::i3RegINT32(void)
{
	m_nDataType = I3REG_TYPE_INT32;
}


void i3RegINT32::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3RegData::CopyTo( pDest, method);

	I3ASSERT( i3::kind_of<i3RegINT32*>(pDest));

	i3RegINT32 * pINT32 = ( i3RegINT32*)pDest;

	pINT32->setValue( m_Value);
}

void	i3RegINT32::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	i3RegData::OnBuildObjectList( List);
}

UINT32	i3RegINT32::OnSave( i3ResourceFile * pResFile)
{
	i3Stream * pStream = pResFile->GetStream();
	UINT32	Result, Rc = 0;

	Result = i3RegData::OnSave( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Write( &m_Value, sizeof( INT32));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3RegINT32::OnLoad( i3ResourceFile * pResFile)
{
	UINT32				Result, Rc = 0;
	i3Stream	*		pStream = pResFile->GetStream();

	Result = i3RegData::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc	= pStream->Read( &m_Value, sizeof( INT32));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

void i3RegINT32::GetValueByString( i3::wstring& out )
{
	i3::sprintf(out, L"%5d", m_Value);
}
