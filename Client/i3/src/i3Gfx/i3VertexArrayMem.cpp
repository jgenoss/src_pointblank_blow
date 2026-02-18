#include "i3GfxType.h"
#include "../i3Math/i3Color.h"
#include "i3VertexArrayMem.h"

I3_CLASS_INSTANCE( i3VertexArrayMem);


i3VertexArrayMem::~i3VertexArrayMem(void)
{
	I3MEM_SAFE_FREE_POINTER(m_pBuffer); 
}

UINT32 i3VertexArrayMem::CalcBufferSize( i3VertexFormat * pFormat, UINT32 Count)
{
	i3::pack::VERTEX_OFFSET Info;
	INT32 i;

	Info.m_Format = * pFormat;

	CalcPersistantBufferSize25( &Info, 1);

	m_Stride = Info.m_Stride;
	m_OffsetPos = Info.m_OffsetPos;
	m_OffsetNormal = Info.m_OffsetNormal;
	m_OffsetColor = Info.m_OffsetColor;
	m_OffsetWeight = Info.m_OffsetWeight;
	m_OffsetTangent = Info.m_OffsetTangent;
	m_OffsetBinormal = Info.m_OffsetBinormal;
	
	for( i = 0; i < pFormat->GetTextureCoordSetCount(); i++)
	{
		m_OffsetTexCoord[i] = Info.m_OffsetTexCoord[i];
	}

	m_OffsetBlendIndex = Info.m_OffsetBlendIndex;

	return m_Stride * Count;
}


bool i3VertexArrayMem::Create( i3VertexFormat * pFormat, UINT32 count, I3G_USAGE usage)
{
	UINT32 BuffSz;

	i3VertexArray::Create( pFormat, count, usage);

	BuffSz = CalcBufferSize( pFormat, count);

	m_VertexFormatCode = 0;

	switch( pFormat->GetPlatform())
	{
		case I3VF_PLATFORM_PSP :
			m_ColorOrder = I3VF_COLORORDER_RGBA;
			break;
	}
	
	//m_BufferIdx = i3MemAlloc( BuffSz);
	m_pBuffer	= (void *) i3MemAlloc( BuffSz);
	I3ASSERT( m_pBuffer != nullptr);

	return true;
};


bool i3VertexArrayMem::Lock( I3G_LOCKMETHOD lock, UINT32 StartIdx, UINT32 Count)
{
	if( m_bLocked)
		return false;

	I3ASSERT( m_pBuffer);

	m_LockStartIdx = 0;			// PSP Version은 항상 첫번째 Vertex부터 Lock을 하는 것과 동일하다

	if( Count == 0)
		Count = m_Count;

	m_LockCount = Count;

	m_pLockedBuf = (INT8 *) m_pBuffer;

	m_bLocked = true;

	return true;
}

void i3VertexArrayMem::Unlock(void)
{
	I3ASSERT( m_pBuffer);

	m_pLockedBuf = nullptr;
	m_LockCount = 0;

	m_bLocked = false;
}

bool i3VertexArrayMem::SafeLock( I3G_LOCKMETHOD lock /*= I3G_LOCK_NONE*/, UINT32 StartIdx /*= 0*/, UINT32 Count /*= 0*/)
{
	m_Mutex.Enter();

	return Lock( lock, StartIdx, Count);
}

bool i3VertexArrayMem::SafeUnlock(void)
{
	Unlock();
	m_Mutex.Leave();
	return true;
}
