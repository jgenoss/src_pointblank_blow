
#include "stdafx.h"
#include "i3TDKGameRegistry.h"
#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/mb_to_utf16.h"


I3_EXPORT_TDK 
BOOL i3TDKGameRegistry::Make( RegData * pRegData)
{
	i3Registry::Init( nullptr);	//	Global Registry 정보를 초기화한다.
	I3ASSERT( g_pRegistry != nullptr);

	if( pRegData == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN,  "Warning! Registry data is nullptr");
		return FALSE;
	}
		
	i3Framework * pFrm = pRegData->_pi3Framework;
	if( pFrm == nullptr)
	{
		I3PRINTLOG(I3LOG_WARN,  "cannot make registry data of i3Framework");
		return FALSE;
	}
	
	i3RegKey * pRegRoot = g_pRegistry->getRoot();	
	I3ASSERT( pRegRoot != nullptr);

	i3::wstring utf16str;

	//(0) config
	if( pRegData->_szRSCFileName[0] != 0)
	{
		i3RegKey * pRegConfig = i3RegKey::new_object_ref();
		pRegConfig->SetName( REG_CONFIG_ROOT);
		pRegRoot->AddChild( pRegConfig);

		IR_STRING * pRSCFileName = IR_STRING::new_object_ref();
		
		i3::mb_to_utf16(pRegData->_szRSCFileName, utf16str);

		pRSCFileName->Set( utf16str );
		pRSCFileName->SetName("RSCFileName");
		pRegConfig->AddData( pRSCFileName);
	}
	
	//	(1) set Optional Info
	{
		i3RegKey * pRegOptional = i3RegKey::new_object_ref();
		pRegOptional->SetName( REG_OPTIONAL_ROOT);
		pRegRoot->AddChild( pRegOptional);

		IR_STRING * pRegWorkDir = IR_STRING::new_object_ref();

		i3::mb_to_utf16(pRegData->_szWorkingDir, utf16str);

		pRegWorkDir->Set( utf16str );	//	작업 디렉토리 저장
		pRegWorkDir->SetName( "Working Directory");
		pRegOptional->AddData( pRegWorkDir);		
	}

	//	(2) set Layer in i3Framework
	{
		i3RegKey * pRegLayerRoot = i3RegKey::new_object_ref();
		pRegLayerRoot->SetName( REG_LAYER_ROOT);
		pRegRoot->AddChild( pRegLayerRoot);

		for( INT32 i = 0; i < pFrm->getLayerCount(); i++)
		{
			IR_STRING * pRegName = IR_STRING::new_object_ref();			

			i3SgLayer * pCurLayer = pFrm->getLayer( i);
			if( pCurLayer == nullptr)
			{
				I3PRINTLOG(I3LOG_WARN,  "Fail to make registry data. i3SgLayer is nullptr");
				return FALSE;
			}

			char szName[MAX_PATH];
			sprintf( szName, "Layer%d", i);

			i3::mb_to_utf16(pCurLayer->GetName() , utf16str);

			pRegName->Set( utf16str );	//	레이어의 이름들 차례로 저장
			pRegName->SetName( szName);

			pRegLayerRoot->AddData( pRegName);			
		}
	}	

	return TRUE;
}

I3_EXPORT_TDK 
void i3TDKGameRegistry::Destroy( void)
{
	if( g_pRegistry != nullptr)
	{
		//	Destroy All Registry of Optional
//		i3RegKey * pOptionalRoot = i3Registry::FindKey( REG_OPTIONAL_ROOT);
//		I3_SAFE_RELEASE( pOptionalRoot);

		//	Destroy All Registry of Layers
//		i3RegKey * pLayerRoot = i3Registry::FindKey( REG_LAYER_ROOT);
//		I3_SAFE_RELEASE( pLayerRoot);
	}

	i3Registry::Destroy();
}

I3_EXPORT_TDK 
UINT32 i3TDKGameRegistry::Save( const char * pszFileName)
{
	char szRegFileName[256] = "";
//	i3String::SwapExt( szRegFileName, 256, pszFileName, REG_FILE_EXT);	//	확장자 변경 
	i3::change_file_ext_copy(szRegFileName, pszFileName, REG_FILE_EXT);
	return g_pRegistry->SaveRegistrySet( szRegFileName);
}

I3_EXPORT_TDK 
UINT32 i3TDKGameRegistry::Load( const char * pszFileName)
{
	char szRegFileName[256] = "";
//	i3String::SwapExt( szRegFileName, 256, pszFileName, REG_FILE_EXT);	//	확장자 변경 
	i3::change_file_ext_copy(szRegFileName, pszFileName, REG_FILE_EXT);

	return i3Registry::Init( szRegFileName);
}

I3_EXPORT_TDK 
BOOL i3TDKGameRegistry::Create( const char * pszFileName, RegData * pRegData)
{
	BOOL re = Make( pRegData);
	if( re == FALSE)	return FALSE;
	
	UINT32 st = Save( pszFileName);
	if( st == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN,  "Fail to save file");
		return FALSE;
	}

	return TRUE;
}

