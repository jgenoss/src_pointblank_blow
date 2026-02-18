#include "i3GfxType.h"
#include "i3GfxUtil.h"
#include "i3TextureMem.h"
#include "i3SurfaceMem.h"

I3_CLASS_INSTANCE( i3TextureMem);

i3TextureMem::~i3TextureMem(void)
{
	I3MEM_SAFE_FREE_POINTER( m_pBuffer ); 
}

bool i3TextureMem::Create( UINT32 width, UINT32 height, UINT32 levels, I3G_IMAGE_FORMAT format, I3G_USAGE Usage)
{
	UINT32 BufferSz = 0;
	UINT32 i;
	i3SurfaceMem * pSurface;

	i3Texture::Create( width, height, levels, format, Usage);

	if( m_RuntimePlatform != I3_PLATFORM_UNKNOWN )
	{
		UINT32 NewPlatformMask = 0;
		switch( m_RuntimePlatform )
		{
		case I3_PLATFORM_WINDOWS:	break; //NewPlatformMask = I3G_IMAGE_FORMAT_MASK_PC; break;	// PC, DirectX
		case I3_PLATFORM_XBOX:		NewPlatformMask = I3G_IMAGE_FORMAT_MASK_XBOX;	break;
		case I3_PLATFORM_XENON:		NewPlatformMask = I3G_IMAGE_FORMAT_MASK_XBOX2;	break;
		case I3_PLATFORM_PS2:		NewPlatformMask = I3G_IMAGE_FORMAT_MASK_PS2; break;		// PlayStation 2
		case I3_PLATFORM_PS3:		I3ASSERT_0;	break;
		case I3_PLATFORM_PSP:		NewPlatformMask = I3G_IMAGE_FORMAT_MASK_PSP; break;		// PlayStation Portable
		case I3_PLATFORM_WIPI:		I3ASSERT_0;	break;
		case I3_PLATFORM_UNKNOWN :	I3ASSERT_0;	break;
		}

		m_Format = (I3G_IMAGE_FORMAT)( m_Format & ~I3G_IMAGE_FORMAT_MASK_PLATFORM );
		m_Format = (I3G_IMAGE_FORMAT)( m_Format | NewPlatformMask );
	}

	for( i = 0; i < levels; i++)
	{
		pSurface  = i3SurfaceMem::new_object();
		pSurface->setFormat( m_Format);
		pSurface->setPersistFormat( m_Format);
		pSurface->setWidth( static_cast<UINT16>(width));
		pSurface->setHeight( static_cast<UINT16>(height));
		pSurface->setBitsPerPixel( static_cast<UINT8>(i3Gfx::GetImageFormatBitCount( m_Format)));
		pSurface->setStride( i3Gfx::CalcRasterBufferSize( 1, 1, 1, format));
		pSurface->setPitch( i3Gfx::CalcRasterBufferSize( width, 1, 1, format));

		BufferSz = i3Gfx::CalcRasterBufferSize( (INT32) width, (INT32) height, 1, format);
		pSurface->CreateBuffer( BufferSz);

		m_SurfaceList.push_back( pSurface);
        
		width = width >> 1;
		height = height >> 1;

		if((width < 1) || (height < 1))
			break;
	}

	return true;
}

char * i3TextureMem::Lock( INT32 level, I3G_LOCKMETHOD lock, RECT * pRect)
{
	I3_BOUNDCHK( level, MAX_MIPMAP_LEVEL);
	m_pLockedBuf[ level] = static_cast<i3SurfaceMem *>(m_SurfaceList[level])->getBuffer();

	m_LockedLevel = static_cast<INT8>(level);
	m_LockMethod = lock;

	return m_pLockedBuf[ level];
}

void i3TextureMem::Unlock( INT32 level)
{
	m_pLockedBuf[ level] = nullptr;
}

void i3TextureMem::SetPersistData( INT32 level, INT8 * pData)
{
	I3ASSERT( getSurfaceCount() > level);

	i3SurfaceMem * pSurface = (i3SurfaceMem *)m_SurfaceList[level];

	pSurface->SetPersistData( pData);
}

