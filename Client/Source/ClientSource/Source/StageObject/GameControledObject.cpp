#include "pch.h"
#include "GameControledObject.h"
#include "BattleObjContext.h"

//I3_ABSTRACT_CLASS_INSTANCE( CGameControledObject, i3Object);
I3_CLASS_INSTANCE( CGameControledObject);

CGameControledObject::CGameControledObject(void)
{
	m_bManualUpdate = false;
}

CGameControledObject::~CGameControledObject( void)
{

}

void CGameControledObject::ProcessObject( REAL32 rDeltaSeconds)
{
	GAMEINFO_OBJECT * pObjectInfo = BattleObjContext::i()->setGameInfo_Object( getNetIndex());

	if( pObjectInfo->_tNetData.getObjectType() & UDP_OBJECT_TYPE_POSITION )
	{
		VEC3D vecTemp;
		UDP_UTIL::ConvertNet3DtoVec3D( &vecTemp, pObjectInfo->_tNetData.getPosition() );
		SetPos( &vecTemp );
	}
}
