#include "pch.h"
#include "WeaponOnObject.h"
#include "GlobalVariables.h"
#include "Camera/GameCamera_OnObject.h"
#include "StageObject/ObjectWithWeapon.h"
#include "BattleSlotContext.h"

WeaponOnObject::WeaponOnObject()
{
	InitVariables();
}

/*virtual*/ WeaponOnObject::~WeaponOnObject()
{
}

void WeaponOnObject::InitVariables()
{
	m_rObjectRho = 0;
	m_rObjectTheta = 0;

	m_iCameraLookAtIdx = -1;
	m_pObjectWithWeapon = nullptr;
	m_bActive = false;
}

void WeaponOnObject::Reset(void)
{
	WeaponBase::Reset();
	InitVariables();
}

void WeaponOnObject::InitWeaponOnObject()
{
	m_rObjectRho = 0;
	m_rObjectTheta = 0;
}

void WeaponOnObject::ActiveWeaponOnObject(CGameObjectWithWeapon* pWeapon)
{
	SetObjectWithWeapon(pWeapon);
	m_bActive = true;
}

void WeaponOnObject::DeactiveWeaponOnObject()
{
	SetObjectWithWeapon(nullptr);
	m_bActive = false;
}

MATRIX* WeaponOnObject::GetCameraLookAtCacheMatrix()
{
	I3ASSERT(m_iCameraLookAtIdx != -1);
	I3ASSERT( getSceneObject()->GetBody() );

	return getSceneObject()->GetBody()->getCacheMatrix(m_iCameraLookAtIdx);
}

MATRIX* WeaponOnObject::GetCameraLookAtMatrix()
{
	I3ASSERT(m_iCameraLookAtIdx != -1);
	I3ASSERT( getSceneObject()->GetBody() );

	return getSceneObject()->GetBody()->getBone(m_iCameraLookAtIdx)->getMatrix();
}

bool WeaponOnObject::IsControlUser()
{
	if(m_bActive) 
	{
		if( i3::same_of<CGameCamera_OnObject*>(g_pCamera)
			&& (m_pObjectWithWeapon->GetEnterCharaIdx() == BattleSlotContext::i()->getMySlotIdx()) ) 
		{
			return true;
		}
	}
	
	return false;
}

INT32 WeaponOnObject::GetEnterCharaIdx()
{ 
	if(GetObjectWithWeapon() == nullptr)
	{
		return -1;
	}
	else
	{
		return GetObjectWithWeapon()->GetEnterCharaIdx();
	}
	
}