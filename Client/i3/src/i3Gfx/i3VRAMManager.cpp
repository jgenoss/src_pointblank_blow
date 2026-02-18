#include "i3GfxType.h"

#ifdef I3_PSP
#include "i3VRAMManager.h"

#define i3_VRAMINDEX	 0x80000000
#define I3_NOTVRAMINDEX 0x7FFFFFFF

typedef struct _tagVRAMBLOCK
{
	char	*	m_pPointer; 
	UINT32		m_Size;
}VRAMBLOCK, *LPVRAMBLOCK; 

namespace i3VRAMManager
{	
	INT32			m_VRamCount			= 0; 
	INT32			m_VRamIdx			= 0; 
	VRAMBLOCK	*	m_pVramBlockInfo	= NULL; 	
	char		*	m_pVRamStartPointer = NULL; 
	char		*	m_pVRamIdxPointer	 = NULL; 
	char		*	m_pVRamEndPointer	 = NULL; 
}

INT32 i3VRAMManager::Create(INT32 BlockCount, INT32 DrawBuffer)
{	
	INT32 idx; 
	char * pPointer; 

	//1. Set BlockInfo 
	m_VRamCount	= BlockCount; 
	m_VRamIdx	= 0;

	idx = i3MemAlloc( (sizeof(VRAMBLOCK) * m_VRamCount) ); 
	m_pVramBlockInfo = ( VRAMBLOCK* )i3mem::Lock( idx ); 

	i3mem::FillZero(m_pVramBlockInfo, (sizeof(VRAMBLOCK) * m_VRamCount) );  

	//2. Cal Empty Memeory 
	pPointer = (char *)sceGeEdramGetAddr(); 

	INT32 StartBuffer = 0; 
	switch( DrawBuffer )
	{
	case I3VRAM_DRAW_16BIT: StartBuffer = (SCEGU_VRAM_BUFSIZE*3)	; break; 
	case I3VRAM_DRAW_32BIT: StartBuffer = (SCEGU_VRAM_BUFSIZE32*3)	; break; 	
	}
	m_pVRamStartPointer = (char *) (pPointer + StartBuffer);


	m_pVRamIdxPointer	 = m_pVRamStartPointer;

	i3System::DisableInterrupt();
	m_pVRamEndPointer	 = (char *) (pPointer + sceGeEdramGetSize() ); 
	i3System::EnableInterrupt();
	I3TRACE("i3VRAMManager::Create Start Addr %p End Addr %p \n",m_pVRamStartPointer, m_pVRamEndPointer); 
	return BlockCount; 
}

void  i3VRAMManager::Destroy(void)
{
	i3mem::Free( m_pVramBlockInfo );

	m_VRamCount			= 0; 
	m_VRamIdx			= 0; 
	m_pVramBlockInfo	= NULL; 
	m_pVRamStartPointer = NULL; 
	m_pVRamIdxPointer	 = NULL;
	m_pVRamEndPointer	 = NULL;
}

INT32 i3VRAMManager::Alloc(UINT32 nBytes, INT32 align)
{
	INT32 idx = 0; 
	char * pPointer; 

	//1. 남은 메모리를 확인한다. 
	pPointer	= m_pVRamIdxPointer; 
	nBytes		= (UINT32) ALIGNUP(nBytes, align);
	pPointer	= (char *) ALIGNUP(pPointer, align);

	if(nBytes < (UINT32)(m_pVRamEndPointer - pPointer) )
	{
		//VRam을 사용할수 있다. 
		LPVRAMBLOCK pBlock; 
		idx = FindEmptyBlock(); 
		pBlock = &m_pVramBlockInfo[idx]; 
		pBlock->m_pPointer	= pPointer; 
		pBlock->m_Size		= nBytes; 

		//다음 사용하기 위해서.
		m_pVRamIdxPointer = pPointer + nBytes; 		
		idx = idx | i3_VRAMINDEX; 		
		//I3TRACE("i3VRAMManager::Alloc %p %p %d\n", pBlock->m_pPointer, sceGeEdramGetAddr(),idx);
	}else
	{		
		idx = i3MemAlign(nBytes, align); 
	}

	return idx; 
}
 
INT32 i3VRAMManager::FindEmptyBlock(void)
{	
	INT32 i; 
	INT32 rv; 
	VRAMBLOCK * pBlock = NULL;

	for(i = 0; i < m_VRamCount; i++)
	{
		pBlock = &m_pVramBlockInfo[m_VRamIdx];  	
		if( pBlock->m_Size == 0 )
		{
			rv = m_VRamIdx; 
			m_VRamIdx++; 
			if(m_VRamIdx == m_VRamCount)m_VRamIdx = 0;
			return rv;
		}

		m_VRamIdx++; 
		if(m_VRamIdx == m_VRamCount)m_VRamIdx = 0;
	}
#ifdef I3_DEBUG
	I3TRACE("[i3VRAMManager::FindEmptyBlock] Need More Memory Block \n"); 
#endif
	I3ASSERT_0; 
	return -1;
}


void * i3VRAMManager::Lock(INT32 idx)
{	
	void * pPointer = NULL; 

	if( (idx & i3_VRAMINDEX) == i3_VRAMINDEX )
	{
		VRAMBLOCK * pBlock = NULL;
		idx = idx & I3_NOTVRAMINDEX; 
		pBlock = &m_pVramBlockInfo[idx]; 
		pPointer = (void *) pBlock->m_pPointer; 
	}else
	{
		pPointer = i3mem::Lock(idx); 
	}

	return pPointer; 
}

void * i3VRAMManager::UnLock(INT32 idx)
{
	void * pPointer = NULL; 

	if( (idx & i3_VRAMINDEX) == i3_VRAMINDEX )
	{
		idx = idx & I3_NOTVRAMINDEX; 
	}else
	{
		pPointer = i3mem::UnLock(idx);//UnLock(idx); 
	}

	return pPointer; 
}


void i3VRAMManager::Free(INT32 idx)
{
	if( (idx & i3_VRAMINDEX) == i3_VRAMINDEX )
	{
		idx = idx & I3_NOTVRAMINDEX; 
		#ifdef I3_DEBUG
		if( (idx < 0) && (idx > (m_VRamCount - 1)) )
		{
			I3TRACE("index Error %d Mac Index %d [i3VRAMManager::Free] \n", idx, m_VRamCount); 
			I3ASSERT_0; 
		}
		#endif
		VRAMBLOCK * pBlock = &m_pVramBlockInfo[m_VRamIdx];  	
		i3mem::FillZero( pBlock, sizeof(VRAMBLOCK)); 
	}else
	{
		i3mem::Free(idx); 
	}
 
	return; 
}

void i3VRAMManager::Reset(void)
{
	INT32 i; 
	BOOL	IsEmpty = TRUE; 
	VRAMBLOCK * pBlock; 

	for(i = 0; i < m_VRamCount; i++)
	{
		pBlock = &m_pVramBlockInfo[m_VRamIdx];
		if( pBlock->m_Size > 0 )
		{
			IsEmpty = FALSE; 
			break; 
		}
	}
	
	if(IsEmpty == FALSE)
	{
		#ifdef I3_DEBUG
		I3TRACE("VRam Is Not Empty [i3VRAMManager::Reset] \n"); 
		#endif
		Dump(I3VRAM_DEBUG_TEXT | I3VRAM_DEBUG_EACH);
		return; 
	}
	m_VRamIdx = 0; 
	m_pVRamIdxPointer	 = m_pVRamStartPointer;
	i3mem::FillZero(m_pVramBlockInfo, (sizeof(VRAMBLOCK) * m_VRamCount) );
}

void i3VRAMManager::Dump(INT32 Type)
{
	INT32			i; 
	VRAMBLOCK	*	pBlock; 	

	////////////////////////////////////////////////////////////////////////
	if( (Type & I3VRAM_DEBUG_SUM)  ==  I3VRAM_DEBUG_SUM)
	{
		INT32	UseSize		= 0;
		INT32	BlockCount	= 0; 
 		for(i = 0; i < m_VRamCount; i++)
		{
			pBlock = &m_pVramBlockInfo[m_VRamIdx]; 
			if( pBlock->m_Size > 0 )
			{
				UseSize += pBlock->m_Size; 
				BlockCount++; 
			}
		}

		if((Type & I3VRAM_DEBUG_TEXT) == I3VRAM_DEBUG_TEXT)
		{
			I3TRACE("===========================================\n"); 
			I3TRACE("I3VRAMMANAGER TotalSize : %d \n", (m_pVRamEndPointer - m_pVRamStartPointer) ); 
			I3TRACE("I3VRAMMANAGER BlockInfo Size : %d \n", ( sizeof(VRAMBLOCK) * m_VRamCount ) ); 
			I3TRACE("I3VRAMMANAGER BlockCount : %d Size : %d \n", BlockCount, UseSize); 
			I3TRACE("==========================================\n"); 
		}

		if( (Type & I3VRAM_DEBUG_FILE) == I3VRAM_DEBUG_FILE)
		{
			//아직은 지원할 생각 없음

		}
	}

	////////////////////////////////////////////////////////////////////////
	//I3VRAM_DEBUG_EACH
	if( (Type & I3VRAM_DEBUG_EACH)  ==  I3VRAM_DEBUG_EACH)
	{
		I3TRACE("===========================================\n"); 
		for(i = 0; i < m_VRamCount; i++)
		{
			pBlock = &m_pVramBlockInfo[m_VRamIdx]; 
			if( pBlock->m_Size > 0 )
			{
				if((Type & I3VRAM_DEBUG_TEXT) == I3VRAM_DEBUG_TEXT)
				{
					I3TRACE("Idx : %d, Size %d  \n", i, pBlock->m_Size); 
				}

				if( (Type & I3VRAM_DEBUG_FILE) == I3VRAM_DEBUG_FILE)
				{
					//아직 지원할 생각 없음

				}
			}
		}
		I3TRACE("===========================================\n"); 
	}

}

#endif
