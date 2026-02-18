#include "pch.h"
#include "WeaponLRU_Manager.h"
#include "SimpleMemObject.h"
#include "i3Base/string/compare/generic_compare.h"

#define USE_WEAPON_BGLOAD 1			// BGLoad 사용
#define USE_ALLWAYS_BGLOAD 0		// 모든 무기에 대해 항시 BGload 하도록 사용(off 하면 Limit로 동작)

namespace 
{
	void RecSetSoundCommonFlag(tConSharedRes & shared_res_map, i3GameRes * pCharaRes, i3GameNode * pNode, bool bState);
	bool IsSubCommonSound( tConSharedRes & shared_res_map, i3GameRes * pCharaRes, i3GameResSound * pSoundRes, bool bState );

	void RecSetSoundCommonFlag(tConSharedRes & shared_res_map, i3GameRes * pCharaRes, i3GameNode * pNode, bool bState)
	{
		if (pNode == nullptr)
		{
			I3ASSERT( pNode != nullptr);
			return;
		}

		if( i3::same_of<i3AIContext* >(pNode))
		{
			i3AI * pAI = static_cast<i3AIContext*>(pNode)->getAI();

			if( pAI != nullptr )
			{
				INT32 ai_cnt = pAI->getAIStateCount();

				for( INT32 ai_idx = 0; ai_idx < ai_cnt; ++ai_idx )
				{
					i3AIState * pState = pAI->getAIState(ai_idx);

					if( pState->getTimeSeq() == nullptr )
						continue;

					INT32 seq_cnt = pState->getTimeSeq()->GetSequenceCount();

					for( INT32 seq_idx = 0; seq_idx < seq_cnt; ++seq_idx )
					{
						i3TimeSequenceInfo * pSeqInfo = pState->getTimeSeq()->GetSequence(seq_idx);

						if( !i3::same_of<i3SoundNode*>(pSeqInfo->GetNode()) )
							continue;

						i3SoundNode * pSoundNode = static_cast<i3SoundNode*>( pSeqInfo->GetNode() );

						if( pSoundNode->getSoundPlayInfo() == nullptr)
							continue;

						if( pSoundNode->hasName() )
						{
							i3ResourceManager * pResMgr = g_pFramework->GetResourceManager();
							i3GameResSound * pRes = static_cast<i3GameResSound*>( pResMgr->FindResourceByOnlyName(pSoundNode->GetName()) );
							// I3ASSERT( pRes != nullptr);

							if( pRes != nullptr )
							{
								bState = IsSubCommonSound(shared_res_map, pCharaRes, pRes, bState);
								pRes->setSubCommonRes( bState );
							}
							else
							{
								I3PRINTLOG(I3LOG_NOTICE, "LRU Sound Resource nullptr Name : %s", pSoundNode->GetName());
							}
						}
						else
						{
							I3TRACE( "not set Sound Name in CHR file!!!");
						}
					}
				}
			}
		}

		i3GameNode * pChild = pNode->getFirstChild();

		while( pChild != nullptr)
		{
			RecSetSoundCommonFlag( shared_res_map, pCharaRes, pChild, bState);
			pChild = pChild->getNext();
		}
	}

	bool IsSubCommonSound( tConSharedRes & shared_res_map, i3GameRes * pCharaRes, i3GameResSound * pSoundRes, bool bState )
	{
		tConSharedRes::iterator map_iter = shared_res_map.find(pSoundRes);

		if( map_iter != shared_res_map.end() )
		{
			i3::vector<tSharedResPair>& res_list = map_iter->second;
			bool old_state = bState;

			// set common flag
			struct is_common_res
			{
				bool operator()(tSharedResPair  pRes) const
				{
					if (pRes.first != nullptr)
						return pRes.second;
					return false;
				}
			};

			if( std::count_if( res_list.begin(), res_list.end(), is_common_res() ) > 0 )
				bState = true;

			// set resource list 
			struct find_same_res
			{
				i3GameRes * _pRes;
				find_same_res(i3GameRes * pRes) : _pRes(pRes) {}

				bool operator()(const tSharedResPair pRes) const
				{
					return _pRes == pRes.first;
				}
			};

			i3::vector<tSharedResPair>::iterator res_iter = std::find_if(res_list.begin(), res_list.end(), find_same_res(pCharaRes) );

			if( res_iter != res_list.end() )
			{
				if(old_state == false)
					res_list.erase( res_iter );
			}
			else
			{
				if (bState == true)
				{
					tSharedResPair tResList;
					tResList.second = pCharaRes->isCommonRes();
					tResList.first = pCharaRes;
					res_list.push_back(tResList);
				}
			}

			if( res_list.size() == 0 )
				shared_res_map.erase(map_iter);
		}
		else
		{
			if( bState == true )
			{
				i3::vector<tSharedResPair> tInsertRes;
				tSharedResPair tResList;
				tResList.second = pCharaRes->isCommonRes();
				tResList.first = pCharaRes;

				tInsertRes.push_back(tResList);
				shared_res_map.insert( tConSharedRes::value_type(pSoundRes, tInsertRes) );
			}
		}

		return bState;
	}
};

WeaponLRU_Manager::WeaponLRU_Manager()
{
	m_TempAddList.reserve( 1024);
	m_ItemList.reserve( 64);
}

WeaponLRU_Manager::~WeaponLRU_Manager()
{
}

bool WeaponLRU_Manager::saveData()
{
	i3FileStream file;

	BackingDataFromAddList();

	UINT32	fileCount = m_PreLoadItemMap.size();
	UINT32	currentdate = 0;

	CString	strBuffer;
	CString	strTemp;

	if(fileCount <= 0)
		return false;
	getCurretDateToValue(&currentdate);

	if(file.Open("V3PreCommon.pbc", STREAM_WRITE) == false)
	{
		if( file.Create( "V3PreCommon.pbc", STREAM_WRITE) == false)
			return false;
	}

	UINT32 i=0;
	i3::unordered_map<T_ItemID, LRUData>::iterator it = m_PreLoadItemMap.begin();

	while( it != m_PreLoadItemMap.end() )
	{
		LRUData& pData = it->second;
		if(pData._dayScore > 0)
			i++;
		it++;
	}

	strTemp.Format("[Info]\n");
	strBuffer += strTemp;
	strTemp.Format("totalCount = %d\n", i);
	strBuffer += strTemp;
	strTemp.Format("lastRecordDate = %d\n", currentdate);
	strBuffer += strTemp;

	it = m_PreLoadItemMap.begin();

	i = 0;
	while( it != m_PreLoadItemMap.end() )
	{
		LRUData& pData = it->second;
		if(pData._dayScore > 0)
		{
			strTemp.Format("[Item%d]\n", i++);
			strBuffer += strTemp;
			strTemp.Format("itemID = %d\n", pData._ItemID);
			strBuffer += strTemp;
			strTemp.Format("count = %d\n", pData._count);
			strBuffer += strTemp;
			strTemp.Format("over = %d\n", pData._dayScore);
			strBuffer += strTemp;
		}

		it++;
	}

	INT32	TotalSize		= strBuffer.GetLength() + 1;
	UINT8 * pFileInfoBuffer = nullptr;
	pFileInfoBuffer = (UINT8*)malloc(TotalSize);
	if ( pFileInfoBuffer == nullptr ) return false;
	memcpy(pFileInfoBuffer, strBuffer.GetBuffer(), TotalSize);

	BitRotateEncript(pFileInfoBuffer, TotalSize, 7);

	file.Write(pFileInfoBuffer, TotalSize);
	file.Close();

	free(pFileInfoBuffer);

	return true;
}

bool WeaponLRU_Manager::loadData()
{
	i3IniParser Parser;
	i3FileStream file;
	UINT32 itemCount	= 0;
	UINT32 lastdate		= 0;
	UINT32 currentdate	= 0;
	UINT32 dayOver		= 0;

	char label[MAX_STRING_COUNT] = {0,};

 	if(file.Open("V3PreCommon.pbc", STREAM_READ) == false)
	{
		return false;
	}

	const UINT32 size = file.GetSize();
	char * pCh = new char[size];
	MEMDUMP_NEW( pCh, sizeof(char) * size);
	CSimpleMemObject<char> buff ( pCh, true );
	i3MemStream strm;

	file.Read( buff, size);
	BitRotateDecript( buff, size, 7);

	strm.Open( buff, STREAM_READ | STREAM_SHAREREAD, size);
	if (false == Parser.Open(&strm))
		return false;

	if(Parser.ReadSection("Info") == false)
		return false;
	Parser.GetValue("totalCount", &itemCount);
	Parser.GetValue("lastRecordDate", &lastdate);
	getCurretDateToValue(&currentdate);
	dayOver = currentdate - lastdate;

	LRUData _tData;
	_tData._ItemID = 0;
	_tData._count = 0;
	_tData._dayScore = 0;

	for(UINT32 i=0; i<itemCount; i++)
	{
		sprintf_s(label, "Item%d", i);
		if(Parser.ReadSection(label) == false)
			continue;
		Parser.GetValue("itemID", _tData._ItemID, &_tData._ItemID);
		Parser.GetValue("count",_tData._count, &_tData._count);
		Parser.GetValue("over",_tData._dayScore, &_tData._dayScore);
		
		if(_tData._dayScore < dayOver)
			_tData._dayScore = 0;
		else
			_tData._dayScore -= dayOver;

		m_PreLoadItemMap.insert(i3::unordered_map<T_ItemID, LRUData>::value_type(_tData._ItemID, _tData));
	}

	SortWeaponByPriority();

	return true;
}

void WeaponLRU_Manager::Init()
{
}

void WeaponLRU_Manager::addCount( T_ItemID _ID )
{
	m_TempAddList.push_back( _ID);
}

void WeaponLRU_Manager::BackingDataFromAddList( void)
{
	UINT32 cnt = m_TempAddList.size();
	for( UINT32 i = 0; i < cnt; i++)
	{
		T_ItemID itemID = m_TempAddList.at(i);

		i3::unordered_map<T_ItemID, LRUData>::iterator it = m_PreLoadItemMap.find( itemID);

		if ( it != m_PreLoadItemMap.end() )
		{
			LRUData& Data = it->second;
			Data._count++;
			Data._dayScore = UNUSED_LRU_DELETE;
		}
		else
		{
			LRUData Data;
			Data._ItemID = itemID;
			Data._count = 1;
			Data._dayScore = UNUSED_LRU_DELETE;
			m_PreLoadItemMap.insert(i3::unordered_map<T_ItemID, LRUData>::value_type(Data._ItemID, Data));
		}
	}
	m_TempAddList.clear();
}

void WeaponLRU_Manager::SortWeaponByPriority( void)
{
	m_ItemList.clear();

	i3::unordered_map<T_ItemID, LRUData>::iterator it = m_PreLoadItemMap.begin();
	while( it != m_PreLoadItemMap.end())
	{
		i3GameResChara * pRes = g_pWeaponManager->QueryResource( it->first, false);
		if( pRes == nullptr || !pRes->isResStyle(I3_RES_STYLE_COMMON))
			m_ItemList.push_back( it->second);
		
		it++;
	}
	sort(m_ItemList.begin(), m_ItemList.end(), fn_sort_LRU());
}

void WeaponLRU_Manager::getCurretDateToValue( UINT32 * day )
{
	SYSTEMTIME time;
	::GetSystemTime(&time);
	*day = (time.wYear * 365) + time.wDay;
}

bool WeaponLRU_Manager::cbFindFilePath( char *szWeaponName, bool isCommon)
{
	if(isCommon == true)
		return false;

	CWeaponInfo * pWeaponInfo = nullptr;
	size_t count = m_ItemList.size();
	char szPath[256] = {0,};

	i3::generic_string_copy(szPath,szWeaponName);
	PathStripPath(szPath);
	PathRemoveExtension(szPath);
	
	if(count > MAX_LRU_LOADING_COUNT)
		count = MAX_LRU_LOADING_COUNT;
	for(size_t i=0; i<count; i++)
	{
		pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( m_ItemList[i]._ItemID );
		if(pWeaponInfo != nullptr && i3::generic_compare( pWeaponInfo->GetName(), szPath) == 0)
			return true;

	}
	return false;
}

void WeaponLRU_Manager::PreLoad( void)
{
	UINT32 cnt = m_ItemList.size();
	if( cnt > MAX_LRU_LOADING_COUNT)
		cnt = MAX_LRU_LOADING_COUNT;
	for( UINT32 i=0; i<cnt; i++)
	{
		i3GameResChara * pRes = g_pWeaponManager->QueryResource( m_ItemList[i]._ItemID);
		if( pRes != nullptr)
		{
			pRes->setSubCommonRes( true);
			//I3ASSERT( pRes->getChara() != nullptr);

			if ( pRes->getChara() != nullptr )
			{
				RecSetSoundCommonFlag(m_CommonResList, pRes, pRes->getChara(), true);
			}
		}
	}
}

void WeaponLRU_Manager::AfterLoad( void)
{
	UINT32 cnt = m_ItemList.size();

	//I3TRACE( "----- weapon LRU system (%d) -----\n", cnt);
	
	for( UINT32 i=0; i<cnt; i++)
	{
		bool bCommon = false;
		if( i < WEAPON_PRIORITY_COUNT && m_ItemList[i]._count > WEAPON_PRIORITY_RESPAWN_COUNT)
		{
			bCommon = true;
		}
		
		i3GameResChara * pRes = g_pWeaponManager->QueryResource( m_ItemList[i]._ItemID, false);
		if( pRes != nullptr )
		{	
			pRes->setSubCommonRes( bCommon);
//			I3ASSERT( pRes->getChara() != nullptr);

			if ( pRes->getChara() != nullptr )
			{
				RecSetSoundCommonFlag(m_CommonResList, pRes, pRes->getChara(), bCommon);
			}
			//I3TRACE( "(%d) weapon disable common\n", m_ItemList[i]._ItemID);
		}
		
	}
	//I3TRACE( "----- ---------------- -----\n");
}

bool WeaponLRU_Manager::IsPriorityWeapon( T_ItemID itemID)
{
#if defined( USE_WEAPON_BGLOAD)
	#if( USE_ALLWAYS_BGLOAD)
		return true;
	#else
		UINT32 cnt = m_ItemList.size();

		if( cnt <= MAX_LRU_LOADING_COUNT)
			return true;
		
		if( cnt > WEAPON_PRIORITY_COUNT)
			cnt = WEAPON_PRIORITY_COUNT;

		for( UINT32 i = MAX_LRU_LOADING_COUNT; i < cnt; i++)
		{
			if( m_ItemList[i]._ItemID == itemID)
				return true;
		}

		return false;
	#endif
#else
	return false;
#endif
}