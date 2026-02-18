#include "i3GfxType.h"

#if defined( I3G_XBOX )
#include "i3IndexArrayXbox.h"
#include "i3GfxGlobalVariableXbox.h"
#include "i3GfxUtilXbox.h"

//I3_CONCRETE_CLASS_INSTANCE( i3IndexArrayXbox, i3IndexArray);
I3_CLASS_INSTANCE( i3IndexArrayXbox);

i3IndexArrayXbox::i3IndexArrayXbox(void)
{
	m_pBuffer = nullptr;

	m_pLockedData = nullptr;
}

i3IndexArrayXbox::~i3IndexArrayXbox(void)
{
	if( m_pBuffer != nullptr)
	{
		m_pBuffer->Release();
		m_pBuffer = nullptr;
	}
}

bool i3IndexArrayXbox::Create( UINT32 count, I3G_USAGE usage)
{
	HRESULT Rv;
	UINT32 Sz;
	DWORD dxUsage;

	m_Count = count;

	Sz = sizeof(UINT16) * count;

	dxUsage = i3_GetNativeUsage( usage);

	Rv = g_pD3DDevice->CreateIndexBuffer( Sz, dxUsage, D3DFMT_INDEX16, 
		D3DPOOL_MANAGED, &m_pBuffer);
	I3ASSERT( !FAILED( Rv));

	return true;
}
	
void i3IndexArrayXbox::SetUsageFlag( UINT32 flag)
{
	IDirect3DIndexBuffer9 * pNewBuf;
	HRESULT Rv;
	UINT32 Sz;
	DWORD dxUsage;
	char * pDest;

	Sz = sizeof(UINT16) * m_Count;

	dxUsage = i3_GetNativeUsage( flag);

	Rv = g_pD3DDevice->CreateIndexBuffer( Sz, dxUsage, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pNewBuf);
	I3ASSERT( !FAILED( Rv));

	Lock();
	Rv = pNewBuf->Lock( 0, Sz, (BYTE **) &pDest, 0);
	I3ASSERT( !FAILED( Rv));

	// Copy
	memcpy( pDest, GetData(), Sz);
	
	pNewBuf->Unlock();
	Unlock();

	m_pBuffer->Release();

	m_pBuffer = pNewBuf;
}

void i3IndexArrayXbox::Lock( UINT32 StartIdx, UINT32 count, I3G_LOCKMETHOD lock)
{
	HRESULT Rv;
	UINT32 lockStart, lockSz;

	I3ASSERT( m_pBuffer != nullptr);

	lockStart = sizeof(UINT16) * StartIdx;

	if( count == 0)
	{
		lockSz = sizeof(UINT16) * m_Count;
	}
	else
	{
		lockSz = sizeof(UINT16) * count;
	}

	Rv = m_pBuffer->Lock( lockStart, lockSz, (BYTE **) &m_pLockedData, lock);
	I3ASSERT( !FAILED( Rv));
}

void i3IndexArrayXbox::Unlock(void)
{
	HRESULT Rv;

	I3ASSERT( m_pBuffer != nullptr);

	Rv = m_pBuffer->Unlock();
	I3ASSERT( !FAILED( Rv));

	m_pLockedData = nullptr;
}

void i3IndexArrayXbox::SetIndex16( UINT32 idx, UINT16 val)
{
	I3ASSERT( m_pLockedData != nullptr);

	m_pLockedData[idx] = val;
}

UINT16 i3IndexArrayXbox::GetIndex16( UINT32 idx)
{
	I3ASSERT( m_pLockedData != nullptr);

	return m_pLockedData[idx];
}

UINT16 *	i3IndexArrayXbox::GetData(void)
{
	return m_pLockedData;
}

#endif