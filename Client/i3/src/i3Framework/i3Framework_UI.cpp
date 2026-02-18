#include "i3FrameworkPCH.h"
#include "i3Framework.h"

#define MAX_UI_SPRITE_COUNT 9600		// 16bit 지원 그래픽카드에서는 10900개까지 가능

bool i3Framework::_CreateUI(const bool& bLegacy)
{
	m_pUIRenderer = i3UIRenderer::new_object();
	m_pUIRenderer->Create( this, MAX_UI_SPRITE_COUNT, bLegacy);

	I3_GAMENODE_ADDCHILD( this, m_pUIRenderer);

	return true;
}

void i3Framework::_DestroyUI(void)
{
	if( m_pUIRenderer != nullptr)
	{
		RemoveChild( m_pUIRenderer);
		I3_SAFE_RELEASE( m_pUIRenderer);
	}
}
