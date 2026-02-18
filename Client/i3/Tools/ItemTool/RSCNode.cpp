#include "stdafx.h"
#include "RSCNode.h"
//#include "GlobalVar.h"
#include "i3Base/string/ext/insert_string.h"
#include "i3Base/string/compare/generic_is_niequal.h"

I3_CLASS_INSTANCE( RSCNode); //, i3NamedElement);

RSCNode::RSCNode(void)
{
	m_pParent			= NULL;
	m_Entry.m_pField	= NULL;

	m_MD5 = 0;
}

RSCNode::~RSCNode(void)
{
	RemoveAllChild();

	m_pParent			= NULL;
}

void RSCNode::SetRelativePath( char * pszPath)
{
	i3::string_ncopy_nullpad( m_Entry.m_szFilePath, pszPath, MAX_PATH -1);

	i3MD5::Gen( pszPath, strlen( pszPath), (char *) &m_MD5);
}

void RSCNode::GetFullPath( char * pszPath)
{
	char szTemp[MAX_PATH];
	pszPath[0] = 0;

#if defined ( I3RSC_BROWSER_HIERACHY )
	RSCNode * pNode = this;

	while( pNode->GetParent() != 0)
	{
		if( pszPath[0] != 0)
			sprintf( szTemp, "%s\\", pNode->GetName());
		else
			sprintf( szTemp, "%s", pNode->GetName());

	//	i3String::InsertStr( pszPath, szTemp, 0, (INT32)strlen( szTemp));
		i3::insert_string(pszPath, 0, szTemp);

		pNode = pNode->GetParent();
	}

	sprintf( szTemp, "%s\\", g_pProfile->m_szWorkDir);
	// i3String::InsertStr( pszPath, szTemp, 0, (INT32)strlen( szTemp));
	i3::insert_string(pszPath, 0, szTemp);
#else
	sprintf( szTemp, "%s\\%s", g_strRootPath, m_Entry.m_szFilePath);
	// i3String::InsertStr( pszPath, szTemp, 0, (INT32)strlen( szTemp));
	i3::insert_string(pszPath, 0, szTemp);
#endif
}


void RSCNode::AddChild( RSCNode * pChild)
{
	I3ASSERT( pChild != NULL);

	pChild->AddRef();
	m_ChildList.push_back( pChild);

	pChild->SetParent( this);
}

void RSCNode::RemoveChild( RSCNode * pChild)
{
//	INT32 idx = m_ChildList.IndexOf( pChild);

	i3::vector<RSCNode*>::iterator it = i3::find(m_ChildList.begin(), m_ChildList.end(), pChild);
	I3ASSERT( it != m_ChildList.end() );

	pChild->Release();
//	m_ChildList.Delete( idx);
	m_ChildList.erase(it);
}

void RSCNode::RemoveAllChild( void)
{
	
	RSCNode * pChild;

	for(size_t i = 0; i < m_ChildList.size(); i++)
	{
		pChild = m_ChildList[i];
		
		I3_SAFE_RELEASE( pChild);
	}

	m_ChildList.clear();
}

RSCNode * RSCNode::_Rec_FindChildByName( const char * pszName, const char * pszRelPath)
{
	
	RSCNode * pChild, *pNode;

	if( hasName())
	{
		if( i3::generic_is_iequal( pszName, GetName()) )
		{
			if( i3::generic_is_iequal( pszRelPath, GetRelativePath()) )
				return this;
		}

	}

	for( size_t i = 0; i < m_ChildList.size(); ++i)
	{
		pChild = m_ChildList[i];

		pNode = pChild->_Rec_FindChildByName( pszName, pszRelPath);

		if( pNode != NULL)	
			return pNode;
	}

	return NULL;
}

void RSCNode::GetAllChildCount(INT32 & nCount)
{
	RSCNode * pChild;
	

	nCount += (INT32)m_ChildList.size();

	for(size_t i = 0; i < m_ChildList.size(); ++i)
	{
		pChild = m_ChildList[i];

		pChild->GetAllChildCount( nCount);
	}
}

RSCNode * RSCNode::FindChildByName( const char * pszName)
{
	
	RSCNode * pChild;

	for( size_t i = 0; i < m_ChildList.size(); ++i)
	{
		pChild = m_ChildList[i];

		if( i3::generic_is_iequal( pszName, pChild->GetName()) )
			return pChild;
			
		pChild->FindChildByName( pszName);
	}

	return NULL;
}

void	RSCNode::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	for( INT32 i = 0; i< GetChildCount(); ++i)
	{
		RSCNode * pChild = GetChild( i);

		pChild->OnBuildObjectList( List);
	}

	i3NamedElement::OnBuildObjectList( List);
}

UINT32	RSCNode::Export( i3FileStream * pStream)
{
	UINT32 Rc, Result = 0;

	if( m_Entry.m_Attr & I3RSC_ATTR_FOLDER)
	{
		I3WARN("Exactly\n");
		return Result;
	}

	UINT32 nType = (m_Entry.m_Attr & I3RSC_TYPE_MASK);
	UINT32 nStyle = 0;
	INT32 fieldOffset = 0;
	if( ( nType == I3RSC_TYPE_SOUND) && (g_RSCStyleDef[ nType].m_nFieldPackStyle & I3RSC_FIELDPACK_COMMON) == I3RSC_FIELDPACK_COMMON)
	{
		fieldOffset = 1;
		nStyle = (UINT32) atoi( (const char*)m_Entry.m_pField->GetField( 2)->m_pFieldData);
	}

	I3_PERSIST_RSC_HEADER Header;
	i3mem::FillZero( &Header, sizeof( Header));

	i3mem::Copy( Header.m_ID, "RSC1", sizeof( UINT8) * 4);
	
	Header.m_nType			= m_Entry.m_Attr;

	if( nStyle > 0 )
		Header.m_nType |= 0x00010000;		// Common flag

	Header.m_nNameBufSize	= i3::generic_string_size( GetName());
	Header.m_nFilePathSize	= i3::generic_string_size( m_Entry.m_szFilePath);
	Header.m_pParent		= 0;

	Result = pStream->Write( &Header, sizeof( Header));
	I3_CHKIO( Result);

	if( Header.m_nNameBufSize > 0)
	{
		Rc = pStream->Write( (void*)GetName(), sizeof( char) * Header.m_nNameBufSize + 1);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	if( Header.m_nFilePathSize > 0)
	{
		Rc = pStream->Write( m_Entry.m_szFilePath, sizeof( char) * Header.m_nFilePathSize + 1);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	//Typeº° Save
	if( (nType == I3RSC_TYPE_SCENE) ||  (nType == I3RSC_TYPE_CHARA) || (nType == I3RSC_TYPE_WORLD))
	{
		I3_PERSIST_RSC_SCENE scene;
		i3mem::FillZero( &scene, sizeof( scene));		

		scene.m_nLayer	= *((INT32*)m_Entry.m_pField->GetField( 2 + fieldOffset )->m_pFieldData);
		strcpy(	scene.m_szClass, (char*)m_Entry.m_pField->GetField( 3 + fieldOffset )->m_pFieldData);
		strcpy(	scene.m_szCommon, (char*)m_Entry.m_pField->GetField( 4 + fieldOffset )->m_pFieldData);

		Rc = pStream->Write( &scene, sizeof( I3_PERSIST_RSC_SCENE));
		I3_CHKIO( Rc);
		Result += Rc;
	}
	else if(nType == I3RSC_TYPE_SOUND)
	{
		I3_PERSIST_RSC_SOUND sound;
		i3mem::FillZero( &sound, sizeof( sound));

		sound.m_rMaxDist		= *((REAL32*)m_Entry.m_pField->GetField(2 + fieldOffset)->m_pFieldData);
		sound.m_rMinDist		= *((REAL32*)m_Entry.m_pField->GetField(3 + fieldOffset)->m_pFieldData);
		sound.m_nDefaultVolume	= *((INT32*)m_Entry.m_pField->GetField(4 + fieldOffset)->m_pFieldData);
		sound.m_nListenerVolume = *((INT32*)m_Entry.m_pField->GetField(5 + fieldOffset)->m_pFieldData);

		Rc	= pStream->Write( &sound, sizeof( sound));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

UINT32	RSCNode::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	I3_PERSIST_RSC_HEADER Header;
	i3mem::FillZero( &Header, sizeof( Header));

	i3mem::Copy( Header.m_ID, "RSC1", sizeof( UINT8) * 4);
	Header.m_nType		= m_Entry.m_Attr;

	char * pszName = NULL;
	if( m_Entry.m_Attr & I3RSC_ATTR_FOLDER)
		pszName = (char*)GetName();
	else
		pszName = m_Entry.m_szFilePath;

	Header.m_nNameBufSize	= i3::generic_string_size( GetName());
	Header.m_nFilePathSize	= i3::generic_string_size( pszName);
	Header.m_pParent		= (OBJREF)pResFile->GetObjectPersistID( GetParent());

	Result = pStream->Write( &Header, sizeof( Header));
	I3_CHKIO( Result);

	if( Header.m_nNameBufSize > 0)
	{
		Rc = pStream->Write( (void*)GetName(), sizeof( char) * Header.m_nNameBufSize + 1);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	if( Header.m_nFilePathSize > 0)
	{
		Rc = pStream->Write( pszName, sizeof( char) * Header.m_nFilePathSize + 1);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	if( m_Entry.m_Attr & I3RSC_ATTR_FOLDER)
		return Result;

	//Typeº° Save
	UINT32 nStyle = (m_Entry.m_Attr & I3RSC_TYPE_MASK);
	INT32 fieldOffset = 0;
	if( (m_Entry.m_Attr & I3RSC_FIELDPACK_COMMON) == I3RSC_FIELDPACK_COMMON)
	{
		fieldOffset = 1;
	}

	if( (nStyle == I3RSC_TYPE_SCENE) ||  (nStyle == I3RSC_TYPE_CHARA) || (nStyle == I3RSC_TYPE_WORLD))
	{
		I3_PERSIST_RSC_SCENE scene;
		i3mem::FillZero( &scene, sizeof( scene));

		scene.m_nLayer	= *((INT32*)m_Entry.m_pField->GetField( 2 + fieldOffset)->m_pFieldData);

		strcpy(	scene.m_szClass, (char*)m_Entry.m_pField->GetField( 3 + fieldOffset)->m_pFieldData);
		strcpy(	scene.m_szCommon, (char*)m_Entry.m_pField->GetField( 4 + fieldOffset)->m_pFieldData);

		Rc = pStream->Write( &scene, sizeof( I3_PERSIST_RSC_SCENE));
		I3_CHKIO( Rc);
		Result += Rc;
	}
	else if(nStyle == I3RSC_TYPE_SOUND)
	{
		I3_PERSIST_RSC_SOUND sound;
		i3mem::FillZero( &sound, sizeof( sound));

		sound.m_rMaxDist		= *((REAL32*)m_Entry.m_pField->GetField(2+ fieldOffset)->m_pFieldData);
		sound.m_rMinDist		= *((REAL32*)m_Entry.m_pField->GetField(3+ fieldOffset)->m_pFieldData);
		sound.m_nDefaultVolume	= *((INT32*)m_Entry.m_pField->GetField(4+ fieldOffset)->m_pFieldData);
		sound.m_nListenerVolume = *((INT32*)m_Entry.m_pField->GetField(5+ fieldOffset)->m_pFieldData);

		Rc	= pStream->Write( &sound, sizeof( sound));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

UINT32	RSCNode::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();
	UINT32	nStyle = 0;

	I3_PERSIST_RSC_HEADER Header;
	Result = pStream->Read( &Header, sizeof( Header));
	I3_CHKIO( Result);

	INT32 nVersion = 1;

	if( i3::generic_is_niequal( (char*)Header.m_ID, "RSC1", 4) )
		nVersion = 1;
	else if( i3::generic_is_niequal( (char*) Header.m_ID, "RSC3", 4) )
	{
		nVersion = 3;
		if( pStream->Read( &nStyle, sizeof( nStyle)) == STREAM_ERR)
			return STREAM_ERR;

		SetStyle( nStyle);
	}
	else
		return STREAM_ERR;

	m_Entry.m_Attr	= Header.m_nType;
	if( Header.m_pParent != NULL)
	{
		RSCNode * pParent = (RSCNode *)pResFile->FindObjectByID( Header.m_pParent);
		I3ASSERT( pParent);

		pParent->AddChild( this);
	}

	char szTemp[MAX_PATH];
	if( Header.m_nNameBufSize > 0)
	{
		Rc = pStream->Read( szTemp, sizeof( char) * Header.m_nNameBufSize + 1);
		I3_CHKIO( Rc);
		Result += Rc;

		SetName( szTemp);
	}

	if( Header.m_nFilePathSize > 0)
	{
		Rc = pStream->Read( m_Entry.m_szFilePath, sizeof( char) * Header.m_nFilePathSize + 1);
		I3_CHKIO( Rc);
		Result += Rc;

		if( Header.m_nNameBufSize <= 0)
		{
			SetName( m_Entry.m_szFilePath);
		}
	}

	if( m_Entry.m_Attr & I3RSC_ATTR_FOLDER)
	{
		return Result;
	}

	nStyle = m_Entry.m_Attr & I3RSC_TYPE_MASK;

	m_Entry.m_pField = CFieldPack::new_object();
	m_Entry.m_pField->Create( nStyle);

	RSC_FIELD_INFO * pInfo = ( RSC_FIELD_INFO*)m_Entry.m_pField->GetField( 1);
	strcpy( (char*)pInfo->m_pFieldData ,m_Entry.m_szFilePath);

	if((nStyle == I3RSC_TYPE_SCENE) || (nStyle == I3RSC_TYPE_CHARA) || (nStyle == I3RSC_TYPE_WORLD))
	{
		I3_PERSIST_RSC_SCENE scene;

		Rc = pStream->Read( &scene, sizeof( scene));
		I3_CHKIO( Rc);
		Result += Rc;

		{
			pInfo = ( RSC_FIELD_INFO*)m_Entry.m_pField->GetField( 2);
			*((INT32*)pInfo->m_pFieldData) = scene.m_nLayer;

			pInfo = ( RSC_FIELD_INFO*)m_Entry.m_pField->GetField( 3);
			strcpy( (char*)pInfo->m_pFieldData, scene.m_szClass);

			pInfo = ( RSC_FIELD_INFO*)m_Entry.m_pField->GetField( 4);
			strcpy( (char*)pInfo->m_pFieldData, scene.m_szCommon);
		}
	}
	else if(nStyle == I3RSC_TYPE_SOUND)
	{
#if 0
		I3_PERSIST_RSC_SOUND1 sound;

		Rc = pStream->Read( &sound, sizeof( I3_PERSIST_RSC_SOUND1));
		if( Rc == STREAM_ERR)
		{
			return STREAM_ERR;
		}
		Result += Rc;

		{
			pInfo = ( RSC_FIELD_INFO*)m_Entry.m_pField->GetField( 2);
			*((REAL32*)pInfo->m_pFieldData) = sound.m_rMaxDist;

			pInfo = ( RSC_FIELD_INFO*)m_Entry.m_pField->GetField( 3);
			*((REAL32*)pInfo->m_pFieldData) = sound.m_rMinDist;
		}
#else
		I3_PERSIST_RSC_SOUND sound;

		Rc = pStream->Read( &sound, sizeof( I3_PERSIST_RSC_SOUND));
		if( Rc == STREAM_ERR)
		{
			return STREAM_ERR;
		}
		Result += Rc;

		{
			pInfo = ( RSC_FIELD_INFO*)m_Entry.m_pField->GetField( 2);
			*((REAL32*)pInfo->m_pFieldData) = sound.m_rMaxDist;

			pInfo = ( RSC_FIELD_INFO*)m_Entry.m_pField->GetField( 3);
			*((REAL32*)pInfo->m_pFieldData) = sound.m_rMinDist;

			pInfo = ( RSC_FIELD_INFO*)m_Entry.m_pField->GetField( 4);
			*((INT32*)pInfo->m_pFieldData) = sound.m_nDefaultVolume;

			pInfo = ( RSC_FIELD_INFO*)m_Entry.m_pField->GetField( 5);
			*((INT32*)pInfo->m_pFieldData) = sound.m_nListenerVolume;
		}
#endif
	}

	return Result;
}

void RSCNode::ScanAllNode( i3::vector_set<RSCNode*, RSCNode_NodeComp>& List )
{
	List.insert( this);

	INT32 i;

	for( i = 0; i < GetChildCount(); i++)
	{
		RSCNode * pChild = GetChild( i);

		pChild->ScanAllNode( List);
	}
}
