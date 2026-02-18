#include "i3SceneDef.h"
#include "i3RegREAL32.h"

I3_CLASS_INSTANCE( i3RegREAL32);

i3RegREAL32::i3RegREAL32(void)
{
	m_nDataType = I3REG_TYPE_REAL32;
}

void i3RegREAL32::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3RegData::CopyTo( pDest, method);

	I3ASSERT( i3::kind_of<i3RegREAL32*>(pDest)); 

	i3RegREAL32 * pReal32 = ( i3RegREAL32*)pDest;

	pReal32->set( *this);
}

void	i3RegREAL32::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	i3RegData::OnBuildObjectList( List);
}

UINT32	i3RegREAL32::OnSave( i3ResourceFile * pResFile)
{
	i3Stream * pStream = pResFile->GetStream();
	UINT32	Result, Rc = 0;

	Result = i3RegData::OnSave( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Write( &m_Value, sizeof( REAL32));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3RegREAL32::OnLoad( i3ResourceFile * pResFile)
{
	UINT32				Result, Rc = 0;
	i3Stream	*		pStream = pResFile->GetStream();

	Result = i3RegData::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc	= pStream->Read( &m_Value, sizeof( REAL32));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

void i3RegREAL32::GetValueByString( i3::wstring& out )
{
	i3::sprintf(out, L"%f", m_Value);
}