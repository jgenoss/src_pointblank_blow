#include "stdafx.h"
#include "RSCNode.h"
//#include "GlobalVar.h"
#include "i3Base/string/ext/insert_string.h"
#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/itl/algorithm/replace.h"

I3_CLASS_INSTANCE( RSCNode); //, i3NamedElement);

RSCNode::RSCNode(void) : m_pParent(NULL)
{

}

RSCNode::~RSCNode(void)
{
	RemoveAllChild();
}

void RSCNode::SetRelativePath( const i3::rc_string& relPath )
{
	m_Entry.m_strFilePath = relPath;
}

i3::fixed_string	RSCNode::CalcKeyFixedString(const i3::rc_string& relPath)
{
	// m_MD5가 너무 거지같다고 판단되서...unordered_map + fixed_string 조합으로 변경..
	i3::stack_string str = relPath;
	i3::replace(str.begin(), str.end(), '\\', '/');
	i3::to_upper(str);

	return i3::fixed_string(str);
}


//g_pProfile->m_strWorkDir

void RSCNode::GetFullPath( const i3::const_char_range& work_path, i3::string& outFullPath)
{
	i3::sprintf(outFullPath, "%s\\%s", work_path, m_Entry.m_strFilePath);
}


void RSCNode::AddChild( RSCNode * pChild)
{
	I3ASSERT( pChild != NULL);

	pChild->AddRef();

	if (pChild->GetParent())
	{
		pChild->GetParent()->RemoveChild(pChild);
	}

	m_ChildList.push_back( pChild);
	pChild->SetParent( this);
}

void	RSCNode::SetChild(INT32 idx, RSCNode* newChild)
{
	newChild->AddRef();
	
	if (newChild->GetParent())
		newChild->GetParent()->RemoveChild(newChild);

	m_ChildList[idx]->Release();

	newChild->m_pParent = this;
	m_ChildList[idx] = newChild;
}


INT32	RSCNode::FindChildIndex(RSCNode* child)
{
	i3::vector<RSCNode*>::iterator it = i3::find(m_ChildList.begin(), m_ChildList.end(), child);
	return  ( it != m_ChildList.end() ) ? i3::distance(m_ChildList.begin(), it) : -1;
}


void RSCNode::RemoveChild( RSCNode * pChild)
{
	i3::vector<RSCNode*>::iterator it = i3::find(m_ChildList.begin(), m_ChildList.end(), pChild);
	I3ASSERT( it != m_ChildList.end() );

	pChild->m_pParent = NULL;
	pChild->Release();
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
	}

	return NULL;
}



UINT32	RSCNode::Export( i3FileStream * pStream)
{
	UINT32 Rc, Result = 0;

	if( m_Entry.m_Attr & I3RSC_ATTR_FOLDER)
	{
		I3PRINTLOG(I3LOG_WARN, "Exactly\n");
		return Result;
	}

	UINT32 nType = (m_Entry.m_Attr & I3RSC_TYPE_MASK);
	UINT32 nStyle = 0;
	INT32 fieldOffset = 0;
	if( ( nType == I3RSC_TYPE_SOUND) && 
		(g_RSCStyleDef[ nType].m_nFieldPackStyle & I3RSC_FIELDPACK_COMMON) == I3RSC_FIELDPACK_COMMON)
	{
		fieldOffset = 1;			
		// I3RSC_FIELDPACK_COMMON은 문자열값임 "0" or "1" ...
		// 사운드 아닌 다른 것들은 좀더 아래쪽의 szCommon에서 처리된다..
		i3::rc_string* strVal = i3::unsafe_any_cast<i3::rc_string>(&m_Entry.m_spField->GetField(2)->m_FieldData);
		nStyle = (UINT32) atoi( strVal->c_str() );
	}

	I3_PERSIST_RSC_HEADER Header;
	i3mem::FillZero( &Header, sizeof( Header));

	i3mem::Copy( Header.m_ID, "RSC1", sizeof( UINT8) * 4);
	
	Header.m_nType			= m_Entry.m_Attr;

	if( nStyle > 0 )
		Header.m_nType |= 0x00010000;		// Common flag

	Header.m_nNameBufSize	= i3::generic_string_size( GetName());
	Header.m_nFilePathSize	= i3::generic_string_size( m_Entry.m_strFilePath);
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
		Rc = pStream->Write( m_Entry.m_strFilePath.c_str(), sizeof( char) * Header.m_nFilePathSize + 1);
		I3_CHKIO( Rc);
		Result += Rc;
	}

	//Type별 Save
	if( (nType == I3RSC_TYPE_SCENE) ||  (nType == I3RSC_TYPE_CHARA) || (nType == I3RSC_TYPE_WORLD))
	{
		I3_PERSIST_RSC_SCENE scene;
		i3mem::FillZero( &scene, sizeof( scene));		

		scene.m_nLayer	= *i3::unsafe_any_cast<INT32>(&m_Entry.m_spField->GetField(2+fieldOffset)->m_FieldData);

		const i3::rc_string* strVal = i3::unsafe_any_cast<i3::rc_string>(&m_Entry.m_spField->GetField(3+fieldOffset)->m_FieldData);
		i3::safe_string_copy(scene.m_szClass, *strVal, sizeof(scene.m_szClass));
			
		strVal = i3::unsafe_any_cast<i3::rc_string>(&m_Entry.m_spField->GetField(4+fieldOffset)->m_FieldData);
		i3::safe_string_copy(scene.m_szCommon, *strVal, sizeof(scene.m_szCommon));


		Rc = pStream->Write( &scene, sizeof( I3_PERSIST_RSC_SCENE));
		I3_CHKIO( Rc);
		Result += Rc;
	}
	else if(nType == I3RSC_TYPE_SOUND)
	{
		I3_PERSIST_RSC_SOUND sound;
		i3mem::FillZero( &sound, sizeof( sound));

		sound.m_rMaxDist		= *i3::unsafe_any_cast<float>(&m_Entry.m_spField->GetField(2 + fieldOffset)->m_FieldData);
		sound.m_rMinDist		= *i3::unsafe_any_cast<float>(&m_Entry.m_spField->GetField(3 + fieldOffset)->m_FieldData);

		sound.m_nDefaultVolume	= *i3::unsafe_any_cast<int>(&m_Entry.m_spField->GetField(4 + fieldOffset)->m_FieldData);
		sound.m_nListenerVolume = *i3::unsafe_any_cast<int>(&m_Entry.m_spField->GetField(5 + fieldOffset)->m_FieldData);

		Rc	= pStream->Write( &sound, sizeof( sound));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}


void RSCNode::GatherAllNode( i3::vector<RSCNode*>& List )
{
	List.push_back( this);
	const INT32 num_child = GetChildCount();
	for( INT32 i = 0; i <num_child; ++i)
		GetChild( i)->GatherAllNode( List);
}

void	RSCNode::GatherAllNode_NonFolder( i3::vector<RSCNode*>& List)
{
	if (m_pParent)		// 없으면 root이겠지..그건 제외..
	{
		RSC_DATA* data = this->GetData();
		if ( ( data->m_Attr & I3RSC_ATTR_FOLDER) == 0 )
			List.push_back(this);
	}

	const INT32 num_child = this->GetChildCount();
	for (INT32  i = 0 ; i < num_child ; ++i)
		m_ChildList[i]->GatherAllNode_NonFolder(List);
}




void	RSCNode::GatherAllNodeToPathMap( i3::unordered_map<i3::fixed_string, RSCNode*>& inout)
{
	// 일단 insert 실패는 무시한다고 가정하고 처리한다..
	i3::fixed_string strKey = CalcKeyFixedString(m_Entry.m_strFilePath);
	
	inout.insert(i3::make_pair(strKey, this));		// 리턴값은 무시한다..

	const INT32 num_child = GetChildCount();
	for( INT32 i = 0; i <num_child; ++i)
		GetChild( i)->GatherAllNodeToPathMap( inout);		
}

void	RSCNode::AddThisToPathMap(i3::unordered_map<i3::fixed_string, RSCNode*>& inout)
{
	i3::fixed_string strKey = CalcKeyFixedString(m_Entry.m_strFilePath);
	inout.insert(i3::make_pair(strKey, this));		// 리턴값은 무시한다..
}



INT32	GetPriority( UINT32 nType)
{
	switch( nType)
	{
	case  I3RSC_TYPE_BINARY:		return 0;
	case  I3RSC_TYPE_TEXTURE:		return 1;
	case  I3RSC_TYPE_ANIMPACK:		return 2;
	case  I3RSC_TYPE_ANIMATION:		return 3;
	case  I3RSC_TYPE_SOUND:			return 4;
	case  I3RSC_TYPE_SCENE:			return 5;
	case  I3RSC_TYPE_WORLD:			return 6;
	case  I3RSC_TYPE_CHARA:			return 7;
	case  I3RSC_TYPE_OBJECT:		return 8;
	case  I3RSC_TYPE_UI:			return 9;
	}

	return 0;
}

