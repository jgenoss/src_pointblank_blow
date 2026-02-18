#include "i3GfxType.h"
#include "i3SurfaceMem.h"
#include "i3ImageUtil.h"
#include "i3GfxUtil.h"

#if defined( I3G_DX)
#include "i3GfxUtilDX.h"
#endif

I3_CLASS_INSTANCE( i3SurfaceMem);

i3SurfaceMem::~i3SurfaceMem(void)
{
	I3MEM_SAFE_FREE_POINTER( m_pBuffer);
}

bool i3SurfaceMem::Lock( I3G_LOCKMETHOD lock)
{
	return true;
}

void i3SurfaceMem::Unlock(void)
{
}

void i3SurfaceMem::MakeRuntimeForm( bool bMakeLinear)
{
	//i3Surface::MakeRuntimeForm( bMakeLinear);

	UINT32 Width, Height;
	char * pSrc, * pDest;

	Width = getWidth();
	Height = getHeight();

	Lock();

	pSrc = (char *) getPersistData();
	pDest = m_pBuffer;

	{
		if( getPersistFormat() == getFormat())
		{
			i3mem::Copy( pDest, pSrc, m_Pitch*m_Height);
		}
		else
		{
			i3Image_Convert( m_PersistFormat, pSrc, Width, Height, 1, getClut(), 
				m_Format, pDest, 4, getClut());
		}
	}

	Unlock();

}


void i3SurfaceMem::CreateBuffer( INT32 size)
{
	m_pBuffer = (char *) i3MemAlloc( size);
	I3ASSERT( m_pBuffer != nullptr);
	
	m_pLockedBuf = m_pBuffer;
}
