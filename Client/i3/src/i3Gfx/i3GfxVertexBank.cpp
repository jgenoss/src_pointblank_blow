#include "i3GfxType.h"
#include "i3Base.h"
#include "i3GfxGlobalVariable.h"
#include "i3GfxGlobalVariableDX.h"
#include "i3GfxVertexBank.h"
#include "i3GfxUtil.h"

#define INDEXED_IMPLEMENT	0

I3_CLASS_INSTANCE( i3GfxVertexBank);

i3GfxVertexBank::i3GfxVertexBank(void)
{
	m_VertextBankList.reserve(32);
}

i3GfxVertexBank::~i3GfxVertexBank(void)
{
	
	VERTEX_BANK_ELEMENT * pElement = nullptr;

	for(size_t i = 0; i < m_VertextBankList.size(); i++)
	{
		pElement = m_VertextBankList[i];
		
		// VeretexBank's Elements must be free!!
		I3_SAFE_RELEASE( pElement->m_pVA);
		pElement->m_nCurIdx = 0;
		pElement->m_nNextIdx = 0;
		
		// VertexBank Element free!!
		I3MEM_SAFE_FREE_POINTER( pElement);	
	}

	m_VertextBankList.clear();
}

VERTEX_BANK_ELEMENT* i3GfxVertexBank::CreateVertexBank( i3VertexFormat* pformat, UINT32 nSize)
{
	VERTEX_BANK_ELEMENT * pElement = (VERTEX_BANK_ELEMENT *)i3MemAlloc( sizeof( VERTEX_BANK_ELEMENT));
	I3ASSERT( pElement != nullptr);

	pElement->m_nCurIdx = 0;
	pElement->m_nNextIdx = 0;
	pElement->m_pVA = i3VertexArray::new_object();	
	pElement->m_pVA->Create( pformat, nSize, I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE);

	I3ASSERT( pElement->m_pVA != nullptr);

	m_VertextBankList.push_back( pElement);

	return pElement;
}

VERTEX_BANK_ELEMENT* i3GfxVertexBank::GetVertexArray( i3VertexFormat* pformat, UINT32 nSize)
{
	UINT32 nCurVertexBufferCount = 0;
	bool bReCreateVertexBuffer = false;
	VERTEX_BANK_ELEMENT * pElement = FindVertexArray( pformat, nSize);
	I3ASSERT( pElement != nullptr);

	// if VB isn't enough, VB must be recreate.
	{
		nCurVertexBufferCount = pElement->m_pVA->GetCount();
		while( pElement->m_nNextIdx+nSize >= nCurVertexBufferCount)
		{
			bReCreateVertexBuffer = true;
			nCurVertexBufferCount = nCurVertexBufferCount * 2;	
		}

		if( bReCreateVertexBuffer)
		{
			I3_SAFE_RELEASE( pElement->m_pVA); 
			pElement->m_pVA = i3VertexArray::new_object();	
			pElement->m_pVA->Create( pformat, nCurVertexBufferCount, I3G_USAGE_WRITEONLY | I3G_USAGE_FREQUENT_UPDATE);		
			//pElement->m_pVA->Create( pformat, nCurVertexBufferCount, 0);
			//pElement->m_pVA->setRestoreType( I3G_RESTORE_MEM);

			pElement->m_nCurIdx = 0;
			pElement->m_nNextIdx = 0;
		}
	}

	pElement->m_nCurIdx = pElement->m_nNextIdx;
	pElement->m_nNextIdx = pElement->m_nNextIdx + nSize;

	return pElement;
}

VERTEX_BANK_ELEMENT* i3GfxVertexBank::FindVertexArray( i3VertexFormat* pformat, UINT32 nSize)
{
	
	VERTEX_BANK_ELEMENT * pElement = nullptr;
	i3VertexFormat * pVBListFormat = nullptr;

	// Vertex array condition set!
	bool bHasPosition = pformat->GetHasPosition();
	bool bHasColor = pformat->GetHasColor();
	INT8 nCoordCount = pformat->GetTextureCoordCount();
	INT8 nCoordSetCount = pformat->GetTextureCoordSetCount();
	bool isTransformed = pformat->IsTransformed();

	for(size_t i = 0; i < m_VertextBankList.size(); i++)
	{
		pElement = m_VertextBankList[i];		
		pVBListFormat = pElement->m_pVA->GetFormat();

		if( pVBListFormat->GetHasPosition() == bHasPosition && pVBListFormat->GetHasColor() == bHasColor &&
			pVBListFormat->GetTextureCoordCount() == nCoordCount && pVBListFormat->GetTextureCoordSetCount() == nCoordSetCount &&
			pVBListFormat->IsTransformed() == isTransformed)
		{
			return pElement;
		}
	}

	// Vertext array List didn't has this format.
	pElement = CreateVertexBank( pformat, DEFAULT_VERTEXBANK_VERTEX_COUNT_MAX);

	I3ASSERT( pElement != nullptr);

	return pElement;
}

void i3GfxVertexBank::ResetIndex(void)
{
	
	VERTEX_BANK_ELEMENT * pElement = nullptr;

	for(size_t i = 0; i < m_VertextBankList.size(); i++)
	{
		pElement = m_VertextBankList[i];
		pElement->m_nCurIdx = 0;
		pElement->m_nNextIdx = 0;
	}
}

bool i3GfxVertexBank::Destroy( bool bLostDevice)
{
	return i3GfxResource::Destroy( bLostDevice);
}

