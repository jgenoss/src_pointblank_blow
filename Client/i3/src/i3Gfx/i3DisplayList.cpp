#include "i3GfxType.h"
#include "i3DisplayList.h"

#if defined( I3G_DX)
#include "i3GfxGlobalVariableDX.h"
#include "i3Input/dxerr.h"
#endif

I3_CLASS_INSTANCE( i3DisplayList);

// SVN Test

i3DisplayList::i3DisplayList(void)
{
	setRestoreType( I3G_RESTORE_MEM);
}

i3DisplayList::~i3DisplayList(void)
{
#if defined( I3G_DX)
	I3_SAFE_RELEASE_NODBG( m_pStateBlock);
#endif
}

bool i3DisplayList::Create(void)
{
#if defined( I3G_DX)
	CHECKRESULT( g_pD3DDevice->CreateStateBlock( D3DSBT_ALL, &m_pStateBlock));
#endif

	m_State |= I3G_DSPL_STATE_VALID;

	return true;
}

bool i3DisplayList::Destroy( bool bLostDevice)
{
	// State Block은 항상 제거되어야 한다.
	I3_SAFE_RELEASE_NODBG( m_pStateBlock);

	m_State &= ~I3G_DSPL_STATE_VALID;

	return true;
}


