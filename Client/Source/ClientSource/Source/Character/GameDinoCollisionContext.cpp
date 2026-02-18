#include "pch.h"
#include "GameDinoCollisionContext.h"

I3_CLASS_INSTANCE( CGameDinoCollisionContext);

void CGameDinoCollisionContext::OnCreate( CGameCharaBase * pOwner, INT32 defaultView, INT32 defaultNetwork)
{
	CGameCharaCollisionContext::OnCreate( pOwner, defaultView, defaultNetwork);

	if( pOwner != nullptr)
	{
		if( pOwner->IsRex())						m_rColRadius = 5.6f;
		else if( pOwner->IsRaptorBase())			m_rColRadius = 1.5f;
#ifdef DOMI_DINO
		else if( pOwner->IsDomiRaptorBoneBase())	m_rColRadius = 1.5f;
		else if( pOwner->IsDomiMutantRex())			m_rColRadius = 5.6f;
#endif
	}
}