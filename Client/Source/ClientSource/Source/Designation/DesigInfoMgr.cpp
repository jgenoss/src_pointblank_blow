#include "pch.h"
#include "DesigInfoMgr.h"
#include "UserDesigInfo.h"
#include "DesigAttr.h"
#include "DesigSet.h"

#include "i3Base/factory/factory.h"

class DesigAttr_None : public DesigAttr
{
public:
	DesigAttr_None(const i3::rc_wstring& wstrID, const i3::rc_wstring& wstrText, REAL32 val) :
	  DesigAttr(wstrID, wstrText, val) {}

	  virtual void	AddAttr( DesigApplyInfo* inout_info) const override { /* None Impl.*/ }
};

DesigInfoMgr::DesigInfoMgr()
{
	m_pMyUserDesigInfo = new CUserDesigInfo;
	m_pOtherUserDesigInfo = new CUserDesigInfo;
	m_pTempUserDesigInfo = new TempUserDesigInfo;
	Reset_TempUserDesigInfo(*m_pTempUserDesigInfo);
}

DesigInfoMgr::~DesigInfoMgr()
{
	delete m_pTempUserDesigInfo;
	delete m_pOtherUserDesigInfo;
	delete m_pMyUserDesigInfo;

	i3::cu::for_each_delete(m_vecDesigAttr);
	i3::cu::for_each_delete(m_vecDesigSetByUI);
}

bool	DesigInfoMgr::LoadRegistryFile(const i3::rc_string& strFileName)
{
	// 추후 Clear는 여기에.

	i3RegistrySet regFile;
	INT32 nReadBytes = regFile.LoadRegistrySet(strFileName.c_str(), REGISTRY_FILE_BINARY);
	if (nReadBytes == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_FATAL,  "Failed to load %s", strFileName );		
		return false;
	}
	
	i3RegKey * pRootRegKey = regFile.getRoot();
	
	i3::vector<i3::rc_wstring>	vecStrID;
	i3::vector<i3::rc_wstring>  vecStrText;
	i3::vector<REAL32>			vecValue;
	i3::vector<DesigAttr*>		vecDesigAttr;

	// 1. ID 스트링 획득..
	{
		i3RegKey* pKey = static_cast<i3RegKey*>(pRootRegKey->FindChildByName("DesignationID"));

		if (pKey == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Can't Find i3RegKey DesignationID");		
			return false;
		}
	
		const INT32 numID = pKey->GetDataCount();
		
		for (INT32 i = 0 ; i < numID ; ++i )
		{
			i3RegData* data = pKey->GetData(i);
			if ( data->getDataType() == I3REG_TYPE_STRING )
			{
				const i3::rc_string& strID = data->GetNameString();
				int nID = i3::atoi(strID);
				const i3::rc_wstring& wstrID = static_cast<i3RegString*>(data)->getValue();

				i3::vu::set_value_force(vecStrID, nID, wstrID);
			}
		}
			
	}

	// 2. GAME_STRING 획득
	{
		i3RegKey* pKey     = static_cast<i3RegKey*>(pRootRegKey->FindChildByName("DesignationIDText"));

		if (pKey == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Can't Find i3RegKey DesignationIDText");		
			return false;
		}

		const INT32 numID = pKey->GetDataCount();

		for (INT32 i = 0 ; i < numID ; ++i )
		{
			i3RegData* data = pKey->GetData(i);
			if ( data->getDataType() == I3REG_TYPE_STRING )
			{
				const i3::rc_string& strID = data->GetNameString();
				int nID = i3::atoi(strID);
				
				const i3::rc_wstring& wstrText = static_cast<i3RegString*>(data)->getValue();
				i3::string strText;		i3::utf16_to_utf8(wstrText, strText);
				
				i3::vu::set_value_force(vecStrText, nID, GAME_RCSTRING(strText) );
			}
		}
	}
	

	// 3. Meta에 REAL32 속성 값 배정..
	{
		
		i3RegKey* pKey     = static_cast<i3RegKey*>(pRootRegKey->FindChildByName("DesignationIDValue"));
		if (pKey == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Can't Find i3RegKey DesignationIDValue");		
			return false;
		}

		const INT32 numID = pKey->GetDataCount();
		size_t locationIndex = i3Registry::Get_LocationIndex();

		for (INT32 i = 0 ; i < numID ; ++i )
		{
			i3RegData* data = pKey->GetData(i);
			if ( data->getDataType() == I3REG_TYPE_ARRAY )		// 배열인 경우 먼저 배열조건을 따져야한다.
			{
				i3RegArray* regArray = static_cast<i3RegArray*>(data);
				size_t num_array = regArray->getSize();

				if ( regArray->getElementType() == I3REG_TYPE_REAL32 && ( num_array == 1 || locationIndex < num_array ) )
				{
					const i3::rc_string& strID = data->GetNameString();
					int nID = i3::atoi(strID);

					REAL32 fVal = 0.f;

					if (num_array == 1)
						(*regArray)[0]->GetValue(&fVal);
					else
						(*regArray)[locationIndex]->GetValue(&fVal);
					
					i3::vu::set_value_force(vecValue, nID, fVal);
				}
			}
		}
	}

	// 4. 메타 클래스 생성
	{

		i3RegKey* pKey     = static_cast<i3RegKey*>(pRootRegKey->FindChildByName("DesignationIDMeta"));
		if (pKey == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Can't Find i3RegKey DesignationIDMeta");		
			return false;
		}

		DesigAttrFactory	DesigFactory;
		RegisterDesigAttrs(DesigFactory);

		const INT32 numID = pKey->GetDataCount();

		for (INT32 i = 0 ; i < numID ; ++i )
		{
			i3RegData* data = pKey->GetData(i);
			if ( data->getDataType() == I3REG_TYPE_STRING )
			{
				const i3::rc_string& strID = data->GetNameString();
				int nID = i3::atoi(strID);

				const i3::rc_wstring& wstrMeta = static_cast<i3RegString*>(data)->getValue();

				i3::rc_wstring wstrID = (nID < int(vecStrID.size())) ? vecStrID[nID] : i3::rc_wstring();
				i3::rc_wstring wstrText = (nID <  int(vecStrText.size())) ? vecStrText[nID] : i3::rc_wstring();
				REAL32		   fVal = (nID <  int(vecValue.size())) ? vecValue[nID] : 0.f;

				if (!i3::vu::is_value_valid(vecDesigAttr, nID))
				{
					DesigAttr* new_attr = DesigFactory.create_factory_object(wstrMeta, wstrID, wstrText, fVal);

					if (new_attr == nullptr)
						new_attr = new DesigAttr_None(wstrID, wstrText, fVal);

					i3::vu::set_value_force(vecDesigAttr, nID, new_attr);
				}
				else
				{
					I3TRACE("DesigAttr is collided. strID = %s nID = %d strText = %s.\n", wstrID, nID, wstrText);
				}
			}
		}
		
		size_t maxSize = vecStrID.size();
		if (vecStrText.size() > maxSize) maxSize = vecStrText.size();
		if (vecValue.size() > maxSize) maxSize = vecValue.size();
		if (vecDesigAttr.size() > maxSize) maxSize = vecDesigAttr.size();
		
		vecDesigAttr.resize(maxSize);

		// 비어있는 것은 None클래스로 채운다..

		for (size_t i = 0 ; i < maxSize ; ++i)
		{	
			if ( vecDesigAttr[i] == nullptr)
			{
				i3::rc_wstring wstrID = (i < vecStrID.size()) ? vecStrID[i] : i3::rc_wstring();
				i3::rc_wstring wstrText = (i <  vecStrText.size()) ? vecStrText[i] : i3::rc_wstring();
				REAL32		   fVal =(i <  vecValue.size()) ? vecValue[i] : 0.f ;
				vecDesigAttr[i] = new DesigAttr_None(wstrID, wstrText, fVal);
			}
		}
		
		m_vecDesigAttr.assign( vecDesigAttr.begin(), vecDesigAttr.end());
	}
		
	// 5. DesignationSet 처리..
	{
		i3RegKey* pKey     = static_cast<i3RegKey*>(pRootRegKey->FindChildByName("DesignationSet"));
		
		if (pKey == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Can't Find i3RegKey DesignationSet");		
			return false;
		}
		
		const INT32 numSet = pKey->getChildCount();

		for (INT32 i = 0 ; i < numSet ; ++i)
		{
			i3RegKey* pSetKey = static_cast<i3RegKey*>(pKey->getChild(i));
			
			const i3::rc_string& keyName = pSetKey->GetNameString();

			if ( i3::contain_string_head(keyName, "Set_") )
			{
				INT32 value;
				i3::rc_string strID = keyName.substr(4);		// "Set_" 
				INT32 readID = i3::atoi(strID);
				I3ASSERT(readID > 0);					// 1 이상이 되며 1을 빼서 0부터 인덱스를 세준다.
				INT32 iSetID = readID - 1;
								
				DesigSet* pNewSet = new DesigSet;
				InitDesigSet(pNewSet);
				
				if (i3::vu::is_value_valid(m_vecDesigSetByUI, iSetID) == true)
				{
					DesigSet* pAlreadySet = m_vecDesigSetByUI[iSetID];
					I3PRINTLOG(I3LOG_FATAL, "%s : Already Set exists (from Set_%d)", keyName, pAlreadySet->desigSetUIIndex+1);
					delete pAlreadySet;
					m_vecDesigSetByUI[iSetID] = nullptr;
				}
				
				i3::vu::set_value_force(m_vecDesigSetByUI, iSetID, pNewSet);
				pNewSet->desigSetUIIndex = iSetID;

				GetKeyData(pSetKey, "_ApplyType", &value);
				I3_BOUNDCHK( value , DESIGNATION_APPLY_TYPE_COUNT);
				pNewSet->applyType = DESIGNATION_APPLY_TYPE(value);
				
				value = -1;
				GetKeyData(pSetKey, "_Type", &value);
				
				if (value == -1)
				{
					I3PRINTLOG(I3LOG_FATAL, "%s : _Type attr doesn't exist or written incorrectly.", keyName);
				}
				else if (i3::vu::is_value_valid(m_vecDesigSetByType, value) == true)
				{
					DesigSet* pAlreadySet =	m_vecDesigSetByType[value];
					I3PRINTLOG(I3LOG_FATAL, "%s : Already Set DesignationSetType type:%d (from Set_%d)", keyName, value, pAlreadySet->desigSetUIIndex+1);
					m_vecDesigSetByType[value] = nullptr;
				}

				if (value > -1)
					i3::vu::set_value_force(m_vecDesigSetByType, value, pNewSet);	

				pNewSet->desigSetType = value;
				
				for( INT32 j = 0; j < DESIG_EFFECT_COUNT; j++)
				{
					char szTableName[MAX_STRING_COUNT];
					i3::snprintf( szTableName, MAX_STRING_COUNT, "_PlusEffect_%d", j);
					GetKeyData(pSetKey, szTableName, &value);
					pNewSet->plusEffect[j] = UINT8(value);

					i3::snprintf( szTableName, MAX_STRING_COUNT, "_MinusEffect_%d", j);
					GetKeyData(pSetKey, szTableName, &value);
					pNewSet->minusEffect[j] = UINT8(value);

					i3::snprintf( szTableName, MAX_STRING_COUNT, "_AddEffect_%d", j);
					GetKeyData(pSetKey, szTableName, &value);
					pNewSet->addEffect[j] = UINT8(value);

//					value = 0;
//					i3::snprintf( szTableName, MAX_STRING_COUNT, "_AuthWeaponClass_%d", j);
//					GetKeyData(pSetKey, szTableName, &value);
//					pNewSet->authWeaponClass[j] = UINT8(value);

//					value = 0;
//					i3::snprintf( szTableName, MAX_STRING_COUNT, "_AuthWeaponNumber_%d", j);
//					GetKeyData(pSetKey, szTableName, &value);
//					pNewSet->authWeaponNumber[j] = UINT8(value);
				}
			}
		
		}
		
		// 중간에 빈 것이 있는지 체크한다.. 비어있는 것의 경우 널값 검사를 피하도록 그냥 빈 객체포인터라도 넣어준다..
		const size_t num_vecDesigSet = m_vecDesigSetByUI.size();
		for (size_t i = 0 ; i < num_vecDesigSet ; ++i)
		{
			if (m_vecDesigSetByUI[i] == nullptr)
			{
				I3PRINTLOG(I3LOG_FATAL, "Set_%d doesn't exist...!!", i + 1);		
				DesigSet* pNewSet = new DesigSet;
				InitDesigSet(pNewSet);
				pNewSet->desigSetUIIndex = i;
				pNewSet->desigSetType = -1;
				m_vecDesigSetByUI[i] = pNewSet;
			}
		}

		// SetByType의 경우 원소가 없는 경우에 동적할당은 하지 않고 널체크에 의존하도록 한다..
		const size_t num_vecDesitSetByType = m_vecDesigSetByType.size();
		for (size_t i = 1; i < num_vecDesitSetByType ; ++i)
		{
			if (m_vecDesigSetByType[i] == nullptr)
			{
				I3PRINTLOG(I3LOG_FATAL, "DesigSetType %d doesn't exist...!!!!", i);
			}
		}
	}
	
	
	// 6. DesignationSetCommon 처리..
	{
		i3RegKey* pKey     = static_cast<i3RegKey*>(pRootRegKey->FindChildByName("DesignationSetCommon"));

		if (pKey == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Can't Find i3RegKey DesignationSetCommon");		
			return false;
		}

		const INT32 numSet = pKey->getChildCount();
		i3::rc_wstring	wstrData;
		i3::string		strData;

		for (INT32 i = 0 ; i < numSet ; ++i)
		{
			i3RegKey* pSetKey = static_cast<i3RegKey*>(pKey->getChild(i));

			const i3::rc_string& keyName = pSetKey->GetNameString();

			if ( i3::contain_string_head(keyName, "Set_") )
			{
				i3::rc_string strID = keyName.substr(4);		// "Set_" 
				INT32 readID = i3::atoi(strID);
				I3ASSERT(readID > 0);
				INT32 iSetID = readID - 1;

				if (i3::vu::is_value_valid(m_vecDesigSetByUI, iSetID) == false)
				{
					I3PRINTLOG(I3LOG_FATAL, "%s : doesn't exist (from DesignationSet)", keyName );
					continue;
				}
				
				DesigSet* pSet = m_vecDesigSetByUI[iSetID];
				
				GetKeyData(pSetKey, "Explain", wstrData);
				i3::utf16_to_utf8(wstrData, strData);
				pSet->wstrExplain = GAME_RCSTRING(strData);

				GetKeyData(pSetKey, "_Name", wstrData);
				i3::utf16_to_utf8(wstrData, strData);
				pSet->wstrName = GAME_RCSTRING(strData);

				GetKeyData(pSetKey, "_NameExt", wstrData);
				i3::utf16_to_utf8(wstrData, strData);
				pSet->wstrNameE = GAME_RCSTRING(strData);
			}
		}
	}
	
	return true;
}

void	DesigInfoMgr::ClearDesigItemInfosFromServer()
{
	m_map_ItemID_DesigType.clear();
	m_map_DesigType_ItemID.clear();
}

void	DesigInfoMgr::UpdateDesigItemInfosFromServer(INT32 begShopItemIdx, INT32 currShopItemCount)	// 서버로부터 상점 아이템 정보 패킷받고 Shop갱신 직후에 넣어줌..
{
//	m_map_ItemID_DesigType.clear();

	// 깔끔하게 호칭 목록을 지우고 시작합니다.
	// 두 번 리스트를 받으면 이미 받은 데이터들을 삭제합니다..
	// 이게 제가 잘못했어요...(jinsik.kim)
	if(begShopItemIdx == 0)
		ClearDesigItemInfosFromServer();

	CShop* pShop = CShop::i();

	SHOP_ITEM_DATA*		pItem		= pShop->GetItemAllData();
	
	const INT32 numTotItemCount = (INT32)pShop->GetItemCount();

	I3_BOUNDCHK( begShopItemIdx, numTotItemCount);
	I3_BOUNDCHK( begShopItemIdx + currShopItemCount, numTotItemCount + 1);
	
	const INT32 endItemIdx = begShopItemIdx + currShopItemCount;

	for( INT32 i= begShopItemIdx; i< endItemIdx; ++i)	
	{
		I3ASSERT( i < MAX_SHOP_ITEM_COUNT);

		SHOP_ITEM_DATA*		pItemData = &pItem[i];

		//	호칭 아이템 enum값이 갱신되었으므로 TYPE_ITEM_RESTRICTION_TITLE인 것만을 호칭아이템으로 간주한다..

		if ( pItemData->_ui8RestrictionType == TYPE_ITEM_RESTRICTION_TITLE )
		{
			if(pItemData->_ui8Restriction > 0)
			{
				ItemID_DesigType_maptype::insert_commit_data	commit_data1;

				i3::pair<ItemID_DesigType_maptype::iterator, bool> pr1 = m_map_ItemID_DesigType.insert_check(pItemData->_ui32ItemID, commit_data1);

				if (pr1.second == true)	
					pr1.first = m_map_ItemID_DesigType.insert_commit( i3::make_pair(pItemData->_ui32ItemID, i3::vector<UINT8>()), commit_data1);

				i3::vector<UINT8>& vecDesig = pr1.first->second;
				vecDesig.push_back(pItemData->_ui8Restriction);
				
				DesigType_ItemID_maptype::insert_commit_data	commit_data2;
				
				i3::pair<DesigType_ItemID_maptype::iterator, bool> pr2 = m_map_DesigType_ItemID.insert_check(pItemData->_ui8Restriction, commit_data2);

				if (pr2.second == true)	
					pr2.first = m_map_DesigType_ItemID.insert_commit( i3::make_pair(pItemData->_ui8Restriction, i3::vector<UINT32>()), commit_data2);

				i3::vector<UINT32>& vecItemID = pr2.first->second;
				vecItemID.push_back(pItemData->_ui32ItemID);								
			}
		}
	}	
}

const i3::vector<UINT32>*	DesigInfoMgr::FindItemIDsFromDesigType( UINT8 desigSetType) const
{
	DesigType_ItemID_maptype::const_iterator it = m_map_DesigType_ItemID.find(desigSetType);

	return ( it != m_map_DesigType_ItemID.end() ) ? &it->second : nullptr;
}

const i3::vector<UINT8>*	DesigInfoMgr::FindDesigTypesFromItemID( UINT32 itemID) const
{
	ItemID_DesigType_maptype::const_iterator it = m_map_ItemID_DesigType.find(itemID);
	return (it != m_map_ItemID_DesigType.end() ) ? &it->second : nullptr;
}

