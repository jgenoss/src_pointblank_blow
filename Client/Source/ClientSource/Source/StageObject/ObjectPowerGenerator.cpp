#include "pch.h"
#include "GlobalVariables.h"
#include "ObjectPowerGenerator.h"

I3_CLASS_INSTANCE( CGameObjectPowerGenerator);//, CGameControledObject);

CGameObjectPowerGenerator::CGameObjectPowerGenerator(void)
{

}

CGameObjectPowerGenerator::~CGameObjectPowerGenerator(void)
{

}

void CGameObjectPowerGenerator::InitObject()
{
	m_bManualUpdate = true;
}

void CGameObjectPowerGenerator::ResetObject()
{

}

void CGameObjectPowerGenerator::EnableObject()
{

}

void CGameObjectPowerGenerator::ProcessObject( REAL32 rDeltaSeconds)
{
	CGameControledObject::ProcessObject(rDeltaSeconds);
}

void CGameObjectPowerGenerator::SetControledObjectState( INT32 state)
{

}