#include "i3SceneDef.h"
#include "i3RegKey.h"
#include "i3RegData.h"
#include <algorithm>

#include "i3Registry.h"
#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/generic_string_cat.h"

I3_CLASS_INSTANCE( i3RegKey);

i3RegKey::i3RegKey(void)
{
	m_vecValueList.clear();
}

i3RegKey::~i3RegKey(void)
{
	i3::cu::for_each_SafeRelease_clear(m_vecValueList);

	ClearStampList();
}

void	 i3RegKey::ClearStampList()
{
	i3::cu::for_each_SafeRelease_clear(m_StampValueList);
}

void		i3RegKey::AddData( const char* szName, INT32 val)
{
	i3RegINT32 * regData = i3RegINT32::new_object();	
	regData->SetName(szName);
	*regData = val;
	AddData( regData);
}
void		i3RegKey::AddData( const char* szName, REAL32 val)
{
	i3RegREAL32 * regData = i3RegREAL32::new_object();	
	regData->SetName(szName);
	*regData = val;
	AddData( regData);
}
void		i3RegKey::AddData( const char* szName, const i3::rc_wstring& val)
{
	i3RegString * regData = i3RegString::new_object();	
	regData->SetName(szName);
	*regData = val;
	AddData( regData);
}
void		i3RegKey::AddData( const char* szName, VEC2D val)
{
	i3RegVEC2D * regData = i3RegVEC2D::new_object();	
	regData->SetName(szName);
	*regData = val;
	AddData( regData);
}
void		i3RegKey::AddData( const char* szName, VEC3D val)
{
	i3RegVEC3D * regData = i3RegVEC3D::new_object();	
	regData->SetName(szName);
	*regData = val;
	AddData( regData);
}
void		i3RegKey::AddData( const char* szName, VEC4D val)
{
	i3RegVEC4D * regData = i3RegVEC4D::new_object();	
	regData->SetName(szName);
	*regData = val;
	AddData( regData);
}
void		i3RegKey::AddData( const char* szName, COLOR val)
{
	i3RegCOLOR * regData = i3RegCOLOR::new_object();	
	regData->SetName(szName);
	*regData = val;
	AddData( regData);
}
//나중에 사용할지 모름
//void		i3RegKey::AddData( const char* szName, i3RegMATRIX val)
//{
//	i3RegMATRIX * regData = i3RegMATRIX::new_object();	
//	regData->SetName(szName);
//	*regData = val;
//	AddData( regData);
//}


void		i3RegKey::AddDataMulti( const char* szName, INT32 val)
{
	i3RegArray * pNewArrayData = i3RegArray::new_object();
	pNewArrayData->SetName( szName );
	pNewArrayData->setElementType( I3REG_TYPE_INT32 );

	i3RegINT32 * regData = i3RegINT32::new_object();
	*regData = val;
	pNewArrayData->addRegData( regData );

	AddData( pNewArrayData );
}
void		i3RegKey::AddDataMulti( const char* szName, REAL32 val)
{
	i3RegArray * pNewArrayData = i3RegArray::new_object();
	pNewArrayData->SetName( szName );
	pNewArrayData->setElementType( I3REG_TYPE_REAL32 );

	i3RegREAL32 * regData = i3RegREAL32::new_object();
	*regData = val;
	pNewArrayData->addRegData( regData );

	AddData( pNewArrayData );
}
void		i3RegKey::AddDataMulti( const char* szName, const i3::rc_wstring& val)
{
	i3RegArray * pNewArrayData = i3RegArray::new_object();
	pNewArrayData->SetName( szName );
	pNewArrayData->setElementType( I3REG_TYPE_STRING );

	i3RegString * regData = i3RegString::new_object();
	*regData = val;
	pNewArrayData->addRegData( regData );

	AddData( pNewArrayData );
}
void		i3RegKey::AddDataMulti( const char* szName, VEC2D val)
{
	i3RegArray * pNewArrayData = i3RegArray::new_object();
	pNewArrayData->SetName( szName );
	pNewArrayData->setElementType( I3REG_TYPE_VEC2D );

	i3RegVEC2D * regData = i3RegVEC2D::new_object();
	*regData = val;
	pNewArrayData->addRegData( regData );

	AddData( pNewArrayData );
}
void		i3RegKey::AddDataMulti( const char* szName, VEC3D val)
{
	i3RegArray * pNewArrayData = i3RegArray::new_object();
	pNewArrayData->SetName( szName );
	pNewArrayData->setElementType( I3REG_TYPE_VEC3D );

	i3RegVEC3D * regData = i3RegVEC3D::new_object();
	*regData = val;
	pNewArrayData->addRegData( regData );

	AddData( pNewArrayData );
}
void		i3RegKey::AddDataMulti( const char* szName, VEC4D val)
{
	i3RegArray * pNewArrayData = i3RegArray::new_object();
	pNewArrayData->SetName( szName );
	pNewArrayData->setElementType( I3REG_TYPE_VEC4D );

	i3RegVEC4D * regData = i3RegVEC4D::new_object();
	*regData = val;
	pNewArrayData->addRegData( regData );

	AddData( pNewArrayData );
}
void		i3RegKey::AddDataMulti( const char* szName, COLOR val)
{
	i3RegArray * pNewArrayData = i3RegArray::new_object();
	pNewArrayData->SetName( szName );
	pNewArrayData->setElementType( I3REG_TYPE_COLOR );

	i3RegCOLOR * regData = i3RegCOLOR::new_object();
	*regData = val;
	pNewArrayData->addRegData( regData );

	AddData( pNewArrayData );
}

//나중에 사용할지 모름
//void		i3RegKey::AddDataMulti( const char* szName, i3RegMATRIX val)
//{
//	i3RegArray * pNewArrayData = i3RegArray::new_object();
//	pNewArrayData->SetName( szName );
//	pNewArrayData->setElementType( I3REG_TYPE_MATRIX );
//
//	i3RegMATRIX * regData = i3RegMATRIX::new_object();
//	*regData = val;
//	pNewArrayData->addRegData( regData );
//
//	AddData( pNewArrayData );
//}


bool i3RegKey::GetData( const char * pszName, INT32 * val)
{
	i3RegData * pData = FindDataByName( pszName);

	if( pData != nullptr)
	{
		if( pData->getDataType() == I3REG_TYPE_ARRAY )
		{
			i3RegArray* pArrayData = (i3RegArray*)pData;

			if( pArrayData->getSize() == 1 )
				(*pArrayData)[ 0 ]->GetValue( val );
			else
				(*pArrayData)[ i3Registry::Get_LocationIndex() ]->GetValue( val );
		}
		else
			pData->GetValue( val);

		return true;
	}
	else	return false;
}
bool i3RegKey::GetData( const char * pszName, REAL32 * val)
{
	i3RegData * pData = FindDataByName( pszName);

	if( pData != nullptr)
	{
		if( pData->getDataType() == I3REG_TYPE_ARRAY )
		{
			i3RegArray* pArrayData = (i3RegArray*)pData;

			if( pArrayData->getSize() == 1 )
				(*pArrayData)[ 0 ]->GetValue( val );
			else
				(*pArrayData)[ i3Registry::Get_LocationIndex() ]->GetValue( val );
		}
		else
			pData->GetValue( val);

		return true;
	}
	else	return false;
}
bool i3RegKey::GetData( const char * pszName, i3::rc_wstring& outVal)
{
	i3RegData * pData = FindDataByName( pszName);

	if( pData != nullptr)
	{
		if( pData->getDataType() == I3REG_TYPE_ARRAY )
		{
			i3RegArray* pArrayData = (i3RegArray*)pData;

			if( pArrayData->getSize() == 1 )
				(*pArrayData)[ 0 ]->GetValue( outVal );
			else
				(*pArrayData)[ i3Registry::Get_LocationIndex() ]->GetValue( outVal );
		}
		else
			pData->GetValue( outVal );

		return true;
	}
	else	return false;
}

bool i3RegKey::GetData( const char * pszName, VEC2D * val)
{
	i3RegData * pData = FindDataByName( pszName);

	if( pData != nullptr)
	{
		if( pData->getDataType() == I3REG_TYPE_ARRAY )
		{
			i3RegArray* pArrayData = (i3RegArray*)pData;

			if( pArrayData->getSize() == 1 )
				(*pArrayData)[ 0 ]->GetValue( val );
			else
				(*pArrayData)[ i3Registry::Get_LocationIndex() ]->GetValue( val );
		}
		else
			pData->GetValue( val);

		return true;
	}
	else	return false;
}
bool i3RegKey::GetData( const char * pszName, VEC3D * val)
{
	i3RegData * pData = FindDataByName( pszName);

	if( pData != nullptr)
	{
		if( pData->getDataType() == I3REG_TYPE_ARRAY )
		{
			i3RegArray* pArrayData = (i3RegArray*)pData;

			if( pArrayData->getSize() == 1 )
				(*pArrayData)[ 0 ]->GetValue( val );
			else
				(*pArrayData)[ i3Registry::Get_LocationIndex() ]->GetValue( val );
		}
		else
			pData->GetValue( val);

		return true;
	}
	else	return false;
}
bool i3RegKey::GetData( const char * pszName, VEC4D * val)
{
	i3RegData * pData = FindDataByName( pszName);

	if( pData != nullptr)
	{
		if( pData->getDataType() == I3REG_TYPE_ARRAY )
		{
			i3RegArray* pArrayData = (i3RegArray*)pData;

			if( pArrayData->getSize() == 1 )
				(*pArrayData)[ 0 ]->GetValue( val );
			else
				(*pArrayData)[ i3Registry::Get_LocationIndex() ]->GetValue( val );
		}
		else
			pData->GetValue( val);

		return true;
	}
	else	return false;
}
bool i3RegKey::GetData( const char * pszName, I3COLOR * val)
{
	i3RegData * pData = FindDataByName( pszName);

	if( pData != nullptr)
	{
		if( pData->getDataType() == I3REG_TYPE_ARRAY )
		{
			i3RegArray* pArrayData = (i3RegArray*)pData;

			if( pArrayData->getSize() == 1 )
				(*pArrayData)[ 0 ]->GetValue( val );
			else
				(*pArrayData)[ i3Registry::Get_LocationIndex() ]->GetValue( val );
		}
		else
			pData->GetValue( val);

		return true;
	}
	else	return false;
}
//나중에 사용할지 모름
//bool i3RegKey::GetData( const char * pszName, MATRIX * val)
//{
//	i3RegData * pData = FindDataByName( pszName);
//
//	if( pData != nullptr)
//	{
//		if( pData->getDataType() == I3REG_TYPE_ARRAY )
//		{
//			i3RegArray* pArrayData = (i3RegArray*)pData;
//
//			(*pArrayData)[ i3Registry::Get_LocationIndex() ]->GetValue( val );
//		}
//		else
//			pData->GetValue( val);
//
//		return true;
//	}
//	else	return false;
//}


bool i3RegKey::GetDataMulti( const char * pszName, INT32 * val, size_t Index)
{
	i3RegData * pData = FindDataByName( pszName);

	if( pData != nullptr)
	{
		if( pData->getDataType() == I3REG_TYPE_ARRAY )
		{
			i3RegArray* pArrayData = (i3RegArray*)pData;

			(*pArrayData)[ Index ]->GetValue( val );

			return true;
		}
	}
	return false;
}
bool i3RegKey::GetDataMulti( const char * pszName, REAL32 * val, size_t Index)
{
	i3RegData * pData = FindDataByName( pszName);

	if( pData != nullptr)
	{
		if( pData->getDataType() == I3REG_TYPE_ARRAY )
		{
			i3RegArray* pArrayData = (i3RegArray*)pData;

			(*pArrayData)[ Index ]->GetValue( val );

			return true;
		}
	}
	return false;
}
bool i3RegKey::GetDataMulti( const char * pszName, i3::rc_wstring& outVal, size_t Index)
{
	i3RegData * pData = FindDataByName( pszName);

	if( pData != nullptr)
	{
		if( pData->getDataType() == I3REG_TYPE_ARRAY )
		{
			i3RegArray* pArrayData = (i3RegArray*)pData;

			(*pArrayData)[ Index ]->GetValue( outVal );

			return true;
		}
	}
	return false;
}
bool i3RegKey::GetDataMulti( const char * pszName, VEC2D * val, size_t Index)
{
	i3RegData * pData = FindDataByName( pszName);

	if( pData != nullptr)
	{
		if( pData->getDataType() == I3REG_TYPE_ARRAY )
		{
			i3RegArray* pArrayData = (i3RegArray*)pData;
			
			(*pArrayData)[ Index ]->GetValue( val );

			return true;
		}
	}
	return false;
}
bool i3RegKey::GetDataMulti( const char * pszName, VEC3D * val, size_t Index)
{
	i3RegData * pData = FindDataByName( pszName);

	if( pData != nullptr)
	{
		if( pData->getDataType() == I3REG_TYPE_ARRAY )
		{
			i3RegArray* pArrayData = (i3RegArray*)pData;

			(*pArrayData)[ Index ]->GetValue( val );

			return true;
		}
	}
	return false;
}
bool i3RegKey::GetDataMulti( const char * pszName, VEC4D * val, size_t Index)
{
	i3RegData * pData = FindDataByName( pszName);

	if( pData != nullptr)
	{
		if( pData->getDataType() == I3REG_TYPE_ARRAY )
		{
			i3RegArray* pArrayData = (i3RegArray*)pData;

			(*pArrayData)[ Index ]->GetValue( val );
		
			return true;
		}
	}
	return false;
}
bool i3RegKey::GetDataMulti( const char * pszName, I3COLOR * val, size_t Index)
{
	i3RegData * pData = FindDataByName( pszName);

	if( pData != nullptr)
	{
		if( pData->getDataType() == I3REG_TYPE_ARRAY )
		{
			i3RegArray* pArrayData = (i3RegArray*)pData;

			(*pArrayData)[ Index ]->GetValue( val );

			return true;
		}
	}
	return false;
}
//나중에 사용할지 모름
//bool i3RegKey::GetDataMulti( const char * pszName, MATRIX * val, size_t Index)
//{
//	i3RegData * pData = FindDataByName( pszName);
//
//	if( pData != nullptr)
//	{
//		if( pData->getDataType() == I3REG_TYPE_ARRAY )
//		{
//			i3RegArray* pArrayData = (i3RegArray*)pData;
//
//			(*pArrayData)[ Index ]->GetValue( val );
//
//			return true;
//		}
//	}
//	return false;
//}

void	i3RegKey::AddData(i3RegData * p)
{
	m_vecValueList.push_back(p);
}

i3RegData * i3RegKey::GetData( INT32 nIdx)
{
	i3RegData* pData = m_vecValueList.at(nIdx);

	return pData;
}

void i3RegKey::RemoveData( i3RegData * p)
{
	if( p == nullptr)	return;

	i3::vector<i3RegData*>::iterator itor = i3::find(m_vecValueList.begin(), m_vecValueList.end(), p);

	if (itor != m_vecValueList.end() )		// 안전하게 if검사해야함.(2013.04.05.수빈)
	{
		m_vecValueList.erase(itor);
		I3_SAFE_RELEASE( p);
	}
}

void i3RegKey::RemoveData( INT32	nIdx)
{
	
	i3RegData* pData = m_vecValueList.at(nIdx);
	I3_SAFE_RELEASE(pData);
	i3::vector<i3RegData*>::iterator itor = m_vecValueList.begin()+nIdx;
	m_vecValueList.erase(itor);

}

i3::vector<i3RegData*>::iterator i3RegKey::FindItorByName( const char * pszName)
{
	i3::vector<i3RegData*>::iterator itor = m_vecValueList.end();
	
	if( pszName == nullptr)	
	{
		return itor;
	}
	
	i3RegData* pData = nullptr;

	for(itor=m_vecValueList.begin(); itor!=m_vecValueList.end(); itor++)
	{
		pData = (i3RegData*)*itor;

		if( pData->hasName())
		{
			if(i3::generic_is_iequal( pData->GetNameString(), pszName) )
			{
				return itor;
			}
		}

	}

	return itor;
}


i3RegData * i3RegKey::FindDataByName( const char * pszName) const
{
	if( pszName == nullptr)	return nullptr;

	i3::vector<i3RegData*>::const_iterator itor;
	i3RegData* pData = nullptr;

	for(itor=m_vecValueList.begin(); itor!=m_vecValueList.end(); itor++)
	{
		pData = (i3RegData*)*itor;

		if( pData->hasName())
		{
			if( i3::generic_is_iequal( pData->GetNameString(), pszName) )
			{
				return pData;
			}
		}
	}

	return nullptr;
}

INT32	i3RegKey::GetDataCount()
{
	return (INT32)m_vecValueList.size();
}

void	i3RegKey::SetData( i3RegData * p, INT32 nIdx)
{
	i3::vector<i3RegData*>::iterator itor;

	itor = m_vecValueList.begin()+nIdx;

	i3RegData* pData = *itor;
	*itor = p;

	I3_SAFE_RELEASE( pData);
}

void	i3RegKey::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	I3ASSERT( i3::kind_of<i3RegKey*>(pDest)); 

    i3TreeNode::CopyTo( pDest, method);

	i3RegKey * pDstKey = (i3RegKey*)pDest;
	INT32 nCount = GetDataCount();
	for( INT32 i = 0; i < nCount; ++i)
	{
		i3RegData * pData = m_vecValueList.at(i);

		i3RegData * pNew = pData->create_instance(); //(i3RegData *)CREATEINSTANCE( pMeta);
		I3_MUST_ADDREF( pNew);
		pData->CopyTo( pNew, method);
		pDstKey->AddData( pNew);
	}
}

void	i3RegKey::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	for(UINT32 i=0; i<m_vecValueList.size(); i++)
	{
		i3RegData* pData = m_vecValueList.at(i);
		pData->OnBuildObjectList( List);
	}
	/*if( m_pValueList != nullptr)
	{
		INT32 nCount = m_pValueList->GetCount();
		for( INT32 i = 0; i < nCount; ++i)
		{
			i3RegData * pData = (i3RegData*)m_pValueList->GetItem( i);
			pData->OnBuildObjectList( pList);
		}
	}*/

	i3TreeNode::OnBuildObjectList( List);
}

#if defined ( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED KEY
	{
		UINT8	m_ID[4] = { 'R', 'G', 'K', '1' };
		INT32	m_nValueCount = 0;
		UINT32	pad[2] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif


UINT32	i3RegKey::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Result, Rc = 0;
	pack::KEY data;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3TreeNode::OnSave( pResFile);
	I3_CHKIO( Result);
	
	//if( m_pValueList != nullptr)
	{
		// data.m_nValueCount = m_pValueList->GetCount();
		data.m_nValueCount = (INT32)m_vecValueList.size();
	}

	Rc = pStream->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	for( INT32 i = 0; i< data.m_nValueCount; ++i)
	{
		//i3RegData * pData = ( i3RegData*)m_pValueList->GetItem( i);
		i3RegData * pData = m_vecValueList.at(i);
		I3ASSERT( pData != nullptr);

		OBJREF64 ref = (OBJREF64)pResFile->GetObjectPersistID( pData);
		Rc = pStream->Write( &ref, sizeof( OBJREF64));
		I3_CHKIO( Rc);
		Result += Rc;
	}	
	
	return Result;
}

UINT32	i3RegKey::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	pack::KEY data;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3TreeNode::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.m_nValueCount > 0)
	{
		OBJREF64 ref;

		for( INT32 i = 0; i< data.m_nValueCount; ++i)
		{
			Rc = pStream->Read( &ref, sizeof( OBJREF64));
			I3_CHKIO( Rc);
			Result += Rc;

			if( ref != 0)
			{
				i3RegData * pChild = (i3RegData*)pResFile->FindObjectByID( (OBJREF)ref);
				I3ASSERT( pChild);

				if( !i3Registry::IsStampString( pChild->GetName() ) )
				{
					m_vecValueList.push_back(pChild);
				}
				else
				{
					m_StampValueList.push_back(pChild);
				}

				I3_SAFE_ADDREF( pChild);
			}
		}
	}

	
	return Result;
}





///////////////////////////////////////////////////////////////////////////////////////////////////
//I3_CLASS_INSTANCE( i3RegKeyFile, i3NamedElement);
I3_CLASS_INSTANCE( i3RegKeyFile);

i3RegKeyFile::~i3RegKeyFile()
{
	I3_SAFE_RELEASE( m_pRootKey);
}

void i3RegKeyFile::setRoot( i3RegKey * pKey)
{
	I3_REF_CHANGE( m_pRootKey, pKey);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct REGKEYFILE
	{
		UINT8		m_ID[4] = { 'R', 'K', 'F', '1' };
		OBJREF64	m_pRoot = 0;
		UINT32		pad[4] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

void	i3RegKeyFile::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	if( m_pRootKey != nullptr)
	{
		m_pRootKey->OnBuildObjectList( List);
	}

	i3NamedElement::OnBuildObjectList( List);
}

UINT32	i3RegKeyFile::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;
	pack::REGKEYFILE data;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3NamedElement::OnSave( pResFile);
	I3_CHKIO( Result);

	data.m_pRoot = (OBJREF64)pResFile->GetObjectPersistID( m_pRootKey);

	Rc = pStream->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32	i3RegKeyFile::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc =0;
	pack::REGKEYFILE data;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3NamedElement::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

//	if( i3String::NCompare( (char*)data.m_ID, "RKF1", 4) != 0)
	if (i3::generic_is_niequal( (char*)data.m_ID, "RKF1", 4) == false)
	{
		I3TRACE("Not a i3RegKeyFile\n");
		return STREAM_ERR;
	}

	if( data.m_pRoot != 0)
	{
		i3RegKey * pKey = (i3RegKey*)pResFile->FindObjectByID( (OBJREF)data.m_pRoot);
		I3ASSERT( pKey != nullptr);

		setRoot( pKey);
	}

	return Result;
}

UINT32	i3RegKeyFile::OnExportSQLCreate( const char * pFileName )
{
	UINT32 Result = 0; 
	i3FileStream  FileStream; 
	char TableName[128]; 
	char TempLine[512]; 

	if( !FileStream.Create( pFileName, STREAM_WRITE ) )
	{
		FileStream.Close(); 
		return STREAM_ERR;  
	}

	//Set Table Name 
	i3::safe_string_copy( TableName, m_pRootKey->GetNameString(), 128 ); 
	i3::generic_string_cat(TableName, "_Table"); 
	sprintf( TempLine,"// Table Name ; %s",  TableName); 
	Result += FileStream.WriteLine( TempLine );

    //
	i3RegKey * pChildKey; 

	i3::vector<i3TreeNode*>& List = m_pRootKey->getChildList(); 

	for(INT32 i = 0; i < m_pRootKey->getChildCount(); i++)
	{
		pChildKey = (i3RegKey*)List[i];
		i3::snprintf( TempLine, 512, "// Data Name ; %s",  pChildKey->GetNameString()); 
		Result += FileStream.WriteLine( TempLine );		
				
		INT32 nDataCount = pChildKey->GetDataCount();
		for( INT32 j = 0; j < nDataCount; j++)
		{
			i3RegData * pData = pChildKey->GetData( j );
			I3TRACE("%s \n",pData->GetName()); 			
		}
		//결정되면 이 부분 소스를 만들어요..성백

		//sprintf(Query, "UPDATE ", );
	}

	FileStream.Close(); 
	return Result; 
}

UINT32	i3RegKeyFile::OnExportSQLUpdate( const char * pFileName  )
{
	UINT32 Result = 0; 

//	FileStream.Close(); 
	return Result; 
}

