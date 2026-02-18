#include "pch.h"
#include "ReplaySystem.h"
#include "GlobalVariables.h"

#if defined( USE_REPLAY_SYSTEM)

#define RECORD_TABLE_INCREASE_COUNT	64

I3_CLASS_INSTANCE( CReplaySystem);//, i3ElementBase);

CReplaySystem::CReplaySystem()
{
	m_pDataBuffer = NULL;

	m_iRecordIndex = 0;
	m_iRecordBufferPos = 0;
}

CReplaySystem::~CReplaySystem()
{
	Destroy();
}

bool CReplaySystem::_CheckTableCount( void)
{
	if( m_iRecordIndex == (INT32)m_RecordTable.size())
	{
		return false;
	}

	return true;
}

void CReplaySystem::_MoreCreateTable( INT32 nCount)
{
	
	for(INT32 i = 0; i < nCount; i++)
	{
		RECORD_TABLE_INFO * pInfo = (RECORD_TABLE_INFO*) i3MemAlloc( sizeof( RECORD_TABLE_INFO));
		m_RecordTable.push_back( pInfo);
	}

	I3TRACE( "ReplaySystem::MoreCreateTable(%d)\n", nCount);
}

void CReplaySystem::_DeleteTable( void)
{
	
	for(size_t i = 0; i < m_RecordTable.size(); i++)
	{
		RECORD_TABLE_INFO * pInfo = m_RecordTable[ i];
		I3MEM_SAFE_FREE( pInfo);
	}
}

bool CReplaySystem::_CheckBuffer( UINT32 size)
{
	I3ASSERT( m_pDataBuffer != NULL);

	if( m_pDataBuffer->GetSize() <= m_iRecordBufferPos + size)
		return false;

	return true;
}

void CReplaySystem::_MoreCreateBuffer( UINT32 size)
{
	if( m_pDataBuffer == NULL)
		m_pDataBuffer = i3MemoryBuffer::new_object();

	UINT32 preSize = m_pDataBuffer->GetSize();
	void * pTemp = NULL;

	if( preSize > 0)
	{
		pTemp = i3MemAlloc( preSize);

		i3mem::Copy( pTemp, m_pDataBuffer->getBuffer(), preSize);
	}

	m_pDataBuffer->Create( size);

	if( preSize > 0)
	{
		i3mem::Copy( m_pDataBuffer->getBuffer(), pTemp, preSize);
		I3MEM_SAFE_FREE( pTemp);
	}

	I3TRACE( "ReplaySystem::MoreCreateBuffer(%d)\n", size);
}

void * CReplaySystem::_GetDataBuffer( UINT32 size)
{
	if( _CheckBuffer( size) == false)
	{	// overflow
		_MoreCreateBuffer( (m_iRecordBufferPos+size) * 2);
	}
	
	void * pPointer = (void*)((char*)m_pDataBuffer->getBuffer() + m_iRecordBufferPos);
	m_iRecordBufferPos += size;

	return pPointer;
}

void CReplaySystem::_ReverseDataBufferPos( UINT32 size)
{
	m_iRecordBufferPos -= size;
}


VEC3D * CReplaySystem::_GetVec3DBuffer( VEC3D * pVec)
{
	VEC3D * pBuffer = (VEC3D*) _GetDataBuffer( sizeof( VEC3D));

	i3Vector::Copy( pBuffer, pVec);

	RECORD_SUMMARY_INFO * pSummaryInfo = (RECORD_SUMMARY_INFO*) _GetDataBuffer( sizeof( RECORD_SUMMARY_INFO));
	pSummaryInfo->_tableIndex = m_iRecordIndex;
	pSummaryInfo->_pData = pBuffer;
	m_IndexVEC3D.push_back( pSummaryInfo);

	return pBuffer;
}

QUATERNION * CReplaySystem::_GetQuatBuffer( QUATERNION * pQuat)
{	
	QUATERNION * pQuatBuffer;

	pQuatBuffer = (QUATERNION*) _GetDataBuffer( sizeof( QUATERNION));

	i3Quat::Copy( pQuatBuffer, pQuat);

	RECORD_SUMMARY_INFO * pSummaryInfo = (RECORD_SUMMARY_INFO*) _GetDataBuffer( sizeof( RECORD_SUMMARY_INFO));
	pSummaryInfo->_tableIndex = m_iRecordIndex;
	pSummaryInfo->_pData = pQuatBuffer;
	m_IndexQuaternion.push_back( pSummaryInfo);

	return pQuatBuffer;
}

UINT32 * CReplaySystem::_GetWeaponListBuffer( UINT32 * pWeapon)
{
	UINT32 * pWeaponBuffer;

	pWeaponBuffer = (UINT32*) _GetDataBuffer( sizeof( UINT32) * WEAPON_SLOT_COUNT);

	i3mem::Copy( pWeaponBuffer, pWeapon, sizeof( UINT32) * WEAPON_SLOT_COUNT);

	RECORD_SUMMARY_INFO * pSummaryInfo = (RECORD_SUMMARY_INFO*) _GetDataBuffer( sizeof( RECORD_SUMMARY_INFO));
	pSummaryInfo->_pData = pWeaponBuffer;
	pSummaryInfo->_tableIndex = m_iRecordIndex;
	m_IndexWeaponList.push_back( pSummaryInfo);

	return pWeaponBuffer;
}

UINT32 * CReplaySystem::_GetWeaponListBuffer( UINT32 prim, UINT32 sec, UINT32 melee, UINT32 thro, UINT32 item, UINT32 mission)
{
	UINT32 weapons[ WEAPON_SLOT_COUNT];

	weapons[ 0] = prim;
	weapons[ 1] = sec;
	weapons[ 2] = melee;
	weapons[ 3] = thro;
	weapons[ 4] = item;
	weapons[ 5] = mission;

	return _GetWeaponListBuffer( (UINT32*) weapons);
}

REC_INFO_WEAPON_INFO * CReplaySystem::_GetWeaponInfoBuffer( WeaponBase * pWeapon)
{	
	REC_INFO_WEAPON_INFO * pInfoBuffer;

	pInfoBuffer = (REC_INFO_WEAPON_INFO*) _GetDataBuffer( sizeof( REC_INFO_WEAPON_INFO));

	pInfoBuffer->_loadedBullet		= (UINT8) pWeapon->getLoadedBulletCount();
	pInfoBuffer->_loadedDualBullet	= (UINT8) pWeapon->getLoadedBulletDualCount();
	pInfoBuffer->_totalBullet		= (UINT8) pWeapon->getTotalBulletCount();
	pInfoBuffer->_weaponClass		= (UINT8) pWeapon->getWeaponInfo()->GetTypeClass();
	pInfoBuffer->_weaponNum			= (UINT8) pWeapon->getWeaponInfo()->GetNumber();

	RECORD_SUMMARY_INFO * pSummaryInfo = (RECORD_SUMMARY_INFO*) _GetDataBuffer( sizeof( RECORD_SUMMARY_INFO));
	pSummaryInfo->_pData = pInfoBuffer;
	pSummaryInfo->_tableIndex = m_iRecordIndex;
	m_IndexWeaponInfo.push_back( pSummaryInfo);

	return pInfoBuffer;
}

void CReplaySystem::_WriteEvent( REC_TYPE type, UINT32 evt, void * pData)
{
	if( _CheckTableCount() == false)
		_MoreCreateTable( RECORD_TABLE_INCREASE_COUNT);

	RECORD_TABLE_INFO * pTable = __getTableInfo( m_iRecordIndex);

	pTable->_rTime = g_pGameContext->GetAccTime();
	pTable->_uType = type;
	pTable->_uEvent = evt;
	pTable->_pData = pData;

	RECORD_SUMMARY_INFO * pSummaryInfo = (RECORD_SUMMARY_INFO*) _GetDataBuffer( sizeof( RECORD_SUMMARY_INFO));
	pSummaryInfo->_tableIndex = m_iRecordIndex;
	pSummaryInfo->_pData = pData;

	switch( type)
	{
	case REC_TYPE_STAGE :	m_IndexStage.push_back( pSummaryInfo);	break;
	case REC_TYPE_CHARA :	m_IndexChara.push_back( pSummaryInfo);	break;
	case REC_TYPE_WEAPON :	m_IndexWeapon.push_back( pSummaryInfo);	break;
	case REC_TYPE_OBJECT :	m_IndexObject.push_back( pSummaryInfo);	break;
	}

	m_iRecordIndex++;
}


void CReplaySystem::_SummaryData( void)
{
	_SummaryVec2D();
	_SummaryVec3D();
	_SummaryQuaternion();

	_SummaryWeaponInfo();
	_SummaryStage();
	_SummaryChara();
	_SummaryWeapon();
	_SummaryObject();
}

void CReplaySystem::_SummaryVec2D( void)
{
	INT32 i, j;
	RECORD_SUMMARY_INFO * pSrc1, * pSrc2;
	
	for( i = 0; i < (INT32)m_IndexVEC3D.size()-1; i++)
	{
		pSrc1 = m_IndexVEC3D[ i];

		for( j = i+1; j < (INT32)m_IndexVEC3D.size(); j++)
		{
			pSrc2 = m_IndexVEC3D[ j];

			if( i3Vector::IsSame( (VEC2D*)pSrc1->_pData, (VEC2D*)pSrc2->_pData) == FALSE)
				continue;

			I3ASSERT( pSrc2->_tableIndex != -1);

//			RECORD_TABLE_INFO * pTableInfo = m_RecordTable[ pSrc2->_tableIndex];

			i3::vu::erase_index(m_IndexVEC3D, j);
			j--;
		}
	}
}

void CReplaySystem::_SummaryVec3D( void)
{
	INT32 i, j;
	RECORD_SUMMARY_INFO * pSrc1, * pSrc2;
	
	for( i = 0; i < (INT32)m_IndexVEC3D.size()-1; i++)
	{
		pSrc1 = m_IndexVEC3D[ i];

		for( j = i+1; j < (INT32)m_IndexVEC3D.size(); j++)
		{
			pSrc2 = m_IndexVEC3D[ j];

			if( i3Vector::IsSame( (VEC3D*)pSrc1->_pData, (VEC3D*)pSrc2->_pData) == FALSE)
				continue;

			I3ASSERT( pSrc2->_tableIndex != -1);

			RECORD_TABLE_INFO * pTableInfo =m_RecordTable[ pSrc2->_tableIndex];

			switch( pTableInfo->_uType)
			{
			case REC_TYPE_CHARA :
				{
					switch( pTableInfo->_uEvent)
					{
					case REC_CHARA_DEATH :
						{
							REC_INFO_CHARA_DEATH * pInfo = (REC_INFO_CHARA_DEATH*) pTableInfo->_pData;
							
							if( i3Vector::IsSame( pInfo->_pDir, (VEC3D*)pSrc1->_pData))
								pInfo->_pDir = (VEC3D*)pSrc1->_pData;
						}
						break;

					case REC_CHARA_POS :
						{
							REC_INFO_CHARA_POS * pInfo = (REC_INFO_CHARA_POS*) pTableInfo->_pData;
							
							if( i3Vector::IsSame( pInfo->_pPos, (VEC3D*)pSrc1->_pData))
								pInfo->_pPos = (VEC3D*)pSrc1->_pData;
						}
						break;
					}
				}
				break;

			case REC_TYPE_WEAPON :
				{
					if( pTableInfo->_uEvent == REC_WEAPON_TRANSFORM)
					{
						REC_INFO_WEAPON_TRANSFORM * pInfo = (REC_INFO_WEAPON_TRANSFORM*) pTableInfo->_pData;

						if( i3Vector::IsSame( pInfo->_pPos, (VEC3D*)pSrc1->_pData))
							pInfo->_pPos = (VEC3D*)pSrc1->_pData;
					}
				}
				break;
				
			case REC_TYPE_OBJECT :
				{
					if( pTableInfo->_uEvent == REC_OBJECT_TRANSFORM)
					{
						REC_INFO_OBJECT_TRANSFORM * pInfo = (REC_INFO_OBJECT_TRANSFORM*) pTableInfo->_pData;

						if( i3Vector::IsSame( pInfo->_pPos, (VEC3D*)pSrc1->_pData))
							pInfo->_pPos = (VEC3D*)pSrc1->_pData;
					}
				}
				break;
			}

			i3::vu::erase_index(m_IndexVEC3D, j);
			j--;
		}

		
	}			
	
}

void CReplaySystem::_SummaryQuaternion( void)
{
	INT32 i, j;

	RECORD_SUMMARY_INFO * pSrc1, * pSrc2;

	for( i = 0; i < (INT32)m_IndexQuaternion.size()-1; i++)
	{
		pSrc1 = m_IndexQuaternion[ i];

		for( j = i+1; j < (INT32)m_IndexQuaternion.size(); j++)
		{
			pSrc2 = m_IndexQuaternion[ j];

			if( i3Vector::IsSame( (QUATERNION*)pSrc1->_pData, (QUATERNION*)pSrc2->_pData) == FALSE)
				continue;

			I3ASSERT( pSrc2->_tableIndex != -1);

			RECORD_TABLE_INFO * pTableInfo = m_RecordTable[ pSrc2->_tableIndex];

			switch( pTableInfo->_uType)
			{
			case REC_TYPE_WEAPON :
				if( pTableInfo->_uEvent == REC_WEAPON_TRANSFORM)
				{
					REC_INFO_WEAPON_TRANSFORM * pInfo = (REC_INFO_WEAPON_TRANSFORM*) pTableInfo->_pData;

					if( i3Vector::IsSame( pInfo->_pRot, (QUATERNION*)pSrc1->_pData))
						pInfo->_pRot = (QUATERNION*)pSrc1->_pData;
				}
				break;

			case REC_TYPE_OBJECT :
				if( pTableInfo->_uEvent == REC_OBJECT_TRANSFORM)
				{
					REC_INFO_OBJECT_TRANSFORM * pInfo = (REC_INFO_OBJECT_TRANSFORM*) pTableInfo->_pData;

					if( i3Vector::IsSame( pInfo->_pRot, (QUATERNION*)pSrc1->_pData))
						pInfo->_pRot = (QUATERNION*)pSrc1->_pData;
				}
				break;
			}

			i3::vu::erase_index(m_IndexQuaternion, j);
			j--;
		}
	}
				
}

void CReplaySystem::_SummaryWeaponList( void)
{
	INT32 i, j;

	RECORD_SUMMARY_INFO * pSrc1, * pSrc2;

	for( i = 0; i < (INT32)m_IndexWeaponList.size()-1; i++)
	{
		pSrc1 =m_IndexWeaponList[ i];

		for( j = i+1; j < (INT32)m_IndexWeaponList.size(); j++)
		{
			pSrc2 = m_IndexWeaponList[ j];

			if( IS_SAME_WEAPONLIST( ((UINT32*)pSrc1->_pData), ((UINT32*)pSrc2->_pData)) == FALSE)
				continue;

			I3ASSERT( pSrc2->_tableIndex != -1);

			RECORD_TABLE_INFO * pTableInfo = m_RecordTable[ pSrc2->_tableIndex];

			if( pTableInfo->_uType == REC_TYPE_CHARA)
			{
				switch( pTableInfo->_uEvent)
				{
				case REC_CHARA_ENTER :
					{
						REC_INFO_CHARA_ENTER * pInfo = (REC_INFO_CHARA_ENTER*) pTableInfo->_pData;

						if( IS_SAME_WEAPONLIST( pInfo->_weapon, ((UINT32*)pSrc1->_pData)))
							pInfo->_weapon = (UINT32*)pSrc1->_pData;
					}
					break;
				case REC_CHARA_LEAVE :
					{
						REC_INFO_CHARA_LEAVE * pInfo = (REC_INFO_CHARA_LEAVE*) pTableInfo->_pData;

						if( IS_SAME_WEAPONLIST( pInfo->_weapon, ((UINT32*)pSrc1->_pData)))
							pInfo->_weapon = (UINT32*)pSrc1->_pData;
					}
					break;
				case REC_CHARA_RESPAWN :
					{
						REC_INFO_CHARA_RESPAWN * pInfo = (REC_INFO_CHARA_RESPAWN*) pTableInfo->_pData;

						if( IS_SAME_WEAPONLIST( pInfo->_weapon, ((UINT32*)pSrc1->_pData)))
							pInfo->_weapon = (UINT32*)pSrc1->_pData;
					}
					break;
				case REC_CHARA_DEATH :
					{
						REC_INFO_CHARA_DEATH * pInfo = (REC_INFO_CHARA_DEATH*) pTableInfo->_pData;

						if( IS_SAME_WEAPONLIST( pInfo->_weapon, ((UINT32*)pSrc1->_pData)))
							pInfo->_weapon = (UINT32*)pSrc1->_pData;
					}
					break;

				default :
					I3FATAL( "Not yet process type...!!");
					break;
				}
			}

			i3::vu::erase_index(m_IndexWeaponList, j);
			j--;
		}
	}
}

void CReplaySystem::_SummaryWeaponInfo( void)
{
	INT32 i, j;

	RECORD_SUMMARY_INFO * pSrc1, * pSrc2;

	for( i = 0; i < (INT32)m_IndexWeaponInfo.size()-1; i++)
	{
		pSrc1 =m_IndexWeaponInfo[ i];

		for( j = i+1; j < (INT32)m_IndexWeaponInfo.size(); j++)
		{
			pSrc2 = m_IndexWeaponInfo[ j];

			if( ((REC_INFO_WEAPON_INFO*)pSrc1->_pData)->isSame( (REC_INFO_WEAPON_INFO*) pSrc2->_pData) == FALSE)
				continue;
			
			I3ASSERT( pSrc2->_tableIndex != -1);

			RECORD_TABLE_INFO * pTableInfo =m_RecordTable[ pSrc2->_tableIndex];
			pTableInfo->_pData = pSrc1->_pData;

			i3::vu::erase_index(m_IndexWeaponInfo, j);
			j--;
		}
	}
}

void  CReplaySystem::_SummaryStage( void)
{
	INT32 i, j;

	RECORD_SUMMARY_INFO * pSrc1, * pSrc2;
	RECORD_TABLE_INFO * pTable1, * pTable2;
	
	for( i = 0; i < (INT32)m_IndexStage.size()-1; i++)
	{
		pSrc1 = m_IndexStage[ i];

		I3ASSERT( pSrc1->_tableIndex != -1);
		pTable1 = m_RecordTable[ pSrc1->_tableIndex];

		for( j = i+1; j < (INT32)m_IndexStage.size(); j++)
		{
			bool bSame = false;

			pSrc2 = m_IndexStage[ j];

			I3ASSERT( pSrc2->_tableIndex != -1);
			pTable2 = m_RecordTable[ pSrc2->_tableIndex];

			if( pTable1->_uEvent != pTable2->_uEvent)
				continue;

			switch( pTable1->_uEvent)
			{
			case REC_STAGE_START :
			case REC_STAGE_END :
			case REC_STAGE_ROUNDSTART :
			case REC_STAGE_ROUNDEND :
			case REC_STAGE_BATTLE_END :
				break;
			case REC_STAGE_KILL :
				if( ((REC_INFO_STAGE_KILL*)pSrc1->_pData)->isSame( (REC_INFO_STAGE_KILL*)pSrc2->_pData) )
					bSame = true;
				break;
			case REC_STAGE_ENTER_USER :
			case REC_STAGE_LEAVE_USER :
				break;
			default:
				I3FATAL( "no setting process event..");
				break;
			}
			
			if( bSame)
			{	
				pTable2->_pData = pTable1->_pData;

				i3::vu::erase_index(m_IndexStage, j);
				j--;
			}
		}
	}

}

void CReplaySystem::_SummaryChara( void)
{
	INT32 i, j;
	RECORD_SUMMARY_INFO * pSrc1, * pSrc2;
	RECORD_TABLE_INFO * pTable1, * pTable2;

	for( i = 0; i < (INT32)m_IndexChara.size()-1; i++)
	{
		pSrc1 = m_IndexChara[ i];

		I3ASSERT( pSrc1->_tableIndex != -1);
		pTable1 = m_RecordTable[ pSrc1->_tableIndex];

		for( j = i+1; j < (INT32)m_IndexChara.size(); j++)
		{
			bool bSame = false;

			pSrc2 = m_IndexChara[ j];

			I3ASSERT( pSrc2->_tableIndex != -1);
			pTable2 = m_RecordTable[ pSrc2->_tableIndex];

			if( pTable1->_uEvent != pTable2->_uEvent)
				continue;

			switch( pTable1->_uEvent)
			{
			case REC_CHARA_ENTER :
				if( ((REC_INFO_CHARA_ENTER*)pSrc1->_pData)->isSame( (REC_INFO_CHARA_ENTER*)pSrc2->_pData) )
					bSame = true;
				break;
			case REC_CHARA_LEAVE :
				if( ((REC_INFO_CHARA_LEAVE*)pSrc1->_pData)->isSame( (REC_INFO_CHARA_LEAVE*)pSrc2->_pData) )
					bSame = true;
				break;
			case REC_CHARA_RESPAWN :
				if( ((REC_INFO_CHARA_RESPAWN*)pSrc1->_pData)->isSame( (REC_INFO_CHARA_RESPAWN*)pSrc2->_pData) )
					bSame = true;
				break;
			case REC_CHARA_INVINCIBLE :
				if( ((REC_INFO_CHARA_INVINCIBLE*)pSrc1->_pData)->isSame( (REC_INFO_CHARA_INVINCIBLE*)pSrc2->_pData))
					bSame = true;
				break;
			case REC_CHARA_DEATH :
				if( ((REC_INFO_CHARA_DEATH*)pSrc1->_pData)->isSame( (REC_INFO_CHARA_DEATH*)pSrc2->_pData))
					bSame = true;
				break;
			case REC_CHARA_ACTION_BODY :
				if( ((REC_INFO_CHARA_BODY_ACTION*)pSrc1->_pData)->isSame( (REC_INFO_CHARA_BODY_ACTION*)pSrc2->_pData))
					bSame = true;
				break;
			case REC_CHARA_ACTION_UPPER :
				if( ((REC_INFO_CHARA_UPPER_ACTION*)pSrc1->_pData)->isSame( (REC_INFO_CHARA_UPPER_ACTION*)pSrc2->_pData))
					bSame = true;
				break;
			case REC_CHARA_ACTION_LOWER :
				if( ((REC_INFO_CHARA_LOWER_ACTION*)pSrc1->_pData)->isSame( (REC_INFO_CHARA_LOWER_ACTION*)pSrc2->_pData))
					bSame = true;
				break;
			case REC_CHARA_ACTION_ZOOM :
				if( ((REC_INFO_CHARA_ZOOM_ACTION*)pSrc1->_pData)->isSame( (REC_INFO_CHARA_ZOOM_ACTION*)pSrc2->_pData))
					bSame = true;
				break;
			case REC_CHARA_POS :
				if( ((REC_INFO_CHARA_POS*)pSrc1->_pData)->isSame( (REC_INFO_CHARA_POS*)pSrc2->_pData))
					bSame = true;
				break;
			case REC_CHARA_DROPWEAPON :
				if( ((REC_INFO_CHARA_DROPWEAPON*)pSrc1->_pData)->isSame( (REC_INFO_CHARA_DROPWEAPON*)pSrc2->_pData))
					bSame = true;
				break;
			case REC_CHARA_HIDE :
				if( ((REC_INFO_CHARA_HIDE*)pSrc1->_pData)->isSame( (REC_INFO_CHARA_HIDE*)pSrc2->_pData))
					bSame = true;
				break;
			case REC_CHARA_EQUIPMENT :
				if( ((REC_INFO_CHARA_EQUIPMENT*)pSrc1->_pData)->isSame( (REC_INFO_CHARA_EQUIPMENT*)pSrc2->_pData))
					bSame = true;
				break;
			default :
				I3FATAL( "no setting process event!!");
				break;
			}

			if( bSame)
			{	
				pTable2->_pData = pTable1->_pData;

				i3::vu::erase_index(m_IndexChara, j);
				j--;
			}
		}
	}
	
}




void CReplaySystem::_SummaryWeapon( void)
{
	INT32 i, j;

	RECORD_SUMMARY_INFO * pSrc1, *pSrc2;
	RECORD_TABLE_INFO * pTable1, * pTable2;

	for( i = 0; i < (INT32)m_IndexWeapon.size()-1; i++)
	{
		pSrc1 = m_IndexWeapon[ i];

		I3ASSERT( pSrc1->_tableIndex != -1);
		pTable1 = m_RecordTable[ pSrc1->_tableIndex];

		for( j = i+1; j < (INT32)m_IndexWeapon.size(); j++)
		{
			bool bSame = false;

			pSrc2 = m_IndexWeapon[ j];

			I3ASSERT( pSrc2->_tableIndex != -1);
			pTable2 = m_RecordTable[ pSrc2->_tableIndex];

			if( pTable1->_uEvent != pTable2->_uEvent)
				continue;

			switch( pTable1->_uEvent)
			{
			case REC_WEAPON_CREATE :
				if( ((REC_INFO_WEAPON_CREATE*)pSrc1->_pData)->isSame( (REC_INFO_WEAPON_CREATE*)pSrc2->_pData))
					bSame = true;
				break;
			case REC_WEAPON_TRANSFORM :
			case REC_WEAPON_HIDE :
				break;
			}

			if( bSame)
			{
				pTable2->_pData = pTable1->_pData;

				i3::vu::erase_index(m_IndexWeapon, j);
				j--;
			}
		}
	}
}

void CReplaySystem::_SummaryObject( void)
{
	INT32 i, j;

	RECORD_SUMMARY_INFO * pSrc1, *pSrc2;
	RECORD_TABLE_INFO * pTable1, * pTable2;
	
	for( i = 0; i < (INT32)m_IndexObject.size()-1; i++)
	{
		pSrc1 =  m_IndexObject[ i];

		I3ASSERT( pSrc1->_tableIndex != -1);
		pTable1 = m_RecordTable[ pSrc1->_tableIndex];

		for( j = i+1; j < (INT32)m_IndexObject.size(); j++)
		{
			bool bSame = false;

			pSrc2 = m_IndexObject[ j];

			I3ASSERT( pSrc2->_tableIndex != -1);
			pTable2 = m_RecordTable[ pSrc2->_tableIndex];

			switch( pTable1->_uEvent)
			{
			case REC_OBJECT_INIT :
				if( ((REC_INFO_OBJECT_INIT*)pSrc1->_pData)->isSame( (REC_INFO_OBJECT_INIT*)pSrc2->_pData))
					bSame = true;
				break;
			case REC_OBJECT_STATE :
				if( ((REC_INFO_OBJECT_STATE*)pSrc1->_pData)->isSame( (REC_INFO_OBJECT_STATE*)pSrc2->_pData))
					bSame = true;
				break;
			case REC_OBJECT_ANIMATION :
			case REC_OBJECT_TRANSFORM :
				break;
			default :
				I3FATAL( "no setting process");
				break;
			}

			if( bSame)
			{
				pTable2->_pData = pTable1->_pData;

				i3::vu::erase_index(m_IndexObject, j);
				j--;
			}
		}
	}
}

INT32 CReplaySystem::_FindFromVec2D( VEC2D * pVec)
{
	
	RECORD_SUMMARY_INFO * pInfo;
	
	for(size_t i = 0; i < m_IndexVEC2D.size(); i++)
	{
		pInfo = m_IndexVEC2D[ i];

		if( pInfo->_pData == pVec)
			return i;
	}

	return -1;
}

INT32 CReplaySystem::_FindFromVec3D( VEC3D * pVec)
{
	
	RECORD_SUMMARY_INFO * pInfo;

	for(size_t i = 0; i < m_IndexVEC3D.size(); i++)
	{
		pInfo = m_IndexVEC3D[ i];

		if( pInfo->_pData == pVec)
			return i;
	}

	return -1;
}

INT32 CReplaySystem::_FindFromQuat( QUATERNION * pQuat)
{
	
	RECORD_SUMMARY_INFO * pInfo;

	for( size_t i = 0; i < m_IndexQuaternion.size(); i++)
	{
		pInfo = m_IndexQuaternion[ i];

		if( pInfo->_pData == pQuat)
			return i;
	}

	return -1;
}

INT32 CReplaySystem::_FindFromWeaponList( UINT32 * pWeapon)
{
	
	RECORD_SUMMARY_INFO * pInfo;

	for(size_t i = 0; i < m_IndexWeaponList.size(); i++)
	{
		pInfo = m_IndexWeaponList[ i];

		if( pInfo->_pData == pWeapon)
			return i;
	}

	return -1;
}

INT32 CReplaySystem::_FindFromWeaponInfo( REC_INFO_WEAPON_INFO * pData)
{
	
	RECORD_SUMMARY_INFO * pInfo;

	for(size_t i = 0; i < m_IndexWeaponInfo.size(); i++)
	{
		pInfo = m_IndexWeaponInfo[ i];
		if( pInfo->_pData == pData)
			return i;
	}

	return -1;
}

INT32 CReplaySystem::_FindFromStage( void * pData)
{
	
	RECORD_SUMMARY_INFO * pInfo;

	for(size_t i = 0; i < m_IndexStage.size(); i++)
	{
		pInfo = m_IndexStage[ i];

		if( pInfo->_pData == pData)
			return i;
	}

	return -1;
}

INT32 CReplaySystem::_FindFromCharacter( void * pData)
{
	
	RECORD_SUMMARY_INFO * pInfo;

	for(size_t i = 0; i < m_IndexChara.size(); i++)
	{
		pInfo = m_IndexChara[ i];

		if( pInfo->_pData == pData)
			return i;
	}

	return -1;
}

INT32 CReplaySystem::_FindFromWeapon( void * pData)
{

	RECORD_SUMMARY_INFO * pInfo;

	for(size_t i = 0; i < m_IndexWeapon.size(); i++)
	{
		pInfo = m_IndexWeapon[ i];

		if( pInfo->_pData == pData)
			return i;
	}

	return -1;
}

INT32 CReplaySystem::_FindFromObject( void * pData)
{


	RECORD_SUMMARY_INFO * pInfo;

	for(size_t i = 0; i < m_IndexObject.size(); i++)
	{
		pInfo = m_IndexObject[ i];

		if( pInfo->_pData == pData)
			return i;
	}

	return -1;
}

INT32 CReplaySystem::_FindFromData( UINT32 uType, void * pData)
{
	switch( uType)
	{
	case REC_TYPE_GAMEEVENT :	break;
	case REC_TYPE_STAGE :		return _FindFromStage( pData);
	case REC_TYPE_CHARA :		return _FindFromCharacter( pData);
	case REC_TYPE_WEAPON :		return _FindFromWeapon( pData);
	case REC_TYPE_OBJECT :		return _FindFromObject( pData);
	case REC_TYPE_EFFECT :		break;
	}

	return -1;
}

UINT32 CReplaySystem::_SaveVec2D( i3FileStream * pFile)
{
	
	UINT32 Rc, Result = 0;

	PERSIST_VEC2D Vec2DData;
	RECORD_SUMMARY_INFO * pInfo;

	for(size_t i = 0; i < m_IndexVEC2D.size(); i++)
	{
		pInfo = m_IndexVEC2D[ i];
		VEC2D * pVec = (VEC2D*) pInfo->_pData;

		Vec2DData.x = i3Vector::GetX( pVec);
		Vec2DData.y = i3Vector::GetY( pVec);

		Rc = pFile->Write( &Vec2DData, sizeof( Vec2DData));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

UINT32 CReplaySystem::_SaveVec3D( i3FileStream * pFile)
{

	UINT32 Rc, Result = 0;
	PERSIST_VEC3D Vec3DData;
	RECORD_SUMMARY_INFO * pInfo;

	for( size_t i = 0; i < m_IndexVEC3D.size(); i++)
	{
		pInfo = m_IndexVEC3D[ i];
		VEC3D * pVec = (VEC3D*) pInfo->_pData;

		Vec3DData.x = i3Vector::GetX( pVec);
		Vec3DData.y = i3Vector::GetY( pVec);
		Vec3DData.z = i3Vector::GetZ( pVec);

		Rc = pFile->Write( &Vec3DData, sizeof( Vec3DData));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

UINT32 CReplaySystem::_SaveQuat( i3FileStream * pFile)
{
	
	UINT32 Rc, Result = 0;
	PERSIST_VEC4D Vec4DData;
	RECORD_SUMMARY_INFO * pInfo;
	
	for( size_t i = 0; i < m_IndexQuaternion.size(); i++)
	{
		pInfo =m_IndexQuaternion[ i];
		QUATERNION * pQuat = (QUATERNION*) pInfo->_pData;

		Vec4DData.x = i3Quat::GetA( pQuat);
		Vec4DData.y = i3Quat::GetB( pQuat);
		Vec4DData.z = i3Quat::GetC( pQuat);
		Vec4DData.w = i3Quat::GetD( pQuat);

		Rc = pFile->Write( &Vec4DData, sizeof( Vec4DData));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

UINT32 CReplaySystem::_SaveWeaponList( i3FileStream * pFile)
{
	
	UINT32 Rc, Result = 0;
	RECORD_SUMMARY_INFO * pInfo;

	for(size_t i = 0; i < m_IndexWeaponList.size(); i++)
	{
		pInfo = m_IndexWeaponList[ i];
		
		Rc = pFile->Write( pInfo->_pData, sizeof( UINT32) * WEAPON_SLOT_COUNT);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

UINT32 CReplaySystem::_SaveWeaponInfo( i3FileStream * pFile)
{
	
	UINT32 Rc, Result = 0;
	RECORD_SUMMARY_INFO * pInfo;

	for(size_t i = 0; i < m_IndexWeaponInfo.size(); i++)
	{
		pInfo = m_IndexWeaponInfo[ i];
		REC_INFO_WEAPON_INFO * pData = (REC_INFO_WEAPON_INFO*) pInfo->_pData;
		
		Rc = pFile->Write( pData, sizeof( REC_INFO_WEAPON_INFO));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

UINT32 CReplaySystem::_SaveStage( i3FileStream * pFile)
{
	INT32  j;
	UINT16	size;
	UINT32	Rc, Result = 0;

	RECORD_SUMMARY_INFO * pInfo;
	RECORD_TABLE_INFO * pTable;
	
	for( size_t i = 0; i < m_IndexStage.size(); i++)
	{
		pInfo = m_IndexStage[ i];
			
		I3ASSERT( pInfo->_tableIndex != -1);
		pTable = m_RecordTable[ pInfo->_tableIndex];
		size = __getSummaryInfoSize( pTable->_uType, pTable->_uEvent);
		I3ASSERT( size != 0);

		Rc = pFile->Write( pInfo->_pData, size);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
typedef struct ALIGN4 __tagPersist_Character_Enter
{
	UINT8	_index;							///< user index
	UINT32	_resType;						///< resource type
	UINT8	_team;							///< team flag
	INT32	_title[ MAX_EQUIP_USER_TITLE];	///< title
	UINT32	_hair;							///< hair resource type (helmet)
	UINT8	_item;							///< item resource type
	UINT32	_clan;							///< clan mark index
	UINT8	_respawnIndex;					///< respawn index
	INT32	_weaponIdx;						///< equipment weapon info [ WEAPON_SLOT_COUNT]
} PERSIST_REC_CHARACTER_ENTER;

typedef struct ALIGN4 __tagPersist_Character_Respawn
{
	UINT8	_index;
	UINT8	_RespawnIdx;
	INT32	_weaponIdx;
	REAL32	_invincibleTime;

} PERSIST_REC_CHARACTER_RESPAWN;

typedef struct ALIGN4 __tagPersist_Character_Death
{
	UINT8	_index;
	UINT8	_deathType;
	INT8	_deathMotion;
	INT32	_weaponIdx;
	INT32	_vec3Idx;
} PERSIST_REC_CHARACTER_DEATH;

typedef struct ALIGN4 __tagPersist_Character_Position
{
	UINT8	_index;
	INT32	_vec3DIdx;
	REAL32	_theta;
	REAL32	_rho;
} PERSIST_REC_CHARACTER_POSITION;

#pragma pack( pop)
#endif

UINT32 CReplaySystem::_SaveCharacter( i3FileStream * pFile)
{	
	
	UINT32 Rc, Result = 0;

	RECORD_SUMMARY_INFO * pInfo;
	RECORD_TABLE_INFO * pTableInfo;

	// Save Character info
	for(size_t i = 0; i < m_IndexChara.size(); i++)
	{
		pInfo = m_IndexChara[ i];
				
		I3ASSERT( pInfo->_tableIndex != -1);
		
		pTableInfo = m_RecordTable[ pInfo->_tableIndex];

		switch( pTableInfo->_uEvent)
		{
		case REC_CHARA_ENTER :
		case REC_CHARA_LEAVE :
			{
				PERSIST_REC_CHARACTER_ENTER data;
				REC_INFO_CHARA_ENTER * pCharaInfo = (REC_INFO_CHARA_ENTER*) pInfo->_pData;

				data._clan = pCharaInfo->_clan;
				data._hair = pCharaInfo->_hair;
				data._index = pCharaInfo->_index;
				data._item = pCharaInfo->_item;
				data._respawnIndex = pCharaInfo->_respawnIndex;
				data._resType = pCharaInfo->_resType;
				data._team = pCharaInfo->_team;
				data._title[0] = pCharaInfo->_title[0];
				data._title[1] = pCharaInfo->_title[1];
				data._title[2] = pCharaInfo->_title[2];
				data._weaponIdx = _FindFromWeaponList( pCharaInfo->_weapon);

				Rc = pFile->Write( &data, sizeof( data));
			}
			break;
		case REC_CHARA_RESPAWN :
			{
				PERSIST_REC_CHARACTER_RESPAWN data;
				REC_INFO_CHARA_RESPAWN * pCharaInfo = (REC_INFO_CHARA_RESPAWN*) pInfo->_pData;

				data._index = pCharaInfo->_index;
				data._invincibleTime = pCharaInfo->_invincibleTime;
				data._RespawnIdx = pCharaInfo->_RespawnIdx;
				data._weaponIdx = _FindFromWeaponList( pCharaInfo->_weapon);

				Rc = pFile->Write( &data, sizeof( data));
			}
			break;
		case REC_CHARA_DEATH :
			{
				PERSIST_REC_CHARACTER_DEATH data;
				REC_INFO_CHARA_DEATH * pCharaInfo = (REC_INFO_CHARA_DEATH*) pInfo->_pData;

				data._deathMotion = pCharaInfo->_deathMotion;
				data._deathType = pCharaInfo->_deathType;
				data._index = pCharaInfo->_index;
				data._vec3Idx = _FindFromVec3D( pCharaInfo->_pDir);
				data._weaponIdx = _FindFromWeaponList( pCharaInfo->_weapon);

				Rc = pFile->Write( &data, sizeof( data));
			}
			break;
		
		case REC_CHARA_POS :
			{
				PERSIST_REC_CHARACTER_POSITION data;
				REC_INFO_CHARA_POS * pCharaInfo = (REC_INFO_CHARA_POS*) pInfo->_pData;

				data._index = pCharaInfo->_index;
				data._rho = pCharaInfo->_rho;
				data._theta = pCharaInfo->_theta;
				data._vec3DIdx = _FindFromVec3D( pCharaInfo->_pPos);

				Rc = pFile->Write( &data, sizeof( data));
			}
			break;
		default :
			Rc = pFile->Write( pInfo->_pData, __getSummaryInfoSize( pTableInfo->_uType, pTableInfo->_uEvent));
			break;
		}

		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
typedef struct __tagPersist_Weapon_Create
{
	UINT8	_index;
	UINT8	_netIndex;
	INT32	_weaponIdx;
	REAL32	_time;
} PERSIST_REC_WEAPON_CREATE;

typedef struct __tagPersist_Weapon_Transform
{
	UINT8		_index;
	INT32		_vec3DIdx;
	INT32		_quatIdx;
} PERSIST_REC_WEAPON_TRANSFORM;

#pragma pack( pop)
#endif

UINT32 CReplaySystem::_SaveWeapon( i3FileStream * pFile)
{
	
	UINT32 Rc, Result = 0;
	RECORD_SUMMARY_INFO * pInfo;
	RECORD_TABLE_INFO * pTableInfo;

	for( size_t i = 0; i < m_IndexWeapon.size(); i++)
	{
		pInfo = m_IndexWeapon[ i];
				
		I3ASSERT( pInfo->_tableIndex != -1);
		
		pTableInfo = m_RecordTable[ pInfo->_tableIndex];

		switch( pTableInfo->_uEvent)
		{
		case REC_WEAPON_CREATE :
		case REC_WEAPON_DELETE :
		case REC_WEAPON_HIDE :
			{
				PERSIST_REC_WEAPON_CREATE data;
				REC_INFO_WEAPON_CREATE * pWeaponInfo = (REC_INFO_WEAPON_CREATE*) pInfo->_pData;

				data._index = pWeaponInfo->_index;
				data._netIndex = pWeaponInfo->_netIndex;
				data._time = pWeaponInfo->_time;
				data._weaponIdx = _FindFromWeapon( pWeaponInfo->_pWeaponInfo);

				Rc = pFile->Write( &data, sizeof( data));
			}
			break;
		case REC_WEAPON_TRANSFORM :
			{
				PERSIST_REC_WEAPON_TRANSFORM data;
				REC_INFO_WEAPON_TRANSFORM * pWeaponInfo = (REC_INFO_WEAPON_TRANSFORM*) pInfo->_pData;

				data._index = pWeaponInfo->_index;
				data._quatIdx = _FindFromQuat( pWeaponInfo->_pRot);
				data._vec3DIdx = _FindFromVec3D( pWeaponInfo->_pPos);

				Rc = pFile->Write( &data, sizeof( data));
			}
			break;
		default :
			Rc = pFile->Write( pInfo->_pData, __getSummaryInfoSize( pTableInfo->_uType, pTableInfo->_uEvent));
			break;
		}

		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}


#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
typedef struct __tagPersist_Object_Transform
{
	INT32		_index;
	INT32		_vec3DIdx;
	INT32		_quatIdx;
} PERSIST_REC_OBJECT_TRANSFORM;
#pragma pack( pop)
#endif

UINT32 CReplaySystem::_SaveObject( i3FileStream * pFile)
{
	
	UINT32 Rc, Result = 0;
	RECORD_SUMMARY_INFO * pInfo;
	RECORD_TABLE_INFO * pTableInfo;

	for(size_t i = 0; i < m_IndexWeapon.size(); i++)
	{
		pInfo = m_IndexWeapon[ i];
				
		I3ASSERT( pInfo->_tableIndex != -1);
		
		pTableInfo = m_RecordTable[ pInfo->_tableIndex];

		switch( pTableInfo->_uEvent)
		{
		case REC_OBJECT_TRANSFORM :
			{
				PERSIST_REC_OBJECT_TRANSFORM data;
				REC_INFO_OBJECT_TRANSFORM * pObjectInfo = (REC_INFO_OBJECT_TRANSFORM*) pInfo->_pData;

				data._index = pObjectInfo->_index;
				data._quatIdx = _FindFromQuat( pObjectInfo->_pRot);
				data._vec3DIdx = _FindFromVec3D( pObjectInfo->_pPos);

				Rc = pFile->Write( &data, sizeof( data));
			}
			break;
		default :
			Rc = pFile->Write( pInfo->_pData, __getSummaryInfoSize( pTableInfo->_uType, pTableInfo->_uEvent));
			break;
		}

		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}


void CReplaySystem::Create( INT32 nCount)
{
	_MoreCreateBuffer( 64 * nCount);
	_MoreCreateTable( nCount);
}

void CReplaySystem::Destroy( void)
{
	

	m_IndexVEC3D.clear();
	m_IndexQuaternion.clear();
	m_IndexWeaponList.clear();
	m_IndexWeaponInfo.clear();
	m_IndexStage.clear();
	m_IndexChara.clear();
	m_IndexWeapon.clear();
	m_IndexObject.clear();

	I3_SAFE_RELEASE( m_pDataBuffer);

	_DeleteTable();
}

void CReplaySystem::Record_GameEvent( GAME_EVENT gameEvent, INT32 arg)
{
	REC_INFO_GAME_EVENT * pInfo = (REC_INFO_GAME_EVENT*) _GetDataBuffer( sizeof( REC_INFO_GAME_EVENT));

	// write data
	pInfo->_arg = arg;

	_WriteEvent( REC_TYPE_GAMEEVENT, gameEvent, pInfo);

}

void CReplaySystem::Record_Start( void)
{
	REC_INFO_STAGE_START * pInfo = (REC_INFO_STAGE_START*) _GetDataBuffer( sizeof( REC_INFO_STAGE_START));

	// write data
	pInfo->_Time = UserContext::i()->fBattleTime;
	pInfo->_recordIndex = m_iStartorIdx;

	_WriteEvent( REC_TYPE_STAGE, REC_STAGE_START, pInfo);
}

void CReplaySystem::Record_End( void)
{
	REC_INFO_STAGE_END * pInfo = (REC_INFO_STAGE_END*) _GetDataBuffer( sizeof( REC_INFO_STAGE_END));

	// write data
	pInfo->_Time = UserContext::i()->fBattleTime;
	pInfo->_recordIndex = m_iStartorIdx;

	m_iStartorIdx++;

	_WriteEvent( REC_TYPE_STAGE, REC_STAGE_END, pInfo);
}

void CReplaySystem::Record_Stage_RoundStart( void)
{
	REC_INFO_STAGE_ROUNDINFO * pInfo = (REC_INFO_STAGE_ROUNDINFO*) _GetDataBuffer( sizeof( REC_INFO_STAGE_ROUNDINFO));

	// write data
	pInfo->_RoundNum	= ScoreContext::i()->getCurrentRound();
	pInfo->_RoundFlag	= 0;
	pInfo->_PlayingUser = UserContext::i()->PlayingSlotFlag;
	pInfo->_BattleTime	= UserContext::i()->fBattleTime;

	_WriteEvent( REC_TYPE_STAGE, REC_STAGE_ROUNDSTART, pInfo);
}

void CReplaySystem::Record_Stage_RoundEnd( void)
{
	REC_INFO_STAGE_ROUNDINFO * pInfo = (REC_INFO_STAGE_ROUNDINFO*) _GetDataBuffer( sizeof( REC_INFO_STAGE_ROUNDINFO));

	// write data
	pInfo->_RoundNum	= ScoreContext::i()->getCurrentRound();
	pInfo->_RoundFlag	= 1;
	pInfo->_PlayingUser	= UserContext::i()->PlayingSlotFlag;
	pInfo->_BattleTime	= UserContext::i()->fBattleTime;

	_WriteEvent( REC_TYPE_STAGE, REC_STAGE_ROUNDEND, pInfo);

}

void CReplaySystem::Record_Stage_Kill( INT32 killer, DEATH_INFO_CHARA * pDeathInfo, WEAPON_CLASS_TYPE weaponClass, INT32 weaponNum)
{
	REC_INFO_STAGE_KILL * pInfo = (REC_INFO_STAGE_KILL*) _GetDataBuffer( sizeof( REC_INFO_STAGE_KILL));

	// write data
	I3_BOUNDCHK( killer, SLOT_MAX_COUNT);
	I3_BOUNDCHK( weaponClass, 255);
	I3_BOUNDCHK( weaponNum, 255);

	pInfo->_killUser	= (UINT8) killer;
	pInfo->_deathUser	= pDeathInfo->_DeathInfo & DEATH_INFO_BIT_FLAG_INDEX;
	pInfo->_deathType	= (pDeathInfo->_DeathInfo & DEATH_INFO_BIT_FLAG_DEATH_TYPE) >> 4;
	pInfo->_weaponClass	= (UINT8) weaponClass;
	pInfo->_weaponNum	= (UINT8) weaponNum;

	_WriteEvent( REC_TYPE_STAGE, REC_STAGE_KILL, pInfo);
}


//-----------------------------------------------------------------------------------------------------//

void CReplaySystem::Record_Chara_Enter( INT32 userIdx)
{
	INT32 i;

	REC_INFO_CHARA_ENTER * pInfo;

	I3_BOUNDCHK( userIdx, SLOT_MAX_COUNT);

	UINT32 resType = GET_ITEM_FLAG_NUMBER( g_pGameContext->GetUserInfoEquipmentChara()->_noBlue);
	NET_CHARA_INFO * pNetInfo = g_pGameContext->getNetCharaInfo( userIdx);
	USER_INFO_EQUIPMENT_WEAPON * pWeaponInfo = g_pGameContext->GetUserInfoEquipmentWeapon();

	if( 0 == (userIdx % 2))
		resType = GET_ITEM_FLAG_NUMBER( g_pGameContext->GetUserInfoEquipmentChara()->_noRed);
	
	pInfo = (REC_INFO_CHARA_ENTER*) _GetDataBuffer( sizeof( REC_INFO_CHARA_ENTER));

	// write data
	pInfo->_clan = g_pGameContext->GetUserInfoBasic()->_clanMark;
	pInfo->_hair = g_pGameContext->GetUserInfoEquipmentChara()->_nohair;
	pInfo->_index = (UINT8) userIdx;
	pInfo->_respawnIndex = 0;
	pInfo->_resType = resType;
	pInfo->_team = (userIdx % 2);
	
#if defined( NEW_SKILL_SYSTEM)
	for( i = 0; i < MAX_EQUIP_USER_TITLE; i++)
	{
		pInfo->_title[ i] = pNetInfo->_SkillSystem[ i].getIndex();
	}
#endif

	pInfo->_weapon = _GetWeaponListBuffer( pWeaponInfo->_noprim, pWeaponInfo->_nosub, pWeaponInfo->_nomelee, pWeaponInfo->_nothrow, pWeaponInfo->_noitem);

	_WriteEvent( REC_TYPE_CHARA, REC_CHARA_ENTER, pInfo);
}

void CReplaySystem::Record_Chara_Leave( INT32 userIdx)
{
	INT32 i;

	REC_INFO_CHARA_LEAVE * pInfo = (REC_INFO_CHARA_LEAVE*) _GetDataBuffer( sizeof( REC_INFO_CHARA_LEAVE));

	// write data
	I3_BOUNDCHK( userIdx, SLOT_MAX_COUNT);

	pInfo->_clan = g_pGameContext->GetUserInfoBasic()->_clanMark;
	pInfo->_hair = g_pGameContext->GetUserInfoEquipmentChara()->_nohair;
	pInfo->_index = (UINT8) userIdx;
	pInfo->_respawnIndex = 0;
	if( 0 == (userIdx % 2))
		pInfo->_resType = GET_ITEM_FLAG_NUMBER( g_pGameContext->GetUserInfoEquipmentChara()->_noRed) ;
	else
		pInfo->_resType = GET_ITEM_FLAG_NUMBER( g_pGameContext->GetUserInfoEquipmentChara()->_noBlue);
	pInfo->_team = (userIdx % 2);

	NET_CHARA_INFO * pNetInfo = g_pGameContext->getNetCharaInfo( userIdx);
#if defined(NEW_SKILL_SYSTEM)
	for( i = 0; i < MAX_EQUIP_USER_TITLE; i++)
	{
		pInfo->_title[ i] = pNetInfo->_SkillSystem[ i].getIndex();
	}
#endif

	const USER_INFO_EQUIPMENT_WEAPON * pWeaponInfo = g_pGameContext->GetUserInfoEquipmentWeapon();

	pInfo->_weapon = _GetWeaponListBuffer( pWeaponInfo->_noprim, pWeaponInfo->_nosub, pWeaponInfo->_nomelee, pWeaponInfo->_nothrow, pWeaponInfo->_noitem);

	_WriteEvent( REC_TYPE_CHARA, REC_CHARA_LEAVE, pInfo);
}

void CReplaySystem::Record_Chara_Respawn( INT32 userIdx, INT32 respawnIdx, REAL32 invincibleTime)
{
	REC_INFO_CHARA_RESPAWN * pInfo;

	I3_BOUNDCHK( userIdx, 255);
	I3_BOUNDCHK( respawnIdx, 255);

	const USER_INFO_EQUIPMENT_WEAPON * pWeaponInfo = g_pGameContext->GetUserInfoEquipmentWeapon();

	pInfo = (REC_INFO_CHARA_RESPAWN*) _GetDataBuffer( sizeof( REC_INFO_CHARA_RESPAWN));

	// write data
	pInfo->_index = (UINT8) userIdx;
	pInfo->_invincibleTime = invincibleTime;
	pInfo->_RespawnIdx = (UINT8) respawnIdx;

	pInfo->_weapon = _GetWeaponListBuffer( pWeaponInfo->_noprim, pWeaponInfo->_nosub, pWeaponInfo->_nomelee, pWeaponInfo->_nothrow, pWeaponInfo->_noitem);

	_WriteEvent( REC_TYPE_CHARA, REC_CHARA_RESPAWN, pInfo);
}

void CReplaySystem::Record_Chara_Invincible( INT32 userIdx, REAL32 tm)
{
	REC_INFO_CHARA_INVINCIBLE * pInfo = (REC_INFO_CHARA_INVINCIBLE*) _GetDataBuffer( sizeof( REC_INFO_CHARA_INVINCIBLE));

	// write data
	I3_BOUNDCHK( userIdx, 255);

	pInfo->_index = (UINT8) userIdx;
	pInfo->_time = tm;

	_WriteEvent( REC_TYPE_CHARA, REC_CHARA_INVINCIBLE, pInfo);
}

void CReplaySystem::Record_Chara_Death( INT32 userIdx, UINT8 deathType, INT8 deathMotion, VEC3D * pDir)
{
	REC_INFO_CHARA_DEATH * pInfo = (REC_INFO_CHARA_DEATH*) _GetDataBuffer( sizeof( REC_INFO_CHARA_DEATH));

	// write data
	I3_BOUNDCHK( userIdx, 255);

	pInfo->_deathMotion = deathMotion;
	pInfo->_deathType = deathType;
	pInfo->_index = (UINT8) userIdx;

	const USER_INFO_EQUIPMENT_WEAPON * pWeaponInfo = g_pGameContext->GetUserInfoEquipmentWeapon();

	pInfo->_weapon = _GetWeaponListBuffer( pWeaponInfo->_noprim, pWeaponInfo->_nosub, pWeaponInfo->_nomelee, pWeaponInfo->_nothrow, pWeaponInfo->_noitem);

	pInfo->_pDir = _GetVec3DBuffer( pDir);

	_WriteEvent( REC_TYPE_CHARA, REC_CHARA_DEATH, pInfo);
}

void CReplaySystem::Record_Chara_BodyAction( UINT8 userIdx, INT8 currentState, INT8 nextState)
{
	REC_INFO_CHARA_BODY_ACTION * pInfo = (REC_INFO_CHARA_BODY_ACTION*) _GetDataBuffer( sizeof( REC_INFO_CHARA_BODY_ACTION));

	// write data
	pInfo->_index = userIdx;
	pInfo->_currentState = currentState;
	pInfo->_nextState = nextState;

	_WriteEvent( REC_TYPE_CHARA, REC_CHARA_ACTION_BODY, pInfo);
}

void CReplaySystem::Record_Chara_ZoomAction( UINT8 userIdx, INT8 currentState, INT8 nextState)
{
	REC_INFO_CHARA_ZOOM_ACTION * pInfo = (REC_INFO_CHARA_ZOOM_ACTION*) _GetDataBuffer( sizeof( REC_INFO_CHARA_ZOOM_ACTION));

	// write data
	pInfo->_index = userIdx;
	pInfo->_currentState = currentState;
	pInfo->_nextState = nextState;

	_WriteEvent( REC_TYPE_CHARA, REC_CHARA_ACTION_ZOOM, pInfo);
}

void CReplaySystem::Record_Chara_UpperAction( UINT8 userIdx, INT8 currentState, INT8 nextState, REAL32 curSpeed, REAL32 nextSpeed)
{
	REC_INFO_CHARA_UPPER_ACTION * pInfo;

	pInfo = (REC_INFO_CHARA_UPPER_ACTION*) _GetDataBuffer( sizeof( REC_INFO_CHARA_UPPER_ACTION));

	// write data
	pInfo->_index = userIdx;
	pInfo->_currentState = currentState;
	pInfo->_nextState = nextState;
	pInfo->_currentSpeed = curSpeed;
	pInfo->_nextSpeed = nextSpeed;

	_WriteEvent( REC_TYPE_CHARA, REC_CHARA_ACTION_UPPER, pInfo);
}

void CReplaySystem::Record_Chara_LowerAction( UINT8 userIdx, INT8 currentState, INT8 nextState, REAL32 curSpeed, REAL32 nextSpeed)
{
	REC_INFO_CHARA_LOWER_ACTION * pInfo;

	pInfo = (REC_INFO_CHARA_LOWER_ACTION*) _GetDataBuffer( sizeof( REC_INFO_CHARA_LOWER_ACTION));

	// write data
	pInfo->_index = userIdx;
	pInfo->_currentState = currentState;
	pInfo->_nextState = nextState;
	pInfo->_currentSpeed = curSpeed;
	pInfo->_nextSpeed = nextSpeed;

	_WriteEvent( REC_TYPE_CHARA, REC_CHARA_ACTION_LOWER, pInfo);
}

void CReplaySystem::Record_Chara_Position( UINT8 userIdx, VEC3D * pPos, REAL32 theta, REAL32 rho)
{
	REC_INFO_CHARA_POS * pInfo = (REC_INFO_CHARA_POS*) _GetDataBuffer( sizeof( REC_INFO_CHARA_POS));

	// write data
	pInfo->_index = userIdx;
	pInfo->_pPos = _GetVec3DBuffer( pPos);
	pInfo->_theta = theta;
	pInfo->_rho = rho;

	_WriteEvent( REC_TYPE_CHARA, REC_CHARA_POS, pInfo);
}

void CReplaySystem::Record_Chara_DropWeapon( UINT8 userIdx, WeaponBase * pWeapon)
{
	REC_INFO_CHARA_DROPWEAPON * pInfo = (REC_INFO_CHARA_DROPWEAPON*) _GetDataBuffer( sizeof( REC_INFO_CHARA_DROPWEAPON));

	// write data
	pInfo->_index				= userIdx;
	pInfo->_dropIndex			= (UINT8) pWeapon->getNetworkIdx();

	pInfo->_pWeaponInfo = _GetWeaponInfoBuffer( pWeapon);

	_WriteEvent( REC_TYPE_CHARA, REC_CHARA_DROPWEAPON, pInfo);
}

void CReplaySystem::Record_Chara_GetWeapon( UINT8 userIdx, WeaponBase * pWeapon)
{
	REC_INFO_CHARA_GETWEAPON * pInfo = (REC_INFO_CHARA_GETWEAPON*) _GetDataBuffer( sizeof( REC_INFO_CHARA_GETWEAPON));

	// write data
	pInfo->_index				= userIdx;
	pInfo->_dropIndex			= (UINT8) pWeapon->getNetworkIdx();

	pInfo->_pWeaponInfo = _GetWeaponInfoBuffer( pWeapon);

	_WriteEvent( REC_TYPE_CHARA, REC_CHARA_GETWEAPON, pInfo);
}

void CReplaySystem::Record_Chara_Hide( UINT8 userIdx, INT8 motion, REAL32 tm, REAL32 reverseTime)
{
	REC_INFO_CHARA_HIDE * pInfo = (REC_INFO_CHARA_HIDE*) _GetDataBuffer( sizeof( REC_INFO_CHARA_HIDE));

	// write data
	pInfo->_index			= userIdx;
	pInfo->_deathMotion		= motion;
	pInfo->_time			= tm;
	pInfo->_deathIntTime	= reverseTime;

	_WriteEvent( REC_TYPE_CHARA, REC_CHARA_HIDE, pInfo);
}

void CReplaySystem::Record_Chara_Equipment( UINT8 userIdx, INT8 helmet, INT8 extension)
{
	REC_INFO_CHARA_EQUIPMENT * pInfo = (REC_INFO_CHARA_EQUIPMENT*) _GetDataBuffer( sizeof( REC_INFO_CHARA_EQUIPMENT));

	// write data
	pInfo->_index			= userIdx;
	pInfo->_helmetState		= helmet;
	pInfo->_weaponExtension	= extension;

	_WriteEvent( REC_TYPE_CHARA, REC_CHARA_EQUIPMENT, pInfo);
}

void CReplaySystem::Record_Weapon_Create( UINT8 idx, WeaponBase * pWeapon)
{
	REC_INFO_WEAPON_CREATE * pInfo = (REC_INFO_WEAPON_CREATE*) _GetDataBuffer( sizeof( REC_INFO_WEAPON_CREATE));

	// write data
	pInfo->_index				= idx;
	pInfo->_netIndex			= (UINT8) pWeapon->getNetworkIdx();
	pInfo->_pWeaponInfo = _GetWeaponInfoBuffer( pWeapon);
	pInfo->_time				= 0.f;

	_WriteEvent( REC_TYPE_WEAPON, REC_WEAPON_CREATE, pInfo);

}

void CReplaySystem::Record_Weapon_Delete( UINT8 idx, WeaponBase * pWeapon, REAL32 hideTime)
{
	REC_INFO_WEAPON_DELETE * pInfo = (REC_INFO_WEAPON_DELETE*) _GetDataBuffer( sizeof( REC_INFO_WEAPON_DELETE));

	// write data
	pInfo->_index		= idx;
	pInfo->_netIndex	= (UINT8) pWeapon->getNetworkIdx();
	pInfo->_pWeaponInfo = _GetWeaponInfoBuffer( pWeapon);
	pInfo->_time		= hideTime;

	_WriteEvent( REC_TYPE_WEAPON, REC_WEAPON_DELETE, pInfo);

}

void CReplaySystem::Record_Weapon_Transform( UINT8 idx, WeaponBase * pWeapon)
{
	QUATERNION q;

	REC_INFO_WEAPON_TRANSFORM * pInfo = (REC_INFO_WEAPON_TRANSFORM*) _GetDataBuffer( sizeof( REC_INFO_WEAPON_TRANSFORM));

	// write data
	pInfo->_index = idx;
	pInfo->_pPos = _GetVec3DBuffer( pWeapon->GetPos());

	i3Matrix::GetRotateQuat( &q, pWeapon->GetMatrix());
	pInfo->_pRot = _GetQuatBuffer( &q);

	_WriteEvent( REC_TYPE_WEAPON, REC_WEAPON_TRANSFORM, pInfo);

}

void CReplaySystem::Record_Weapon_Hide( UINT8 idx, WeaponBase * pWeapon, REAL32 time)
{
	REC_INFO_WEAPON_HIDE * pInfo = (REC_INFO_WEAPON_HIDE*) _GetDataBuffer( sizeof( REC_INFO_WEAPON_HIDE));

	// write data
	pInfo->_index		= idx;
	pInfo->_pWeaponInfo = _GetWeaponInfoBuffer( pWeapon);
	pInfo->_time		= time;

	_WriteEvent( REC_TYPE_WEAPON, REC_WEAPON_HIDE, pInfo);

}

void CReplaySystem::Record_Object_Init( INT32 idx, i3Object * pObj)
{
	REC_INFO_OBJECT_INIT * pInfo = (REC_INFO_OBJECT_INIT*) _GetDataBuffer( sizeof( REC_INFO_OBJECT_INIT));

	// write data
	pInfo->_index	= idx;
	pInfo->_state	= pObj->getCurState();
	pInfo->_hp		= pObj->getCurrentHP();

	_WriteEvent( REC_TYPE_OBJECT, REC_OBJECT_INIT, pInfo);

}

void CReplaySystem::Record_Object_State( INT32 idx, i3Object * pObj)
{
	REC_INFO_OBJECT_STATE * pInfo = (REC_INFO_OBJECT_STATE*) _GetDataBuffer( sizeof( REC_INFO_OBJECT_STATE));

	// write data
	pInfo->_index			= idx;
	pInfo->_currentState	= pObj->getOldState();
	pInfo->_nextState		= pObj->getCurState();
	pInfo->_hp				= pObj->getCurrentHP();

	_WriteEvent( REC_TYPE_OBJECT, REC_OBJECT_STATE, pInfo);

}

void CReplaySystem::Record_Object_Animation( INT32 idx, i3Object * pObj)
{
	REC_INFO_OBJECT_ANIMATION * pInfo = (REC_INFO_OBJECT_ANIMATION*) _GetDataBuffer( sizeof( REC_INFO_OBJECT_ANIMATION));

	// write data
	pInfo->_index		= idx;
	pInfo->_flag		= 0;

	_WriteEvent( REC_TYPE_OBJECT, REC_OBJECT_ANIMATION, pInfo);

}

void CReplaySystem::Record_Object_Transform( INT32 idx, i3Object * pObj)
{
	QUATERNION q;

	REC_INFO_OBJECT_TRANSFORM * pInfo = (REC_INFO_OBJECT_TRANSFORM*) _GetDataBuffer( sizeof( REC_INFO_OBJECT_TRANSFORM));

	// write data
	pInfo->_index		= idx;
	pInfo->_pPos = _GetVec3DBuffer( pObj->GetPos());

	i3Matrix::GetRotateQuat( &q, pObj->GetMatrix());
	pInfo->_pRot = _GetQuatBuffer( &q);

	_WriteEvent( REC_TYPE_OBJECT, REC_OBJECT_TRANSFORM, pInfo);
}

#if defined( I3_COMPILER_VC)
typedef struct __tagPersist_Replay_TimeTableInfo
{
	REAL32		_rTime;
	REC_TYPE	_uType;
	UINT32		_uEvent;
	INT32		_iDataIdx;
} PERSIST_REPLAY_TIME_TABLE_INFO;
#endif

UINT32 CReplaySystem::Save( const char * pszPath, const char * pszName)
{
	INT32 i, j;
	i3FileStream file;
	UINT32 Rc, Result = 0;

	RECORD_TABLE_INFO * pTableInfo;

	// 최적화 작업
	_SummaryData();

	if( file.Create( pszPath, STREAM_WRITE) == FALSE)
	{
		I3NOTICE( "Can't save Replay.");
	}

	REC_INFO_HEADER head;

	i3::string_ncopy_nullpad( head._szName, pszName, sizeof( head._szName));

	SYSTEMTIME sysTime;
	::GetLocalTime( &sysTime);

	head._date = sysTime.wYear;
    head._date |= ((UINT64)sysTime.wMonth << 16);
    head._date |= ((UINT64)sysTime.wDayOfWeek << 32);
	head._date |= ((UINT64)sysTime.wDay << 48);
	head._time = sysTime.wHour;
	head._time |= ((UINT64)sysTime.wMinute << 16);
	head._time |= ((UINT64)sysTime.wSecond << 32);
	head._time |= ((UINT64)sysTime.wMilliseconds << 48);
	
	head._stageType = m_stageType;
	head._stageMapIndex = m_stageMapIndex;
	head._recordCount = m_iRecordIndex;
	head._recordTime = m_recordTime;

	head._vec2dCount = m_IndexVEC2D.size();
	head._vec3dCount = m_IndexVEC3D.size();
	head._quatCount = m_IndexQuaternion.size();
	head._vertexCount = 0;
	head._weaponListCount = m_IndexWeaponList.size();
	head._weaponInfoCount = m_IndexWeaponInfo.size();
	
	head._stageCount = m_IndexStage.size();
	head._charaCount = m_IndexChara.size();
	head._weaponCount = m_IndexWeapon.size();
	head._objectCount = m_IndexObject.size();

	head._stringCount = 0;

	Rc = file.Write( &head, sizeof( head));
	I3_CHKIO( Rc);
	Result += Rc;

	RECORD_SUMMARY_INFO * pInfo;

	// VEC2D 저장
	Rc = _SaveVec2D( &file);
	I3_CHKIO( Rc);
	Result += Rc;

	// VEC3D 저장
	Rc = _SaveVec3D( &file);
	I3_CHKIO( Rc);
	Result += Rc;
	

	// Quaternion 저장
	Rc = _SaveQuat( &file);
	I3_CHKIO( Rc);
	Result += Rc;

	// 무기 리스트 저장
	Rc = _SaveWeaponList( &file);
	I3_CHKIO( Rc);
	Result += Rc;

	// 무기 정보 저장
	Rc = _SaveWeaponInfo(&file);
	I3_CHKIO( Rc);
	Result += Rc;

	// Stage Event
	Rc = _SaveStage( &file);
	I3_CHKIO( Rc);
	Result += Rc;

	// Character Event
	Rc = _SaveCharacter( &file);
	I3_CHKIO( Rc);
	Result += Rc;

	// Weapon Event
	Rc = _SaveWeapon( &file);
	I3_CHKIO( Rc);
	Result += Rc;

	// Time event
	PERSIST_REPLAY_TIME_TABLE_INFO tableInfo;

	for( i = 0; i < m_iRecordIndex; i++)
	{	
		RECORD_TABLE_INFO * pTableInfo = m_RecordTable[ i];

		tableInfo._iDataIdx = _FindFromData( pTableInfo->_uType, pTableInfo->_pData);
		I3ASSERT( tableInfo._iDataIdx != -1);

		tableInfo._rTime = pTableInfo->_rTime;
		tableInfo._uEvent = pTableInfo->_uEvent;
		tableInfo._uType = pTableInfo->_uType;

		Rc = file.Write( &tableInfo, sizeof( tableInfo));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return 0;
}

UINT32 CReplaySystem::Load( const char * pszPath)
{
	return 0;
}


#endif	// USE_REPLAY_SYSTEM
