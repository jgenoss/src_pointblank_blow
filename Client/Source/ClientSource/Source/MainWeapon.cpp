#include "pch.h"
#include "MainWeapon.h"




void	MainWeapon::OnPlayChangeAni( REAL32 rTimeScale)
{
	m_pWeaponBase->OnPlayChangeAniImpl(rTimeScale);					// 구조가 복잡하기 때문에 우선 이렇게 처리하고 나중에 재수정한다..
}


