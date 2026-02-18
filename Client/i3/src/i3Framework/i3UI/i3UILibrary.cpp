#include "i3FrameworkPCH.h"
#include "i3UILibrary.h"
#include "i3UIRenderer.h"
#include "i3UIManager.h"
#include "i3UITemplate_User.h"

#include "i3UILTreeNode_Template.h"
#include "i3Base/itl/container_util.h"

#include "i3Base/string/stack_string.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/ext/erase_string.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3UIFilePathMgr.h"

#include "i3UILibHdr.h"

#define UILIBRARY_VERSION	2	// 2010.11.30.	i3UILTreeNode_Filter, i3UILTreeNode_Template을 이용한 트리구조로 변경

I3_CLASS_INSTANCE( i3UILibrary);


i3UILibrary::i3UILibrary(void) : m_nVersion(UILIBRARY_VERSION)
{
}

i3UILibrary::~i3UILibrary(void)
{
	I3_SAFE_RELEASE( m_pLibRoot);
	I3_SAFE_RELEASE( m_pNewLibRoot);
	i3::cu::for_each_release_clear(m_List, &i3UITemplate::Release);
}

void i3UILibrary::_ConvertVersion( void)
{
	if(  m_nVersion == 1 && UILIBRARY_VERSION == 2)
	{
		// Converting to Version: 2
		m_nVersion = 2;
		Create();		//Create new root

		for( size_t i=0; i< m_List.size(); i++)
		{
			i3UITemplate* pTemplate = m_List.get_vector()[i].second;
			
			//add template nodes to the root
			i3UILTreeNode_Template* pTNode = i3UILTreeNode_Template::new_object_ref();
			pTNode->SetTemplate( pTemplate);
			pTNode->SetName( pTemplate->GetNameString());
			
			m_pNewLibRoot->AddChild( pTNode);
		}

		I3TRACE("UILib Converted To version: %d\n", m_nVersion);
	}
}

void i3UILibrary::Create(void)
{
	if( m_nVersion == 1)
	{
		I3ASSERT( m_pLibRoot == nullptr);

		m_pLibRoot = i3UILTreeFilter::new_object();
		m_pLibRoot->SetName("Root");

		m_List.clear();
	}
	else if( m_nVersion == 2)
	{
		I3ASSERT( m_pNewLibRoot == nullptr);

		m_pNewLibRoot = i3UILTreeNode_Filter::new_object();
		m_pNewLibRoot->SetName( "Root");
	}

	I3TRACE("UILib Created to version: %d\n", m_nVersion);
}

i3UILibHdr*		i3UILibrary::CreateUILibHdr()
{
	i3UILibHdr* pNewObj = i3UILibHdr::new_object();
	pNewObj->SetName(this->GetNameString());
	pNewObj->SetData(m_nVersion, m_strVTexPath, m_strScript);

	m_bNewTemplateFormat = true;

	return pNewObj;
}

void			i3UILibrary::ApplyUILibHdr(const i3UILibHdr* pHdr)
{
	this->SetName(pHdr->GetNameString());
	m_nVersion = pHdr->GetVersion();
	m_strVTexPath = pHdr->GetVTexPath();
	m_strScript = pHdr->GetScriptPath();

	m_bNewTemplateFormat = true;
}


bool	i3UILibrary::RegisterScript(lua_State *L)
{
	//INT32 result = 0;
	// char szRel[MAX_PATH];

	if( m_strScript.empty())
		return true;

	/*
	char * pszWorkingFolder = i3ResourceFile::GetWorkingFolder();
	if( *pszWorkingFolder == 0)
	{
		::GetCurrentDirectory( MAX_PATH -1, szRel);

		i3::rc_string strCommonRootDir = i3UIFilePathMgr::i()->Retrieve_CommonUIRootDirectory(false);

		i3::stack_string strFullPath = szRel;
		strFullPath += '/';
		strFullPath += strCommonRootDir;
		strFullPath += '/';
		strFullPath += m_strScript;
		result = i3Lua::DoFile(L, strFullPath.c_str());

//		if (i3UI::getManager()->GetLocale()->length() > 0)
//		{
//			i3::snprintf( szFull, MAX_PATH -1, "%s\\%s\\%s", szRel, i3UI::getManager()->GetCommonLocaleUIFolder()->c_str(), m_strScript.c_str());
//		}
//		result = i3Lua::DoFile( L, szFull);
	}
	else
	{
		i3::rc_string strCommonRootDir = i3UIFilePathMgr::i()->Retrieve_CommonUIRootDirectory(true);

		i3::stack_string strFullPath = strCommonRootDir;
		strFullPath += '/';
		strFullPath += m_strScript;

//		i3::make_relative_path( i3ResourceFile::GetWorkingFolder(), m_strScript.c_str(), szRel);
//		i3::snprintf( szFull, MAX_PATH -1, "%s\\%s", i3ResourceFile::GetWorkingFolder(), szRel);

		result = i3Lua::DoFile( L, strFullPath.c_str());
	}
	*/

	i3::stack_string strFileNameOnly;		// root.lua파일이다..
	i3::extract_filename(m_strScript, strFileNameOnly);  // 파일 이름외엔 모두 버리고, 버추얼텍스쳐 경로에 맞춰서 찾도록 한다...
	i3::rc_string strVTexDir = i3UIFilePathMgr::i()->Retrieve_Current_VTexDirectory(true);

	i3::stack_string strFullPath = strVTexDir;
	strFullPath += '/';
	strFullPath += strFileNameOnly;
	
	INT32 result = i3Lua::DoFile(L, strFullPath.c_str());
	
	if( result != 0)
		return false;
	else
		return true;
}

void	i3UILibrary::setScriptFile( const char * pszFileName)
{
	if( pszFileName != nullptr && pszFileName[0] != 0 )
	{
		// Rel Path
	//	char conv[MAX_PATH];
	//	i3::make_relative_path( i3ResourceFile::GetWorkingFolder(), pszFileName, conv);
		i3::stack_string strConvertedPath;
		i3UIFilePathMgr::i()->ConvertPathToV3UIStyle(pszFileName, strConvertedPath);
	//	i3UIFilePathMgr::i()->ObtainUniquePath(pszFileName, strUniquePath);

		m_strScript = strConvertedPath;

		// Full Path
		//i3::string_ncopy_nullpad( m_szScript, pszFileName, MAX_PATH -1);
	}
	else
	{
		m_strScript.clear();
	}
}

const char*	i3UILibrary::getScriptFile(void)
{
	if( m_strScript.empty())
		return nullptr;
	else
		return m_strScript.c_str();
}

////////////////////////////////////////////////////////////////////////////
INT32 i3UILibrary::AddElement( i3UITemplate * pTemplate, bool bLoad /*= true*/)
{
	I3ASSERT( pTemplate != nullptr);
	I3ASSERT( pTemplate->hasName());

//	m_List.SetCompareProc( (COMPAREPROC)_FindFunc);

	UINT128 checkSum = pTemplate->getChecksum();
//	INT32 idx = m_List.FindItem((void *)&checkSum);
//	m_List.SetCompareProc( (COMPAREPROC)_CompareFunc);
	i3::vector_map<UINT128, i3UITemplate*>::iterator it = m_List.lower_bound(checkSum);
	
	if ( it != m_List.end() && !m_List.key_comp()(checkSum, it->first) )		// 이럴때 같음..
	{
		return (INT32) i3::distance( m_List.begin(), it);
	}
	I3_SAFE_ADDREF( pTemplate);

	pTemplate->SetResourcePath( pTemplate->GetName());
		
	i3::vector_map<UINT128, i3UITemplate*>::iterator new_it = m_List.insert(it, i3::vector_map<UINT128, i3UITemplate*>::value_type(checkSum, pTemplate) );
	
#if defined( I3_DEBUG)
		if( bLoad == false)
			m_bEdited = true;
#endif

	return INT32(i3::distance(m_List.begin(), new_it));
}

INT32 i3UILibrary::RemoveElement( i3UITemplate * pTemplate)
{
	I3ASSERT( pTemplate != nullptr);
	I3ASSERT( pTemplate->hasName());

	UINT128 checkSum = pTemplate->getChecksum();

	i3::vector_map<UINT128, i3UITemplate*>::iterator it = m_List.find(checkSum);

	I3ASSERT( it != m_List.end() );		// 
		
	i3UITemplate * pInnerTemplate = it->second;

	I3_MUST_RELEASE( pInnerTemplate);

	m_List.erase( it);

#if defined( I3_DEBUG)
	m_bEdited = true;
#endif

	return (INT32)m_List.size();
}

bool i3UILibrary::RenameElement( i3UITemplate * pTemplate, const char * pszName)
{
	I3ASSERT( pTemplate != nullptr);
	I3ASSERT( pTemplate->hasName());

//	m_List.SetCompareProc( (COMPAREPROC)_FindFunc);

	UINT128 checkSum = pTemplate->getChecksum();

//	INT32 idx = m_List.FindItem((void *)&checkSum);
	i3::vector_map<UINT128, i3UITemplate*>::iterator it = m_List.find(checkSum);

//	m_List.SetCompareProc( (COMPAREPROC)_CompareFunc);
	if (it == m_List.end())
	{
		pTemplate->SetResourcePath( pszName);
		return true;
	}

	i3UITemplate * pElm = it->second; 
	m_List.erase(it);
		
	pTemplate->SetResourcePath( pszName);
	checkSum = pTemplate->getChecksum();
	m_List.insert( i3::vector_map<UINT128, i3UITemplate*>::value_type( checkSum, pElm) );

#if defined( I3_DEBUG)
		m_bEdited = true;
#endif

	return true;
}

bool i3UILibrary::FindUniqueName( char * prefix, char * pszName)
{
	INT32 i = 0;

	i3::snprintf( pszName, MAX_PATH, "%s", prefix);

	while( FindByName( pszName) != nullptr)
	{
		i3::snprintf( pszName, MAX_PATH, "%s%d", prefix, i);
		i++;
	}

	return true;
}

i3UITemplate * i3UILibrary::FindByName( const char * pszName)
{
	UINT128 code;

	char conv[MAX_PATH];
	i3::safe_string_copy( conv, pszName, MAX_PATH);
	i3::to_upper( conv);

	//Checksum을 이용해서 찾기 때문에 Checksum을 만드는것과 같은방식으로 해야한다.
	i3MD5::Gen( (char*)conv, i3::generic_string_size( conv), (char *) &code);

	return FindByHash( code);
}

i3UITemplate * i3UILibrary::FindByHash( UINT128 hash)
{
	i3::vector_map<UINT128, i3UITemplate*>::iterator it = m_List.find(hash);
	
	return (it != m_List.end() ) ? it->second : nullptr;
}

////////////////////////////////////////////////////////////////////////////
void	i3UILibrary::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	if( m_nVersion == 1)
	{
		if(( m_bProtected == false) && (m_pLibRoot != nullptr))
			m_pLibRoot->OnBuildObjectList( List);
	}
	else if( m_nVersion == 2)
	{
		if(( m_bProtected == false) && (m_pNewLibRoot != nullptr))
			m_pNewLibRoot->OnBuildObjectList( List);
	}

	i3NamedElement::OnBuildObjectList( List);
}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(push, 8)			// [test required] 패킷사이즈가 명시되지 않아 추가합니다. 2017.02.09 soon9
#endif

namespace pack
{
	struct UILIBRARY_HEAD
	{
		UINT8	ID[4] = { 'U', 'I', 'L', '1' };
		UINT32	Version = 0;
		UINT8	Mode = 0;
		UINT32	VTexPathLength = 0;
		UINT32	ScriptPathLength = 0;
		UINT32	TemplateCount = 0;
		OBJREF	Root = 0;

		UINT32	pad[10] = { 0 };
	};
}

#if defined(I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack(pop)
#endif

UINT32	i3UILibrary::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc;
	
	// 
	i3::rc_string currName = GetNameString();
	i3::stack_string oldStyleName;

	i3UIFilePathMgr::i()->ConvertPathToOldUIStyle(currName, oldStyleName);

	this->SetName(oldStyleName);
	Result = i3NamedElement::OnSave( pResFile);
	this->SetName(currName);
	// 

	if( Result == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN,  "Failed in \"i3NamedElement::OnSave()\"");
		return Result;
	}

	pack::UILIBRARY_HEAD head;
	head.Version = m_nVersion;
	head.Mode = (m_bProtected)? 0 : 1;

	i3VirtualTexture * pVTex = i3UI::getVirtualTexture();

	i3::stack_string strOldStyleVirtualTextureName;
	{
		m_strVTexPath.clear();
	
		if( pVTex != nullptr)
		{
			i3::stack_string strUniquePath;
			i3UIFilePathMgr::i()->ObtainUniquePath(pVTex->GetNameString(), strUniquePath);

			m_strVTexPath = strUniquePath;
			i3UIFilePathMgr::i()->ConvertPathToOldUIStyle(strUniquePath, strOldStyleVirtualTextureName);

		}

		head.VTexPathLength = strOldStyleVirtualTextureName.length();
	}

	i3::stack_string strOldStyleScriptName;
	i3UIFilePathMgr::i()->ConvertPathToOldUIStyle(m_strScript, strOldStyleScriptName);

	head.ScriptPathLength = strOldStyleScriptName.length();
	head.TemplateCount = (UINT32) m_List.size();

	if( m_bProtected == false)
	{
		if( m_nVersion == 1)
			head.Root = pResFile->GetObjectPersistID( m_pLibRoot);
		else if( m_nVersion == 2)
			head.Root = pResFile->GetObjectPersistID( m_pNewLibRoot);
	}

	Rc = pResFile->Write( &head, sizeof( head));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN,  "Failed in \"pResFile->Write()\"");
		return Rc;
	}
	Result += Rc;

	// vTexpath
	if( head.VTexPathLength > 0)
	{
		Rc = pResFile->Write(strOldStyleVirtualTextureName.c_str(), head.VTexPathLength);
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN,  "Failed in \"i3NamedElement::OnSave()\"");
			return Rc;
		}
		Result += Rc;
	}

	if( head.ScriptPathLength > 0)
	{
		Rc = pResFile->Write(strOldStyleScriptName.c_str(), head.ScriptPathLength);

		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN,  "Failed in \"i3NamedElement::OnSave()\"");
			return Rc;
		}
		Result += Rc;
	}
	
	OBJREF ref;
	for( size_t i = 0; i < m_List.size(); ++i)
	{
		i3UITemplate * pTemplate = m_List.get_vector()[i].second;
		
		ref = pResFile->GetObjectPersistID( pTemplate);

		Rc = pResFile->Write( &ref, sizeof( ref));

		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN,  "Failed in \"i3NamedElement::OnSave()\"");
			return Rc;
		}
		Result += Rc;
	}

	m_bNewTemplateFormat = false;

	return Result;
}

UINT32	i3UILibrary::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 i;
	UINT32 Result, Rc;
	pack::UILIBRARY_HEAD head;

	Result = i3NamedElement::OnLoad( pResFile);

	if( Result == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN,  "Failed in \"i3NamedElement::OnLoad()\"");
		return Result;
	}

	// 이름에서 UIVersion을 제외시킨다...
	i3::stack_string strNewName;
	i3UIFilePathMgr::i()->ConvertPathToV3UIStyle(GetNameString(), strNewName);
	this->SetName(strNewName);
	
	//

	Rc = pResFile->Read( &head, sizeof( head));
	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN,  "Failed in \"pResFile->Read()\"");
		return Rc;
	}
	Result += Rc;

	m_nVersion = head.Version;
	m_bProtected = head.Mode? false : true;

	{
		i3::stack_string	strRes(head.VTexPathLength, 0);
		Rc = pResFile->Read( &strRes[0], head.VTexPathLength);
		
		i3::stack_string newStyleRes;
		i3UIFilePathMgr::i()->ConvertPathToV3UIStyle(strRes, newStyleRes);

		m_strVTexPath = newStyleRes;
	}

	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN,  "Failed in \"pResFile->Read()\"");
		return Rc;
	}
	Result += Rc;
	
	{
		i3::stack_string	strRes(head.ScriptPathLength, 0);
		Rc = pResFile->Read( &strRes[0], head.ScriptPathLength);

		i3::stack_string newStyleRes;
		i3UIFilePathMgr::i()->ConvertPathToV3UIStyle(strRes, newStyleRes);

		m_strScript = newStyleRes;
	}

	if( Rc == STREAM_ERR)
	{
		I3PRINTLOG(I3LOG_WARN,  "Failed in \"pResFile->Read()\"");
		return Rc;
	}
	Result += Rc;


	if( m_bProtected == false)
	{
		if( m_nVersion == 1)
		{
			i3UILTreeFilter* pRoot = (i3UILTreeFilter*) pResFile->FindObjectByID( head.Root);
			I3ASSERT( pRoot != nullptr);

			setLibRoot( pRoot);
		}
		else if( m_nVersion == 2)
		{
			i3UILTreeNode_Filter* pNewRoot = (i3UILTreeNode_Filter*)pResFile->FindObjectByID( head.Root);
			I3ASSERT( pNewRoot != nullptr);

			setNewLibRoot( pNewRoot);
		}
	}

	OBJREF ref;
	for( i = 0; i < head.TemplateCount; ++i)
	{
		Rc = pResFile->Read( &ref, sizeof(ref));
		if( Rc == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_WARN,  "Failed in \"pResFile->Read()\"");
			return Rc;
		}

		if( ref > 0)
		{
			i3UITemplate * pTemplate = (i3UITemplate*) pResFile->FindObjectByID( ref);

			AddElement( pTemplate, true);
		}
	}
	
	_ConvertVersion();
	
	m_bNewTemplateFormat = false;

	return Result;
}

bool	i3UILibrary::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	i3::rc_string currName = GetNameString();
	i3::stack_string oldStyleName;

	i3UIFilePathMgr::i()->ConvertPathToOldUIStyle(currName, oldStyleName);

	this->SetName(oldStyleName);
	bool bRv = i3NamedElement::OnSaveXML( pFile, pXML);
	this->SetName(currName);

	if( bRv == false)
		return false;

	INT32 iVal;
	char conv[MAX_PATH];

	pXML->addAttr( "Version", m_nVersion);
	pXML->addAttr( "mode", (m_bProtected)? 0 : 1);
	if( m_bProtected == false)
	{
		if( m_nVersion == 1)
			pXML->addAttr( "Root", pFile->GetObjectPersistID( m_pLibRoot));
		else if( m_nVersion == 2)
			pXML->addAttr( "Root", pFile->GetObjectPersistID( m_pNewLibRoot));
	}

	i3VirtualTexture * pVTex = i3UI::getVirtualTexture();

	{
		m_strVTexPath.clear();

		i3::stack_string oldStyleVirtualTextureName;

		if( pVTex != nullptr)
		{
			i3::stack_string refinedPath;
			i3UIFilePathMgr::i()->ObtainUniquePath(pVTex->GetNameString(), refinedPath);
			m_strVTexPath = refinedPath;

			i3UIFilePathMgr::i()->ConvertPathToOldUIStyle(m_strVTexPath, oldStyleVirtualTextureName);
		}

		pXML->addAttr( "VTexture", oldStyleVirtualTextureName.c_str());
	}

	pXML->addAttr( "TCount", (INT32)m_List.size());
	for( size_t i = 0;i < m_List.size(); ++i)
	{
		i3UITemplate * pTemplate = m_List.get_vector()[i].second;

		i3::snprintf( conv, MAX_PATH, "T_%d", i);

		iVal = pFile->GetObjectPersistID( pTemplate);

		pXML->addAttr( conv, iVal);
	}

	i3::stack_string strOldStyleScriptName;
	i3UIFilePathMgr::i()->ConvertPathToOldUIStyle(m_strScript, strOldStyleScriptName);
	pXML->addAttr( "Script", strOldStyleScriptName.c_str());

	I3TRACE("UILib Saved to version: %d\n", m_nVersion);

	m_bNewTemplateFormat = false;

	return true;
}

bool	i3UILibrary::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bRv = i3NamedElement::OnLoadXML( pFile, pXML);
	if( bRv == false)
		return false;

	// 이름에서 UIVersion을 제외시킨다...
	i3::stack_string strNewName;
	i3UIFilePathMgr::i()->ConvertPathToV3UIStyle(GetNameString(), strNewName);
	this->SetName(strNewName);
	

	UINT32	iVal;
	INT32	i, count;
	char	conv[MAX_PATH];

	pXML->getAttr( "Version", &m_nVersion);
	pXML->getAttr( "mode", &iVal);

	m_bProtected = (iVal > 0)? false : true;
	if( m_bProtected == false)
	{
		if( m_nVersion == 1 || UILIBRARY_VERSION == 1)
		{
			pXML->getAttr( "Root", &iVal);
			if( iVal != 0)
			{
				i3UILTreeFilter* pRoot = (i3UILTreeFilter*)pFile->FindObjectByID( (OBJREF) iVal);
				I3ASSERT( pRoot != nullptr);

				setLibRoot( pRoot);
			}
		}
		else if( m_nVersion == 2)
		{
			pXML->getAttr( "Root", &iVal);
			if( iVal != 0)
			{
				i3UILTreeNode_Filter* pNewRoot = (i3UILTreeNode_Filter*)pFile->FindObjectByID( (OBJREF)iVal);
				I3ASSERT( pNewRoot != nullptr);

				setNewLibRoot( pNewRoot);
			}
		}
	}

	I3TRACE("UILib Loaded from version: %d\n", m_nVersion);

	const char * pszScript = pXML->getAttr( "Script");
	
	if (pszScript != nullptr)
	{
		i3::stack_string newStyleRes;
		i3UIFilePathMgr::i()->ConvertPathToV3UIStyle(pszScript, newStyleRes);
		setScriptFile(newStyleRes.c_str());
	}
	
	const char * pszVTexture = pXML->getAttr( "VTexture");

	if( pszVTexture != nullptr)
	{
		i3::stack_string newStyleRes;
		i3UIFilePathMgr::i()->ConvertPathToV3UIStyle(pszVTexture, newStyleRes);
		
		m_strVTexPath = newStyleRes;
	}

	pXML->getAttr( "TCount", &count);
	for( i = 0;i < count;++i)
	{
		i3::snprintf( conv, MAX_PATH, "T_%d", i);

		pXML->getAttr( conv, &iVal);

		if( iVal != 0)
		{

			i3UITemplate * pTemplate = (i3UITemplate*)pFile->FindObjectByID((OBJREF)iVal);

			//I3TRACE("AddTemplate: %s\n", pTemplate->GetName());

			AddElement( pTemplate, true);
		}
		else
		{
			I3PRINTLOG(I3LOG_FATAL, " something is wrong!");
		}
	}
	
	_ConvertVersion();

#if defined( I3_DEBUG)
	//_ValidateTemplateList();//비상 처방전.
#endif

	m_bNewTemplateFormat = false;

	return true;
}

#if defined( I3_DEBUG)
void i3UILibrary::_recValidateTemplate( i3NamedElement * pElement)
{
	if( pElement == nullptr)
		return;

	INT32 i;
	i3NamedElement * pChild = nullptr;
	if( i3::same_of<i3UILTreeElement*>(pElement))
	{
		i3UILTreeElement * pElm = (i3UILTreeElement*)pElement;
		for( i = 0; i < pElm->getElementCount(); ++i)
		{
			pChild = (i3UITemplate*)pElm->getElement(i);

			_recValidateTemplate( pChild);
		}
	}
	else if( i3::kind_of<i3TreeNode*>(pElement))
	{
		i3TreeNode * pTNode = (i3TreeNode*)pElement;
		for( i = 0; i < pTNode->getChildCount(); i++)
		{
			pChild = pTNode->getChild( i);

			_recValidateTemplate( pChild);
		}
	}
	else if( i3::kind_of<i3UITemplate*>(pElement))
	{
		i3UITemplate * pTemplate = (i3UITemplate*)pElement;

		if( FindByHash( pTemplate->getChecksum()) == nullptr)
		{
			AddElement( pTemplate);
		}
	}
}

bool i3UILibrary::_ValidateTemplateList()
{
	if( m_nVersion == 1)
	{
		if( m_pLibRoot == nullptr)
			return false;

		_recValidateTemplate( m_pLibRoot);
	}

	return true;
}
#endif

bool i3UILibrary::Defragment()
{
	i3VirtualTexture * pVTex = i3UI::getVirtualTexture();
	if( pVTex == nullptr)
		return false;

	for(size_t i = 0;i < m_List.size(); ++i)
	{
		i3UITemplate * pTemplate = m_List.get_vector()[i].second;

		pTemplate->RefreshOffset( pVTex);
	}

	return true;
}


bool i3UILibrary::MergeLibrary( i3UILibrary * pLibrary, bool bOverwrite)
{
	/*
	const char * pszVirtualTexture = (const char*)pXML->Attribute("VTexture");
	if( pszVirtualTexture != nullptr)
	{
		i3VirtualTexture * pVTex = i3UI::getVirtualTexture();
		if( pVTex == nullptr)
		{
			i3UI::OpenVirtualTexture( pszVirtualTexture, 2048, 2048 );
		}
		else
		{
			i3String::MakeRelativePath( i3ResourceFile::GetWorkingFolder(), pVTex->GetName(), conv);

			INT32 diff = i3String::NCompare( conv, pszVirtualTexture, MAX_PATH -1);
			if( diff != 0)
			{
				I3PRINTLOG(I3LOG_FATAL, "읽어들인 라이브러리는 현재 설정된 virtual texture를 사용하고 있지 않습니다.");
			}
		}
	}*/
	return true;
}
