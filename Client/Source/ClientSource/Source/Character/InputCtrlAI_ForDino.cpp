#include "pch.h"
#include "InputCtrlAI_ForDino.h"


I3_CLASS_INSTANCE( CInputCtrlAI_ForDino);

bool CInputCtrlAI_ForDino::MoveToTarget( i3GameObj* pTarget, REAL32 minLength, REAL32 rDeltaSeconds)
{
	m_nMoveState = 0;

	VEC3D	vDir;
	i3Vector::Sub( &vDir, pTarget->GetPos(), m_pOwner->GetPos());
	if( i3Vector::Length( &vDir) < minLength)
	{
		return false;
	}

	//걸리는 부분이 생기면 점프...;;
	m_rElapsedJumpTime += rDeltaSeconds;

	if( m_rElapsedJumpTime > 1.5f)
	{
		m_rElapsedJumpTime = 0.0f;

		
		{
			m_nActionState |= AI_ACTION_JUMP;
		}
	}

	MakeKeyInput( &vDir, m_pOwner->GetMatrix());

	return true;
}
