#include "i3GfxType.h"
#include "i3GPUCommandList.h"
#include "i3Base/itl/vector_util.h"

#if defined( I3_PSP)
#include "i3GPUCommandParserPSP.h"
#include "i3VertexArray.h"
#include "i3IndexArray.h"
#include "i3TexturePSP.h"
#endif

//I3_CLASS_INSTANCE( i3GPUCommandList, i3PersistantElement);
I3_CLASS_INSTANCE( i3GPUCommandList);

i3GPUCommandList::i3GPUCommandList(void)
{
	m_ResList.reserve(32);
}

i3GPUCommandList::~i3GPUCommandList(void)
{
	I3MEM_SAFE_FREE_POINTER(m_pCommand); 

	RemoveAllResource();

	m_Size = 0;
}

void i3GPUCommandList::SetBuffer( INT8 * pBuffer)
{
	I3MEM_SAFE_FREE_POINTER(m_pCommand); 

	m_pCommand		= pBuffer;
}

void i3GPUCommandList::CopyBuffer( INT8 * pBuffer, UINT32 Sz)
{
	I3MEM_SAFE_FREE_POINTER(m_pCommand); 

	{
		m_pCommand	 = (INT8 *) i3MemAlloc( Sz); 
		I3ASSERT( m_pCommand != nullptr);
	}

	memcpy( m_pCommand, pBuffer, Sz);

	m_Size = Sz;
}

INT32 i3GPUCommandList::AddResource( i3GfxResource * pRes)
{
	size_t idx = i3::vu::index_of(m_ResList, pRes);
	if( idx == m_ResList.size() )
	{
		I3_SAFE_ADDREF( pRes);
		idx = m_ResList.size();
		m_ResList.push_back( pRes);
	}

	return idx;
}

void i3GPUCommandList::RemoveAllResource(void)
{
	i3GfxResource * pRes;

	for(size_t i = 0; i < m_ResList.size(); i++)
	{
		pRes = m_ResList[i];

		I3_MUST_RELEASE(pRes);
	}

	m_ResList.clear();
}

#if defined( I3G_PSP)

#define	__RESET()	\
	pBASE = nullptr;\
	pVADR = nullptr;\
	pIADR = nullptr;\
	pVBASE = nullptr;\
	pIBASE = nullptr;\
	pCBP = nullptr;\
	pCBW = nullptr;\
	pTBP = nullptr

void i3GPUCommandList::BindForPSP(void)
{
	UINT32 * pCur;
	UINT32 * pBASE, * pVADR, * pIADR, * pVBASE, * pIBASE;
	UINT32 * pCBP, *pCBW, *pTBP;
	UINT32 addr;

	pCur = (UINT32 *) m_pCommand;

	__RESET();

	I3TRACE( "------------------- Start of binding -----------------\n");
	//Dump();

	while((*pCur & 0xFF000000) != I3G_PSP_CMD_RET)
	{
		switch( (*pCur & 0xFF000000))
		{
			case I3G_PSP_CMD_BASE :		pBASE = pCur;	break;
			case I3G_PSP_CMD_VADR :		pVADR = pCur;	pVBASE = pBASE;	break;
			case I3G_PSP_CMD_IADR :		pIADR = pCur;	pIBASE = pBASE; break;

			case I3G_PSP_CMD_TBP0	:
			case I3G_PSP_CMD_TBP1	:
			case I3G_PSP_CMD_TBP2	:
			case I3G_PSP_CMD_TBP3	:
			case I3G_PSP_CMD_TBP4	:
			case I3G_PSP_CMD_TBP5	:
			case I3G_PSP_CMD_TBP6	:
			case I3G_PSP_CMD_TBP7	:	pTBP = pCur;	break;

			case I3G_PSP_CMD_CBP	:	pCBP = pCur;	break;
			case I3G_PSP_CMD_CBW	:	pCBW = pCur;	break;

			case I3G_PSP_CMD_TBW0	:
			case I3G_PSP_CMD_TBW1	:
			case I3G_PSP_CMD_TBW2	:
			case I3G_PSP_CMD_TBW3	:
			case I3G_PSP_CMD_TBW4	:
			case I3G_PSP_CMD_TBW5	:
			case I3G_PSP_CMD_TBW6	:
			case I3G_PSP_CMD_TBW7	:	
				{
					UINT32 idx, level;
					i3TexturePSP * pTex;

					I3ASSERT( pTBP != nullptr);

					level = (*pTBP & 0xFF000000) - I3G_PSP_CMD_TBP0;
					level = level >> 24;

					idx = (*pTBP & 0x00FFFFFF) | ((*pCur << 8) & 0xFF000000);

					I3ASSERT( GetResource(idx)->IsExactTypeOf( i3TexturePSP::GetClassMeta()));

					pTex = (i3TexturePSP *) GetResource( idx);

					*pCur = i3PSPGfx::TBW( level, (UINT32) pTex->GetImageData( level), pTex->GetBufferWidth(level));
					*pTBP = i3PSPGfx::TBP( level, (UINT32) pTex->GetImageData( level));

					if( pCBP != nullptr)
					{
						i3Clut * pClut = pTex->GetClut(level);

						*pCBP = i3PSPGfx::CBP( (UINT32) pClut->GetData());
						*pCBW = i3PSPGfx::CBW( (UINT32) pClut->GetData());
					}
				}
				break;

			// 주소 보정.
			case I3G_PSP_CMD_CALL :
			case I3G_PSP_CMD_BJUMP :
			case I3G_PSP_CMD_JUMP :
				addr = (*pCur & 0x00FFFFFF) | ((*pBASE & 0x00FF0000) << 8);	
				addr += (UINT32) m_pCommand;

				*pBASE = i3PSPGfx::BASE( (void *) addr);

				switch( (*pCur & 0xFF000000))
				{
					case I3G_PSP_CMD_CALL :	
						*pCur = i3PSPGfx::CALL( (void *) addr);	
						break;

					case I3G_PSP_CMD_BJUMP :
						*pCur = i3PSPGfx::BJUMP( (void *) addr);
						break;

					case I3G_PSP_CMD_JUMP :		
						*pCur = i3PSPGfx::JUMP( (void *) addr);	
						pCur = (UINT32 *) addr;	pCur--;
						break;
				}

				__RESET();
				break;

			case I3G_PSP_CMD_BBOX :
				// 앞서 나타난 VADR, IADR, BASE 등을 재조정해 주어야 한다.
				addr = (*pVADR & 0x00FFFFFF) | ((*pVBASE & 0x00FF0000) << 8);
				addr += (UINT32) m_pCommand;

				*pVBASE = i3PSPGfx::BASE( (void *) addr);
				*pVADR = i3PSPGfx::VADR( (void *) addr);
				break;

			case I3G_PSP_CMD_PRIM :
				if( pVADR != nullptr)
				{
					UINT32 idx, offset;
					i3VertexArray * pVA;

					addr = (*pVADR & 0x00FFFFFF) | ((*pVBASE & 0x00FF0000) << 8);

					idx = (addr >> 16);
					offset = (addr & 0xFFFF) << 2;		// VADR의 Offset은 4의 배수로 지정된다.

					pVA = (i3VertexArray *) GetResource( idx);
					I3ASSERT( GetResource(idx)->IsTypeOf( i3VertexArray::GetClassMeta()));

					addr = (UINT32)pVA->GetInstance() + offset;

					*pVBASE = i3PSPGfx::BASE( (void *) addr);
					*pVADR = i3PSPGfx::VADR( (void *) addr);
				}

				if( pIADR != nullptr)
				{
					UINT32 idx, offset;
					i3IndexArray * pIA;

					addr = (*pIADR & 0x00FFFFFF) | ((*pIBASE & 0x00FF0000) << 8);

					idx = (addr >> 16);
					offset = (addr & 0xFFFF);		// IADR의 Offset은 1의 배수로 지정된다.

					pIA = (i3IndexArray *) GetResource( idx);
					I3ASSERT( GetResource(idx)->IsExactTypeOf( i3IndexArray::GetClassMeta()));

					addr = (UINT32)pIA->GetData() + offset;

					*pIBASE = i3PSPGfx::BASE( (void *) addr);
					*pIADR = i3PSPGfx::IADR( (void *) addr);
				}

				__RESET();
				break;
		}

		pCur++;
	}

	I3TRACE( "------------------- end of binding -----------------\n");

	i3System::DCacheWritebackAll();
}

#endif

void i3GPUCommandList::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	i3GfxResource * pRes;

	for( size_t i = 0; i < m_ResList.size(); i++)
	{
		pRes = m_ResList[i];

		pRes->OnBuildObjectList( List);
	}

	i3PersistantElement::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 GPU_COMMAND_LIST
	{
		INT8	m_ID[4] = { 'G', 'C', 'L', '1' };
		UINT32	m_GPUType = 0;
		UINT32	m_Size = 0;
		UINT32	m_ResCount = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3GPUCommandList::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GPU_COMMAND_LIST Info;
	
	// Information
	{
		Info.m_GPUType = (UINT32) m_GPUType;
		Info.m_Size = m_Size;
		Info.m_ResCount = m_ResList.size();

		Rc = pStream->Write( &Info, sizeof(Info));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3GPUCommandList::OnSave()", "Could not write command list information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Resources
	{
		
		OBJREF ref;

		for(size_t i = 0; i < m_ResList.size(); i++)
		{
			ref = pResFile->GetObjectPersistID( m_ResList[i]);

			Rc = pStream->Write( &ref, sizeof(OBJREF));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3GPUCommandList::OnSave()", "Could not write gfx resource object reference.");
				return STREAM_ERR;
			}
			Result += Rc;
		}
	}

	// Command Data
	if( m_pCommand != nullptr)
	{
		Rc = pStream->Write( m_pCommand, m_Size);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3GPUCommandList::OnSave()", "Could not write command list data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3GPUCommandList::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	pack::GPU_COMMAND_LIST Info;
	INT8 * pBuffer;

	// Information
	{
		Rc = pStream->Read( &Info, sizeof(Info));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3GPUCommandList::OnLoad()", "Could not read command list information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	m_GPUType = (I3G_GPU_TYPE) Info.m_GPUType;
	m_Size = Info.m_Size;

	{
		UINT32 i;
		OBJREF ref;

		for( i = 0; i < Info.m_ResCount; i++)
		{
			Rc = pStream->Read( &ref, sizeof(OBJREF));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3GPUCommandList::OnLoad()", "Could not read gfx resource object reference.");
				return STREAM_ERR;
			}
			Result += Rc;

			if( ref != 0)
			{
				i3GfxResource * pRes = (i3GfxResource *) pResFile->FindObjectByID( ref);

				I3ASSERT( pRes != nullptr);

				AddResource( pRes);
			}
		}
	}

	// Command List Data
	if( Info.m_Size > 0)
	{
		/*
		INT32 hMem;
		switch( GetGPUType())
		{
			case I3G_GPU_PSP :		hMem = i3MemAlign( Info.m_Size, 256);	break;	// PSP는 256 Align된 Address 이어야 한다.
			default :				hMem = i3MemAlloc( Info.m_Size);		break;
		}
		pBuffer = (INT8 *) i3mem::Lock( hMem);
		*/
		switch( GetGPUType())
		{
			case I3G_GPU_PSP :		pBuffer =(INT8 *) i3MemAlign( Info.m_Size, 256 );	break;	// PSP는 256 Align된 Address 이어야 한다.
			default :				pBuffer =(INT8 *) i3MemAlloc( Info.m_Size);		break;
		}

		
		
		I3ASSERT( pBuffer != nullptr);

		Rc = pStream->Read( pBuffer, m_Size);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3GPUCommandList::OnLoad()", "Could not read command list data.");
			//i3mem::Free( hMem );
			i3MemFree(pBuffer); 
			return STREAM_ERR;
		}
		Result += Rc;

		//SetBuffer( pBuffer, hMem);
		SetBuffer( pBuffer );
	}

	#if defined( I3G_PSP)
	if( GetGPUType() == I3G_GPU_PSP)
	{
		BindForPSP();
	}
	#endif

	return Result;
}

#if defined( I3_DEBUG)
void i3GPUCommandList::Dump(void)
{
#if defined( I3_PSP)
	i3GPUContextPSP parser;

	parser.Parse( (UINT32 *) m_pCommand, -1);
#endif
}
#endif
