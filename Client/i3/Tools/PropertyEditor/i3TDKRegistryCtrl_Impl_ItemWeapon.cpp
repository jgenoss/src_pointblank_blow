// i3RegistryCtrl.cpp : implementation file
//

#include "stdafx.h"

#include "GlobalVariable.h"

#include "i3TDKRegistryCtrl_Impl.h"
#include "i3TDKUtilShell.h"

#include "MainFrm.h"

#include "WeaponContextDialog.h"

#include "i3Base/string/ext/utf16_to_mb.h"


//***********************************************//
bool _GetArrayRegIntMatch(i3RegArray* pRegArray, INT32 Value)
{
	if( pRegArray->getDataType() != I3REG_TYPE_ARRAY )
		return 0;

	if( pRegArray->getElementType() != I3REG_TYPE_INT32 )
		return 0;

	size_t size = pRegArray->getSize();

	INT32 ret = 0;
	for(size_t i=0;i<size;++i)
	{
		(*pRegArray)[i]->GetValue( &ret );
		if( ret != 0 )
		{
			if( Value != ret )
				return false;
		}
	}
	return true;
}

INT32 _GetArrayRegIntValue(i3RegArray* pRegArray)
{
	if( pRegArray->getDataType() != I3REG_TYPE_ARRAY )
		return 0;

	if( pRegArray->getElementType() != I3REG_TYPE_INT32 )
		return 0;

	size_t size = pRegArray->getSize();

	INT32 ret = 0;
	for(size_t i=0;i<size;++i)
	{
		(*pRegArray)[i]->GetValue( &ret );
		if( ret != 0 )
			break;
	}
	return ret;
}

i3::rc_wstring _GetArrayRegStringValue(i3RegArray* pRegArray)
{
	if( pRegArray->getDataType() != I3REG_TYPE_ARRAY )
		return i3::rc_wstring();

	if( pRegArray->getElementType() != I3REG_TYPE_STRING )
		return i3::rc_wstring();

	size_t size = pRegArray->getSize();

//	const char* Ret = 0;
	
	i3::rc_wstring wstrRet;

	for(size_t i=0;i<size;++i)
	{
		wstrRet = ((i3RegString*)(*pRegArray)[i])->getValue();
		if( i3::generic_is_equal(wstrRet, L"") == false )
			break;
	}

	return wstrRet;
}

//***********************************************//
bool i3TDKRegistryCtrl_Impl::_DuplicateClassTypeRegistry(const std::string & ClassType, size_t Index)
{
	i3RegData* pData = i3Registry::FindData( m_pClassTypeRoot, ClassType.c_str() );
	if( pData )
	{
		if( strcmp( pData->GetName(), ClassType.c_str() ) == 0)
		{
			return true;
		}
	}

	return false;
}

bool i3TDKRegistryCtrl_Impl::_DuplicateClassTypeLookup(const std::string & ClassType, size_t Index)
{
	ClassTypeLookup::iterator It = m_ClassTypeLookup.find( Index );
	if( m_ClassTypeLookup.end() == It )
	{
		return false;
	}

	if( It->second != ClassType )
		return false;

	return true;
}

void i3TDKRegistryCtrl_Impl::_AddClassTypeRegistry(const std::string & ClassType, size_t Index)
{
	i3RegData* pData = i3Registry::FindData( m_pClassTypeRoot, ClassType.c_str() );
	if( pData )
	{
		if( strcmp( pData->GetName(), ClassType.c_str() ) == 0)
		{
			I3PRINTLOG(I3LOG_FATAL, "The Weapon class type[ %s - %d ] has been already registered in the ClassTypeRoot Registry!", 
				ClassType.c_str(), Index );
		}
	}

	i3RegINT32 * pNewData = i3RegINT32::new_object();

	pNewData->SetName( ClassType.c_str() );
	pNewData->setValue( Index );

	m_pClassTypeRoot->AddData( pNewData );

	if( m_MaxClassTypeIndex < Index && Index != UN_INCLUDE_MAX_INDEX )
		m_MaxClassTypeIndex = Index;
}

void i3TDKRegistryCtrl_Impl::_AddClassTypeLookup(const std::string & ClassType, size_t Index)
{
	if( _DuplicateClassTypeLookup(ClassType, Index) )
	{
		I3PRINTLOG(I3LOG_FATAL, "The Weapon class type[ %s - %d ] has been already registered in the ClassTypeLookup !", 
			ClassType.c_str(), Index );
	}

	m_ClassTypeLookup.insert( ClassTypeLookup::value_type(Index, ClassType ) );

	if( m_MaxClassTypeIndex < Index && Index != UN_INCLUDE_MAX_INDEX )
		m_MaxClassTypeIndex = Index;
}

void i3TDKRegistryCtrl_Impl::DelWeaponData(const std::string & ClassType, INT32 WeaponNumber)
{
	WeaponDictionary::iterator DicIt = m_WeaponDictionary.find( ClassType );
	if( DicIt == m_WeaponDictionary.end() )
	{
		I3PRINTLOG(I3LOG_FATAL, "The Weapon name[ %s ] does not registered in the Weapons Dictionary !", ClassType.c_str() );
		return;
	}

	//무기 번호에 해당하는 Registry 를 찾는다.
	WeaponRegDictionary::iterator RegIt = DicIt->second->find( WeaponNumber );
	if( RegIt == DicIt->second->end() )
	{
		I3PRINTLOG(I3LOG_FATAL, "The Weapon number[ %d ] does not registered in the Weapons Registry Dictionary !", WeaponNumber );
	}

	i3RegKey *pGrandParent = 0, *pParent = 0, *pChild = 0;
	//안에 든게 있다.
	if( !RegIt->second.empty() )
	{
		//든 수 만큼 삭제한다.
		size_t size = RegIt->second.size();
		for(size_t i=0; i<size; ++i)
		{
			pChild = (i3RegKey*)RegIt->second.at(i);
			pParent = (i3RegKey*)pChild->getParent();
			if( pParent )
			{
				pParent->RemoveChild( pChild );

				if( pParent->getChildCount() == 0 )
				{
					pGrandParent = (i3RegKey*)pParent->getParent();
					if( pGrandParent )
						pGrandParent->RemoveChild( pParent );
				}
			}
		}
	}

	//다 삭제 했으니 이걸 지우자.
	DicIt->second->erase( RegIt );

	//더 이상 값이 없는 경우. 삭제 하자.
	if( DicIt->second->empty() )
	{
		delete DicIt->second;
		m_WeaponDictionary.erase( DicIt );
	}
}

void i3TDKRegistryCtrl_Impl::_DelClassTypeRegistry(const std::string & ClassType)
{
	i3RegData* pData = i3Registry::FindData( m_pClassTypeRoot, ClassType.c_str() );

	if( pData )
	{
		m_pClassTypeRoot->RemoveData( pData );
	}
	else
	{
		I3PRINTLOG(I3LOG_FATAL, "The Weapon name[ %s ] does not registered in the Weapons Dictionary !", ClassType.c_str() );
	}

	WeaponDictionary::iterator DicIt = m_WeaponDictionary.find( ClassType );
	if( DicIt == m_WeaponDictionary.end() )
	{
		I3PRINTLOG(I3LOG_FATAL, "The Weapon name[ %s ] does not registered in the Weapons Dictionary !", ClassType.c_str() );
	}

	//WeaponRegDictionary에 들어 있는 모든 i3RegKey* 를 삭제한다.
	WeaponRegDictionary::iterator RegIt = DicIt->second->begin();
	WeaponRegDictionary::iterator RegEnd = DicIt->second->end();

	i3RegKey *pGrandParent = 0, *pParent = 0, *pChild = 0;
	size_t size;
	for(; RegIt != RegEnd; ++RegIt )
	{
		if( RegIt->second.empty() )
			continue;

		size = RegIt->second.size();
		for(size_t i=0; i<size; ++i)
		{
			pChild = (i3RegKey*)RegIt->second.at(i);
			pParent = (i3RegKey*)pChild->getParent();
			if( pParent )
			{
				pParent->RemoveChild( pChild );

				if( pParent->getChildCount() == 0 )
				{
					pGrandParent = (i3RegKey*)pParent->getParent();
					if( pGrandParent )
						pGrandParent->RemoveChild( pParent );
				}
			}
		}
	}

	//모든 i3RegKey*을 삭제 했으니 청소를 해준다.
	DicIt->second->clear();

	//WeaponDictionary 에서 다 사용 햇으니 삭제 
	delete DicIt->second;

	//WeaponDictionary에 필요 없는 거니 삭제
	m_WeaponDictionary.erase( DicIt );
}

void i3TDKRegistryCtrl_Impl::_DelClassTypeLookup(const std::string & ClassType)
{
	ClassTypeLookup::iterator It = m_ClassTypeLookup.begin();
	ClassTypeLookup::iterator End = m_ClassTypeLookup.end();

	for(; It != End; ++It)
	{
		if( It->second == ClassType )
		{
			It = m_ClassTypeLookup.erase( It );
			break;
		}
	}
} 

void i3TDKRegistryCtrl_Impl::_RenameClassTypeRegistry(const std::string & ClassType, 
												   const std::string & NewClassType)
{
	i3RegData* pData = i3Registry::FindData( m_pClassTypeRoot, ClassType.c_str() );

	if( pData )
	{
		pData->SetName( NewClassType.c_str() );
	}
}

void i3TDKRegistryCtrl_Impl::_RenameClassTypeLookup(const std::string & ClassType, 
												   const std::string & NewClassType)
{
	ClassTypeLookup::iterator It = m_ClassTypeLookup.begin();
	ClassTypeLookup::iterator End = m_ClassTypeLookup.end();

	for(; It != End; ++It)
	{
		if( It->second == ClassType )
		{
			It->second = NewClassType;
			break;
		}
	}
}

void i3TDKRegistryCtrl_Impl::AddClassType(const std::string & ClassType, size_t Index, bool ApplyRegistry)
{
	_AddClassTypeLookup(ClassType, Index);

	if( ApplyRegistry )
		_AddClassTypeRegistry(ClassType, Index);
}

void i3TDKRegistryCtrl_Impl::DelClassType(const std::string & ClassType, bool ApplyRegistry)
{
	_DelClassTypeLookup(ClassType);

	if( ApplyRegistry )
		_DelClassTypeRegistry(ClassType);
} 


void i3TDKRegistryCtrl_Impl::RenameClassType(const std::string & ClassType, 
												   const std::string & NewClassType, bool ApplyRegistry)
{
	_RenameClassTypeLookup(ClassType, NewClassType);

	if( ApplyRegistry )
		_RenameClassTypeRegistry(ClassType, NewClassType);
}

size_t i3TDKRegistryCtrl_Impl::GetClassTypeLookupIndex(const std::string & ClassType) const
{
	ClassTypeLookup::const_iterator It = m_ClassTypeLookup.begin();
	ClassTypeLookup::const_iterator End = m_ClassTypeLookup.end();

	for(; It != End; ++It)
	{
		if( It->second == ClassType )
			return It->first;
	}
	return 0;
}


//***********************************************//

void i3TDKRegistryCtrl_Impl::AddWeaponDictionary(const char* Name, INT32 Value)
{
	WeaponDictionary::iterator It = m_WeaponDictionary.find( Name );

	if( It == m_WeaponDictionary.end() )
	{
		WeaponRegDictionary *pRegDictionary = new WeaponRegDictionary;
		It = m_WeaponDictionary.insert( 
			WeaponDictionary::value_type(Name, pRegDictionary) ).first;
	}

	It->second->insert(
		WeaponRegDictionary::value_type(Value, WeaponRegBank() ) );
}

void i3TDKRegistryCtrl_Impl::RegistryWeaponDictionary(INT32 ClassType, INT32 WeaponNumber, i3RegKey * pKey)
{
	//LookupTable에서 Weapon이름을 찾는다.
	ClassTypeLookup::iterator LookupIt = m_ClassTypeLookup.find( ClassType );
	if( LookupIt == m_ClassTypeLookup.end() )
	{
		I3PRINTLOG(I3LOG_FATAL, "The Weapon[ %s ] classtype[ %d ] does not registered in the Weapons LookupTable !", pKey->GetName(), ClassType );
	}

	WeaponDictionary::iterator DicIt = m_WeaponDictionary.find( LookupIt->second );
	if( DicIt == m_WeaponDictionary.end() )
	{
		I3PRINTLOG(I3LOG_FATAL, "The Weapon[ %s ] name[ %s ] does not registered in the Weapons Dictionary !", pKey->GetName(), LookupIt->second.c_str() );
	}

	WeaponRegDictionary::iterator RegIt = DicIt->second->find( WeaponNumber );
	if( RegIt == DicIt->second->end() )
	{
		I3PRINTLOG(I3LOG_FATAL, "The Weapon[ %s ] number[ %d ] does not registered in the Weapons Registry Dictionary !", pKey->GetName(), WeaponNumber );
	}

	if( !RegIt->second.empty() )
	{
		I3PRINTLOG(I3LOG_FATAL, "The Weapon[ %s ] classtype[ %d ],"
			"name[ %s ],"
			"number[ %d ] already registered in the Weapons Registry Dictionary !", 
			pKey->GetName(),
			ClassType,
			LookupIt->second.c_str(),
			WeaponNumber );
	}

	RegIt->second.push_back( pKey );
}

void i3TDKRegistryCtrl_Impl::ClearWeaponDictionary()
{
	WeaponDictionary::iterator It = m_WeaponDictionary.begin();
	WeaponDictionary::iterator End = m_WeaponDictionary.end();

	for(; It != End; ++It)
	{
		delete It->second;
	}

	m_WeaponDictionary.clear();

	m_ClassTypeLookup.clear();
}

i3TDKRegistryCtrl_Impl::WeaponRegBank* 
i3TDKRegistryCtrl_Impl::_GetWeaponRegKey(size_t Number)
{
	WeaponDictionary::iterator DicIt = m_WeaponDictionary.find( m_CurrentWeaponType );
	if( DicIt == m_WeaponDictionary.end() )
	{
		CString Message;
		Message.Format( "The Weapon name[ %s ] does not exist in the Weapons Dictionary !", 
			m_CurrentWeaponType.c_str(), IDOK | MB_DEFBUTTON1 );
		AfxMessageBox(Message);
		return NULL;
	}

	WeaponRegDictionary::iterator RegIt = DicIt->second->find( Number );
	if( RegIt == DicIt->second->end() )
	{
		CString Message;
		Message.Format( "The Weapon number[ %d ] does not exist in the Weapons Registry Dictionary !", Number);
		AfxMessageBox(Message, IDOK | MB_DEFBUTTON1);
		return NULL;
	}

	return &RegIt->second;
}

void i3TDKRegistryCtrl_Impl::DisplayRegistryWeaponDictionary(size_t Number)
{
	WeaponRegBank* pRegBank = _GetWeaponRegKey(Number);

	m_WeaponListViewCtrl.DeleteAllItems();
	m_SubTree.DeleteAllItems();

	m_ImageViewCtrl.ResetScrollPos();
	m_ImageViewCtrl.ResetImage();

	AfxGetMainFrame()->ResetStatusText2();

	if( pRegBank == NULL || pRegBank->empty() )
	{
		CString Message;
		Message.Format( "[ %s - %d ] has no item!", 
			m_CurrentWeaponType.c_str(), Number );
		AfxGetMainFrame()->SetStatusText2( Message );
	}
	else
	{
		size_t size = pRegBank->size();
		for(size_t i=0;i<size;++i)
		{
			i3RegKey* pKey = pRegBank->at(i);

			Rec_AddTree(m_SubTree, TVI_ROOT, pKey );
		}
	}
}

void i3TDKRegistryCtrl_Impl::DisplayWeaponRegString(i3RegKey* pKey)
{
	AfxGetMainFrame()->ResetStatusText3();

	CString Str = _MakeReverseChainString(pKey, '/');

	AfxGetMainFrame()->SetStatusText3( Str );
}

void i3TDKRegistryCtrl_Impl::DisplayWeaponImage(i3RegKey* pKey)
{
	i3RegData * pUiPath = (i3RegData*)i3Registry::FindData( pKey, "UiPath" );

	if( !pUiPath )
	{
		AfxGetMainFrame()->SetStatusText2( "No Image!" );
		return;
	}

	AfxGetMainFrame()->ResetStatusText2();

	i3::rc_wstring wstrImagePath = _GetArrayRegStringValue( (i3RegArray *)pUiPath );

	if( !wstrImagePath.empty() )
	{
		i3::string Path( g_pProfile->m_CurrentFilePath );

		size_t Index = Path.rfind("\\");
		Index = Path.rfind( "\\", Index-1 );

		Path.erase( Index + 1 );

		i3::string strImagePath;		i3::utf16_to_mb(wstrImagePath, strImagePath);
		Path.append( strImagePath );

		CFileFind cFileFinder;
		if (cFileFinder.FindFile( Path.c_str() ) )
		{
			m_ImageViewCtrl.LoadImageFile( (char*)Path.c_str() );
		}
		else
		{
			CString Message;
			Message.Format( "[ %s ] doesn't file exist!", strImagePath.c_str() );
			AfxGetMainFrame()->SetStatusText2( Message );
		}
	}
	else
		AfxGetMainFrame()->SetStatusText2( "No Image!" );

	m_ImageViewCtrl.Invalidate( true );
}



void	i3TDKRegistryCtrl_Impl::Rec_AddClassType( HTREEITEM hParent, i3RegKey * pKey)
{
	i3::vector<i3RegData*>* List = pKey->GetValueList();
	size_t size = List->size();
	for(size_t i=0;i<size;++i)
	{
		const char* ClassName = (*List)[i]->GetName();

		if( i3TDKListCtrl_Impl::IsStampText(ClassName) )
			continue;

		INT32 Value = 0;
		(*List)[i]->GetValue( &Value );

		AddClassType(ClassName, Value);
	}

	for (INT32 i =0; i < pKey->getChildCount(); i++)
	{
		i3RegKey * pChild = (i3RegKey*)pKey->getChild( i);

		Rec_AddClassType( hParent, pChild);
	}

	m_pClassTypeRoot = pKey;
}


void	i3TDKRegistryCtrl_Impl::Rec_AddItem( HTREEITEM hParent, i3RegKey * pKey)
{
	HTREEITEM hItem	= AddTree( m_TreeCtrl, hParent, pKey);

	const char* Name = pKey->GetName();

	i3::vector<i3RegData*>* List = pKey->GetValueList();
	size_t size = List->size();
	for(size_t i=0;i<size;++i)
	{
		const char* WeaponName = (*List)[i]->GetName();

		if( i3TDKListCtrl_Impl::IsStampText(WeaponName) )
			continue;

		INT32 Value = 0;
		(*List)[i]->GetValue( &Value );

		AddWeaponDictionary(Name, Value);
	}

	for (INT32 i =0; i < pKey->getChildCount(); i++)
	{
		i3RegKey * pChild = (i3RegKey*)pKey->getChild( i);

		Rec_AddItem( hItem, pChild);
	}

	m_pItemListRoot = pKey;
}


void	i3TDKRegistryCtrl_Impl::Rec_AddWeapon( HTREEITEM hParent, i3RegKey * pKey)
{
	bool IsAddWeapon = true;
	const char* Name = pKey->GetName();
	if( strcmp(Name, "Weapon") == 0 )
		IsAddWeapon = false;

	if( IsAddWeapon )
	{
		i3RegData * pClassType = (i3RegData*)i3Registry::FindData( pKey, "_ClassType" );
		i3RegData * pNumberType = (i3RegData*)i3Registry::FindData( pKey, "_Number" );

		//등록 해야될 물건들
		if( pClassType && pNumberType )
		{
			INT32 ClassType = _GetArrayRegIntValue( (i3RegArray*)pClassType);

			//등록 해야 되는데 ClassType의 값이 모두 동일하지 않는 경우
			if( !_GetArrayRegIntMatch( (i3RegArray*)pClassType, ClassType ) )
			{
				I3PRINTLOG(I3LOG_FATAL, "The Weapon[ %s ] classtype[ %d ] does not matched all Items!", 
					pKey->GetName(), ClassType );
			}

			INT32  WeaponNumber = _GetArrayRegIntValue( (i3RegArray*)pNumberType);

			//등록 해야 되는데 Number의 값이 모두 동일하지 않는 경우
			if( !_GetArrayRegIntMatch( (i3RegArray*)pNumberType, WeaponNumber ) )
			{
				I3PRINTLOG(I3LOG_FATAL, "The Weapon[ %s ] number[ %d ] does not matched all Items!", 
					pKey->GetName(), WeaponNumber );
			}

			if( ClassType && WeaponNumber && ClassType != DO_NOT_USE_CLASSTYPE )
				RegistryWeaponDictionary(ClassType, WeaponNumber, pKey);
		}
	}

	for (INT32 i =0; i < pKey->getChildCount(); i++)
	{
		i3RegKey * pChild = (i3RegKey*)pKey->getChild( i);
		Rec_AddWeapon( hParent, pChild);
	}

	m_pWeaponRoot = pKey;
}

void i3TDKRegistryCtrl_Impl::SetItemWeaponPef(bool ItemWeaponPef)
{
	if( !ItemWeaponPef )
	{
		g_pProfile->SetComplexUseType( false );
		m_SubTree.ShowWindow( FALSE );
		m_WeaponListViewCtrl.ShowWindow( FALSE );
		m_ImageViewCtrl.ShowWindow( FALSE );
		AfxGetMainFrame()->HideStatusBar();
	}
	else
	{
		g_pProfile->SetComplexUseType( true );
		m_SubTree.ShowWindow( TRUE );
		m_WeaponListViewCtrl.ShowWindow( TRUE );
		m_ImageViewCtrl.ShowWindow( TRUE );
		AfxGetMainFrame()->ShowStatusBar();
	}

	m_bItemWeaponPef = ItemWeaponPef;	
}