#include "pch.h"
#include "EquipData.h"

#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/compare/generic_compare.h"

#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/mb_to_utf16.h"

#define CSZ_EQUIPMENT	("Equipment")
#define CSZ_HEADGEAR	("Head")
#define CSZ_FACEGEAR	("Facegear")
#define CSZ_BELT		("Belt")
#define CSZ_HOLSTER		("Holster")
#define CSZ_TRANSFORM	("_Transform")
#define CSZ_HEADTYPE	("HeadType")
#define CSZ_FACESHOW	("FacegearShow")

#define CSZ_EQUIPMENT_PEF		("Equipment.pef")
#define CSZ_EQUIPMENT_I3REGXML	("Equipment.i3RegXML")

#define CSZ_MAKE_TIME		("__Make Time")
#define CSZ_UPDATE_TIME		("__Update Time")

CEquipData::CEquipData()
{
	m_pRegSet = NULL;
}

CEquipData::~CEquipData()
{
	I3_SAFE_RELEASE( m_pRegSet );
}
bool CEquipData::Load( bool bI3RegXml )
{
	if( bI3RegXml )
		return Load( CSZ_EQUIPMENT_I3REGXML );
	else
		return Load( CSZ_EQUIPMENT_PEF );
}

bool CEquipData::Save( bool bI3RegXml )
{
	if( bI3RegXml )
		return Save( CSZ_EQUIPMENT_I3REGXML );
	else
		return Save( CSZ_EQUIPMENT_PEF );
}

bool CEquipData::Load( const char* pszFile )
{
	if( i3::generic_string_size( i3ResourceFile::GetWorkingFolder()) <= 0 )
		return false;
	else
	{
		::BOOL bDir = SetCurrentDirectory(i3ResourceFile::GetWorkingFolder());
		if (FALSE == bDir)
		{
			i3ResourceFile::SetWorkingFolder("");
			return false;
		}
	}
	Init();

	char szFullPath[MAX_PATH];

	i3::snprintf( szFullPath, sizeof( szFullPath ), "%s/config/Nation.nlf", i3ResourceFile::GetWorkingFolder() );
	__LoadNationFile( szFullPath);

	i3::snprintf( szFullPath, sizeof( szFullPath ), "%s/config/%s", i3ResourceFile::GetWorkingFolder(), pszFile );

	char	szExt[32] = {};
	i3::extract_fileext( pszFile, szExt);
	REGISTRY_FILE_TYPE	RegFileType	= REGISTRY_FILE_UNKNOWN;
	INT32				CodePage	= I3_LANG_CODE_ANSI;

	if(	i3::generic_is_iequal( szExt, "PEF")  )
	{
		RegFileType = REGISTRY_FILE_BINARY;
	}
	else if( i3::generic_is_iequal(szExt, "i3RegXML") )
	{
		RegFileType = REGISTRY_FILE_XML_BY_UTF8;
	}

	m_pRegSet = i3RegistrySet::new_object();
	if( NULL == m_pRegSet ) return false;
   	
   	m_pRegSet->SetCodePage( CodePage );
	INT32 nReadBytes = m_pRegSet->LoadRegistrySet( szFullPath ,RegFileType );
	if( nReadBytes == STREAM_ERR)
  	{
  		return false;
  	}

	i3RegKey * pRoot = m_pRegSet->getRoot();
	if( pRoot )
	{
		i3TreeNode * pEquipment = pRoot->FindChildByName( CSZ_EQUIPMENT );
		if( pEquipment )
		{
			i3RegKey * pKey = (i3RegKey *)pEquipment;
			if( pKey )
			{
				i3RegArray * pMakeTime = (i3RegArray *)pKey->FindDataByName( CSZ_MAKE_TIME );
				if( pMakeTime )
					m_nNationCount = pMakeTime->getSize();

			}
			LoadData( EQUIP::ePART_HEADGEAR, pEquipment->FindChildByName( CSZ_HEADGEAR ) );
			LoadData( EQUIP::ePART_FACEGEAR, pEquipment->FindChildByName( CSZ_FACEGEAR ) );
			LoadData( EQUIP::ePART_BELT, pEquipment->FindChildByName( CSZ_BELT ) );
			LoadData( EQUIP::ePART_HOLSTER, pEquipment->FindChildByName( CSZ_HOLSTER ) );

			LoadPartData( pEquipment);
		}
   	}

	return true;
}

bool CEquipData::Save( const char * pszFile )
{
	if( NULL == m_pRegSet ) return false;

	i3RegKey * pRoot = m_pRegSet->getRoot();
  	if( pRoot )
  	{
  		i3TreeNode * pEquipment = pRoot->FindChildByName( CSZ_EQUIPMENT );
  		if( pEquipment )
  		{
  			SaveData( EQUIP::ePART_HEADGEAR, pEquipment->FindChildByName( CSZ_HEADGEAR ) );
  			SaveData( EQUIP::ePART_FACEGEAR, pEquipment->FindChildByName( CSZ_FACEGEAR ) );
  			SaveData( EQUIP::ePART_BELT, pEquipment->FindChildByName( CSZ_BELT ) );
  			SaveData( EQUIP::ePART_HOLSTER, pEquipment->FindChildByName( CSZ_HOLSTER ) );
  		}
  	}

	char szFullPath[MAX_PATH];
	char	szExt[32] = {};
	i3::extract_fileext( pszFile, szExt);

	REGISTRY_FILE_TYPE	RegFileType	= REGISTRY_FILE_UNKNOWN;

	//	저장 형식이 i3RegXML이면 기본 저장되는 Binary 파일은 PEF 파일이된다.
	if( i3::generic_is_iequal( szExt, "i3RegXML"))
	{
		i3::snprintf( szFullPath, sizeof( szFullPath ), "%s/config/%s", i3ResourceFile::GetWorkingFolder(), pszFile );
		m_pRegSet->SaveRegistrySet( szFullPath , REGISTRY_FILE_XML_BY_UTF8);
	}
	else if( i3::generic_is_iequal( szExt, "pef") )
	{
		i3::snprintf( szFullPath, sizeof( szFullPath ), "%s/config/%s", i3ResourceFile::GetWorkingFolder(), pszFile );	
		m_pRegSet->SaveRegistrySet( szFullPath, REGISTRY_FILE_BINARY );
	}

	return true;
}

void CEquipData::Init()
{
	for( INT32 nPart = 0; nPart < EQUIP::ePART_COUNT; ++nPart )
	{
		m_mapEquip[nPart].clear();
		m_mapModify[nPart].clear();
	}
	m_nNationCount = 0;

	I3_SAFE_RELEASE(m_pRegSet);
}

bool CEquipData::LoadData( EQUIP::ePART ePart, i3TreeNode * pNode )
{
	if( !pNode ) return false;
	mapEQUIP& mapEquip = m_mapEquip[ePart];
	
	// Equip Part 
	INT32 nEquipCnt = pNode->getChildCount();
	for( INT32 nEquip = 0; nEquip < nEquipCnt; ++nEquip )
	{
		i3TreeNode * pEquip = pNode->getChild( nEquip );
		if( !pEquip ) continue;
		const char * pszEquip = pEquip->GetName();

		tagEQUIPINFO info;

		i3::rc_wstring str;
		i3RegData * pResName = ((i3RegKey*)pEquip)->FindDataByName( "ResPostName");
		if( pResName != NULL)
		{
			std::vector<i3RegData* >* conValue = ((i3RegArray*)pResName)->getValue();
			conValue->at(0)->GetValue( str);	// 첫번째 Default값으로 설정
			i3::utf16_to_mb( str, info._resName);
			info._resName_red = info._resName;
		}
		else
		{
			pResName = ((i3RegKey*)pEquip)->FindDataByName( "ResName_Red");
			if( pResName != NULL)
			{
				std::vector<i3RegData* >* conValue = ((i3RegArray*)pResName)->getValue();
				conValue->at(0)->GetValue(str);	// 첫번째 Default값으로 설정
				i3::utf16_to_mb( str, info._resName_red);
			}

			pResName = ((i3RegKey*)pEquip)->FindDataByName( "ResName_Blue");
			if( pResName != NULL)
			{
				std::vector<i3RegData* >* conValue = ((i3RegArray*)pResName)->getValue();
				conValue->at(0)->GetValue( str);	// 첫번째 Default값으로 설정
				i3::utf16_to_mb( str, info._resName);
			}
		}

		i3RegData * pHeadType = ((i3RegKey*)pEquip)->FindDataByName( CSZ_HEADTYPE);
		if( pHeadType != NULL)
		{
			std::vector<i3RegData* >* conValue = ((i3RegArray*)pHeadType)->getValue();
			conValue->at(0)->GetValue(&info._headtype);	// 첫번째 Default값으로 설정
			info._headtype_red = info._headtype;
		}
		else
		{
			pHeadType = ((i3RegKey*)pEquip)->FindDataByName( "HeadType_Red");
			if( pHeadType != NULL)
			{
				std::vector<i3RegData* >* conValue = ((i3RegArray*)pHeadType)->getValue();
				conValue->at(0)->GetValue(&info._headtype_red);	// 첫번째 Default값으로 설정
			}
			else
				info._headtype_red = -1;

			pHeadType = ((i3RegKey*)pEquip)->FindDataByName( "HeadType_Blue");
			if( pHeadType != NULL)
			{
				std::vector<i3RegData* >* conValue = ((i3RegArray*)pHeadType)->getValue();
				conValue->at(0)->GetValue(&info._headtype);	// 첫번째 Default값으로 설정
			}
			else
				info._headtype = -1;
		}

		i3RegData * pFaceShow = ((i3RegKey*)pEquip)->FindDataByName( CSZ_FACESHOW);
		if( pFaceShow != NULL)
		{
			std::vector<i3RegData* >* conValue = ((i3RegArray*)pFaceShow)->getValue();
			conValue->at(0)->GetValue(&info._visibleFace);	// 첫번째 Default값으로 설정
			info._visibleFace_red = info._visibleFace;
		}
		else
		{
			pFaceShow = ((i3RegKey*)pEquip)->FindDataByName( "FacegearShow_Red");
			if( pFaceShow != NULL)
			{
				std::vector<i3RegData* >* conValue = ((i3RegArray*)pFaceShow)->getValue();
				conValue->at(0)->GetValue(&info._visibleFace_red);	// 첫번째 Default값으로 설정
			}
			else
				info._visibleFace_red = -1;

			pFaceShow = ((i3RegKey*)pEquip)->FindDataByName( "FacegearShow_Blue");
			if( pFaceShow != NULL)
			{
				std::vector<i3RegData* >* conValue = ((i3RegArray*)pFaceShow)->getValue();
				conValue->at(0)->GetValue(&info._visibleFace);	// 첫번째 Default값으로 설정
			}
			else
				info._visibleFace = -1;
		}
		
		VEC4D vTrans;
		i3RegKey * pKey = (i3RegKey *)pEquip->FindChildByName( CSZ_TRANSFORM );
		if( pKey != NULL)
		{
			INT32 nCharaCnt = pKey->GetDataCount();
			for( INT32 nChara = 0; nChara < nCharaCnt; ++nChara )
			{
				i3RegArray * pArray = (i3RegArray *)pKey->GetData( nChara );
				if( !pArray ) continue;
				if( 0 >= pArray->getSize() ) continue;
				const char* pszChara = pArray->GetName();
				if( pszChara[0] == '_' ) continue;

				std::vector<i3RegData* >* conValue = pArray->getValue();
				conValue->at(0)->GetValue(&vTrans);

				//i3::safe_string_copy( szUpperCase, pszChara, sizeof(szUpperCase) );
				//i3::to_upper( szUpperCase);

				info._trans.insert( std::make_pair( pszChara, vTrans ) );
				info._trans_red.insert( std::make_pair( pszChara, vTrans ) );
			}
		}
		else
		{
			pKey = (i3RegKey *)pEquip->FindChildByName( "_Transform_Red" );
			if( pKey != NULL)
			{
				INT32 nCharaCnt = pKey->GetDataCount();
				for( INT32 nChara = 0; nChara < nCharaCnt; ++nChara )
				{
					i3RegArray * pArray = (i3RegArray *)pKey->GetData( nChara );
					if( !pArray ) continue;
					if( 0 >= pArray->getSize() ) continue;
					const char* pszChara = pArray->GetName();
					if( pszChara[0] == '_' ) continue;

					std::vector<i3RegData* >* conValue = pArray->getValue();
					conValue->at(0)->GetValue(&vTrans);

					//i3::safe_string_copy( szUpperCase, pszChara, sizeof(szUpperCase) );
					//i3::to_upper( szUpperCase);
					
					info._trans_red.insert( std::make_pair( pszChara, vTrans ) );
				}

				info._bValidTrans_Red = true;
			}

			pKey = (i3RegKey *)pEquip->FindChildByName( "_Transform_Blue" );
			if( pKey != NULL)
			{
				INT32 nCharaCnt = pKey->GetDataCount();
				for( INT32 nChara = 0; nChara < nCharaCnt; ++nChara )
				{
					i3RegArray * pArray = (i3RegArray *)pKey->GetData( nChara );
					if( !pArray ) continue;
					if( 0 >= pArray->getSize() ) continue;
					const char* pszChara = pArray->GetName();
					if( pszChara[0] == '_' ) continue;

					std::vector<i3RegData* >* conValue = pArray->getValue();
					conValue->at(0)->GetValue(&vTrans);

					//i3::safe_string_copy( szUpperCase, pszChara, sizeof(szUpperCase) );
					//i3::to_upper( szUpperCase);
					
					info._trans.insert( std::make_pair( pszChara, vTrans ) );
				}
			}
		}

		//i3::safe_string_copy( szUpperCase, pszEquip, sizeof(szUpperCase));
		//i3::to_upper( szUpperCase);

		mapEquip.insert( std::make_pair( pszEquip, info ) );
	}

	return true;
}
i3RegArray * CEquipData::_getTime( const char * pszName )
{
	UpdateTime();
	i3RegArray * _arrTime = i3RegArray::new_object();
	if( _arrTime )
	{
		_arrTime->clearRegData();
		_arrTime->SetName( pszName );
		_arrTime->setElementType( I3REG_TYPE_STRING );

		for( INT32 nCode = 0; nCode < m_nNationCount; ++nCode )
		{
			_arrTime->addRegData( _arrTime->createRegData() );
			i3::rc_wstring str;
			i3::mb_to_utf16( GetTime(), str);
			_arrTime->replaceRegData( nCode, str);
		}
	}
	return _arrTime;
}

static void ChangeData( i3RegKey * pKey, const char * pszFieldName, INT32 data, INT32 arrayCount)
{
	i3RegArray * pField = static_cast<i3RegArray*>(pKey->FindDataByName( pszFieldName));
	if( pField == NULL)
	{
		pField = i3RegArray::new_object();
		pField->SetName( pszFieldName );
		pField->setElementType( I3REG_TYPE_INT32 );

		for( INT32 nCode = 0; nCode < arrayCount; ++nCode )
		{
			i3RegINT32 * pValue = i3RegINT32::new_object();
			(*pValue) = data;
			pField->addRegData( pValue );
		}
		pKey->AddData( pField );
	}
	else
	{
		// 데이터 변경
		INT32 nSize = pField->getSize();
		for( INT32 nCode = 0; nCode < nSize ; ++nCode)
		{
			pField->replaceRegData( nCode, data );
		}
	}
}

static i3TreeNode * sFindChildByResName( i3TreeNode * pNode, const char * pszName)
{
	char szTemp[ 260];
	i3::rc_wstring str;

	I3ASSERT( pNode != NULL);
	
	i3::safe_string_copy( szTemp, pNode->GetName(), sizeof( szTemp));
	i3::to_upper( szTemp);

	if( i3::generic_compare( szTemp, pszName) == 0)
		return pNode;

	i3RegData * pData = ((i3RegKey*)pNode)->FindDataByName( "ResPostName");
	if( pData != NULL)
	{
		std::vector<i3RegData* >* conValue = ((i3RegArray*)pData)->getValue();
		conValue->at(0)->GetValue( str);
		i3::utf16_to_mb( str, szTemp, sizeof(szTemp));
		i3::to_upper( szTemp);
		if( i3::generic_compare( szTemp, pszName) == 0)
			return pNode;
	}

	pData = ((i3RegKey*)pNode)->FindDataByName( "ResName_Red");
	if( pData != NULL)
	{
		std::vector<i3RegData* >* conValue = ((i3RegArray*)pData)->getValue();
		conValue->at(0)->GetValue( str);
		i3::utf16_to_mb( str, szTemp, sizeof(szTemp));
		i3::to_upper( szTemp);

		if( i3::generic_compare( szTemp, pszName) == 0)
			return pNode;
	}

	pData = ((i3RegKey*)pNode)->FindDataByName( "ResName_Blue");
	if( pData != NULL)
	{
		std::vector<i3RegData* >* conValue = ((i3RegArray*)pData)->getValue();
		conValue->at(0)->GetValue( str);
		i3::utf16_to_mb( str, szTemp, sizeof(szTemp));
		i3::to_upper( szTemp);

		if( i3::generic_compare( szTemp, pszName) == 0)
			return pNode;
	}


	for( INT32 i = 0; i < pNode->getChildCount(); i++)
	{
		i3TreeNode * pFind = sFindChildByResName( pNode->getChild( i), pszName);
		if( pFind != NULL)
			return pFind;
	}

	return NULL;
}

i3RegArray * CEquipData::__CreateArrayDataINT32( INT32 nValue, const char * pszName)
{
	i3RegArray * pNewArrayData = i3RegArray::new_object();

	pNewArrayData->setElementType( I3REG_TYPE_INT32 );
	pNewArrayData->SetName( pszName);

	UINT32 size = m_NationList.size();
	for( UINT32 i =0; i<size; i++)
	{
		i3RegData* pData = pNewArrayData->createRegData();
		((i3RegINT32*)pData)->setValue( nValue );

		pNewArrayData->addRegData( pData );
	}

	return pNewArrayData;
}

i3RegArray * CEquipData::__CreateArrayDataString( i3::rc_wstring str, const char * pszName)
{
	i3RegArray * pArray = i3RegArray::new_object();
	I3ASSERT( pArray != NULL);

	pArray->setElementType( I3REG_TYPE_STRING );
	pArray->SetName( pszName );

	UINT32 size = m_NationList.size();
	for( UINT32 i = 0; i < size; ++i )
	{
		i3RegData* pData = pArray->createRegData();
		((i3RegString*)pData)->Set( str);
		pArray->addRegData( pData );
	}

	return pArray;
}

i3RegArray *CEquipData::__CreateArrayDataVEC4D( VEC4D * pVec, const char * pszName)
{
	i3RegArray * pArray = i3RegArray::new_object();
	I3ASSERT( pArray != NULL);

	pArray->setElementType( I3REG_TYPE_VEC4D );
	pArray->SetName( pszName );

	UINT32 size = m_NationList.size();
	for( UINT32 i = 0; i < size; ++i )
	{
		i3RegData* pData = pArray->createRegData();
		((i3RegVEC4D*)pData)->setValue( pVec);
		pArray->addRegData( pData );
	}

	return pArray;
}

bool CEquipData::SaveData( EQUIP::ePART ePart, i3TreeNode * pNode )
{
	if( !pNode ) return false;
	//mapEQUIP& __mapEquip = m_mapEquip[ePart];
	mapEQUIP& mapEquip = m_mapModify[ePart];
	// Equip Part 

	for( iterEQUIP iter_e = mapEquip.begin(); iter_e != mapEquip.end(); ++iter_e )
	{
		i3TreeNode * pItemNode = pNode->FindChildByName( iter_e->first.c_str() );
		if( pItemNode == NULL)
		{
			// resName값에서 찾아보자
			char equipName[ 260];

			i3::safe_string_copy( equipName, iter_e->first, sizeof( equipName));
			i3::to_upper( equipName);

			pItemNode = sFindChildByResName( pNode, equipName);

			if( pItemNode == NULL)
			{
				// 새로 생성
				pItemNode = i3RegKey::new_object_ref();

				i3RegArray * pMakeTime = _getTime( CSZ_MAKE_TIME );
				if( pMakeTime )	((i3RegKey*)pItemNode)->AddData( pMakeTime );
				i3RegArray * pUpdateTime = _getTime( CSZ_UPDATE_TIME );
				if( pUpdateTime )((i3RegKey*)pItemNode)->AddData( pUpdateTime );

				i3RegArray * pNewArrayData = __CreateArrayDataINT32( -1, "ITEMID");
				if( pNewArrayData != NULL)
					((i3RegKey*)pItemNode)->AddData(pNewArrayData);

				i3::rc_wstring str;
				i3::mb_to_utf16( iter_e->first, str);
				pNewArrayData = __CreateArrayDataString( str, "ResPostName");
				if( pNewArrayData != NULL)
					((i3RegKey*)pItemNode)->AddData(pNewArrayData);

				pNewArrayData = __CreateArrayDataINT32( -1, "UIShapeIndex");
				if( pNewArrayData != NULL)
					((i3RegKey*)pItemNode)->AddData(pNewArrayData);

				pNode->AddChild( pItemNode);
			}
		}

		tagEQUIPINFO& info = iter_e->second;

		if( info._headtype_red > -1 )
		{
			ChangeData( (i3RegKey*)pItemNode, "HeadType_Blue", info._headtype, m_nNationCount);
			ChangeData( (i3RegKey*)pItemNode, "HeadType_Red", info._headtype_red, m_nNationCount);
		}
		else if( info._headtype > -1 )
			ChangeData( (i3RegKey*)pItemNode, CSZ_HEADTYPE, info._headtype, m_nNationCount);
		
		if( info._visibleFace_red > -1)
		{
			ChangeData( (i3RegKey*)pItemNode, "FacegearShow_Blue", info._visibleFace, m_nNationCount);
			ChangeData( (i3RegKey*)pItemNode, "FacegearShow_Red", info._visibleFace_red, m_nNationCount);
		}
		else if( info._visibleFace > -1)
			ChangeData( (i3RegKey*)pItemNode, CSZ_FACESHOW, info._visibleFace, m_nNationCount);

		i3RegKey * pTrans = NULL;
		i3RegKey * pTrans_Red = NULL;

		if( info._bValidTrans_Red)
		{
			pTrans = (i3RegKey *)pItemNode->FindChildByName( "_Transform_Blue" );
			if( !pTrans )
			{
				// Transform 생성
				pTrans = i3RegKey::new_object_ref();
				pTrans->SetName( "_Transform_Blue" );
				pItemNode->AddChild( pTrans );
				i3RegArray * pMakeTime = _getTime( CSZ_MAKE_TIME );
				if( pMakeTime )	pTrans->AddData( pMakeTime );
				i3RegArray * pUpdateTime = _getTime( CSZ_UPDATE_TIME );
				if( pUpdateTime )pTrans->AddData( pUpdateTime );
			}

			pTrans_Red = (i3RegKey *)pItemNode->FindChildByName( "_Transform_Red" );
			if( !pTrans_Red )
			{
				// Transform 생성
				pTrans_Red = i3RegKey::new_object_ref();
				pTrans_Red->SetName( "_Transform_Red" );
				pItemNode->AddChild( pTrans_Red );
				i3RegArray * pMakeTime = _getTime( CSZ_MAKE_TIME );
				if( pMakeTime )	pTrans_Red->AddData( pMakeTime );
				i3RegArray * pUpdateTime = _getTime( CSZ_UPDATE_TIME );
				if( pUpdateTime )pTrans_Red->AddData( pUpdateTime );
			}
		}
		else
		{
			pTrans = (i3RegKey *)pItemNode->FindChildByName( CSZ_TRANSFORM );
			if( !pTrans )
			{
				// Transform 생성
				pTrans = i3RegKey::new_object_ref();
				pTrans->SetName( CSZ_TRANSFORM );
				pItemNode->AddChild( pTrans );
				i3RegArray * pMakeTime = _getTime( CSZ_MAKE_TIME );
				if( pMakeTime )	pTrans->AddData( pMakeTime );
				i3RegArray * pUpdateTime = _getTime( CSZ_UPDATE_TIME );
				if( pUpdateTime )pTrans->AddData( pUpdateTime );
			}
		}

		mapTRANS& mapTrans = info._trans;
		for( iterTRANS iter_t = mapTrans.begin(); iter_t != mapTrans.end(); ++iter_t )
		{
			const char* pszChara = iter_t->first.c_str();
			VEC4D vTransform = iter_t->second;

			i3RegArray * pArrChara = (i3RegArray *)pTrans->FindDataByName( pszChara );
			if( pArrChara )
			{ 
				// 데이터 변경
				INT32 nSize = pArrChara->getSize();
				for( INT32 nCode = 0; nCode < nSize ; ++nCode)
				{
					pArrChara->replaceRegData( nCode, vTransform );
				}
			}
			else
			{
				// 데이터 추가
				pArrChara = __CreateArrayDataVEC4D( &vTransform, pszChara);
				pTrans->AddData( pArrChara );
			}

			//////////////////////////////////////////////////////////////////////////
			// 만약 데이터가 업데이트 되었다면...
			i3RegArray * pArrUpdate = (i3RegArray *)pTrans->FindDataByName( CSZ_UPDATE_TIME );
			if( pArrUpdate )
			{
				UpdateTime();
				i3::rc_wstring str;
				i3::mb_to_utf16( GetTime(), str);
				INT32 nSize = pArrUpdate->getSize();	
				for( INT32 nCode = 0; nCode < nSize ; ++nCode )
				{
					pArrUpdate->replaceRegData(nCode, str );
				}
			}
			//////////////////////////////////////////////////////////////////////////
		}

		if( pTrans_Red != NULL)
		{
			mapTRANS& mapTrans_red = info._trans_red;
			for( iterTRANS iter_t = mapTrans_red.begin(); iter_t != mapTrans_red.end(); ++iter_t )
			{
				const char* pszChara = iter_t->first.c_str();
				VEC4D vTransform = iter_t->second;

				i3RegArray * pArrChara = (i3RegArray *)pTrans_Red->FindDataByName( pszChara );
				if( pArrChara )
				{ 
					// 데이터 변경
					INT32 nSize = pArrChara->getSize();
					for( INT32 nCode = 0; nCode < nSize ; ++nCode)
					{
						pArrChara->replaceRegData( nCode, vTransform );
					}
				}
				else
				{
					// 데이터 추가
					pArrChara = __CreateArrayDataVEC4D( &vTransform, pszChara);
					pTrans_Red->AddData( pArrChara );
				}

				//////////////////////////////////////////////////////////////////////////
				// 만약 데이터가 업데이트 되었다면...
				i3RegArray * pArrUpdate = (i3RegArray *)pTrans_Red->FindDataByName( CSZ_UPDATE_TIME );
				if( pArrUpdate )
				{
					UpdateTime();
					i3::rc_wstring str;
					i3::mb_to_utf16( GetTime(), str);
					INT32 nSize = pArrUpdate->getSize();	
					for( INT32 nCode = 0; nCode < nSize ; ++nCode )
					{
						pArrUpdate->replaceRegData(nCode, str );
					}
				}
				//////////////////////////////////////////////////////////////////////////
			}
		}
	}
	return true;
}

void CEquipData::SetPart( EQUIP::ePART ePart, const char * pszItem, const char * pszChara, const VEC4D& vTrans, const INT32& headType )
{
	char szUpperCase[ 260];

	if( !UsePart( ePart ) )
		return ;
	
	if( CheckModify( ePart, pszItem, pszChara, vTrans, headType ) )
	{
		Modify( ePart, pszItem, pszChara, vTrans, headType );
	}

	mapEQUIP& mapEquip = m_mapEquip[ePart];

	i3::safe_string_copy( szUpperCase, pszItem, sizeof(szUpperCase));
	i3::to_upper( szUpperCase);

	bool bFind = false;
	bool bRedTeam = false;
	iterEQUIP iter_e = mapEquip.begin();	//mapEquip.find( szUpperCase );
	while( iter_e != mapEquip.end())
	{
		char szTemp[ 260];
		char szTemp_Red[ 260];

		tagEQUIPINFO& info = iter_e->second;

		i3::safe_string_copy( szTemp, info._resName, sizeof( szTemp));
		i3::to_upper( szTemp);

		i3::safe_string_copy( szTemp_Red, info._resName_red, sizeof( szTemp_Red));
		i3::to_upper( szTemp_Red);

		if( i3::generic_compare( szTemp, szUpperCase) == 0)
		{
			bFind= true;
			break;
		}
		else if(  i3::generic_compare( szTemp_Red, szUpperCase) == 0)
		{
			bFind = true;
			bRedTeam = true;
			break;
		}

		iter_e++;
	}

	if( bFind )
	{
		tagEQUIPINFO& info = iter_e->second;

		info._headtype = headType;
		mapTRANS& mapTrans = info._trans;

		if( bRedTeam)
		{
			info._headtype_red = headType;
			mapTrans = info._trans_red;
		}
		
		iterTRANS iter_t = mapTrans.find( pszChara );

		if( iter_t != mapTrans.end() ) 
			iter_t->second = vTrans;
		else
		{
			mapTrans.insert( std::make_pair( pszChara, vTrans ) );
		}
	}
	else
	{
		tagEQUIPINFO info;

		info._trans.insert( std::make_pair( pszChara, vTrans ) ); 
		info._headtype = headType;

		mapEquip.insert( std::make_pair( pszItem, info ) );
	}
}
bool CEquipData::GetPart( EQUIP::ePART ePart, const char * pszItem, const char * pszChara, VEC4D& vTrans, INT32& headType )
{
	if( !UsePart( ePart ) )
		return false;

	char szUpper[ 260];

	i3::safe_string_copy( szUpper, pszItem, sizeof( szUpper));
	i3::to_upper( szUpper);

	mapEQUIP& mapEquip = m_mapEquip[ePart];
	//iterEQUIP iter_e = mapEquip.find( szUpper);

	bool bFind = false;
	bool bRedTeam = false;
	iterEQUIP iter_e = mapEquip.begin();
	while( iter_e != mapEquip.end())
	{
		char szTemp[ 260];
		char szTempRed[ 260];

		tagEQUIPINFO& info = iter_e->second;
		
		i3::safe_string_copy( szTemp, info._resName, sizeof( szTemp));
		i3::to_upper( szTemp);

		i3::safe_string_copy( szTempRed, info._resName_red, sizeof( szTempRed));
		i3::to_upper( szTempRed);

		if( i3::generic_compare( szTemp, szUpper) == 0 )
		{
			bFind = true;
			break;
		}
		else if( i3::generic_compare( szTempRed, szUpper) == 0)
		{
			bFind = true;
			bRedTeam = true;
			break;
		}

		iter_e++;
	}

	if( bFind)
	{
		tagEQUIPINFO& info = iter_e->second;

		mapTRANS& mapTrans = info._trans;
		headType = info._headtype;

		if( bRedTeam)
		{
			headType = info._headtype_red;
			mapTrans = info._trans_red;
		}

		iterTRANS iter_t = mapTrans.find( pszChara);
		if( iter_t != mapTrans.end() )
		{
			vTrans = iter_t->second;
			return true;
		}
	}
	return false;
}


bool CEquipData::CheckModify( EQUIP::ePART ePart, const char * pszItem, const char * pszChara, const VEC4D& vTrans, const INT32& headType )
{
	VEC4D vOld;
	INT32 headOld;
	if( GetPart( ePart, pszItem, pszChara, vOld, headOld ) )
	{
		if( vOld == vTrans && headOld == headType )
			return false;
	}
	return true;
}

void CEquipData::Modify( EQUIP::ePART ePart, const char * pszItem, const char * pszChara, const VEC4D& vTrans, const INT32& headType )
{
	char szUpperCase[ 260];

	mapEQUIP& mapModify = m_mapModify[ePart];

	i3::safe_string_copy( szUpperCase, pszItem, sizeof(szUpperCase));
	i3::to_upper( szUpperCase);

	bool bFind = false;
	iterEQUIP iter_e = mapModify.begin();
	while( iter_e != mapModify.end())
	{
		char szTemp[ 260];
		char szTempRed[ 260];

		tagEQUIPINFO& info = iter_e->second;

		i3::safe_string_copy( szTemp, info._resName, sizeof( szTemp));
		i3::to_upper( szTemp);

		i3::safe_string_copy( szTempRed, info._resName_red, sizeof( szTempRed));
		i3::to_upper( szTempRed);

		if( i3::generic_compare( szTemp, szUpperCase) == 0)
		{
			mapTRANS& mapTrans = info._trans;
			info._headtype = headType;
			iterTRANS iter_t = mapTrans.find( pszChara );
			if( iter_t != mapTrans.end() )	iter_t->second = vTrans;
			else							mapTrans.insert( std::make_pair( pszChara, vTrans) );

			bFind = true;
			break;
		}
		else if( i3::generic_compare( szTempRed, szUpperCase) == 0)
		{
			mapTRANS& mapTrans = info._trans_red;
			info._headtype_red = headType;
			iterTRANS iter_t = mapTrans.find( pszChara );
			if( iter_t != mapTrans.end() )	iter_t->second = vTrans;
			else							mapTrans.insert( std::make_pair( pszChara, vTrans) );

			bFind = true;
			break;
		}

		iter_e++;
	}

	if( bFind == false)
	{
		tagEQUIPINFO info;

		info._resName = pszItem;
		info._trans.insert( std::make_pair(pszChara, vTrans) );
		info._headtype = headType;

		mapModify.insert( std::make_pair( pszItem, info) );
	}
}

void CEquipData::UpdateTime()
{
	time_t osBinaryTime;  // C run-time time (defined in <time.h>)
	time( &osBinaryTime );

	CTime ct(osBinaryTime);
	_i64toa( osBinaryTime, m_szTime, 10 );
}

const char* CEquipData::GetTime()
{
	return m_szTime;
}

INT32 CEquipData::GetNationCount()
{
	return m_nNationCount;
}

bool CEquipData::UsePart( EQUIP::ePART ePart )
{
	switch( ePart )
	{
	case EQUIP::ePART_VEST:
	case EQUIP::ePART_PANT:
	case EQUIP::ePART_GLOVE:
	case EQUIP::ePART_CHARA:
	case EQUIP::ePART_HEADGEAR:
	case EQUIP::ePART_FACEGEAR:
	case EQUIP::ePART_HOLSTER:
	case EQUIP::ePART_BELT:
		return true;
	}
	return false;
}

void CEquipData::LoadPartData( i3TreeNode * pNode)
{
	bool bValid = false;
	tagPartInfo info;
	i3::rc_wstring str;
	i3RegData * pResName = ((i3RegKey*)pNode)->FindDataByName( "ResPostName");
	
	info._hResItem = NULL;

	if( pResName != NULL)
	{
		std::vector<i3RegData* >* conValue = ((i3RegArray*)pResName)->getValue();
		conValue->at(0)->GetValue( str);	// 첫번째 Default값으로 설정
		i3::utf16_to_mb( str, info._resName);
		info._resName_red = info._resName;

		bValid = true;
	}
	else
	{
		pResName = ((i3RegKey*)pNode)->FindDataByName( "ResName_Red");
		if( pResName != NULL)
		{
			std::vector<i3RegData* >* conValue = ((i3RegArray*)pResName)->getValue();
			conValue->at(0)->GetValue(str);	// 첫번째 Default값으로 설정
			i3::utf16_to_mb( str, info._resName_red);
			bValid = true;
		}

		pResName = ((i3RegKey*)pNode)->FindDataByName( "ResName_Blue");
		if( pResName != NULL)
		{
			std::vector<i3RegData* >* conValue = ((i3RegArray*)pResName)->getValue();
			conValue->at(0)->GetValue( str);	// 첫번째 Default값으로 설정
			i3::utf16_to_mb( str, info._resName);
			bValid = true;
		}
	}

	if( bValid)
		m_mapPart.insert( std::map<i3::string, tagPartInfo>::value_type( pNode->GetName(),info) );

	for( INT32 i = 0 ; i < pNode->getChildCount(); i++)
	{
		LoadPartData( pNode->getChild( i));
	}
}

CEquipData::tagPartInfo * CEquipData::ExistPart( const char * pszName)
{
	std::map<i3::string, tagPartInfo>::iterator it = m_mapPart.begin();

	while( it != m_mapPart.end())
	{
		tagPartInfo &info = it->second;
		if( i3::generic_compare( info._resName, pszName) == 0)
		{
			return &info;
		}

		if( i3::generic_compare( info._resName_red, pszName) == 0)
		{
			return &info;
		}

		it++;
	}

	return NULL;
}

const char * CEquipData::GetTeamPart( const char * pszName, TEAM_TYPE team)
{
	std::map<i3::string, tagPartInfo>::iterator it = m_mapPart.begin();

	while( it != m_mapPart.end())
	{
		tagPartInfo &info = it->second;
		if( (i3::generic_compare( info._resName, pszName) == 0) ||
			(i3::generic_compare( info._resName_red, pszName) == 0))
		{
			if( team == TEAM_BLUE)
				return info._resName.c_str();
			else
				return info._resName_red.c_str();
		}

		it++;
	}

	return NULL;
}

void CEquipData::__LoadNationFile( const char * pszName)
{
	CStdioFile FileStream;
	if( !FileStream.Open( pszName, CFile::modeRead) )
	{
		I3PRINTLOG(I3LOG_FATAL, "Stream IO Error! %s", pszName );
	}

	m_NationList.clear();

	CString Buffer;
	while( FileStream.ReadString(Buffer) )
	{
		i3::string NationString( Buffer.GetString());

		size_t Index;
		//주석 제거
		if( !NationString.empty() )
		{
			Index = NationString.rfind(";");
			if( Index != std::string::npos )
				NationString.erase( Index );
		}
		else
			continue;

		//앞쪽 Trim
		if( !NationString.empty() )
		{
			Index = NationString.find_first_not_of(" \t");
			if( Index != std::string::npos )
				NationString.erase(0, Index );
		}
		else
			continue;

		//뒷쪽 Trim
		if( !NationString.empty() )
		{
			Index = NationString.find_last_not_of(" \t") +1;
			if( Index != std::string::npos )
				NationString.erase( Index );
		}
		else
			continue;

		if( !NationString.empty() )
		{
			m_NationList.push_back( NationString );
		}
	}

	FileStream.Close();
}
