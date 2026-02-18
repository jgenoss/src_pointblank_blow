#include "i3MathType.h"
#include "i3MatrixArray.h"

I3_CLASS_INSTANCE( i3MatrixArray);

i3MatrixArray::~i3MatrixArray(void)
{
	I3MEM_SAFE_FREE_POINTER(m_pArray);
}

bool i3MatrixArray::Create( INT32 Count)
{
	m_Count = Count;
	
	m_pArray = (MATRIX *) i3MemAlloc(sizeof(MATRIX) * m_Count);
	I3ASSERT( m_pArray);

	return true;
}

UINT32 i3MatrixArray::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	INT32 i;
	i3::pack::MATRIX pmtx;
	i3Stream * pStream  = pResFile->GetStream();
	
	Result = pStream->Write( &m_Count, sizeof(INT32));
	if( Result == STREAM_ERR)
	{
		i3Log( "i3MatrixArray::onSave()", "Could not write matrix array count.");
		return STREAM_ERR;
	}

	for( i = 0; i < m_Count; i++)
	{
		i3Matrix::Copy( &pmtx, &m_pArray[i]);

		Rc = pStream->Write( &pmtx, sizeof(i3::pack::MATRIX));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3MatrixArray::OnSave()", "Could not write matrix data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3MatrixArray::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	INT32 i;
	i3::pack::MATRIX pmtx;
	i3Stream * pStream  = pResFile->GetStream();

	Result = pStream->Read( &m_Count, sizeof(INT32));
	if( Result == STREAM_ERR)
	{
		i3Log( "i3MatrixArray::OnSave()", "Could not read matrix array count.");
		return STREAM_ERR;
	}

	Create( m_Count);

	for( i = 0; i < m_Count; i++)
	{
		Rc = pStream->Read( &pmtx, sizeof(i3::pack::MATRIX));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3MatrixArray::OnSave()", "Could not read matrix data.");
			return STREAM_ERR;
		}
		Result += Result;

		i3Matrix::Copy( &m_pArray[i], &pmtx);
	}

	return Result;
}
