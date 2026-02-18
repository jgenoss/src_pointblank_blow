#include "i3GfxType.h"
#include "i3GfxUtil.h"
#include "i3ImageFile.h"
#include "i3VRAMManager.h"

#if defined( I3G_PSP)
#include <sysmem.h>
#include "i3TexturePSP.h"
#include "i3GfxUtilPSP.h"

I3_CONCRETE_CLASS_INSTANCE( i3TexturePSP, i3Texture);

i3TexturePSP::i3TexturePSP(void)
{
	memset( m_pBuffer, 0, sizeof(m_pBuffer));
	m_BufferIdx = -1; 

	m_NativeFormat = 0;

	m_LockedCount = 0;
	memset( m_BufferWidth, 0, sizeof( m_BufferWidth));
}

i3TexturePSP::~i3TexturePSP(void)
{
	if( m_pBuffer[0] != NULL)
	{
		// MIPMAP Level이 1 이상이라 하더라도 모두 선형 메모리로 관리하기 때문에 Level 0 Image에 대해서만
		// 할당 해제하면 된다.		
		//i3mem::Free( m_BufferIdx );
		i3VRAMManager::Free( m_BufferIdx ); 
		m_pBuffer[0] = NULL;
	}
}

static UINT32 s_MemSize = 0;

BOOL i3TexturePSP::Create( UINT32 width, UINT32 height, UINT32 Levels, I3G_IMAGE_FORMAT format, I3G_USAGE Usage)
{
	UINT32 Sz, i, lnSz, bitscount;
	UINT32 BuffSz[8];

	I3ASSERT( width > 0);
	I3ASSERT( height > 0);
	I3ASSERT( Levels > 0);

#if defined( I3_DEBUG)
	if(width>512 || height>512)
	{
		I3TRACE("-----------------------------------------------\n");
		I3TRACE("Texture Size Error(Width:%d, Height%d)\n", width, height );
		I3TRACE("Texture Name is \"%s\"\n", GetName() );
		I3TRACE("-----------------------------------------------\n");
	}
#endif

	i3Texture::Create( width, height, Levels, format, Usage);

	// Find appropriate native image format
	m_NativeFormat = i3PSPUT_GetNativeImageFormat( format, NULL);

	// convert to I3 image format
	format = (I3G_IMAGE_FORMAT)(i3PSPUT_GetI3ImageFormat( m_NativeFormat) | (format & I3G_IMAGE_FORMAT_MASK_PLATFORM));

	m_Format = format;

	bitscount = i3Gfx::GetImageFormatBitCount( format);


/*
	// Bits Count Error Handling
	switch(bitscount)
	{
	case 32:
		if( width<4 && width%4 != 0)
		{
			I3TRACE( "\n----------------------------------------------------------------------------\n");
			I3TRACE( "Texture Buffer Error: Tex Bit(%d), Tex Width(%d), Tex Height(%d)\n", bitscount, width, height );
			I3TRACE( "----------------------------------------------------------------------------\n");
		}
		break;
	case 16:
		if( width<8 && width%8 != 0)
		{
			I3TRACE( "\n----------------------------------------------------------------------------\n");
			I3TRACE( "Texture Buffer Error: Tex Bit(%d), Tex Width(%d), Tex Height(%d)\n", bitscount, width, height );
			I3TRACE( "----------------------------------------------------------------------------\n");
		}
		break;
	case 8:
		if( width<16 && width%16 != 0)
		{
			I3TRACE( "\n----------------------------------------------------------------------------\n");
			I3TRACE( "Texture Buffer Error: Tex Bit(%d), Tex Width(%d), Tex Height(%d)\n", bitscount, width, height );
			I3TRACE( "----------------------------------------------------------------------------\n");
		}
		break;
	case 4:
		if( width<32 && width%32 != 0)
		{
			I3TRACE( "\n----------------------------------------------------------------------------\n");
			I3TRACE( "Texture Buffer Error: Tex Bit(%d), Tex Width(%d), Tex Height(%d)\n", bitscount, width, height );
			I3TRACE( "----------------------------------------------------------------------------\n");
		}
		break;
	}
*/

	Sz = 0;

	for( i = 0; i < Levels; i++)
	{
		I3ASSERT( width > 0);
		I3ASSERT( height > 0);

		m_pBuffer[i] = (char *) Sz;

		// PSP의 Texture는 Width와 Start Address가 16 bytes align되어야 한다.
		lnSz = i3Gfx::CalcRasterBufferSize( width, 1, 4, format);

		m_BufferWidth[i] = ((lnSz << 3) + bitscount - 1) / bitscount;

		BuffSz[i] = (lnSz * height);
		Sz += BuffSz[i];

		width = width >> 1;
		height = height >> 1;
	}

	I3ASSERT( Sz > 0);

	m_DataSize = Sz;

	s_MemSize += m_DataSize;

	//I3TRACE( "TEXTURE MEM : (USE:%d) (Total:%d) (FREE:%d)  Format:%08x\n", m_DataSize, s_MemSize, sceKernelTotalFreeMemSize(), GetFormat());

//	m_BufferIdx = i3MemAlign(Sz, 64);
//	m_pBuffer[0] = (char *) i3mem::Lock(m_BufferIdx);


	m_BufferIdx = i3VRAMManager::Alloc(Sz, 64);
	m_pBuffer[0] = (char *) i3VRAMManager::Lock(m_BufferIdx);
	I3ASSERT( m_pBuffer[0] != NULL);



	// Base Pointer를 기준으로 각 MIPMAP Level의 Base Pointer를 설정한다.
	for( i = 1; i < Levels; i++)
	{
		m_pBuffer[i] = m_pBuffer[i - 1] + BuffSz[i - 1];
	}

	return TRUE;
}

BOOL i3TexturePSP::Create( const char * pszPath, UINT32 /*Levels*/)
{
	i3ImageFile file;

	if( file.Load( pszPath, this) == FALSE)
	{
		return FALSE;
	}

	return TRUE;
}

char * i3TexturePSP::Lock( INT32 level, I3G_LOCKMETHOD lockmode)
{
	I3ASSERT( (level >= 0) && (level < 8));
	I3ASSERT( m_pBuffer[0] != NULL);

	m_LockedCount++;

	return m_pBuffer[ level];
}

void i3TexturePSP::Unlock( INT32 level)
{	
	I3ASSERT( m_LockedCount > 0);

	m_LockedCount --;
}

void i3TexturePSP::MakeRuntimeForm(void)
{
	UINT32 i;

	i3Texture::MakeRuntimeForm();

	for( i = 0; i < GetLevelCount(); i++)
	{
		if( m_pPersistData[i] != NULL)
		{
			i3mem::Free( m_pPersistData[i]);
			m_pPersistData[i] = NULL; 
		}
	}

	if( !IsIndexed())
		return;

	if( m_LevelCount > 1)
	{
		i3Clut * pNewClut = i3Clut::NewObjectRef();
		UINT32 ColorCount = 0, i, idx, j;

		for( i = 0; i < GetLevelCount(); i++)
		{
			i3Clut * pClut = GetClut( i);

			if( pClut != NULL)
				ColorCount += pClut->GetColorCount();
		}

		pNewClut->Create( ColorCount);

		idx = 0;

		for( i = 0; i < GetLevelCount(); i++)
		{
			i3Clut * pClut = GetClut( i);

			if( pClut == NULL)
				continue;

			for( j = 0; j < (UINT32) pClut->GetColorCount(); j++)
			{
				COLOR col;

				pClut->GetColor( j, &col);

				pNewClut->SetColor( idx, &col);
				idx++;
			}
		}

		SetClut( 0, pNewClut);
	}
}
#endif
