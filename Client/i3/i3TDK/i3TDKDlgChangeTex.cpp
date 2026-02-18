#include "stdafx.h"
#include "i3TDK.h"

#include "i3TDKDlgChangeTex.h"
#include "i3TDK_Dlg_SelectImageExt.h"
#include "i3Base/string/ext/contain_string_head.h"
#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/compare/generic_is_iequal.h"
#include "i3Base/string/stack_string.h"
#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/sprintf.h"
#include "i3Base/string/compare/generic_iless.h"
#include "i3Base/string/compare/generic_is_iless.h"

#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"
#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/extract_fileext.h"


struct i3TDKChangeTex_Pef
{
	i3TDKChangeTex_Pef(i3::vector<i3TDKChangeTex_Pef*>& owner_list, const i3::rc_string& relPath, i3RegKey* pRootKey);
	~i3TDKChangeTex_Pef();
	
	i3::rc_string								m_strRelPath;
	i3RegKey*									m_pefRootKey;					// [initialize at constructor]
	i3::vector<i3TDKChangeTex_RefCategory*>		m_refCategoryList;
	INT32										m_currSelectedCategoryIdx = -1;
};


struct i3TDKChangeTex_RefCategory
{
	i3TDKChangeTex_RefCategory(i3TDKChangeTex_Pef* parent, i3RegKey* refCategoryKey);
	i3TDKChangeTex_RefCategory(i3TDKChangeTex_Pef* parent, const char* new_parent_name);
	~i3TDKChangeTex_RefCategory();
	
	i3TDKChangeTex_Pef*					m_parent;			// [initialize at constructor]

	i3RegKey* 							m_pefRefCategoryKey = nullptr;
	i3::vector<i3TDKChangeTex_Refi3s*>	m_Refi3sList;			
	INT32								m_currSelectedi3sIndex = -1;
};

struct i3TDKChangeTex_Refi3s
{
	i3TDKChangeTex_Refi3s( i3TDKChangeTex_RefCategory* parent, i3RegKey* refTexKey );
	i3TDKChangeTex_Refi3s( i3TDKChangeTex_RefCategory* parent, const char* i3s_name );
	~i3TDKChangeTex_Refi3s();

	i3TDKChangeTex_RefCategory* m_parent;					// [initialize at constructor]
	i3RegKey* 					m_pefRefTexKey = nullptr;				//
	INT32						m_currCloneListSelIndex = -1;	// 복구때에만 쓰일것이다..
	INT32						m_currPropertySelIndex = -1;		// 프로퍼티의 셀렉션 복구에만 쓸 것이다.. ( LOD 자체 클릭인 경우엔 -1이 되어야한다..)
	INT32						m_currLODIndex = -1;				// 현재 LODIndex상태도 복구에 반영해야할 것임..(-1:최상단, 0~N : LOD 아래에 속함)
};



namespace
{
	struct SortFn_TreeNode
	{
		bool operator()(const i3TreeNode* lhs, const i3TreeNode* rhs) const
		{
			const i3::rc_string& str_lhs = const_cast<i3TreeNode*>(lhs)->GetNameString();
			const i3::rc_string& str_rhs = const_cast<i3TreeNode*>(rhs)->GetNameString();
			return	i3::generic_is_iless(str_lhs, str_rhs); 
		}
	};
	
	struct SortFn_Pef
	{
		bool operator()(const i3TDKChangeTex_Pef* lhs, const i3TDKChangeTex_Pef* rhs ) const
		{
			return i3::generic_is_iless(lhs->m_strRelPath, rhs->m_strRelPath);
		}
	};

	struct SortFn_RefCategory
	{
		bool operator()(const i3TDKChangeTex_RefCategory* lhs, const i3TDKChangeTex_RefCategory* rhs ) const
		{
			return i3::generic_is_iless(lhs->m_pefRefCategoryKey->GetNameString(), rhs->m_pefRefCategoryKey->GetNameString() );
		}
	};

	struct SortFn_Refi3s
	{
		bool operator()(const i3TDKChangeTex_Refi3s* lhs, const i3TDKChangeTex_Refi3s* rhs ) const
		{
			const i3::rc_string& str_lhs = lhs->m_pefRefTexKey->GetNameString();
			const i3::rc_string& str_rhs = rhs->m_pefRefTexKey->GetNameString();
			return i3::generic_iless()(str_lhs, str_rhs);
		}
	};

}



i3TDKChangeTex_RefCategory::i3TDKChangeTex_RefCategory(i3TDKChangeTex_Pef* parent, i3RegKey* refCategoryKey) : m_parent(parent), m_pefRefCategoryKey(refCategoryKey)
{
	i3::vector<i3TDKChangeTex_RefCategory*>& cat_list = parent->m_refCategoryList;

	if (refCategoryKey) refCategoryKey->AddRef();

	i3::vector<i3TDKChangeTex_RefCategory*>::iterator it = i3::lower_bound(cat_list.begin(), cat_list.end(), this, SortFn_RefCategory() );
	cat_list.insert(it, this);
}

i3TDKChangeTex_RefCategory::i3TDKChangeTex_RefCategory(i3TDKChangeTex_Pef* parent, const char* new_parent_name) : m_parent(parent), m_pefRefCategoryKey(nullptr)
{
	m_pefRefCategoryKey = i3RegKey::new_object();
	m_pefRefCategoryKey->SetName(new_parent_name);

	i3::vector<i3TDKChangeTex_RefCategory*>& cat_list = parent->m_refCategoryList;

	i3::vector<i3TDKChangeTex_RefCategory*>::iterator it1 = i3::lower_bound(cat_list.begin(), cat_list.end(), this, SortFn_RefCategory() );
	cat_list.insert(it1, this);

	i3RegKey* pRootKey = parent->m_pefRootKey;
	I3ASSERT(pRootKey != nullptr);

	i3::vector<i3TreeNode*>& root_child_list = pRootKey->getChildList();
	i3::vector<i3TreeNode*>::iterator it2 = i3::lower_bound(root_child_list.begin(), root_child_list.end(), m_pefRefCategoryKey, SortFn_TreeNode());
	INT32 dist = i3::distance(root_child_list.begin(), it2);
	pRootKey->InsertChild(dist, m_pefRefCategoryKey);

}

i3TDKChangeTex_RefCategory::~i3TDKChangeTex_RefCategory()
{
	if (m_pefRefCategoryKey) m_pefRefCategoryKey->Release();
	i3::cu::for_each_delete(m_Refi3sList);
}

i3TDKChangeTex_Pef::i3TDKChangeTex_Pef(i3::vector<i3TDKChangeTex_Pef*>& owner_list, const i3::rc_string& relPath, i3RegKey* pRootKey) : m_pefRootKey(pRootKey), 
		m_strRelPath(relPath)
{
	if (pRootKey) pRootKey->AddRef();
	
	i3::vector<i3TDKChangeTex_Pef*>::iterator it = i3::lower_bound(owner_list.begin(), owner_list.end(), this, SortFn_Pef() );
	owner_list.insert(it, this);
}

i3TDKChangeTex_Pef::~i3TDKChangeTex_Pef()
{
	if (m_pefRootKey) m_pefRootKey->Release();
	i3::cu::for_each_delete(m_refCategoryList);
}


i3TDKChangeTex_Refi3s::i3TDKChangeTex_Refi3s( i3TDKChangeTex_RefCategory* parent, i3RegKey* refTexKey ) : m_parent(parent), m_currCloneListSelIndex(-1), 
m_currPropertySelIndex(-1),
m_currLODIndex(-1),
m_pefRefTexKey(refTexKey)
{
	I3_MUST_ADDREF(refTexKey);

	if (parent)
	{
		i3::vector<i3TDKChangeTex_Refi3s*>::iterator it = i3::lower_bound(parent->m_Refi3sList.begin(), parent->m_Refi3sList.end(), this, SortFn_Refi3s());
		parent->m_Refi3sList.insert(it, this);
	}
}

i3TDKChangeTex_Refi3s::i3TDKChangeTex_Refi3s( i3TDKChangeTex_RefCategory* parent, const char* i3s_name ) :	m_parent(parent)
{

	m_pefRefTexKey = i3RegKey::new_object();
	m_pefRefTexKey->SetName(i3s_name);

	I3ASSERT( parent != nullptr);

	i3::vector<i3TDKChangeTex_Refi3s*>::iterator it1 = i3::lower_bound(parent->m_Refi3sList.begin(), parent->m_Refi3sList.end(), this, SortFn_Refi3s());
	parent->m_Refi3sList.insert(it1, this);

	i3::vector<i3TreeNode*>& child_list = parent->m_pefRefCategoryKey->getChildList();
	i3::vector<i3TreeNode*>::iterator it2 = i3::lower_bound(child_list.begin(), child_list.end(), m_pefRefTexKey, SortFn_TreeNode());
	INT32 dist = i3::distance(child_list.begin(), it2);
	parent->m_pefRefCategoryKey->InsertChild(dist, m_pefRefTexKey);

}

i3TDKChangeTex_Refi3s::~i3TDKChangeTex_Refi3s() 
{
	I3_SAFE_RELEASE(m_pefRefTexKey);
}




struct i3TDKChangeTex_TexGroup
{
	i3Texture*							m_origTex = nullptr;
	i3::vector<i3TextureBindAttr*>		m_attrList;
	
	i3TDKChangeTex_TexGroup() {}
	~i3TDKChangeTex_TexGroup() { if (m_origTex) m_origTex->Release(); }
};



namespace
{
	INT32		GetSelectedItemIndex(const CListCtrl& l)
	{
		POSITION pos = l.GetFirstSelectedItemPosition();
		INT32 idx = -1;
		if (pos != nullptr)
			idx = l.GetNextSelectedItem(pos);
		return idx;
	}

	// 엔진쪽 아닌 다이알로그에 알맞게 읽어들일수 있는 pef해석루틴이 필요함..
	// 엔진쪽과 달리 ...키가 원본 텍스쳐가 됨..
	
	i3RegKey*	CreatePefRootKey()
	{
		i3RegKey* pNewRoot = i3RegKey::new_object();
		pNewRoot->SetName("i3s reference");
		pNewRoot->setNodeStyle( I3REG_STYLE_ROOT);
		return pNewRoot;
	}
	
	void		Sorti3RegKeysRec( i3RegKey* key)
	{
		i3::vector<i3TreeNode*>& child_list = key->getChildList();
		const size_t num = child_list.size();

		for (size_t i = 0 ; i < num ; ++i )
		{
			i3::rc_string name = child_list[i]->GetNameString();
			i3::make_unix_path(name);
			child_list[i]->SetName(name);
		}

		i3::sort(child_list.begin(), child_list.end(), SortFn_TreeNode());
		
		for (size_t i = 0 ; i < num ; ++i )
			 Sorti3RegKeysRec( static_cast<i3RegKey*>(child_list[i]));
	}

	i3RegKey*	LoadChangeTexPef( const TCHAR* tstrFilename )
	{
		if(!tstrFilename)
			return nullptr;

		i3Registry::Set_LoadStamp(false);
		i3RegistrySet RegFile;

#ifdef _UNICODE
		i3::stack_string strTmp;	i3::utf16_to_mb(tstrFilename, strTmp);
		const char* strFilename = strTmp.c_str();
#else
		const char* strFilename = tstrFilename;
#endif

		INT32 nReadBytes = RegFile.LoadRegistrySet( strFilename, REGISTRY_FILE_XML_BY_UTF8);

		if( nReadBytes == STREAM_ERR)
		{
		//	I3PRINTLOG(I3LOG_FATAL,  "Failed to load %s", strFilename );		
			return nullptr;
		}
		
		i3RegKey * pRootRegKey = RegFile.getRoot();

		if ( !i3::generic_is_equal( pRootRegKey->GetName(), "i3s reference") )
		{
		//	I3PRINTLOG(I3LOG_FATAL,  "%s's file root key name is not 'i3s reference'.", strFilename);
			return nullptr;
		}		
		
		I3_MUST_ADDREF(pRootRegKey);
		
		Sorti3RegKeysRec(pRootRegKey);

		return pRootRegKey;
	}
	
	bool	SaveChangeTexPef(const TCHAR* tszFilename, i3RegKey* pRootKey, bool bPefSave)
	{
		i3RegKeyFile* keyFile = i3RegKeyFile::new_object();
		keyFile->setRoot(pRootKey);

		{
			
			if (bPefSave)
			{
#ifdef _UNICODE
				i3::stack_string strFilename;	i3::utf16_to_mb(tszFilename, strFilename);
				const char* szFilename = strFilename.c_str();
#else
				const char* szFilename = tszFilename;
#endif

				char szxmlFileName[MAX_PATH] = "";
				i3::safe_string_copy(szxmlFileName, szFilename, MAX_PATH);
				i3::change_file_ext_copy(szxmlFileName, szxmlFileName, "pef");

				i3ResourceFile	file;
				file.AddObject(keyFile);
				if (file.Save(szxmlFileName) == STREAM_ERR)
				{
					i3::stack_string strMsg;
					i3::sprintf(strMsg, "Could not save PEF file : %s", tszFilename);
					::AfxMessageBox(strMsg.c_str());
					I3_MUST_RELEASE(keyFile);
					return false;
				}
			}
			
			i3RegistrySet registryset;
			registryset.SetCodePage(I3_LANG_CODE_ANSI);
			registryset.setRoot(pRootKey);

			if (registryset.SaveRegistrySet(tszFilename, REGISTRY_FILE_XML_BY_UTF8) == STREAM_ERR)
			{
				i3::stack_string strMsg;
				i3::sprintf(strMsg, "Could not save XML file : %s", tszFilename);
				::AfxMessageBox(strMsg.c_str());
			
				return false;
			}

		}
		
		I3_MUST_RELEASE(keyFile);


		return true;
	}

	void	SplitFolderAndFile( const char* szPath, 
		i3::stack_string& strSubDir, i3::stack_string& strFilename)
	{
		if (szPath == nullptr || szPath[0] == 0)
		{
			strSubDir.clear(); strFilename.clear();
			return;
		}

		char tmp[MAX_PATH];	
		_fullpath(tmp, szPath, MAX_PATH);
		i3::stack_string	strRelFilePath;

#ifdef _UNICODE
		i3::stack_string work_dir;	i3::utf16_to_mb(g_pTDKEnvSet->getCurWorkDir(), work_dir);
#else
		const i3::literal_range work_dir = g_pTDKEnvSet->getCurWorkDir();
#endif

		i3::make_relative_path( work_dir, tmp, strRelFilePath);
		i3::make_unix_path(strRelFilePath);
		
		i3::extract_directoryname( strRelFilePath, strSubDir );
		if (!strSubDir.empty() )
		{
			strFilename.assign( strRelFilePath, strSubDir.size()+1, i3::stack_string::npos);

			size_t pos = strSubDir.find_last_of('/');
				
			if (pos != i3::stack_string::npos )		// 마지막 폴더가 별도존재하면 파일쪽으로 넣는다.
			{
				strSubDir += '/';
				strFilename.insert(0, strSubDir, pos+1, i3::stack_string::npos);
				strSubDir.erase(pos);	
			}
		}
		else
		{
			strFilename = strRelFilePath;	
		}		
	}
	



	// 자동 소팅와 셀렉션 보존 처리 해 놓을 것..
	// i3RegKey* pRootKey, i3::vector<i3TDKChangeTex_RefCategory*>& catList 곧 삭제..
	i3TDKChangeTex_RefCategory* FindOrCreateRefCategory( i3TDKChangeTex_Pef* pef,  const char* strCat, CListCtrl& catListCtrl, bool& outNeedUpdateRefi3sListCtrl)
	{
		outNeedUpdateRefi3sListCtrl = false;

		INT32 prev_index = GetSelectedItemIndex(catListCtrl);

		const INT32 num_folder_name = (INT32)pef->m_refCategoryList.size();
		INT32 found_index = -1;

		for (INT32 i = 0 ; i < num_folder_name ; ++i )
		{
			i3TDKChangeTex_RefCategory* cat = pef->m_refCategoryList[i];

			if ( cat->m_pefRefCategoryKey && i3::generic_is_iequal(cat->m_pefRefCategoryKey->GetName(), strCat) )
			{
				found_index = i;
				break;
			}
		}
		
		if (found_index != -1)
		{
			if (prev_index != found_index)
				outNeedUpdateRefi3sListCtrl = true;

			catListCtrl.SetItemState( found_index, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
			catListCtrl.EnsureVisible( found_index, FALSE);
			return pef->m_refCategoryList[found_index];
		}
		
		outNeedUpdateRefi3sListCtrl = true;

		i3TDKChangeTex_RefCategory* new_obj = new i3TDKChangeTex_RefCategory(pef, strCat);

		INT32 new_obj_idx = i3::vu::int_index_of(pef->m_refCategoryList, new_obj);
		I3ASSERT( new_obj_idx != -1);

#ifdef _UNICODE
		i3::stack_wstring wstrItemName;	i3::mb_to_utf16(strCat, wstrItemName);
		const wchar_t* szItemName = wstrItemName.c_str();
#else
		const char* szItemName = strCat;
#endif	
		catListCtrl.InsertItem(new_obj_idx, szItemName);
		catListCtrl.SetItemState( new_obj_idx, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		catListCtrl.EnsureVisible(new_obj_idx, FALSE);
		
		return new_obj;
	}

	i3TDKChangeTex_Refi3s*	CreateRefi3s( i3TDKChangeTex_RefCategory* parent, const char* szFileName)
	{
		return new i3TDKChangeTex_Refi3s(parent, szFileName);
	}


/*
	i3TDKChangeTex_Refi3s*	CreateRefi3s(i3RegKey* pRootKey, const TCHAR* szPath)
	{
		//
		i3::stack_string	strSubDir;		i3::stack_string	strFilename;
	
#ifdef _UNICODE
		i3::stack_string strPath;	i3::utf16_to_mb(szPath, strPath);
		const char* szPath2 = strPath.c_str();
#else
		const char* szPath2 = szPath;
#endif
		SplitFolderAndFile(szPath2, strSubDir, strFilename);
		
		const INT32 num_folder_name = pRootKey->getChildCount();
		i3RegKey*	edit_folder_key = nullptr;

		for ( INT32 i = 0 ; i < num_folder_name ; ++i )
		{
			i3RegKey* folder_key = static_cast<i3RegKey*>(pRootKey->getChild(i));
			
			if ( i3::generic_is_iequal( folder_key->GetName(), strSubDir) )
			{
				edit_folder_key = folder_key;
				break;
			}
		}
		
		i3TDKChangeTex_Refi3s* result = nullptr;
		if (edit_folder_key != nullptr)		// 기존 폴더키가 존재하면...그쪽 연결되는 생성자로 생성..
		{
			result = new i3TDKChangeTex_Refi3s( edit_folder_key, strFilename.c_str() );
		}
		else
		{
			result = new i3TDKChangeTex_Refi3s( pRootKey, strSubDir.c_str(), strFilename.c_str() );
		}

		return result;				
	}
*/
	INT32					FindReferenceIndex(i3TDKChangeTex_RefCategory* found_cat, const char* strFilename)
	{
		I3ASSERT(found_cat != nullptr);

		const size_t num_ref_tex = found_cat->m_Refi3sList.size();

		for (size_t i = 0 ; i < num_ref_tex ; ++i)
		{
			i3TDKChangeTex_Refi3s* p = found_cat->m_Refi3sList[i];

			if ( i3::generic_is_iequal( p->m_pefRefTexKey->GetNameString(), strFilename) )
			{
				return INT32(i);
			}
		}

		return -1;
	}
	

	i3::pair<INT32, INT32>	FindReferenceIndex(const i3::vector<i3TDKChangeTex_RefCategory*>& list, const char* refSearchPath)
	{
		if ( list.empty() )
			return i3::make_pair(0,0);
			
		i3::stack_string strDir, strFilename;

		SplitFolderAndFile(refSearchPath, strDir, strFilename);
						
		INT32 cat_idx = -1;
		const size_t num_cat_tex = list.size(); 
		
		for ( size_t i = 0 ; i < num_cat_tex ; ++i )
		{
			i3TDKChangeTex_RefCategory* p = list[i];
			
			if ( i3::generic_is_iequal( p->m_pefRefCategoryKey->GetNameString(), strDir) )
			{
				cat_idx = (INT32)i;
				break;				
			}
		}
		
		if (cat_idx == -1)
			return i3::make_pair(-1, -1);
		
		i3TDKChangeTex_RefCategory* curr_cat = list[cat_idx];

		const size_t num_ref_tex = curr_cat->m_Refi3sList.size();

		for (size_t i = 0 ; i < num_ref_tex ; ++i)
		{
			i3TDKChangeTex_Refi3s* p = curr_cat->m_Refi3sList[i];
			
			if ( i3::generic_is_iequal( p->m_pefRefTexKey->GetNameString(), strFilename) )
			{
				return i3::make_pair(cat_idx, INT32(i));
			}
		}

		return i3::make_pair(cat_idx, -1);
	}
	
	void SetRegStringIntoClonedi3RegKey(const CListCtrl* listCtrl, i3RegString*& inout, const TCHAR* szName, const TCHAR* szValue, INT32 curr_lod_idx)
	{
		assert( szValue != nullptr && szValue[0] != 0 );

		if (inout != nullptr)		// 기존에 있는거면 이름 변경..
		{
#ifdef _UNICODE
			i3::rc_wstring wvarValue = szValue;
#else
			i3::wstring wvarValue;	i3::mb_to_utf16(szValue, wvarValue);
#endif
			if (!i3::generic_is_iequal(inout->getValue(), wvarValue) )
			{
				inout->setValue(wvarValue);
			}
		}
		else	// 없으면 다소 복잡하게 생성..
		{
			INT32 idx = GetSelectedItemIndex(*listCtrl);

			if ( idx != -1)
			{
				inout = i3RegString::new_object();
#ifdef _UNICODE
				i3::stack_string strRegStringName;		i3::utf16_to_mb(szName, strRegStringName);
				const char* RegStringName = strRegStringName.c_str();
#else
				const char* RegStringName = szName;
#endif
				inout->SetName( RegStringName );

#ifdef _UNICODE
				i3::rc_wstring wvarValue = szValue;
#else
				i3::stack_wstring wvarValue;	i3::mb_to_utf16(szValue, wvarValue);
#endif
				inout->setValue( wvarValue );

				i3RegKey* tex_key = (i3RegKey*)listCtrl->GetItemData(idx);				// clonedi3RegKey...
				
				if (curr_lod_idx == -1)
				{
					tex_key->AddData(inout);			// AddData()가 AddRef()를 가지지않는것은 상당한 문제..
				}
				else
				{
					// 이 경우 자식 LODKEY가 미리 마련되어 있어야한다.... (선행 처리 요구)
					INT32 numChild = tex_key->getChildCount();
					I3ASSERT( curr_lod_idx < numChild );

					i3RegKey* lod_key = (i3RegKey*)tex_key->getChild(curr_lod_idx);
					lod_key->AddData(inout);			// AddData()가 AddRef()를 가지지않는것은 상당한 문제..
				}

				I3_MUST_ADDREF(inout);
			}
		}
		
	}


	
	void ExtractAllTexturesFromNode( i3Node* pRoot, i3::vector<i3Texture*>& out)
	{
		i3::vector<i3RenderAttr*> attr_list;
		i3Scene::FindAttrByType(pRoot, i3TextureBindAttr::static_meta(), attr_list);
	
		const size_t num_attr = attr_list.size();

		for (size_t i = 0 ; i < num_attr ; ++i )
		{
			i3TextureBindAttr* tex_attr = static_cast<i3TextureBindAttr*>(attr_list[i]);
			if (tex_attr->GetTexture() )
			{
				out.push_back( tex_attr->GetTexture() );
			}
		}

		i3::vu::remove_duplicates(out);
	}
	
	void ExtractAllTexturesFromNode_LOD( i3Node* pRoot, INT32 LODIndex, i3::vector<i3Texture*>& out)
	{
		i3::vector<i3RenderAttr*> attr_list;

		if (LODIndex == -1)
			i3Scene::FindAttrByType_NonLOD(pRoot, i3TextureBindAttr::static_meta(), attr_list);
		else
			i3Scene::FindAttrByType_LOD(pRoot, LODIndex, i3TextureBindAttr::static_meta(), attr_list);
		
		const size_t num_attr = attr_list.size();

		for (size_t i = 0 ; i < num_attr ; ++i )
		{
			i3TextureBindAttr* tex_attr = static_cast<i3TextureBindAttr*>(attr_list[i]);
			if (tex_attr->GetTexture() )
			{
				out.push_back( tex_attr->GetTexture() );
			}
		}

		i3::vu::remove_duplicates(out);
	}


	//
	void ExtractAllTextureBindAttrFromNode_LOD(i3Node* pRoot, INT32 LodIndex, i3::vector<i3TextureBindAttr*>& out )
	{
		i3::vector<i3RenderAttr*> attr_list;
		
		if (LodIndex == -1)
			i3Scene::FindAttrByType_NonLOD(pRoot, i3TextureBindAttr::static_meta(), attr_list);
		else
			i3Scene::FindAttrByType_LOD(pRoot, LodIndex, i3TextureBindAttr::static_meta(), attr_list);
		
		const size_t num_attr = attr_list.size();

		for (size_t i = 0 ; i < num_attr ; ++i )
		{
			i3TextureBindAttr* tex_attr = static_cast<i3TextureBindAttr*>(attr_list[i]);
			out.push_back( tex_attr );
		}
	}
		
	void ExtractAllTextureBindAttrFromNode( i3Node* pRoot, i3::vector<i3TextureBindAttr*>& out)
	{
		i3::vector<i3RenderAttr*> attr_list;
		i3Scene::FindAttrByType(pRoot, i3TextureBindAttr::static_meta(), attr_list);
	
		const size_t num_attr = attr_list.size();

		for (size_t i = 0 ; i < num_attr ; ++i )
		{
			i3TextureBindAttr* tex_attr = static_cast<i3TextureBindAttr*>(attr_list[i]);
			out.push_back( tex_attr );
		}
	}

	//
	void ExtractTextureBindAttrsFromTexture( const i3::vector<i3TextureBindAttr*>& attr_list, const i3Texture* tex, 
		i3::vector<i3TextureBindAttr*>& out)
	{
		out.clear();

		const size_t num_attr = attr_list.size();

		for (size_t i = 0 ; i < num_attr ; ++i)
		{
			i3TextureBindAttr* attr = attr_list[i];
			if (attr->GetTexture() == tex )
			{
				out.push_back(attr);
			}
		}
	}

	bool ChangeTexture(const i3TDKChangeTex_TexGroup* group, const TCHAR* szToTexName, bool bTexChange)
	{
		i3Texture* new_tex = nullptr;

		if (szToTexName == nullptr || szToTexName[0] == 0 )
		{
			new_tex = group->m_origTex;
		}
		else
		{
			i3NamedResourceManager* pTexManager = i3ResourceFile::GetResourceManager( i3Texture::static_meta());
		
#ifdef _UNICODE
			i3::stack_string strCurWorkDir;	i3::utf16_to_mb(g_pTDKEnvSet->getCurWorkDir(), strCurWorkDir);
			const char* szCurWorkDir = strCurWorkDir.c_str();
			i3::stack_string strToTexNameForGetRsc;	i3::utf16_to_mb(szToTexName, strToTexNameForGetRsc);
			const char* szToTexNameForGetRsc = strToTexNameForGetRsc.c_str();
#else
			const char* szCurWorkDir = g_pTDKEnvSet->getCurWorkDir();
			const char* szToTexNameForGetRsc = szToTexName;
#endif
			new_tex = (i3Texture *) pTexManager->GetResource(szToTexNameForGetRsc , szCurWorkDir, szToTexNameForGetRsc );
		}
		
		if (!new_tex)
			return false;			// 새 텍스쳐가 먹통이면 그냥 끝..

		I3_MUST_ADDREF(new_tex);

		const size_t num_attr =	group->m_attrList.size(); 

		for (size_t i = 0 ; i < num_attr ; ++i )
		{
			i3TextureBindAttr* tex_attr = group->m_attrList[i];
			
			tex_attr->SetTexture(new_tex);
			tex_attr->SetApplyEnable(true);							// 툴에서는 항상 활성화시키도록 한다..
			tex_attr->SetTextureChange(bTexChange);
		}

		I3_MUST_RELEASE(new_tex);

		return true;
	}

	void EnableTexture(const i3TDKChangeTex_TexGroup* group, bool bEnable, bool bChangeTex)
	{

		const size_t num_attr = group->m_attrList.size();

		for (size_t i = 0 ; i < num_attr ; ++i )
		{
			i3TextureBindAttr* tex_attr = group->m_attrList[i];
			
			tex_attr->SetApplyEnable(bEnable);
			tex_attr->SetTextureChange(bChangeTex);
		}
	}



	void MakeOrDestroy_i3RegKey_LOD(const CListCtrl& clonedi3s_ctrl, INT32 cur_sel, INT32 max_lod_count, bool bLOD)
	{
		i3RegKey* clonedi3s_key	 = (i3RegKey*)clonedi3s_ctrl.GetItemData(cur_sel);

		I3ASSERT(clonedi3s_key != nullptr);


		if (bLOD)
		{
			I3ASSERT(clonedi3s_key->getChildCount() == 0);
			I3ASSERT(clonedi3s_key->GetDataCount() == 0);

			i3::stack_string str;

			for (INT32 i = 0 ; i < max_lod_count ; ++i )
			{
				i3RegKey* new_key = i3RegKey::new_object_ref();
				i3::sprintf(str, "LOD %d", i);
				new_key->SetName(str);
				clonedi3s_key->AddChild(new_key);
			}
		}
		else
		{
			I3ASSERT(clonedi3s_key->GetDataCount() == 0);

			const INT32 num_child = clonedi3s_key->getChildCount();

			for (INT32 i = 0 ; i < num_child ; ++i)
			{
				i3RegKey* lod_key = (i3RegKey*)clonedi3s_key->getChild(i);
				I3ASSERT(lod_key->GetDataCount() == 0);
			}

			clonedi3s_key->RemoveAllChild();
		}
	}

	void			ForceLODView(const i3::vector<i3Body*>& bodyList, INT32 lod)
	{


		if (lod == -1)
			lod = 0;
		const size_t num_body = bodyList.size();

		for (size_t i = 0 ; i < num_body ; ++i )
		{
			i3Body* body = bodyList[i];
			body->removeStyle( I3BS_LOD_AUTO );
			body->setCurrentLevel(lod);
		}
	}

	void			ClearForceLODView(const i3::vector<i3Body*>& bodyList)
	{
		const size_t num_body = bodyList.size();
		for (size_t i = 0 ; i < num_body ; ++i )
		{
			i3Body* body = bodyList[i];
			body->addStyle( I3BS_LOD_AUTO );
		}
	}

}

class Refi3sListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(Refi3sListCtrl)
public:
	Refi3sListCtrl() {}
	virtual ~Refi3sListCtrl() {}
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
};


IMPLEMENT_DYNAMIC(Refi3sListCtrl, CListCtrl)

BEGIN_MESSAGE_MAP(Refi3sListCtrl, CListCtrl)

	ON_WM_DROPFILES()
END_MESSAGE_MAP()

void Refi3sListCtrl::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	int count = DragQueryFile( hDropInfo, 0xFFFFFFFF, nullptr, 0);

	if (count > 0)
	{
		// Get the buffer size for the first filename
		DWORD nBuffer = DragQueryFile(hDropInfo, 0, nullptr, 0);

#ifdef _UNICODE
		i3::stack_wstring buff(nBuffer + 1, 0);
#else
		i3::stack_string buff(nBuffer + 1, 0);
#endif
		DragQueryFile(hDropInfo, 0, &buff[0], buff.size() );

		size_t len = i3::generic_string_size(buff.c_str());
		buff.erase(len);

		// 이제 어떡하나? 힛테스트 위치를 통한 삽입 대신 그냥 자동소팅 순서로 삽입하고, EnsureVisible처리할것..

		i3TDKDlgChangeTex* pDlg = static_cast<i3TDKDlgChangeTex*>(this->GetParent());

		pDlg->AddNewRefi3sItem(buff.c_str() );
	}

	CListCtrl::OnDropFiles(hDropInfo);
}

class ChangeTexPefListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(ChangeTexPefListCtrl)
public:
	ChangeTexPefListCtrl() {}
	virtual ~ChangeTexPefListCtrl() {}
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
};

IMPLEMENT_DYNAMIC(ChangeTexPefListCtrl, CListCtrl)

BEGIN_MESSAGE_MAP(ChangeTexPefListCtrl, CListCtrl)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

void ChangeTexPefListCtrl::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int count = DragQueryFile( hDropInfo, 0xFFFFFFFF, nullptr, 0);

	if (count > 0)
	{
		// Get the buffer size for the first filename
		DWORD nBuffer = DragQueryFile(hDropInfo, 0, nullptr, 0);

#ifdef _UNICODE
		i3::stack_wstring buff(nBuffer + 1, 0);
#else
		i3::stack_string buff(nBuffer + 1, 0);
#endif
		DragQueryFile(hDropInfo, 0, &buff[0], buff.size() );

		size_t len = i3::generic_string_size(buff.c_str());
		buff.erase(len);

		// 이제 어떡하나? 힛테스트 위치를 통한 삽입 대신 그냥 자동소팅 순서로 삽입하고, EnsureVisible처리할것..
		i3TDKDlgChangeTex* pDlg = static_cast<i3TDKDlgChangeTex*>(this->GetParent());

		pDlg->AddNewPefFile(buff.c_str());

	}

	CListCtrl::OnDropFiles(hDropInfo);
}




// m_RefCategoryListCtrl
class RefCategoryListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(RefCategoryListCtrl)
public:
	RefCategoryListCtrl() {}
	virtual ~RefCategoryListCtrl() {}
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnDropFiles(HDROP hDropInfo);
};


IMPLEMENT_DYNAMIC(RefCategoryListCtrl, CListCtrl)

BEGIN_MESSAGE_MAP(RefCategoryListCtrl, CListCtrl)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

void RefCategoryListCtrl::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	int count = DragQueryFile( hDropInfo, 0xFFFFFFFF, nullptr, 0);

	if (count > 0)
	{
		// Get the buffer size for the first filename
		DWORD nBuffer = DragQueryFile(hDropInfo, 0, nullptr, 0);

#ifdef _UNICODE
		i3::stack_wstring buff(nBuffer + 1, 0);
#else
		i3::stack_string buff(nBuffer + 1, 0);
#endif
		DragQueryFile(hDropInfo, 0, &buff[0], buff.size() );

		size_t len = i3::generic_string_size(buff.c_str());
		buff.erase(len);

		// 이제 어떡하나? 힛테스트 위치를 통한 삽입 대신 그냥 자동소팅 순서로 삽입하고, EnsureVisible처리할것..

		i3TDKDlgChangeTex* pDlg = static_cast<i3TDKDlgChangeTex*>(this->GetParent());

		pDlg->AddNewRefi3sItem(buff.c_str() );
	}

	CListCtrl::OnDropFiles(hDropInfo);
}

class ChangeTexFileProperty;

class ChangeTexPropertyGridCtrl : public CMFCPropertyGridCtrl
{
	typedef CMFCPropertyGridCtrl	base_type;
public:
	
	bool					IsChangeTexFileProperty(int nIndex) const;
	static bool				IsChangeTexFileProperty(CMFCPropertyGridProperty* prop) ;	
private:

	DECLARE_DYNAMIC( ChangeTexPropertyGridCtrl )

	DECLARE_MESSAGE_MAP()

	virtual void OnChangeSelection( CMFCPropertyGridProperty* pNewSel,  CMFCPropertyGridProperty* pOldSel);
	virtual void OnPropertyChanged( CMFCPropertyGridProperty* pProp) const;
	
	virtual int OnDrawProperty(CDC* pDC, CMFCPropertyGridProperty* pProp) const;

	afx_msg void OnDropFiles(HDROP hDropInfo);
};


void ChangeTexPropertyGridCtrl::OnChangeSelection( CMFCPropertyGridProperty* pNewSel,  CMFCPropertyGridProperty* pOldSel)
{
	// 셀렉션시 텍스쳐 출력 가능해야한다..

	i3TDKDlgChangeTex* pOwner = (i3TDKDlgChangeTex*)GetParent();

	if (pNewSel)
	{
		const TCHAR* szOrigTexName =	pNewSel->GetName();
		pOwner->DisplayTex_Orig(szOrigTexName);
		
		const COleVariant& ole_var = pNewSel->GetValue();
		const _bstr_t&	   bstr_val = ole_var.bstrVal;
		const TCHAR* szTargetTexName = bstr_val;

		pOwner->DisplayTex_Target(szTargetTexName);

		CMFCPropertyGridProperty* parentProp = pNewSel->GetParent();
		
		if (parentProp != nullptr)			//  이경우 LOD에 속함..
		{
			DWORD_PTR lodLv = parentProp->GetData();
			ForceLODView(pOwner->getBodyList(), lodLv);
		}
		else
		{
			ClearForceLODView(pOwner->getBodyList());
		}
	}
	else
	{
		pOwner->DisplayTex_Orig(nullptr);
		pOwner->DisplayTex_Target(nullptr);
		ClearForceLODView(pOwner->getBodyList());
	}

	CMFCPropertyGridCtrl::OnChangeSelection(pNewSel, pOldSel);
}


BEGIN_MESSAGE_MAP(ChangeTexPropertyGridCtrl, CMFCPropertyGridCtrl)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

class CButton_InProperty : public CButton
{
	DECLARE_DYNAMIC(CButton_InProperty)
	DECLARE_MESSAGE_MAP()

	typedef CButton_InProperty		base_type;
public:
	CButton_InProperty() : m_prop(nullptr) {}
	void		 SetProperty(CMFCPropertyGridProperty* prop) { m_prop = prop; }

private:
	afx_msg void OnBnClicked();
	
	CMFCPropertyGridProperty* m_prop;
	
};

IMPLEMENT_DYNAMIC( CButton_InProperty, CButton)

BEGIN_MESSAGE_MAP(CButton_InProperty, CButton)
ON_CONTROL_REFLECT(BN_CLICKED, &CButton_InProperty::OnBnClicked)
END_MESSAGE_MAP()



class ChangeTexFileProperty : public CMFCPropertyGridFileProperty
{
	DECLARE_DYNAMIC( ChangeTexFileProperty)
	typedef CMFCPropertyGridFileProperty	base_type;

public:
	ChangeTexFileProperty(const CString& strName, const CString& strFolderName, DWORD_PTR dwData = 0, LPCTSTR lpszDescr = nullptr) 
		: base_type(strName, strFolderName, dwData, lpszDescr), m_RegString(nullptr) 
	{
	}

	ChangeTexFileProperty(const CString& strName, BOOL bOpenFileDialog, const CString& strFileName, LPCTSTR lpszDefExt = nullptr,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = nullptr, LPCTSTR lpszDescr = nullptr, DWORD_PTR dwData = 0) 
		: base_type(strName, bOpenFileDialog, strFileName, lpszDefExt, dwFlags , lpszFilter, lpszDescr, dwData)
		, m_RegString(nullptr)
	{
	}
	
	virtual ~ChangeTexFileProperty()
	{
		I3_SAFE_RELEASE(m_RegString);
	}
#ifdef _UNICODE
	i3::wstring		GetTargetName() const
	{
		const COleVariant& ole_var = base_type::GetValue();
		return i3::wstring( (const TCHAR*)(const _bstr_t&)ole_var.bstrVal) ; 
	}
#else
	i3::string		GetTargetName() const 
	{ 
		const COleVariant& ole_var = base_type::GetValue();
		return i3::string( (const TCHAR*)(const _bstr_t&)ole_var.bstrVal) ; 
	}
#endif	

	void			CreateCheckBox()
	{

		m_check_ctrl.Create(_T(""),  WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX, CRect(0,0,0,0), m_pWndList, 0);
		m_check_ctrl.SetProperty(this);
		m_check_ctrl.EnableWindow(IsAllowEdit());		// 초기값으로 일단 꺼둔다..
	}

	void			SetCheck(BOOL enable)	{  m_check_ctrl.SetCheck(enable); }
	BOOL			GetCheck() const		{ return m_check_ctrl.GetCheck(); }
		

	void			SetTargetName(const TCHAR* varValue)
	{
		//
		base_type::SetValue((_variant_t)varValue);
		//
		i3TDKDlgChangeTex* pDlg = GetDialog();
		CMFCPropertyGridProperty* parent_prop = GetParentProperty();

//		bool IsLODSeperation = pDlg->IsCheckedLODSeperation();
		INT32 curr_lod_idx  = (parent_prop) ? parent_prop->GetData() : -1;
		
		if (varValue == nullptr || varValue[0] == 0 )			// 이름은 있되..해당 값이 비워진 케이스 (원본 유지)
		{
		
			BOOL bCheck = m_check_ctrl.GetCheck();

			if ( ChangeTexture(m_texGroup.get(), varValue, false) )
			{
				
				if (bCheck == FALSE)
				{
					EnableTexture(m_texGroup.get(), true, false);		// 강제로 true로 만든다..아랫쪽에 체크박스도 강제로 셋팅..
				}

				if ( this->IsSelected() )
				{
					pDlg->DisplayTex_Target(nullptr);
				}
			}
			
			I3_SAFE_RELEASE(m_RegString);
		//	RemoveRegStringFromRegKey();		
			
			m_check_ctrl.EnableWindow(TRUE);
			m_check_ctrl.SetCheck(TRUE);			// 강제로 TRUE체크...
			base_type::Enable(TRUE);
		}
		else if ( !i3::generic_is_equal( varValue, _T("(null)") ) )
		{
					
		//	bool bTexChange = !i3::generic_is_iequal(this->GetName(), szTo);

			if ( ChangeTexture(m_texGroup.get(), varValue, true) )		// 명시적으로 레퍼런스와 같은 텍스쳐이름이 들어간 것도 강제로 켠것으로 간주한다.
			{
				if ( this->IsSelected() )
				{
					pDlg->DisplayTex_Target(varValue);
				}
			}
			else
			{
				if ( ChangeTexture(m_texGroup.get(), nullptr, false) )		// 타겟이 실패한 경우 맨처음의 원래 텍스쳐로 되돌린다.
				{
					if (this->IsSelected())
					{
						pDlg->DisplayTex_Target(nullptr);
					}
				}
			}
			
			SetRegStringIntoClonedi3RegKey(pDlg->GetClonedi3sListCtrl(), m_RegString, this->GetName(), varValue, curr_lod_idx);

			m_check_ctrl.EnableWindow(FALSE);		// 특정값이 들어오면 체크박스 입력을 막는다..
			m_check_ctrl.SetCheck(TRUE);
			base_type::Enable(TRUE);
		}
		else			// (null) 인 경우..
		{
		
			ChangeTexture(m_texGroup.get(), nullptr, true);			// 끄게 되면 텍스쳐변경에 영향을 준다..( 마지막 인수 true )
	
			EnableTexture(m_texGroup.get(), false, true);
																
			if ( this->IsSelected() )
			{
				pDlg->DisplayTex_Target(nullptr);
			}
						
			SetRegStringIntoClonedi3RegKey(pDlg->GetClonedi3sListCtrl(), m_RegString, this->GetName(), varValue, curr_lod_idx);

			m_check_ctrl.EnableWindow(TRUE);		// '(null)'인 경우엔 체크박스입력이 가능..
			m_check_ctrl.SetCheck(FALSE);
			base_type::Enable(FALSE);
		}
		
		pDlg->EnableLODSeperationCheck( pDlg->IsSafeModifyCheckLODSeperation() );

	}
	
	void			SetTargetName(i3RegString* reg_str)
	{
		I3_REF_CHANGE(m_RegString, reg_str);
		
		if (reg_str != nullptr)
		{
#ifdef _UNICODE
			const i3::rc_wstring& str = reg_str->getValue();
#else
			const i3::rc_wstring& wstr = reg_str->getValue();
			i3::string str;		i3::utf16_to_mb(wstr, str);
#endif
			ChangeTexFileProperty::SetTargetName(str.c_str());
		}
		else
		{
			ChangeTexFileProperty::SetTargetName(_T("") );
		}
	}
	
	void			RemoveRegStringFromRegKey()		
	{
		if ( m_RegString )
		{
			i3TDKDlgChangeTex* pDlg = GetDialog();
			CMFCPropertyGridProperty* parent_prop = GetParentProperty();
//			bool IsLODSeperation = pDlg->IsCheckedLODSeperation();
			INT32 curr_lod_idx  = (parent_prop) ? parent_prop->GetData() : -1;

			const CListCtrl* list_ctrl =	pDlg->GetClonedi3sListCtrl();

			INT32 idx = GetSelectedItemIndex(*list_ctrl);
			
			if ( idx != -1)
			{
				i3RegKey* tex_key = (i3RegKey*)list_ctrl->GetItemData(idx);

				if (curr_lod_idx == -1)
				{
					tex_key->RemoveData(m_RegString);				// 실제로 안지워질수도 있다..
				}
				else
				{
					INT32 childCount = tex_key->getChildCount();
					I3ASSERT( curr_lod_idx < childCount);
					i3RegKey* lod_key = (i3RegKey*)tex_key->getChild(curr_lod_idx);
					lod_key->RemoveData(m_RegString);
				}
			}

			I3_SAFE_RELEASE(m_RegString);
		}
	}

	void	OnCheckClicked(BOOL bCheck)
	{
		// 켠것과 끈것이 구현자체 차이가 있음..
		i3TDKDlgChangeTex* pDlg = GetDialog();
		CMFCPropertyGridProperty* parent_prop = GetParentProperty();

//		bool IsLODSeperation = pDlg->IsCheckedLODSeperation();
		INT32 curr_lod_idx  = (parent_prop) ? parent_prop->GetData() : -1;

	
		if (bCheck)
		{	// 끈것을 켜게 되면...?
			I3ASSERT( m_RegString );
			I3ASSERT( i3::generic_is_equal(m_RegString->getValue(), L"(null)" ) );			// 이렇게 되길 가정한다..
	
			EnableTexture(m_texGroup.get(), true, false);		// 레퍼런스 텍스쳐이므로 텍스쳐변경옵션이 안들어간걸로 가정...
			RemoveRegStringFromRegKey();						// 레지스트리에선 삭제한다..

			base_type::SetValue((_variant_t)_T("") );
		}
		else		// 켠것을 끈다면...
		{
			I3ASSERT( !m_RegString);
		
			EnableTexture(m_texGroup.get(), false, true);		// 강제 적용..
			
			// 새로 추가된것...RegString을 만들고 부모에 연결해야한다..
			SetRegStringIntoClonedi3RegKey(pDlg->GetClonedi3sListCtrl(), m_RegString, this->GetName(), _T("(null)"), curr_lod_idx);

			base_type::SetValue((_variant_t)_T("(null)") );
		}	
		
		base_type::Enable(bCheck);				// 체크되어있으면 편집 가능 상태로 둔다..

		pDlg->EnableLODSeperationCheck( pDlg->IsSafeModifyCheckLODSeperation() );

	}
	
	void						EnableCheckbox(BOOL bEnable)		
	{
		m_check_ctrl.EnableWindow(bEnable);
	}

	BOOL						IsEnableCheckbox() const
	{
		return m_check_ctrl.IsWindowEnabled();
	}

	void						ShowCheckBox(BOOL bEnable)
	{
		m_check_ctrl.ShowWindow( (bEnable) ? SW_SHOW : SW_HIDE);
	}

	// orig tex가 정확히 원본이어야함..(원본을 어따 저장할까..)
	void SetTexGroup(const i3TDKChangeTex_TexGroupPtr& texGroup)
	{
		m_texGroup = texGroup;
	}
	//

	ChangeTexPropertyGridCtrl*	GetPropertyCtrl() const 
	{
		return static_cast<ChangeTexPropertyGridCtrl*>(m_pWndList);
	}
	
	CMFCPropertyGridProperty*	GetParentProperty() const			// 톱레벨은 nullptr이고 서브프로퍼티의 경우 부모프로퍼티임..
	{
		return GetParent();
	}

	i3TDKDlgChangeTex*			GetDialog() const 
	{
		ChangeTexPropertyGridCtrl* ctrl = GetPropertyCtrl();
		return (ctrl == nullptr) ? nullptr : static_cast<i3TDKDlgChangeTex*>(ctrl->GetParent());
	}

	virtual void OnClickButton(CPoint point);
	/*
	virtual void AdjustInPlaceEditRect(CRect& rectEdit, CRect& rectSpin)		// rectSpin은 방치..
	{
		base_type::AdjustInPlaceEditRect(rectEdit, rectSpin);
		rectEdit.right -= rectEdit.Height() * 2;
	}
	*/

	virtual BOOL OnSetCursor() const
	{
		// 체크박스 위로 커서가 오면....return FALSE를 때려야한다..
		CPoint point;
		::GetCursorPos(&point);
		CRect rc;
		m_check_ctrl.GetWindowRect(&rc);
		if ( rc.PtInRect(point) )
			return FALSE;

		return base_type::OnSetCursor();
	}

	virtual void OnDrawName(CDC* pDC, CRect rect )
	{
		CRect rc_check = rect;
		int height = rect.Height();
		rect.right -= height;
		
		base_type::OnDrawName(pDC, rect);

	//	rc_check.right -= height;
		rc_check.left = rc_check.right - height;
		
		if ( !m_check_ctrl.GetSafeHwnd() )
		{
			m_check_ctrl.Create(_T(""),  WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX, rc_check, m_pWndList, 0);
			m_check_ctrl.SetProperty(this);
			m_check_ctrl.EnableWindow(IsAllowEdit());		// 초기값으로 일단 꺼둔다..
		}
		else
		{
			m_check_ctrl.SetWindowPos(0,rc_check.left, rc_check.top, 
				rc_check.right - rc_check.left, rc_check.bottom - rc_check.top, SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW );
			//m_check_ctrl.Invalidate();	//vs2015 오면서 부모윈도우까지 호출되는듯?
		}
	}
	

/*
	virtual void OnDrawValue(CDC* pDC, CRect rect )
	{
		CRect rc_check = rect;
		
		int height = rect.Height();
		rect.right -= height*2;

		base_type::OnDrawValue(pDC, rect);

		rc_check.right -= height;
		rc_check.left = rc_check.right - height;
		
		if ( !m_check_ctrl.GetSafeHwnd() )
		{
			m_check_ctrl.Create("",  WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX, rc_check, m_pWndList, 0);
//			m_check_ctrl.SetWindowPos(0,rc_check.left, rc_check.top, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER );
		}
		
	}
*/

//  아랫것은 쓰지 않도록 선언처리..
	virtual const COleVariant& GetValue() const { return base_type::GetValue();  }
	virtual void SetValue(const COleVariant& varValue) { base_type::SetValue(varValue); }
	DWORD_PTR GetData() const;
	void SetData(DWORD_PTR dwData);
//
//#ifdef _UNICODE
//	i3::rc_wstring		m_strCurrTexName;
//#else
//	i3::rc_string		m_strCurrTexName;
//#endif

	i3RegString*		m_RegString;
	
	i3TDKChangeTex_TexGroupPtr	m_texGroup;

	CButton_InProperty	m_check_ctrl;

};

void CButton_InProperty::OnBnClicked()
{
	// TODO: Add your control notification handler code here
	BOOL bCheck = this->GetCheck();
	((ChangeTexFileProperty*)m_prop)->OnCheckClicked(bCheck);


}

bool	ChangeTexPropertyGridCtrl::IsChangeTexFileProperty(int nIndex) const
{
	CMFCPropertyGridProperty* prop = base_type::GetProperty(nIndex);
	return ( prop->IsKindOf( RUNTIME_CLASS(ChangeTexFileProperty) ) != FALSE );
}

bool	ChangeTexPropertyGridCtrl::IsChangeTexFileProperty(CMFCPropertyGridProperty* prop) 
{
	return ( prop->IsKindOf( RUNTIME_CLASS(ChangeTexFileProperty) ) != FALSE );
}

//
//ChangeTexFileProperty*	ChangeTexPropertyGridCtrl::GetProperty(int nIndex) const 
//{ 
//	return static_cast<ChangeTexFileProperty*>(base_type::GetProperty(nIndex) );
//}

int ChangeTexPropertyGridCtrl::OnDrawProperty(CDC* pDC, CMFCPropertyGridProperty* pProp) const
{
	int res = base_type::OnDrawProperty(pDC, pProp);
	
	if ( pProp->IsKindOf( RUNTIME_CLASS(ChangeTexFileProperty)) == FALSE)
	{
		INT32 num_sub = pProp->GetSubItemsCount();

		if ( pProp->IsExpanded() )
		{
			for (INT32 i = 0 ; i < num_sub ; ++i)
			{
				ChangeTexFileProperty* texfile_prop = static_cast<ChangeTexFileProperty*>(pProp->GetSubItem(i));
				texfile_prop->ShowCheckBox(TRUE);
			}
		}
		else
		{
			for (INT32 i = 0 ; i < num_sub ; ++i)
			{
				ChangeTexFileProperty* texfile_prop = static_cast<ChangeTexFileProperty*>(pProp->GetSubItem(i));
				texfile_prop->ShowCheckBox(FALSE);
			}
		}
	}

	return res;
}



void ChangeTexPropertyGridCtrl::OnPropertyChanged( CMFCPropertyGridProperty* mfcProp) const
{
	ChangeTexFileProperty* prop = static_cast<ChangeTexFileProperty*>(mfcProp);

	if (prop != nullptr)
	{
#ifdef _UNICODE
		i3::wstring		abs_path =	prop->GetTargetName();
		i3::rc_wstring	rel_path;
#else
		i3::string		abs_path =	prop->GetTargetName();
		i3::rc_string	rel_path;
#endif
		
		i3::make_relative_path( g_pTDKEnvSet->getCurWorkDir(), abs_path, rel_path);
		i3::make_unix_path(rel_path);

		// 프로퍼티를 수정한 경우에 문자열이 비었다면...i3RegString의 연결고리도 끊어서 저장에서 지울수 있도록 처리하는게 좋겠다..
		if (rel_path.empty() )
		{
			prop->RemoveRegStringFromRegKey();
		}

		prop->SetTargetName( rel_path.c_str() );

	
	}	

	base_type::OnPropertyChanged(prop);
}


void ChangeTexPropertyGridCtrl::OnDropFiles(HDROP hDropInfo)
{
	//
	// TODO: Add your message handler code here and/or call default
	// 셀렉션이 되어있지 않은 곳일수 있다...
	// 정확히 하려면...드래그하면서 셀렉션이 이루어져야한다..(이건 나중에)

	int count = DragQueryFile( hDropInfo, 0xFFFFFFFF, nullptr, 0);
	
	if (count > 0)
    {
        // Get the buffer size for the first filename
        DWORD nBuffer = DragQueryFile(hDropInfo, 0, nullptr, 0);

#ifdef _UNICODE
		i3::stack_wstring buff(nBuffer + 1, 0);
#else
		i3::stack_string buff(nBuffer + 1, 0);
#endif
		DragQueryFile(hDropInfo, 0, &buff[0], buff.size() );
		
		size_t len = i3::generic_string_size(buff.c_str());
		buff.erase(len);
		
		// 이제 어떡하나? (힛테스트?)
		
		CPoint CurrentLocalPosition;
		::GetCursorPos(&CurrentLocalPosition);
		
		this->ScreenToClient( &CurrentLocalPosition);

		CMFCPropertyGridProperty* mfc_prop = this->HitTest(CurrentLocalPosition);
		
		if (mfc_prop && mfc_prop->IsEnabled() && mfc_prop->IsAllowEdit() )
		{
			ChangeTexFileProperty* prop = static_cast<ChangeTexFileProperty*>(mfc_prop);
#ifdef _UNICODE
			i3::stack_wstring rel_path;
#else
			i3::stack_string	rel_path;
#endif
			i3::make_relative_path( g_pTDKEnvSet->getCurWorkDir(), buff, rel_path);
			i3::make_unix_path(rel_path);

			this->SetCurSel(prop);
			prop->SetTargetName(rel_path.c_str());
			this->SetFocus();
			
			if (rel_path.empty())
			{
				prop->RemoveRegStringFromRegKey();
			}
		}
    }

	CMFCPropertyGridCtrl::OnDropFiles(hDropInfo);
}

//


void ChangeTexFileProperty::OnClickButton(CPoint point)
{
	if (this->IsAllowEdit() == FALSE)
		return;
	base_type::OnClickButton(point);
}


IMPLEMENT_DYNAMIC( ChangeTexPropertyGridCtrl, CMFCPropertyGridCtrl)
IMPLEMENT_DYNAMIC( ChangeTexFileProperty, CMFCPropertyGridFileProperty)


IMPLEMENT_DYNAMIC(i3TDKDlgChangeTex, CDialog)

i3TDKDlgChangeTex::i3TDKDlgChangeTex(CWnd* pParent /*=nullptr*/)
: CDialog(i3TDKDlgChangeTex::IDD, pParent), 
m_pSgInfo(nullptr), m_ChangeTexCB(nullptr), m_MaxLODCount(0), m_curPefIdx(-1)
{
	m_wndPropList = new ChangeTexPropertyGridCtrl;
	m_ChangeTexPefListCtrl = new ChangeTexPefListCtrl;
	m_RefCategoryListCtrl = new RefCategoryListCtrl;
	m_Refi3sListCtrl = new Refi3sListCtrl;
}

i3TDKDlgChangeTex::~i3TDKDlgChangeTex()
{
	i3::cu::for_each_delete(m_pefList);

	delete m_Refi3sListCtrl;
	delete m_RefCategoryListCtrl;
	delete m_ChangeTexPefListCtrl;
	delete m_wndPropList;
	delete m_ChangeTexCB;
}

void i3TDKDlgChangeTex::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_CLONED_I3S_LIST, m_Clonedi3sListCtrl);
	DDX_Control(pDX, IDC_CHANGETEX_PEF_LIST, *m_ChangeTexPefListCtrl);
	DDX_Control(pDX, IDC_REF_CATEGORY_LIST, *m_RefCategoryListCtrl);
	DDX_Control(pDX, IDC_REF_I3S_LIST, *m_Refi3sListCtrl);
}

namespace
{
	void ClearTexGroup(i3::vector<i3TDKChangeTex_TexGroupPtr>&	vecTexGroup_NonSep,
		i3::vector<i3TDKChangeTex_TexGroupPtr>& vecTexGroup_NoLOD,
		i3::vector<i3::vector<i3TDKChangeTex_TexGroupPtr> >& vecvecTexGroup_LOD)
	{
		vecTexGroup_NonSep.clear();
		vecTexGroup_NoLOD.clear();
		vecvecTexGroup_LOD.clear();
	}

	void BuildTexGroup( i3Node* pNode, UINT32 maxLODCount,	i3::vector<i3TDKChangeTex_TexGroupPtr>&	vecTexGroup_NonSep,
										i3::vector<i3TDKChangeTex_TexGroupPtr>& vecTexGroup_NoLOD,
										i3::vector<i3::vector<i3TDKChangeTex_TexGroupPtr> >& vecvecTexGroup_LOD)
	{
		i3::vector<i3Texture*>	texList;
		i3::vector<i3TextureBindAttr*>	attrList;

		ClearTexGroup( vecTexGroup_NonSep, vecTexGroup_NoLOD, vecvecTexGroup_LOD);
		
		// 1. NonSep에 대해서 먼저 획득처리..

		ExtractAllTexturesFromNode(pNode, texList);
		ExtractAllTextureBindAttrFromNode(pNode, attrList);

		const size_t numTex_NonSep = texList.size();

		for ( size_t i = 0 ; i < numTex_NonSep ; ++i )
		{
			i3TDKChangeTex_TexGroupPtr new_obj = i3::make_shared<i3TDKChangeTex_TexGroup>();
			i3TDKChangeTex_TexGroup* pobj = new_obj.get();
			pobj->m_origTex = texList[i];
			pobj->m_origTex->AddRef();
			ExtractTextureBindAttrsFromTexture(attrList, pobj->m_origTex, pobj->m_attrList);
			vecTexGroup_NonSep.push_back(new_obj);			
		}
		
		// 2. Sep/ Non LOD에 대한 획득 처리..
		texList.clear();
		attrList.clear();
		ExtractAllTexturesFromNode_LOD(pNode, -1, texList);
		ExtractAllTextureBindAttrFromNode_LOD(pNode, -1, attrList);

		const size_t numTex_NonLOD = texList.size();

		for (size_t i = 0 ; i < numTex_NonLOD ; ++i)
		{
			i3TDKChangeTex_TexGroupPtr new_obj = i3::make_shared<i3TDKChangeTex_TexGroup>();
			i3TDKChangeTex_TexGroup* pobj = new_obj.get();
			pobj->m_origTex = texList[i];
			pobj->m_origTex->AddRef();
			ExtractTextureBindAttrsFromTexture(attrList, pobj->m_origTex, pobj->m_attrList);
			vecTexGroup_NoLOD.push_back(new_obj);
		}
		
		// 3. LOD 에 대한 획득 처리..
		
		vecvecTexGroup_LOD.resize(maxLODCount);

		for (size_t i = 0 ; i < maxLODCount ; ++i )
		{
			texList.clear();
			attrList.clear();
			ExtractAllTexturesFromNode_LOD(pNode, i, texList);
			ExtractAllTextureBindAttrFromNode_LOD(pNode, i, attrList);

			i3::vector<i3TDKChangeTex_TexGroupPtr>& curr_attr_list = vecvecTexGroup_LOD[i];

			const size_t numTex_LOD = texList.size();
						
			for (size_t j = 0; j < numTex_LOD ; ++j)
			{
				i3TDKChangeTex_TexGroupPtr new_obj = i3::make_shared<i3TDKChangeTex_TexGroup>();
				i3TDKChangeTex_TexGroup* pobj = new_obj.get();
				pobj->m_origTex = texList[j];
				pobj->m_origTex->AddRef();
				ExtractTextureBindAttrsFromTexture(attrList, pobj->m_origTex, pobj->m_attrList);
				curr_attr_list.push_back(new_obj);
			}
			
		}
				
	}
	
	
}

void	i3TDKDlgChangeTex::SetFilenameAndSgInfo(const char* szPath, i3SceneGraphInfo* pSgInfo)	
{ 
	// 목록 변경이 이루어져야할것이다..


	m_pSgInfo = pSgInfo; 
	if ( pSgInfo)
	{
		m_MaxLODCount = i3Scene::FindMaxLODCount(pSgInfo->getInstanceSg());

		i3::vector<i3Node*> nodeList;
		i3Scene::FindNodeByExactType(pSgInfo->getInstanceSg(), i3Body::static_meta(), nodeList);
		
		const size_t numBody = nodeList.size();
		m_bodyList.resize(numBody);
		for (size_t i = 0 ; i < numBody ; ++i ) 
			m_bodyList[i] =  static_cast<i3Body*>(nodeList[i]);

		BuildTexGroup(pSgInfo->getInstanceSg(), m_MaxLODCount, m_vecTexGroup_NonSep, m_vecTexGroup_NoLOD, m_vecvecTexGroup_LOD);
	}
	else
	{
		m_MaxLODCount = 0;
		m_bodyList.clear();
		ClearTexGroup(m_vecTexGroup_NonSep, m_vecTexGroup_NoLOD, m_vecvecTexGroup_LOD);
	}
	
	INT32 ref_idx = -1;
	i3::stack_string strFolder, strFile;

	if (szPath != nullptr && szPath[0] != 0)
	{
		SplitFolderAndFile(szPath, strFolder, strFile);
		bool bNeedUpdatei3sListCtrl = false;

		I3ASSERT(m_curPefIdx != -1);
		I3ASSERT(m_curPefIdx < INT32(m_pefList.size()));
		i3TDKChangeTex_Pef* currPef = m_pefList[m_curPefIdx];

		i3TDKChangeTex_RefCategory* pCat = FindOrCreateRefCategory(currPef, strFolder.c_str(), *m_RefCategoryListCtrl, bNeedUpdatei3sListCtrl);
		
		if (bNeedUpdatei3sListCtrl)
		{
			UpdateRefi3sListCtrl();
		}

		ref_idx = FindReferenceIndex(pCat, strFile.c_str());

//		currPef->m_currSelectedCategoryIdx = GetSelectedItemIndex(*m_RefCategoryListCtrl);
	}

	UpdateClonedListCtrls(ref_idx);

		
	this->EnableLODSeperationCheck(false);
	this->SetCheckLODSeperation(m_MaxLODCount > 0);

	BuildProperties();
	DisplayTex_Orig(nullptr);
	DisplayTex_Target(nullptr);

	int sel_cat_idx = GetSelectedItemIndex(*m_RefCategoryListCtrl);
	
	if (ref_idx == -1)
	{
		INT32 prev_idx =	GetSelectedItemIndex(*m_Refi3sListCtrl);
		if (prev_idx != -1)
		{
			m_Refi3sListCtrl->SetItemState(prev_idx, LVIS_FOCUSED | LVIS_SELECTED, 0);
		}
		m_Clonedi3sListCtrl.EnableWindow(FALSE);		
		m_wndPropList->EnableWindow(FALSE);
		
		if (sel_cat_idx != -1)
		{

			I3ASSERT(m_curPefIdx != -1);
			I3ASSERT(m_curPefIdx < INT32(m_pefList.size()));
			i3TDKChangeTex_Pef* currPef = m_pefList[m_curPefIdx];
			i3::vector<i3TDKChangeTex_RefCategory*>& refCategoryList = currPef->m_refCategoryList;

			refCategoryList[sel_cat_idx]->m_currSelectedi3sIndex = -1;
		}
	}
	else
	{
		m_Refi3sListCtrl->SetItemState(ref_idx, LVIS_FOCUSED|LVIS_SELECTED, LVIS_FOCUSED|LVIS_SELECTED);
		m_Clonedi3sListCtrl.EnableWindow(TRUE);		
		m_wndPropList->EnableWindow(TRUE);
		if ( i3::generic_is_iequal(szPath, m_filename_from_filedlg ) ) 
			m_filename_from_filedlg.clear();		// 기존에 있을땐 제거한다..

		if (sel_cat_idx != -1)
		{

			I3ASSERT(m_curPefIdx != -1);
			I3ASSERT(m_curPefIdx < INT32(m_pefList.size()));
			i3TDKChangeTex_Pef* currPef = m_pefList[m_curPefIdx];
			i3::vector<i3TDKChangeTex_RefCategory*>& refCategoryList = currPef->m_refCategoryList;

			refCategoryList[sel_cat_idx]->m_currSelectedi3sIndex = ref_idx;
		}
	}

	if (m_curPefIdx != -1)
	{
		I3ASSERT(m_curPefIdx < INT32(m_pefList.size()));
		i3TDKChangeTex_Pef* currPef = m_pefList[m_curPefIdx];
		currPef->m_currSelectedCategoryIdx = sel_cat_idx;
	}

}	// 노드 참조카운팅 관리는 ViewerFramework에서만 처리하고..

void	i3TDKDlgChangeTex::SetFilenameFromFileDialog(const TCHAR* szPath)
{
	m_filename_from_filedlg = szPath;
}


void	i3TDKDlgChangeTex::ClearClonedi3sListCtrl()
{
	m_Clonedi3sListCtrl.DeleteAllItems();
	EnableLODSeperationCheck(false);
}

void	i3TDKDlgChangeTex::UpdateClonedListCtrls(INT32 ref_idx)
{
	i3TDKDlgChangeTex::ClearClonedi3sListCtrl();
	
	if ( ref_idx == -1 )		// 그냥 로딩된것.. 
	{
		return;
	}
	
	INT32 cat_idx = GetSelectedItemIndex(*m_RefCategoryListCtrl);
	I3ASSERT(cat_idx != -1);
	
	I3ASSERT(m_curPefIdx != -1);
	I3ASSERT(m_curPefIdx < INT32(m_pefList.size()));
	i3TDKChangeTex_Pef* currPef = m_pefList[m_curPefIdx];
	i3::vector<i3TDKChangeTex_RefCategory*>& refCategoryList = currPef->m_refCategoryList;


	i3TDKChangeTex_Refi3s* curri3s = refCategoryList[cat_idx]->m_Refi3sList[ref_idx];
	
	i3RegKey* k = curri3s->m_pefRefTexKey;
	
	const INT32 num_cloned = k->getChildCount();

	for (INT32 i = 0; i < num_cloned; ++i)
	{
		i3RegKey* clone_tex_key = (i3RegKey*)k->getChild(i);

#ifdef _UNICODE
		i3::stack_wstring wstrCloneFileName;	i3::mb_to_utf16(clone_tex_key->GetName(), wstrCloneFileName);
		const wchar_t* clone_file_name = wstrCloneFileName.c_str();
#else
		const char* clone_file_name = clone_tex_key->GetName();
#endif		
		m_Clonedi3sListCtrl.InsertItem(i, clone_file_name);
		m_Clonedi3sListCtrl.SetItemData(i, (DWORD_PTR)clone_tex_key);
	}

//	if (curri3s->m_currCloneListSelIndex)

}

void	i3TDKDlgChangeTex::ClearProperties()
{
	m_wndPropList->RemoveAll();

	ClearForceLODView(m_bodyList);

	m_pOrigTexCtrl.SetImage(nullptr);
	m_pOrigTexCtrl.InvalidateRect(nullptr);
	m_pTargetTexCtrl.SetImage(nullptr);
	m_pTargetTexCtrl.InvalidateRect(nullptr);
}


void	i3TDKDlgChangeTex::BuildProperties()
{
	//
	i3TDKDlgChangeTex::ClearProperties();
	//

	if ( m_pSgInfo )
	{
		if ( this->IsCheckedLODSeperation() )
		{
			i3TDKDlgChangeTex::BuildPropertiesImp_IfLODSeperation();
		}
		else
		{
			i3TDKDlgChangeTex::BuildPropertiesImp_IfNoSeperation();
		}
	}

	m_wndPropList->AdjustLayout();
	m_wndPropList->RedrawWindow();

}

namespace
{
	void BuildPropertiesImp(ChangeTexPropertyGridCtrl* propCtrl, const i3::vector<i3TDKChangeTex_TexGroupPtr>& group_list)
	{
		const size_t num_tex = group_list.size();
		ChangeTexFileProperty*	pProp;

		for (size_t i = 0; i < num_tex ; ++i )
		{
			const i3TDKChangeTex_TexGroupPtr& group = group_list[i];
			i3TDKChangeTex_TexGroup* pGroup = group.get();

			i3Texture* tex = pGroup->m_origTex; 

			const char* ref_tex_path = tex->GetName();
			if ( strlen(ref_tex_path) == 0 )
			{
				ref_tex_path = tex->getResourceLoadPath();
			}

			if (ref_tex_path != nullptr)
			{
				CString strRefTexPath = ToMFCString(ref_tex_path);

				pProp = new ChangeTexFileProperty(strRefTexPath, TRUE, _T("") );
				pProp->SetTexGroup(group);

				propCtrl->AddProperty(pProp, FALSE, FALSE);
				pProp->AllowEdit(FALSE);		// 일단 생성시에는 입력 막는다..
				pProp->CreateCheckBox();
			}
		}
	}
	
	void BuildPropertiesImp(CMFCPropertyGridProperty* propParent, const i3::vector<i3TDKChangeTex_TexGroupPtr>& group_list)
	{
		const size_t num_tex = group_list.size();
		ChangeTexFileProperty*	pProp;

		for (size_t i = 0; i < num_tex ; ++i )
		{
			const i3TDKChangeTex_TexGroupPtr& group = group_list[i];
			i3TDKChangeTex_TexGroup* pGroup = group.get();

			i3Texture* tex = pGroup->m_origTex; 

			const char* ref_tex_path = tex->GetName();
			if ( strlen(ref_tex_path) == 0 )
			{
				ref_tex_path = tex->getResourceLoadPath();
			}

			if (ref_tex_path != nullptr)
			{
				CString strRefTexPath = ToMFCString(ref_tex_path);

				pProp = new ChangeTexFileProperty(strRefTexPath, TRUE, _T("") );
				pProp->SetTexGroup(group);

				propParent->AddSubItem(pProp); 
				pProp->AllowEdit(FALSE);		// 일단 생성시에는 입력 막는다..
				pProp->CreateCheckBox();
			}
		}

		propParent->Expand(TRUE);
	}
}


void	i3TDKDlgChangeTex::BuildPropertiesImp_IfLODSeperation()
{

	BuildPropertiesImp(m_wndPropList, m_vecTexGroup_NoLOD);
		
	const size_t num_LOD = m_MaxLODCount;
	
	CMFCPropertyGridProperty* lod_prop;

#ifdef _UNICODE
	i3::stack_wstring	prop_name;
#else
	i3::stack_string	prop_name;
#endif

	for (size_t i = 0 ; i < num_LOD ; ++i)
	{
		i3::sprintf(prop_name, _T("LOD %d"), i);
		lod_prop = new CMFCPropertyGridProperty(prop_name.c_str(), 0 );
		lod_prop->SetData(i);						// LOD구분 인덱스를 유저데이터로 취해둔다..(후일, 여의치 않으면 구조체를 쎄워야함)
		m_wndPropList->AddProperty(lod_prop);

		BuildPropertiesImp(lod_prop, m_vecvecTexGroup_LOD[i]);
	}
	
}

void	i3TDKDlgChangeTex::BuildPropertiesImp_IfNoSeperation()
{
	BuildPropertiesImp(m_wndPropList, m_vecTexGroup_NonSep);
}


void	i3TDKDlgChangeTex::Process_EnableEditProperties(BOOL bEnable)
{
	const INT32 num_eff_tex = m_wndPropList->GetPropertyCount();
	
	for (INT32 i = 0 ; i < num_eff_tex ; ++i )
	{
		CMFCPropertyGridProperty* prop = m_wndPropList->GetProperty(i);	

		if ( m_wndPropList->IsChangeTexFileProperty(prop) )
		{
			ChangeTexFileProperty* prop_conc = static_cast<ChangeTexFileProperty*>(prop);
			prop_conc->AllowEdit(bEnable);	
			prop_conc->EnableCheckbox(bEnable);
		}
		else
		{
			const INT32 numSubItem = prop->GetSubItemsCount();

			for (INT32 j = 0; j < numSubItem ; ++j)
			{
				ChangeTexFileProperty* prop_conc = static_cast<ChangeTexFileProperty*>(prop->GetSubItem(j));
				prop_conc->AllowEdit(bEnable);	
				prop_conc->EnableCheckbox(bEnable);
			}
		}
	}
}


namespace 
{
	void DisplayTex(i3TDKImageViewCtrl& ctrl, const TCHAR* tszTexName )
	{
		if (tszTexName != nullptr && tszTexName[0] != 0)
		{
			i3NamedResourceManager* pTexManager = i3ResourceFile::GetResourceManager( i3Texture::static_meta());
#ifdef _UNICODE	
			i3::stack_string strCurWorkDir;		i3::utf16_to_mb(g_pTDKEnvSet->getCurWorkDir(), strCurWorkDir);
			const char* szCurWorkDir = strCurWorkDir.c_str();
			i3::stack_string strTexName;		i3::utf16_to_mb(tszTexName, strTexName);
			const char* szTexName = strTexName.c_str();
#else
			const char* szCurWorkDir = g_pTDKEnvSet->getCurWorkDir();
			const char* szTexName = tszTexName;
#endif

			i3Texture* tex = (i3Texture *) pTexManager->GetResource(szTexName , szCurWorkDir, szTexName );

			if (tex)
			{
				ctrl.SetImage(tex);
			}
			else
			{
				ctrl.SetImage(nullptr);
				ctrl.InvalidateRect(nullptr);
			}
		}
		else
		{
			ctrl.SetImage(nullptr);
			ctrl.InvalidateRect(nullptr);
		}
	}
}

void	i3TDKDlgChangeTex::DisplayTex_Orig(const TCHAR* szOrigTexName)
{
	// 어디 담아두지 말고..그냥 그때그때 읽어들인다....
	DisplayTex( m_pOrigTexCtrl, szOrigTexName);
}

void		i3TDKDlgChangeTex::DisplayTex_Target(const TCHAR* szTargetTexName)
{
	DisplayTex( m_pTargetTexCtrl, szTargetTexName);
}



BEGIN_MESSAGE_MAP(i3TDKDlgChangeTex, CDialog)
	ON_BN_CLICKED(IDC_TDK_BTN_PEFPATH, OnBnClickedTdkBtnPefPath)
	ON_NOTIFY(NM_CLICK, IDC_CLONED_I3S_LIST, OnNMClickClonedi3sList)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_RCLICK, IDC_CLONED_I3S_LIST, &i3TDKDlgChangeTex::OnNMRClickClonedI3sList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_CLONED_I3S_LIST, &i3TDKDlgChangeTex::OnLvnEndlabeleditClonedI3sList)
	ON_NOTIFY(NM_CLICK, IDC_REF_I3S_LIST, &i3TDKDlgChangeTex::OnNMClickRefI3sList)
	ON_BN_CLICKED(IDC_TDK_BTN_SAVEPEF, &i3TDKDlgChangeTex::OnBnClickedTdkBtnSavepef)
	ON_NOTIFY(NM_RCLICK, IDC_REF_I3S_LIST, &i3TDKDlgChangeTex::OnNMRClickRefI3sList)
	ON_BN_CLICKED(IDC_CHECK_CHANGETEX_LOD_SEPERATION, &i3TDKDlgChangeTex::OnBnClickedCheckChangetexLodSeperation)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_REF_I3S, &i3TDKDlgChangeTex::OnBnClickedButtonOpenRefI3s)
	ON_NOTIFY(NM_CLICK, IDC_REF_CATEGORY_LIST, &i3TDKDlgChangeTex::OnNMClickRefCategoryList)
	ON_NOTIFY(NM_CLICK, IDC_CHANGETEX_PEF_LIST, &i3TDKDlgChangeTex::OnNMClickChangetexPefList)
	ON_NOTIFY(NM_RCLICK, IDC_CHANGETEX_PEF_LIST, &i3TDKDlgChangeTex::OnNMRClickChangetexPefList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_CHANGETEX_PEF_LIST, &i3TDKDlgChangeTex::OnLvnEndlabeleditChangetexPefList)
END_MESSAGE_MAP()


// i3TDKDlgTextureTool message handlers

BOOL i3TDKDlgChangeTex::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	// 0. 현재 ENVSET과 WORKDIR 표시...아티스트 피드백... (일단 툴 완전 종료하기 전에는 아마 그대로 유지될것 같다..)
	const TCHAR* szCurProj = g_pTDKEnvSet->getCurProject();
	const TCHAR* szCurrWorkDir = g_pTDKEnvSet->getCurWorkDir();
	
#ifdef _UNICODE
	i3::stack_wstring strCurrWorkDir = szCurrWorkDir;	
#else
	i3::stack_string strCurrWorkDir = szCurrWorkDir;	
#endif
	i3::make_unix_path(strCurrWorkDir);

	GetDlgItem(IDC_STATIC_CHANGETEX_ENVSET)->SetWindowText( szCurProj );
	GetDlgItem(IDC_STATIC_CHANGETEX_WORKDIR)->SetWindowText(strCurrWorkDir.c_str() );
	
	// 0. 리스트콘트롤 설정

	{
		CRect rc;
		m_Clonedi3sListCtrl.GetClientRect(&rc);

		LVCOLUMN col;
		memset( &col, 0, sizeof( LVCOLUMN ) );
		col.mask	= LVCF_TEXT |LVCF_WIDTH| LVCF_FMT;
		col.fmt		= LVCFMT_LEFT;
		col.cx		= rc.right - rc.left - 2;
		col.pszText = _T("cloned i3s filename (not real resource file)");
		m_Clonedi3sListCtrl.InsertColumn( 0, &col);
		m_Clonedi3sListCtrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

		m_Clonedi3sListCtrl.GetHeaderCtrl()->EnableWindow(FALSE);		// 헤더버튼 눌림 막기..

		m_Clonedi3sListCtrl.EnableWindow(FALSE);						// 초기에는 불활성화한다....원본 sginfo가 들어가기전에는 아무것도 하지 않는다..
	}
	
	// 0. pef 리스트콘트롤 설정..
	{
		CRect rc;
		m_ChangeTexPefListCtrl->GetClientRect(&rc);

		LVCOLUMN col;
		memset( &col, 0, sizeof( LVCOLUMN ) );
		col.mask	= LVCF_TEXT |LVCF_WIDTH| LVCF_FMT;
		col.fmt		= LVCFMT_LEFT;
		col.cx		= rc.right - rc.left - 2;
		col.pszText = _T("PEF filename");
		m_ChangeTexPefListCtrl->InsertColumn( 0, &col);
		m_ChangeTexPefListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

		m_ChangeTexPefListCtrl->GetHeaderCtrl()->EnableWindow(FALSE);		// 헤더버튼 눌림 막기..

		m_ChangeTexPefListCtrl->ModifyStyle( LVS_EDITLABELS, 0);

	//	m_ChangeTexPefListCtrl->GetEditControl()->EnableWindow(FALSE);		// 에딧박스 일단 막아보기..
	}
	
	i3TDKDlgChangeTex::InitChangeTexPefListCtrl();

	// 1. 프로퍼티콘트롤 설정..
	{
		CRect parent_rc, rc;
		this->GetWindowRect(&parent_rc);
		CRect parent_rc_client;
		this->GetClientRect(&parent_rc_client);
		
		parent_rc.top    = parent_rc.bottom - parent_rc_client.bottom;

		CWnd* pDummy = this->GetDlgItem(IDC_STATIC_CHANGETEX_LIST_DUMMY); 
		pDummy->GetWindowRect(&rc);
		pDummy->ShowWindow(SW_HIDE);

		rc.left = rc.left - parent_rc.left;
		rc.top	=  rc.top - parent_rc.top;	 
		rc.right =  rc.right - parent_rc.left;
		rc.bottom =  rc.bottom - parent_rc.top;

		if (!m_wndPropList->Create( WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | WS_CLIPCHILDREN, rc, this, (UINT)-1))
		{
			TRACE0("Failed to create Properties Grid \n");
			return -1;      // fail to create
		}

		m_wndPropList->EnableHeaderCtrl( TRUE, _T("Src Texture Filename"), _T("Target Texture Filename") );
		m_wndPropList->SetVSDotNetLook();
		m_wndPropList->MarkModifiedProperties();
		
		m_wndPropList->GetHeaderCtrl().EnableWindow(FALSE);		// 헤더 버튼 막기..
		m_wndPropList->EnableWindow(FALSE);						// 초기에는 불활성화한다....원본 sginfo가 들어가기전에는 아무것도 하지 않는다..

		m_wndPropList->DragAcceptFiles(TRUE);
	}

	{
		m_pOrigTexCtrl.Create( WS_CHILD | WS_VISIBLE, this,  1);
		m_pTargetTexCtrl.Create( WS_CHILD | WS_VISIBLE, this,  2);
		ReplaceImageCtrl();
	}

//	


	// ref list 표시 해야함..

	{
		CRect rc;
		m_Refi3sListCtrl->GetClientRect(&rc);

		LVCOLUMN col;
		memset( &col, 0, sizeof( LVCOLUMN ) );
		col.mask	= LVCF_TEXT |LVCF_WIDTH| LVCF_FMT;
		col.fmt		= LVCFMT_LEFT;
		col.cx		= rc.right - rc.left - 2;
		col.pszText = _T("reference i3s filename (real resource file)");
		m_Refi3sListCtrl->InsertColumn( 0, &col);
		m_Refi3sListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

		m_Refi3sListCtrl->GetHeaderCtrl()->EnableWindow(FALSE);		// 헤더버튼 눌림 막기..
	}

	{
		CRect rc;
		m_RefCategoryListCtrl->GetClientRect(&rc);
		LVCOLUMN col;
		memset( &col, 0, sizeof( LVCOLUMN ) );
		col.mask	= LVCF_TEXT |LVCF_WIDTH| LVCF_FMT;
		col.fmt		= LVCFMT_LEFT;
		col.cx		= rc.right - rc.left - 2;
		col.pszText = _T("reference i3s category (resource base folder)");
		m_RefCategoryListCtrl->InsertColumn( 0, &col);
		m_RefCategoryListCtrl->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

		m_RefCategoryListCtrl->GetHeaderCtrl()->EnableWindow(FALSE);		// 헤더버튼 눌림 막기..
	}

//	i3TDKDlgChangeTex::InitRefCategoryi3sList();						// 한번만...

	i3TDKDlgChangeTex::UpdateRefCategoryListCtrl();						// 한번만...

	i3TDKDlgChangeTex::UpdateRefi3sListCtrl();

	// 2. 현재 리소스가 존재하면 해당 리소스의 이름 표시

	if (m_RefCategoryListCtrl->GetItemCount() > 0)
		i3TDKDlgChangeTex::Process_Selection_RefCategoryListCtrl( 0 );

	m_ChangeTexPefListCtrl->SetFocus();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void	i3TDKDlgChangeTex::InitChangeTexPefListCtrl()					// 한번만 실행..
{
	// 등록된 모든 pef파일을 읽는게 좋겠다...  읽고나서 RootKey를 여러개 보관해야하므로, m_pefRootKey는 변수이름을 바꿔야하고 
	// RootKeyList를 멤버로 들고 있어야함... (항상 모든 pef를 미리 다 읽어둔다...)
	m_ChangeTexPefListCtrl->DeleteAllItems();
	INT32 numPef = g_pTDKEnvSet->getCurNumChangeTexPefPath();
	
	i3::stack_string  strRelPefPath;
	i3::stack_string  strFullPath;
	
	for (INT32 i = 0 ; i < numPef ; ++i )
	{
		strRelPefPath = g_pTDKEnvSet->getCurChangeTexPefPath(i);
		
		i3::make_unix_path(strRelPefPath);
		
		strFullPath = g_pTDKEnvSet->getCurWorkDir();
		strFullPath += '/';
		strFullPath += strRelPefPath;
			
		// pef 로딩.. 모두 읽어야한다~
		i3RegKey* pefRootKey = nullptr;

		if( !strRelPefPath.empty() )
			pefRootKey = LoadChangeTexPef(strFullPath.c_str());
		
		if (pefRootKey == nullptr)
		{
			i3::stack_string strMsg;
			i3::sprintf(strMsg, "The project does not have the following PEF file or it is invalid : %s \n Empty PEF file was been set.", strFullPath);
			::AfxMessageBox(strMsg.c_str());
			pefRootKey =  CreatePefRootKey();			// 없으면 일단 새것이라도 만든다..	// 무조건 존재해야함..
		}
		
		i3TDKChangeTex_Pef* new_pef = new i3TDKChangeTex_Pef(m_pefList, strRelPefPath, pefRootKey);
		i3TDKDlgChangeTex::InitRefCategoryi3sList(new_pef);
		pefRootKey->Release();
	}
	
	// 하나도 없었다면 하나 만들어준다.... ( config 상대폴더로 Temp.pef를 만든다....)
	if (numPef == 0)
	{
		i3RegKey* pefRootKey = CreatePefRootKey();
		i3TDKChangeTex_Pef* new_pef = new i3TDKChangeTex_Pef(m_pefList, "config/Temp.i3RegXML", pefRootKey);
		i3TDKDlgChangeTex::InitRefCategoryi3sList(new_pef);
		pefRootKey->Release();
		numPef = 1;
	}
	
	// 리스트콘트롤 등록은 맨 나중에 해주도록 한다...
	
	for ( INT32 i = 0 ; i < numPef ; ++i)
	{
		i3TDKChangeTex_Pef* pef = m_pefList[i];	
		m_ChangeTexPefListCtrl->InsertItem(i, pef->m_strRelPath.c_str());						// 리스트콘트롤에 등록...
		m_ChangeTexPefListCtrl->SetItemData(i, DWORD_PTR(pef));
	}
			
	m_curPefIdx = 0;			// 최소 하나 이상 존재한다 가정했을때 일단은 0을 강제선택한다....

	m_ChangeTexPefListCtrl->SetFocus();
	m_ChangeTexPefListCtrl->SetItemState(m_curPefIdx,  LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	m_ChangeTexPefListCtrl->EnsureVisible(m_curPefIdx, false);
	GetDlgItem(IDC_STATIC_CHANGETEX_PEFPATH)->SetWindowText(m_pefList[m_curPefIdx]->m_strRelPath.c_str());
}


void	i3TDKDlgChangeTex::InitRefCategoryi3sList( i3TDKChangeTex_Pef* pef )			// 각 pef별로 한번씩 수행...					
{

	i3TDKChangeTex_Pef* currPef = pef;

	const INT32 num_subdir = currPef->m_pefRootKey->getChildCount();

	for (INT32 i = 0; i < num_subdir ; ++i )
	{	
		i3TreeNode* sub_dir = currPef->m_pefRootKey->getChild(i);
	
		i3TDKChangeTex_RefCategory* new_cat = new i3TDKChangeTex_RefCategory(currPef, static_cast<i3RegKey*>(sub_dir));		// 생성자내부에서 insert처리..

		const INT32 num_ref_filename = sub_dir->getChildCount();

		for (INT32 j = 0; j < num_ref_filename ; ++j )
		{
			i3RegKey* ref_node = (i3RegKey*)sub_dir->getChild(j);
			new i3TDKChangeTex_Refi3s(new_cat, static_cast<i3RegKey*>(ref_node));			// 생성자 내부 insert처리 때문에 현재는 좌항이 없다.
		}
	}

	if ( !currPef->m_refCategoryList.empty() )
	{
		currPef->m_currSelectedCategoryIdx = 0;				// 처음 시점에서 하나라도 카테고리 존재시..0셀렉션을 지정해준다..
	}

}	

void	i3TDKDlgChangeTex::UpdateRefCategoryListCtrl()			// 한번만 실행..
{
	m_RefCategoryListCtrl->DeleteAllItems();

//	I3ASSERT(m_curPefIdx != -1);
//	I3ASSERT(m_curPefIdx < INT32(m_pefList.size()));

	if (m_curPefIdx == -1)			// pef인덱스가 -1인 경우, 아이템 표시를 UI에서 모두 삭제후 그냥 종료.
		return;

	i3TDKChangeTex_Pef* currPef = m_pefList[m_curPefIdx];
	i3::vector<i3TDKChangeTex_RefCategory*>& refCategoryList = currPef->m_refCategoryList;


	const size_t num_cat = refCategoryList.size();

	for (size_t i = 0 ; i < num_cat ; ++i)
	{
		i3TDKChangeTex_RefCategory* cat = refCategoryList[i];

#ifdef _UNICODE
		i3::stack_wstring wstrItemName;		i3::mb_to_utf16( cat->m_pefRefCategoryKey->GetNameString(), wstrItemName);
		const wchar_t* szItemName = wstrItemName.c_str();
#else
		const char* szItemName = cat->m_pefRefCategoryKey->GetName(); 
#endif
		m_RefCategoryListCtrl->InsertItem(i, szItemName );
	}
		
}


void	i3TDKDlgChangeTex::UpdateRefi3sListCtrl()
{
	m_Refi3sListCtrl->DeleteAllItems();

	INT32 cat_sel = GetSelectedItemIndex(*m_RefCategoryListCtrl);

	if (cat_sel == -1)
		return;				// 클리어 이외의 일처리는 없다.
	
	I3ASSERT(m_curPefIdx != -1);
	I3ASSERT(m_curPefIdx < INT32(m_pefList.size()));
	i3TDKChangeTex_Pef* currPef = m_pefList[m_curPefIdx];
	i3::vector<i3TDKChangeTex_RefCategory*>& refCategoryList = currPef->m_refCategoryList;

	const i3TDKChangeTex_RefCategory* pCurrCat = refCategoryList[cat_sel];

	
	const size_t num_refi3s = pCurrCat->m_Refi3sList.size();

	for (size_t i = 0 ; i < num_refi3s ; ++i )
	{
		i3TDKChangeTex_Refi3s* p = pCurrCat->m_Refi3sList[i];
#ifdef _UNICODE
		i3::stack_wstring wstrItemName;		i3::mb_to_utf16( p->m_pefRefTexKey->GetNameString(), wstrItemName);
		const wchar_t* szItemName = wstrItemName.c_str();
#else
		const char* szItemName = p->m_pefRefTexKey->GetName(); 
#endif
		m_Refi3sListCtrl->InsertItem(i, szItemName );
	}
	
}

void	i3TDKDlgChangeTex::Process_Save_All()
{
	// IDC_TDK_BTN_SAVE_PEFPATH로 된 저장 버튼을 없애고, Save Pef에서 모든 것을 다 저장하도록 합니다..

	// 1. config를 저장...( 레지스트리에 pef 등록하는 것들)
	I3TDK_PROJECT_INFO info;

	g_pTDKEnvSet->getProject( g_pTDKEnvSet->getCurProject(), &info);			// 리턴값 활용 필요하면 그때 붙임..

	// 모든 pef 경로목록을 다 재갱신하여 저장하도록 한다...

	info.m_vecChangeTexPefPath.clear();

	const size_t num_pef = m_pefList.size();

	for (size_t i = 0 ; i < num_pef ; ++i )
	{
		i3TDKChangeTex_Pef* pef = m_pefList[i];
		info.m_vecChangeTexPefPath.push_back(pef->m_strRelPath);
	}

	// Update
	g_pTDKEnvSet->setProject( &info);
	g_pTDKEnvSet->Save();

	// 2. 모든 pef를 이제 저장합니다..

	const char* szWorkPath = g_pTDKEnvSet->getCurWorkDir();

	i3::stack_string full_path;

	const size_t numPef = m_pefList.size();

	for (size_t i = 0 ; i < numPef ; ++i)
	{
		i3TDKChangeTex_Pef* pef = m_pefList[i];

		full_path = szWorkPath;
		full_path += '/';
		full_path += pef->m_strRelPath;
		bool bChecked = ((CButton*)GetDlgItem(IDC_CHECK_CHANGETEX_SAVE_PEF))->GetCheck() ? true : false;
		SaveChangeTexPef(full_path.c_str(), pef->m_pefRootKey, bChecked);
	}

}


void i3TDKDlgChangeTex::ReplaceImageCtrl()
{
	CRect rtOrig, rtTarget;

	CWnd* pOrigStatic = GetDlgItem(IDC_TDK_ST_IMAGE_ORIG);
	CWnd* pTargetStatic = GetDlgItem(IDC_TDK_ST_IMAGE_TARGET);
	
	
	pOrigStatic->GetWindowRect( &rtOrig);
	pTargetStatic->GetWindowRect( &rtTarget);

	ScreenToClient( &rtOrig);
	ScreenToClient( &rtTarget);

	m_pOrigTexCtrl.SetWindowPos( nullptr, rtOrig.left, rtOrig.top, rtOrig.right - rtOrig.left, 
				rtOrig.bottom - rtOrig.top, SWP_NOACTIVATE | SWP_NOZORDER);
	m_pTargetTexCtrl.SetWindowPos( nullptr, rtTarget.left, rtTarget.top, rtTarget.right - rtTarget.left, 
				rtTarget.bottom - rtTarget.top, SWP_NOACTIVATE | SWP_NOZORDER);
}



void	i3TDKDlgChangeTex::ClearPropertiesValue()
{
	const int num_prop = m_wndPropList->GetPropertyCount();

	for (int i = 0; i < num_prop ; ++i)
	{
		CMFCPropertyGridProperty* prop = m_wndPropList->GetProperty(i);

		if (m_wndPropList->IsChangeTexFileProperty(prop))
		{
			ChangeTexFileProperty* prop_conc = static_cast<ChangeTexFileProperty*>(prop);
			prop_conc->SetTargetName(_T("") );
		}
		else
		{
			const int num_sub = prop->GetSubItemsCount();
			
			for (int j = 0 ; j < num_sub ; ++j)
			{
				ChangeTexFileProperty* prop_conc = static_cast<ChangeTexFileProperty*>(prop->GetSubItem(j));
				prop_conc->SetTargetName(_T("") );
			}
		}
	}
}

void	 i3TDKDlgChangeTex::Process_ResetSelection_i3sListCtrl()
{
	// 이 경우 오른쪽 텍스쳐를 비워야한다.
	i3TDKDlgChangeTex::ClearPropertiesValue();
	i3TDKDlgChangeTex::Process_EnableEditProperties(FALSE);		// 프로퍼티쪽 입력을 막는다...
	EnableLODSeperationCheck(false);
	
	m_pTargetTexCtrl.SetImage(nullptr);
	m_pTargetTexCtrl.InvalidateRect(nullptr);
}

namespace
{
	void PartitionChangeTexProperties( ChangeTexPropertyGridCtrl* gridCtrl, i3::vector<ChangeTexFileProperty*>& out1, i3::vector< i3::vector<ChangeTexFileProperty*> >& out2)
	{
		const int num_prop = gridCtrl->GetPropertyCount();
		
		for (int i = 0 ; i < num_prop ; ++i)
		{
			CMFCPropertyGridProperty* prop = gridCtrl->GetProperty(i);
			
			if (gridCtrl->IsChangeTexFileProperty(prop))
			{
				out1.push_back(static_cast<ChangeTexFileProperty*>(prop));
			}
			else
			{
				out2.resize(out2.size()+1);
				const int num_sub = prop->GetSubItemsCount();

				for (int j = 0 ; j < num_sub ; ++j )
				{
					i3::vector<ChangeTexFileProperty*>& vec_lod = out2.back();
					vec_lod.push_back( static_cast<ChangeTexFileProperty*>(prop->GetSubItem(j)));
				}
			}
		}
	}
	
	
}

void	i3TDKDlgChangeTex::Process_Selection_i3sListCtrl(int nIdx)
{
	if (nIdx == -1 )
	{

		bool bPrevCheck = this->IsCheckedLODSeperation();
		bool bCurrCheck = (m_MaxLODCount > 0);
		this->SetCheckLODSeperation( bCurrCheck );			// 여기서 강제 체킹 해줘야함..
		if (bPrevCheck != bCurrCheck)
			BuildProperties();

		i3TDKDlgChangeTex::Process_ResetSelection_i3sListCtrl();
		i3TDKDlgChangeTex::UpdateDlgTitle();

		return;
	}
	
//  모든 프로퍼티 조사해서 값처리..
	i3RegKey* tex_key = (i3RegKey*) m_Clonedi3sListCtrl.GetItemData(nIdx);

	if (tex_key == nullptr)
	{
	
		bool bPrevCheck = this->IsCheckedLODSeperation();
		bool bCurrCheck = (m_MaxLODCount > 0);
		this->SetCheckLODSeperation( bCurrCheck );			// 여기서 강제 체킹 해줘야함..
		if (bPrevCheck != bCurrCheck)
			BuildProperties();

		i3TDKDlgChangeTex::Process_ResetSelection_i3sListCtrl();
		i3TDKDlgChangeTex::UpdateDlgTitle();

		return;
	}

	const INT32 num_tex_reg = tex_key->GetDataCount();
	INT32 num_lod_key = tex_key->getChildCount();

	bool bPrevCheck = this->IsCheckedLODSeperation();
	bool bCurrCheck = (num_lod_key > 0);

	this->SetCheckLODSeperation( bCurrCheck );			// 여기서 강제 체킹 해줘야함..
	//
	if (bPrevCheck != bCurrCheck)
	{
		BuildProperties();
	}
	
	//
	i3TDKDlgChangeTex::ClearPropertiesValue();
	i3TDKDlgChangeTex::Process_EnableEditProperties(TRUE);
	//

		
	i3::vector< ChangeTexFileProperty*> vec_non_lod;
	i3::vector< i3::vector< ChangeTexFileProperty*> > vec_vec_lod;

	PartitionChangeTexProperties(m_wndPropList, vec_non_lod, vec_vec_lod);


	//
	// 구현 방법이 seperation 체크박스에 의해 분기 되어야 한다...
	// 일단, 값 셋팅이 하나라도 되있다면, 체크박스가 꺼지지 않도록 막아야한다..
	// 관련 셋팅이 비워져 있다면, 체크박스활성화가 가능하도록 조정한다..
	//
	

	i3::vector<i3RegString*> vecRegStringToRemove;

	const size_t num_non_lod    = vec_non_lod.size();
	
	for (INT32 i = 0; i < num_tex_reg ; ++i )
	{
		if ( i3RegString* pData = i3::same_cast<i3RegString*>(  const_cast<i3RegKey*>(tex_key)->GetData(i) ) )		
		{
			bool bFound = false;

			for (size_t j = 0 ; j < num_non_lod ; ++j)
			{
				ChangeTexFileProperty* prop_conc = vec_non_lod[j]; 

#ifdef _UNICODE
				i3::stack_string strProp;	i3::utf16_to_mb(prop_conc->GetName(), strProp);
				const char* prop_name = strProp.c_str();
#else
				const char* prop_name =	prop_conc->GetName();
#endif				
				if ( stricmp( pData->GetName(), prop_name ) == 0 )
				{
					prop_conc->SetTargetName( pData ); //		
					bFound = true;
					break;
				}
			}

			if (bFound == false)		// 프로퍼티에 없으면 자동삭제처리해준다..( 자동삭제는 Save에서 처리하기 어려우므로 실제 수정작업이 이뤄지는 것들만 처리해주도록 하자..)
			{
				vecRegStringToRemove.push_back(pData);
			}
		}
	}
	
	if (vecRegStringToRemove.empty() == false)
	{
		const size_t numRemove = vecRegStringToRemove.size();
		i3RegKey* parentKey = const_cast<i3RegKey*>(tex_key);
		for (size_t i = 0 ; i < numRemove ; ++i)
		{
			parentKey->RemoveData(vecRegStringToRemove[i]);			
		}

		vecRegStringToRemove.clear();
	}

	// LOD쪽 처리임..

	// prop와 키값 간 작은 것을 선택해서 표시..
	const INT32 num_child_view_lod = vec_vec_lod.size();			// 이건 ref메시로 선행 결정된것..

	
	// 여기서 강제생성 내지 강제 삭제가 이루어져야한다...

//	const INT32 num_child =	( (INT32)vec_vec_lod.size() < tex_key->getChildCount() ) ? (INT32)vec_vec_lod.size() : tex_key->getChildCount();

	if (num_child_view_lod < num_lod_key)
	{
		const INT32 diff = num_lod_key - num_child_view_lod;

		for (INT32 i = 0 ; i < diff ; ++i)
		{
			num_lod_key = tex_key->getChildCount();
			i3TreeNode* nodeToRemove = tex_key->getChild( num_lod_key - 1);
			tex_key->RemoveChild( nodeToRemove);
		}
	}
	else
	if (num_child_view_lod > num_lod_key )
	{
		const INT32 diff = num_child_view_lod - num_lod_key;
		i3::stack_string lod_name;

		for (INT32 i = 0 ; i < diff ; ++i)
		{
			i3RegKey* new_Key = i3RegKey::new_object_ref();
			num_lod_key = tex_key->getChildCount();
			i3::sprintf(lod_name, "LOD %d", num_lod_key);
			new_Key->SetName(lod_name);
			tex_key->AddChild(new_Key);
		}
	}
	
	for (INT32 i = 0 ; i < num_child_view_lod ; ++i)
	{
		i3RegKey* LODKey = static_cast<i3RegKey*>( tex_key->getChild(i));
		const INT32 num_lod_tex_reg = LODKey->GetDataCount();
		
		i3::vector<ChangeTexFileProperty*>& vec_lod = vec_vec_lod[i];

		for ( INT32 j = 0 ; j < num_lod_tex_reg ; ++j )
		{
			if ( i3RegString* pData = i3::same_cast<i3RegString*>(  const_cast<i3RegKey*>(LODKey)->GetData(j) ) )		
			{
				const size_t num_prop = vec_lod.size();
				bool bFound = false;
				for (size_t k = 0 ; k < num_prop ; ++k )
				{
					ChangeTexFileProperty* prop_conc = vec_lod[k]; 
#ifdef _UNICODE
					i3::stack_string strProp;	i3::utf16_to_mb(prop_conc->GetName(), strProp);
					const char* prop_name = strProp.c_str();
#else
					const char* prop_name =	prop_conc->GetName();
#endif				
					if ( stricmp( pData->GetName(), prop_name ) == 0 )
					{
						prop_conc->SetTargetName( pData ); //		
						bFound = true;
						break;
					}
				}

				if (bFound == false)		// 못찾으면 지우는쪽으로 예약.. 루프 도중에 삭제하는건 위험하므로, 모아놓고 다 돈 뒤에 포인터 찾아가며 삭제..
				{	
					vecRegStringToRemove.push_back(pData);
				}
			}
		}

		if (vecRegStringToRemove.empty() == false)
		{
			const size_t numRemove = vecRegStringToRemove.size();
			i3RegKey* parentKey = const_cast<i3RegKey*>(LODKey);
			for (size_t j = 0 ; j < numRemove ; ++j)
			{
				parentKey->RemoveData(vecRegStringToRemove[j]);			
			}

			vecRegStringToRemove.clear();
		}
	}
		
	m_wndPropList->AdjustLayout();
	m_wndPropList->RedrawWindow();
	
	i3TDKDlgChangeTex::UpdateDlgTitle();
}


void i3TDKDlgChangeTex::OnNMClickClonedi3sList(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	POSITION pos = m_Clonedi3sListCtrl.GetFirstSelectedItemPosition();
	INT32 idx = -1;
	if (pos != nullptr)
	{
		idx = m_Clonedi3sListCtrl.GetNextSelectedItem(pos);
	}
	
	i3TDKDlgChangeTex::Process_Selection_i3sListCtrl(idx);
	//
}


void i3TDKDlgChangeTex::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);


}

void i3TDKDlgChangeTex::OnBnClickedTdkBtnPefPath()
{
#ifdef _UNICODE
	i3::stack_wstring strWorkPath = g_pTDKEnvSet->getCurWorkDir();
#else
	i3::stack_string strWorkPath = g_pTDKEnvSet->getCurWorkDir();
#endif
	
	
	i3::stack_string strFullDir;

	I3ASSERT(m_curPefIdx < INT32(m_pefList.size()));

	if (m_curPefIdx != -1)
	{
		i3TDKChangeTex_Pef* currPef = m_pefList[m_curPefIdx];
		i3::stack_string strFullPath = strWorkPath;
		strFullPath += '/';
		strFullPath += currPef->m_strRelPath;
		i3::extract_directoryname(strFullPath, strFullDir);
	}
	else
	{
		strFullDir = strWorkPath;
	}
	
	i3::range::replace(strFullDir, '/', '\\');
	strFullDir += '\\';

	CFileDialog	Dlg( TRUE, _T("pef"), strFullDir.c_str(), OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,  
		_T("i3 Property Editor File(*.i3RegXML)|*.i3RegXML||"), this);
	

	Dlg.m_ofn.lpstrTitle = _T("Opening texture file PEF setting");
		

	if( Dlg.DoModal() != IDOK)	return;

	CString str_res = Dlg.GetPathName();

	// 이미 존재하는 워킹디렉토리가 있기 때문에 상대경로 기준으로 끊을수 있으면 끊는다..
	
	i3TDKDlgChangeTex::AddNewPefFile(str_res);

}


void i3TDKDlgChangeTex::OnDestroy()
{

	i3TDKDlgChangeTex::ClearClonedi3sListCtrl();

	// TODO: Add your message handler code here
	CDialog::OnDestroy();
}

//
// 컨텍스트 메뉴가 필요함...
//

void i3TDKDlgChangeTex::OnNMRClickClonedI3sList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);	pNMItemActivate;
	// TODO: Add your control notification handler code here

	CPoint CurrentPosition;
	CPoint CurrentLocalPosition;
    ::GetCursorPos(&CurrentPosition);

	CurrentLocalPosition = CurrentPosition;
	CListCtrl &ListCtrl = m_Clonedi3sListCtrl;
	ListCtrl.ScreenToClient(&CurrentLocalPosition);

    INT nIndex = -1;
    nIndex = ListCtrl.HitTest(CurrentLocalPosition);
	
	CMenu MenuTemp;
    CMenu *pContextMenu = nullptr;
    MenuTemp.LoadMenu(IDR_POPUP_CHANGETEX_EDIT_CLONED_I3S);
    pContextMenu = MenuTemp.GetSubMenu(0);
	
	if (-1 == nIndex)
    {
        // 아이템 영역이 아닌 곳에서 마우스 오른쪽 버튼을 선택한 경우 (삭제를 막음)
		pContextMenu->EnableMenuItem( ID_DELETE_CLONED_I3S, MF_DISABLED| MF_GRAYED);
    }
    else
    {
        // 아이템 영역에서 마우스 오른쪽 버튼을 선택한 경우(삽입/삭제 모두 켬)
		pContextMenu->EnableMenuItem( ID_DELETE_CLONED_I3S, MF_ENABLED);
    }

	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, CurrentPosition.x, CurrentPosition.y, this);



	*pResult = 0;
}

namespace
{
	INT32 GetFilePathAlreadyExistIndex( const CListCtrl& listCtrl, const TCHAR* szI3SFileName )
	{
		i3::stack_string	strSubDir;		i3::stack_string	strFilename;

#ifdef _UNICODE
		i3::stack_string strPath;	i3::utf16_to_mb(szI3SFileName, strPath);
		const char* szPath2 = strPath.c_str();
#else
		const char* szPath2 = szI3SFileName;
#endif
		SplitFolderAndFile(szPath2, strSubDir, strFilename);
		
		i3::stack_string& strItemName = strSubDir;
		strItemName += '/';
		strItemName += strFilename;

#ifdef _UNICODE				
		i3::stack_wstring wstrItemName;		i3::mb_to_utf16(strItemName, wstrItemName);
		const wchar_t* szItemName = wstrItemName.c_str();
#else
		const char* szItemName = strItemName.c_str();				
#endif
		INT32 numItem = listCtrl.GetItemCount();

		for (INT32 i = 0 ; i < numItem ; ++i)
		{
			CString listItemName = listCtrl.GetItemText(i, 0);	
			
			if ( i3::generic_is_iequal(listItemName, szItemName) )
			{
				return i;
			}
		}
		
		return -1;
	}
	
}

void	i3TDKDlgChangeTex::AddNewPefFile( const TCHAR* szPefFileName)
{
	// 구현 나중에!!!
	char full_path[MAX_PATH];
	::_fullpath(full_path, szPefFileName, MAX_PATH);
	
	const i3::literal_range work_dir = g_pTDKEnvSet->getCurWorkDir();
	
	i3::stack_string strRelPath;
	i3::make_relative_path(work_dir, full_path, strRelPath);
	i3::make_unix_path(strRelPath);

	// 여기서는 Register의 개념이고, 기존에 목록에 존재하는 파일이면 아무일도 하지 말고, 존재하지 않으면 새로 등록....파일박스쪽은 읽기 개념이므로 저장은 아니다..  
	bool   bPefExist = false;
	size_t numPef = m_pefList.size();

	for (size_t i = 0 ; i < numPef ; ++i)
	{
		if ( i3::generic_is_iequal(m_pefList[i]->m_strRelPath, strRelPath) )
		{
			bPefExist = true;			// 기존 존재시엔 특별이 처리할게 없음..
			break;
		}
	}
	
	if (bPefExist == true)
	{
		::AfxMessageBox("There is already a loaded PEF file.");
		return;
	}

	
	i3RegKey* pefRootKey = LoadChangeTexPef(full_path);
	if (pefRootKey == nullptr)
	{
		::AfxMessageBox("File structure doesn't match to a PEF file. File wasn't loaded.");
		return;
	}

	i3TDKChangeTex_Pef* old_sel_pef = nullptr;

	if (m_curPefIdx != -1)
		old_sel_pef = m_pefList[m_curPefIdx];

	i3TDKChangeTex_Pef* new_pef = new i3TDKChangeTex_Pef(m_pefList, strRelPath, pefRootKey);
	INT32 idx = i3::vu::int_index_of(m_pefList, new_pef);		// 그닥 좋진 않지만...
	m_ChangeTexPefListCtrl->InsertItem(idx, strRelPath.c_str());
	m_ChangeTexPefListCtrl->SetItemData(idx, DWORD_PTR(new_pef));

	if (old_sel_pef)
	{
		INT32 new_sel_idx = i3::vu::int_index_of(m_pefList, old_sel_pef);
		if (new_sel_idx != -1)
			m_curPefIdx = new_sel_idx;				// 삽입이나 삭제 후 셀렉션 인덱스가 바뀔 것 같으므로 그 변경에 대응..
	}

	i3TDKDlgChangeTex::InitRefCategoryi3sList(new_pef);
	pefRootKey->Release();

	i3TDKDlgChangeTex::Process_Selection_ChangeTexPefListCtrl(idx);
	
}



void	i3TDKDlgChangeTex::AddNewRefi3sItem( const TCHAR* szI3SFileName)
{
	if (m_curPefIdx == -1)
	{
		::AfxMessageBox( _T("The PEF file must be selected in the PEF list.")  );
		return;
	}

	I3ASSERT(m_curPefIdx < INT32(m_pefList.size()));
	i3TDKChangeTex_Pef* currPef = m_pefList[m_curPefIdx];

	i3::stack_string strFolder, strFilename;
	
	SplitFolderAndFile(szI3SFileName, strFolder, strFilename);

	bool bNeedUpdateRefi3sListCtrl = false;
	i3TDKChangeTex_RefCategory* pCat = FindOrCreateRefCategory(currPef, strFolder.c_str(), *m_RefCategoryListCtrl, bNeedUpdateRefi3sListCtrl);
	
	currPef->m_currSelectedCategoryIdx = GetSelectedItemIndex(*m_RefCategoryListCtrl);

	if (bNeedUpdateRefi3sListCtrl)
	{
		UpdateRefi3sListCtrl();
	}

	INT32 found_ref_index =	FindReferenceIndex(pCat, strFilename.c_str());

	if (found_ref_index != -1)
	{
		m_Refi3sListCtrl->SetItemState(found_ref_index, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED); 
		m_Refi3sListCtrl->EnsureVisible(found_ref_index, FALSE);
		i3TDKDlgChangeTex::Process_Selection_Refi3sListCtrl(found_ref_index);
		return;
	}
	
	i3TDKChangeTex_Refi3s* pRefi3s = CreateRefi3s(pCat, strFilename.c_str());

	INT32 idx_to_insert = i3::vu::int_index_of(pCat->m_Refi3sList, pRefi3s);
	I3ASSERT(idx_to_insert != -1);

#ifdef _UNICODE
	i3::stack_wstring wstrItemName;		i3::mb_to_utf16(pRefi3s->m_pefRefTexKey->GetNameString(), wstrItemName);
	const wchar_t* szItemName = wstrItemName.c_str();
#else
	const char* szItemName = pRefi3s->m_pefRefTexKey->GetName();
#endif
	
	m_Refi3sListCtrl->InsertItem(idx_to_insert, szItemName );
	m_Refi3sListCtrl->SetItemState(idx_to_insert, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED); 
	m_Refi3sListCtrl->EnsureVisible(idx_to_insert, FALSE);

	i3TDKDlgChangeTex::Process_Selection_Refi3sListCtrl(idx_to_insert);

}


BOOL i3TDKDlgChangeTex::OnCommand(WPARAM wParam, LPARAM lParam)
{


	// TODO: Add your specialized code here and/or call the base class
	WORD wEvent = HIWORD(wParam);	wEvent;
	WORD wID	= LOWORD(wParam);

	switch(wID)
	{
	case ID_ADD_CLONED_I3S:
		{

			I3ASSERT(m_curPefIdx != -1);
			I3ASSERT(m_curPefIdx < INT32(m_pefList.size()));
			i3TDKChangeTex_Pef* currPef = m_pefList[m_curPefIdx];
			i3::vector<i3TDKChangeTex_RefCategory*>& refCategoryList = currPef->m_refCategoryList;

			// 이 경우 리스트콘트롤에 에디트박스생성 및 입력포커스가 들어갈수 있도록 준비한다..
			INT32 idx_cat = GetSelectedItemIndex(*m_RefCategoryListCtrl);
			I3ASSERT(idx_cat != -1);

			INT32 idx_ref  = GetSelectedItemIndex(*m_Refi3sListCtrl);
			
			if (idx_ref == -1)			// 일단, 이런 경우는 수정 불가..
				break;

			INT32 idx_clone = GetSelectedItemIndex(m_Clonedi3sListCtrl);
		
			if (idx_clone == -1) 
				idx_clone = m_Clonedi3sListCtrl.GetItemCount();

			m_Clonedi3sListCtrl.InsertItem(idx_clone, _T("") );

			i3TDKChangeTex_RefCategory* cat = refCategoryList[idx_cat];
			i3TDKChangeTex_Refi3s* p = cat->m_Refi3sList[idx_ref];

			i3RegKey* new_key = i3RegKey::new_object_ref();
			p->m_pefRefTexKey->AddChild(new_key);
					
			m_Clonedi3sListCtrl.SetItemData(idx_clone, (DWORD_PTR)new_key);
			m_Clonedi3sListCtrl.EditLabel(idx_clone);
		
		}
	break;

	case ID_DELETE_CLONED_I3S:
		{
			INT32 idx_ref  = GetSelectedItemIndex(*m_Refi3sListCtrl);
			
			if (idx_ref == -1)			// 일단, 이런 경우는 수정 불가..
				break;

			INT32 idx_clone = GetSelectedItemIndex(m_Clonedi3sListCtrl);

			if (idx_clone == -1)
				break;
			
			i3RegKey* key = (i3RegKey*)m_Clonedi3sListCtrl.GetItemData(idx_clone);
			
			if ( key )
			{
				i3RegKey* parent = (i3RegKey*)key->getParent();
				parent->RemoveChild(key);
			}
			
			m_Clonedi3sListCtrl.DeleteItem(idx_clone);		// 목록만 지워서는 곤란하고, 디스플레이가
														// 반영되어야함..
			i3TDKDlgChangeTex::Process_Selection_i3sListCtrl(-1);											
		}

	break;
		case ID_ADD_REF_I3S:		// 편집을 통한 삽입이 아니고, 지정된 파일이름으로만..
		{
			i3TDKDlgChangeTex::AddNewRefi3sItem(m_filename_from_filedlg.c_str() );

			m_filename_from_filedlg.clear();			
		};
	break;
		case ID_DELETE_REF_I3S:
			{

				I3ASSERT(m_curPefIdx != -1);
				I3ASSERT(m_curPefIdx < INT32(m_pefList.size()));
				i3TDKChangeTex_Pef* currPef = m_pefList[m_curPefIdx];
				i3::vector<i3TDKChangeTex_RefCategory*>& refCategoryList = currPef->m_refCategoryList;


				INT32 idx_cat = GetSelectedItemIndex(*m_RefCategoryListCtrl);
				I3ASSERT(idx_cat != -1);

				INT32 idx_ref  = GetSelectedItemIndex(*m_Refi3sListCtrl);
				
				// 선택된 것 삭제후...유효하지않은 모든 출력들은 제거해야됨..
				if (idx_ref == -1)
					break;

				i3TDKChangeTex_RefCategory* curr_cat = refCategoryList[idx_cat];
				i3TDKChangeTex_Refi3s* refi3s_to_del = curr_cat->m_Refi3sList[idx_ref];
				
				i3RegKey* key_to_del = refi3s_to_del->m_pefRefTexKey;
				i3TreeNode* key_parent = key_to_del->getParent();
				key_parent->RemoveChild(key_to_del);
				
				curr_cat->m_currSelectedi3sIndex = -1;

				curr_cat->m_Refi3sList.erase( curr_cat->m_Refi3sList.begin() + idx_ref);
				delete refi3s_to_del;

				m_Refi3sListCtrl->DeleteItem(idx_ref);
				
				if (key_parent->getChildCount() == 0 )				// 폴더가 비는 경우엔, 폴더도 자동 삭제해둔다.
				{
					I3ASSERT( curr_cat->m_Refi3sList.empty() );

					currPef->m_pefRootKey->RemoveChild(key_parent);
					
					refCategoryList.erase( refCategoryList.begin() + idx_cat);
					delete curr_cat;

					m_RefCategoryListCtrl->DeleteItem(idx_cat);
					
					currPef->m_currSelectedCategoryIdx = -1;

					i3TDKDlgChangeTex::Process_Selection_Refi3sListCtrl(-1);
				}
				else
				if (m_ChangeTexCB)
				{
#ifdef _UNICODE
					i3::string filename_from_filedlg;		i3::utf16_to_mb(m_filename_from_filedlg, filename_from_filedlg);
#else
					const i3::rc_string& filename_from_filedlg = m_filename_from_filedlg;
#endif
					m_ChangeTexCB->OnUnselectedi3sFilename( filename_from_filedlg );	
				}
			}
	break;

		case ID_ADD_PEFFILE:			// 파일 박스 오픈으로 버튼쪽과 기능은 같게 한다..
			{
				i3TDKDlgChangeTex::OnBnClickedTdkBtnPefPath();
			}
		break;

		case ID_DELETE_PEFFILE:			// 해당 pef파일을 목록에서 삭제하고, m_pefList 요소도 삭제, 거의 대체로 셀렉션된 것이 삭제되었으므로 가능하다면, 현재 셀렉션 인덱스 수준에서 다음것, 아니면 그 이전 것을 선택하게 함..
			{
				INT32 del_idx = GetSelectedItemIndex(*m_ChangeTexPefListCtrl);
				
				if (del_idx == -1)
					break;
				
				i3TDKChangeTex_Pef* del_pef = m_pefList[del_idx];
				//  지우기 전에 셀렉션 이동 처리를 하는게 좋겠다..
				i3TDKChangeTex_Pef* sel_pef = nullptr;
				if (m_curPefIdx != -1)
					sel_pef = m_pefList[m_curPefIdx];
								
				m_ChangeTexPefListCtrl->DeleteItem(del_idx);		// 목록만 지워서는 곤란하고, 디스플레이 반영...
				m_pefList.erase( m_pefList.begin() + del_idx);	// pef 리스트에서 삭제... 아직 포인터는 들고 있다...
				

				if (sel_pef != nullptr)
				{
					INT32 found_sel_idx = i3::vu::int_index_of(m_pefList, sel_pef);
					
					if (found_sel_idx == -1)			// 셀렉션된 것이 지워진것... ( del_idx == m_curPefIdx 와 동일하긴 하다..)
					{
						INT32 next_sel_idx = m_curPefIdx;
						if ( next_sel_idx >= INT32(m_pefList.size()) )
						{
							if (m_pefList.size() > 0)
								next_sel_idx = INT32(m_pefList.size()) - 1;
							else
								next_sel_idx = -1;
						}
						
						if (next_sel_idx != -1)
							m_curPefIdx = -1;				// 로 놓아야...redundant filtering을 벗어날 것 같다..
						i3TDKDlgChangeTex::Process_Selection_ChangeTexPefListCtrl( next_sel_idx );
					}
					else		// 셀렉션 된 놈이 지워진게 아닌 경우라면, 이하 자식창은 갱신이 필요없을듯 한데...m_curPefIdx는 갱신되어야 할것 같다..
					{
						m_curPefIdx = found_sel_idx;
						m_ChangeTexPefListCtrl->SetItemState(m_curPefIdx, LVIS_FOCUSED|LVIS_SELECTED ,  LVIS_FOCUSED|LVIS_SELECTED);
					}
				}
				
				delete del_pef;						// 최종 포인터 삭제는 웬만한 것 처리가 끝난뒤에 하면 된다..
			}
		break;

		case ID_NEW_PEFFILE:	// 리스트 박스에 인서트를 하면서 EditBox를 켜야 하는 상태임...EditBox가 비어있으면 다시 없애야하기 때문에 구현이 약간 까다로울 것임..
			{
				INT32 insert_idx = GetSelectedItemIndex(*m_ChangeTexPefListCtrl);

				if (insert_idx == -1)
					insert_idx = m_ChangeTexPefListCtrl->GetItemCount();

				m_ChangeTexPefListCtrl->InsertItem(insert_idx, _T(""));	
			//	m_ChangeTexPefListCtrl->GetEditControl()->EnableWindow(TRUE);
				m_ChangeTexPefListCtrl->ModifyStyle( 0, LVS_EDITLABELS);
				m_ChangeTexPefListCtrl->EditLabel(insert_idx);				// 이 경우에만 유일하게 인서트 가능해야할 것 같다..
			}
		break;

	default:
	break;
	};
	return CDialog::OnCommand(wParam, lParam);
}

// 리스트 콘트롤 에딧처리가 끝났을때의 처리...SetText하기 전에 검증하는 단계가 필요할것 같다..

void i3TDKDlgChangeTex::OnLvnEndlabeleditClonedI3sList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here

	int nIdx = pDispInfo->item.iItem;	

	const TCHAR* szText = pDispInfo->item.pszText;
	
	size_t len = (szText) ? i3::generic_string_size(szText) : 0;		

	i3RegKey* tex_key = (i3RegKey*) m_Clonedi3sListCtrl.GetItemData(nIdx);
		
	const char* i3sName = tex_key->GetName();
		
	if (len == 0)										// 빈 문자열일 경우.....   기존에 있던 것이면 원래의 것으로 복구하고, 새 생성된건 삭제해야한다...(2개를 구별해야한다..)
	{
		if (i3sName == nullptr || i3sName[0] == 0 )		// 새 생성된 것이므로...삭제하고 종료..
		{
			i3RegKey* parent = (i3RegKey*)tex_key->getParent();
			parent->RemoveChild(tex_key);				// pair값 지우는것 잊지 말아야한다...
			m_Clonedi3sListCtrl.DeleteItem(nIdx);		// 이 경우 약간의 주의가 요구되는데, 셀렉션 변경 통지가 있는지 보고, 없으면 추가 작업 필요하다..
		}
		else
		{
#ifdef _UNICODE
			i3::stack_wstring wstri3sName;	i3::mb_to_utf16(i3sName, wstri3sName);
			const wchar_t* szi3sName = wstri3sName.c_str();
#else
			const char* szi3sName = i3sName;
#endif
			m_Clonedi3sListCtrl.SetItemText( nIdx, 0, szi3sName);
		}
	}
	else
	{
		// 일단 문자열이 유효하다면...셋팅해준다..셋팅시에 regKey값도 변경해준다..새 생성된 경우 셀렉션 노티가 가지않기때문에 신경 써야한다..
		// 셋팅 전에 자동소팅 고려해 처리해준다..
		tex_key->AddRef();
		i3RegKey* parent = (i3RegKey*)tex_key->getParent();
		parent->RemoveChild(tex_key);

		m_Clonedi3sListCtrl.DeleteItem(nIdx);

		bool new_tex_key = (i3sName == nullptr || i3sName[0] == 0 );

#ifdef _UNICODE	
		i3::stack_string strTexKeyName;	i3::utf16_to_mb(szText, strTexKeyName);
#else
		const char* strTexKeyName = szText;
#endif

		tex_key->SetName(strTexKeyName);		
		
		i3::vector<i3TreeNode*>& childList = parent->getChildList();

		i3::vector<i3TreeNode*>::iterator it = i3::lower_bound(childList.begin(), childList.end(), tex_key, SortFn_TreeNode());
		nIdx = i3::distance(childList.begin(), it);
		parent->InsertChild(nIdx, tex_key);
		tex_key->Release();
		
		m_Clonedi3sListCtrl.InsertItem(nIdx, szText);
		m_Clonedi3sListCtrl.SetItemData(nIdx, (DWORD_PTR)tex_key);
		m_Clonedi3sListCtrl.SetItemState(nIdx, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED); 
		m_Clonedi3sListCtrl.EnsureVisible(nIdx, FALSE);

		if (new_tex_key)			// 새 생성이며, 셀렉션 노티가 이 경우 가지 않았기 때문에 가게 해준다..
		{
			MakeOrDestroy_i3RegKey_LOD(m_Clonedi3sListCtrl, nIdx, m_MaxLODCount, true);  // 새 생성된건 LOD Sep를 적용한다.  // IsCheckedLODSeperation());

			i3TDKDlgChangeTex::Process_Selection_i3sListCtrl(nIdx);
		}

	}

	i3TDKDlgChangeTex::UpdateDlgTitle();

	*pResult = 0;

}


void i3TDKDlgChangeTex::OnNMClickRefI3sList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);	pNMItemActivate;

	// TODO: Add your control notification handler code here
	POSITION pos = m_Refi3sListCtrl->GetFirstSelectedItemPosition();
	INT32 idx = -1;
	if (pos != nullptr)
	{
		idx = m_Refi3sListCtrl->GetNextSelectedItem(pos);
	}
	
	i3TDKDlgChangeTex::Process_Selection_Refi3sListCtrl(idx);
	
	*pResult = 0;
}

// 즉각 반영되는게 편함..
void i3TDKDlgChangeTex::OnNMClickRefCategoryList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);	pNMItemActivate;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.


	INT32 sel_idx = GetSelectedItemIndex(*m_RefCategoryListCtrl);

	i3TDKDlgChangeTex::Process_Selection_RefCategoryListCtrl(sel_idx);
		
	*pResult = 0;
}


namespace
{
	
	void FindPropertyCurSelIndex( ChangeTexPropertyGridCtrl* GridCtrl, INT32& outLODIndex, INT32& outPropSelIndex )
	{
		CMFCPropertyGridProperty* prop_sel = GridCtrl->GetCurSel();
		
		if (prop_sel == nullptr)
		{
			outLODIndex = -1; outPropSelIndex = -1; return;
		}
		
		const int num_prop = GridCtrl->GetPropertyCount();
		int lod_count = 0;

		for ( int i = 0 ; i < num_prop ; ++i )
		{
			CMFCPropertyGridProperty* prop = GridCtrl->GetProperty(i);

			bool is_chage_texfile_prop = GridCtrl->IsChangeTexFileProperty(prop);

			if (is_chage_texfile_prop == false )
				++lod_count;

			if ( prop == prop_sel)
			{
				if (lod_count == 0)
				{
					outLODIndex = -1;	outPropSelIndex = i;	return;
				}
				else
				{
					outLODIndex = lod_count - 1;	outPropSelIndex = -1;	return;
				}
			}

			if (is_chage_texfile_prop == false)
			{
				const int num_sub = prop->GetSubItemsCount();

				for (int j = 0 ; j < num_sub ; ++j )
				{
					CMFCPropertyGridProperty* sub_prop = prop->GetSubItem(j);
					
					if (sub_prop == prop_sel)
					{
						outLODIndex = lod_count - 1;	outPropSelIndex = j;	return;
					}
				}
			}
						
		}

		outLODIndex = -1;		outPropSelIndex = -1;
	}

	CMFCPropertyGridProperty* FindLODProperty(ChangeTexPropertyGridCtrl* GridCtrl, INT32 LODIndex)
	{
		if (LODIndex == -1) 
			return nullptr;
		
		const int num_prop = GridCtrl->GetPropertyCount();
		int lod_count = 0;

		for (int i = 0 ; i < num_prop ; ++i)
		{
			CMFCPropertyGridProperty* prop = GridCtrl->GetProperty(i);
			
			if (!GridCtrl->IsChangeTexFileProperty(prop) )
			{
				if (LODIndex == lod_count)
					return prop;
				++lod_count;
			}
		}

		return nullptr;
	}

	bool SetCurSel_PropGridCtrl( ChangeTexPropertyGridCtrl* GridCtrl, INT32 LODIndex, INT32 PropSelIndex )
	{
		bool bResetNeed = false;

		if (LODIndex == -1)
		{
			if (PropSelIndex != -1 && PropSelIndex < GridCtrl->GetPropertyCount() )
			{
				CMFCPropertyGridProperty* prop = GridCtrl->GetProperty(PropSelIndex);
				GridCtrl->SetCurSel(prop);
			}
			else
			{
				GridCtrl->SetCurSel(nullptr);
				bResetNeed = true;
			}
		}
		else
		{
			CMFCPropertyGridProperty* prop_lod = FindLODProperty(GridCtrl, LODIndex);
			
			if (prop_lod == nullptr)
			{
				GridCtrl->SetCurSel(nullptr);
				bResetNeed = true;
			}
			else
			{
				if (PropSelIndex != -1 && PropSelIndex < prop_lod->GetSubItemsCount() )
				{
					CMFCPropertyGridProperty* prop = prop_lod->GetSubItem(PropSelIndex);
					GridCtrl->SetCurSel(prop);
				}
				else
				{
					GridCtrl->SetCurSel(prop_lod);
					bResetNeed = true;
				}
			}
		}

		return !bResetNeed;
	}

}

void	i3TDKDlgChangeTex::Process_Selection_RefCategoryListCtrl(int nIdx)
{

//	I3ASSERT(m_curPefIdx != -1);
	I3ASSERT(m_curPefIdx < INT32(m_pefList.size()));

	if ( m_curPefIdx != -1 )
	{
		i3TDKChangeTex_Pef* currPef = m_pefList[m_curPefIdx];
		i3::vector<i3TDKChangeTex_RefCategory*>& refCategoryList = currPef->m_refCategoryList;
		
		// 카테고리 셀렉션 변경시에....변경된 것 이전의 내부 상태값 몇개를 보관해둔다...
		if (nIdx >= INT32(refCategoryList.size()))		// nIdx값이 실제보다 크게 오는 경우.. 셀렉션 변경이 되지 않도록 처리한다...
		{
			return;
		}

		if (currPef->m_currSelectedCategoryIdx != nIdx)
		{
			if (currPef->m_currSelectedCategoryIdx != -1)
			{
				i3TDKChangeTex_RefCategory* old_cat = refCategoryList[currPef->m_currSelectedCategoryIdx];

				if (old_cat->m_currSelectedi3sIndex != -1)
				{
					i3TDKChangeTex_Refi3s* p = old_cat->m_Refi3sList[old_cat->m_currSelectedi3sIndex];
					INT32 idx_clone = GetSelectedItemIndex(m_Clonedi3sListCtrl);
					p->m_currCloneListSelIndex = idx_clone;
					FindPropertyCurSelIndex(m_wndPropList, p->m_currLODIndex, p->m_currPropertySelIndex);
				}
			}

			currPef->m_currSelectedCategoryIdx = nIdx;
		}
		
		if (nIdx != -1)
			m_RefCategoryListCtrl->SetItemState( nIdx, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	}

	UpdateRefi3sListCtrl();

	if (nIdx == -1)
	{
		if (m_ChangeTexCB)
		{
#ifdef _UNICODE
			i3::string filename_from_filedlg;		i3::utf16_to_mb(m_filename_from_filedlg, filename_from_filedlg);
#else
			const i3::rc_string& filename_from_filedlg = m_filename_from_filedlg;
#endif
			m_ChangeTexCB->OnUnselectedi3sFilename( filename_from_filedlg );		
		}

		i3TDKDlgChangeTex::UpdateDlgTitle();

		return;
	}

	if ( m_curPefIdx != -1 )
	{
		i3TDKChangeTex_Pef* currPef = m_pefList[m_curPefIdx];
		i3::vector<i3TDKChangeTex_RefCategory*>& refCategoryList = currPef->m_refCategoryList;
		i3TDKChangeTex_RefCategory* cat = refCategoryList[nIdx];

		if (cat->m_currSelectedi3sIndex != -1)
		{
			m_Refi3sListCtrl->SetItemState(cat->m_currSelectedi3sIndex, 
				LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED); 
			m_Refi3sListCtrl->EnsureVisible(cat->m_currSelectedi3sIndex, FALSE);
		}
	
		i3TDKDlgChangeTex::Process_Selection_Refi3sListCtrl(cat->m_currSelectedi3sIndex);
	}
	else
	{
		i3TDKDlgChangeTex::Process_Selection_Refi3sListCtrl(-1);
	}
}

void	i3TDKDlgChangeTex::Process_Selection_ChangeTexPefListCtrl(int nIdx)
{
	if (m_curPefIdx == nIdx)		// 변경 없음??
		return;


	if ( m_curPefIdx != -1 )		// 여기서는 이전 상태 pefidx...
	{
		i3TDKChangeTex_Pef* currPef = m_pefList[m_curPefIdx];
		i3::vector<i3TDKChangeTex_RefCategory*>& refCategoryList = currPef->m_refCategoryList;

		// 카테고리 셀렉션 변경시에....변경된 것 이전의 내부 상태값 몇개를 보관해둔다...

		if (currPef->m_currSelectedCategoryIdx != -1)
		{
			i3TDKChangeTex_RefCategory* old_cat = refCategoryList[currPef->m_currSelectedCategoryIdx];

			if (old_cat->m_currSelectedi3sIndex != -1)
			{
				i3TDKChangeTex_Refi3s* p = old_cat->m_Refi3sList[old_cat->m_currSelectedi3sIndex];
				INT32 idx_clone = GetSelectedItemIndex(m_Clonedi3sListCtrl);
				p->m_currCloneListSelIndex = idx_clone;
				FindPropertyCurSelIndex(m_wndPropList, p->m_currLODIndex, p->m_currPropertySelIndex);
			}
		}
	}


	m_curPefIdx = nIdx;

	i3TDKDlgChangeTex::UpdateRefCategoryListCtrl();
	
	if (m_curPefIdx != -1)
	{
		INT32 sel_idx = m_pefList[m_curPefIdx]->m_currSelectedCategoryIdx;
		i3TDKDlgChangeTex::Process_Selection_RefCategoryListCtrl( sel_idx );

		m_ChangeTexPefListCtrl->SetItemState(m_curPefIdx, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		m_ChangeTexPefListCtrl->EnsureVisible(m_curPefIdx, false);
	}
	else
	{
		i3TDKDlgChangeTex::Process_Selection_RefCategoryListCtrl(-1);
	}

	if (m_curPefIdx != -1)
		GetDlgItem(IDC_STATIC_CHANGETEX_PEFPATH)->SetWindowText(m_pefList[m_curPefIdx]->m_strRelPath.c_str());
	else
		GetDlgItem(IDC_STATIC_CHANGETEX_PEFPATH)->SetWindowText(_T("(empty)"));
}


void	i3TDKDlgChangeTex::Process_Selection_Refi3sListCtrl(int nIdx)
{
	INT32 cat_idx = GetSelectedItemIndex(*m_RefCategoryListCtrl);

	if (cat_idx == -1)		// 이경우 빈리스트콘트롤 선택된 것임..
		return;

	I3ASSERT(m_curPefIdx != -1);
	I3ASSERT(m_curPefIdx < INT32(m_pefList.size()));
	i3TDKChangeTex_Pef* currPef = m_pefList[m_curPefIdx];
	i3::vector<i3TDKChangeTex_RefCategory*>& refCategoryList = currPef->m_refCategoryList;


	i3TDKChangeTex_RefCategory* cat = refCategoryList[cat_idx];
	INT32 stored_ref_idx = cat->m_currSelectedi3sIndex;
	

	INT32 idx_clone = GetSelectedItemIndex(m_Clonedi3sListCtrl);
		
	if ( stored_ref_idx != nIdx && stored_ref_idx != -1)
	{
		i3TDKChangeTex_Refi3s* p = cat->m_Refi3sList[stored_ref_idx];

		p->m_currCloneListSelIndex = idx_clone;

		FindPropertyCurSelIndex(m_wndPropList, p->m_currLODIndex, p->m_currPropertySelIndex);
	}

	if (nIdx == -1)			// 미선택시 일단은 리턴으로 막고, 나중에 다르게 처리할것..
	{
		cat->m_currSelectedi3sIndex = -1;
		
		if (m_ChangeTexCB)
		{
#ifdef _UNICODE
			i3::string filename_from_filedlg;		i3::utf16_to_mb(m_filename_from_filedlg, filename_from_filedlg);
#else
			const i3::rc_string& filename_from_filedlg = m_filename_from_filedlg;
#endif
			m_ChangeTexCB->OnUnselectedi3sFilename( filename_from_filedlg );		
		}

		i3TDKDlgChangeTex::UpdateDlgTitle();

		return;
	}

	// 이 경우에는 거꾸로 뷰어쪽으로 정보를 보내야한다...(콜백클래스를 사용할것..)	
	
	if (m_ChangeTexCB)
	{	

		const i3::rc_string& folder_name = cat->m_pefRefCategoryKey->GetNameString();
		i3TDKChangeTex_Refi3s* p = cat->m_Refi3sList[nIdx];
		const i3::rc_string& ref_name = p->m_pefRefTexKey->GetNameString();
		
#ifdef _UNICODE
		i3::string strFileName; 	i3::utf16_to_mb(g_pTDKEnvSet->getCurWorkDir(), strFileName);
#else
		i3::string strFileName = g_pTDKEnvSet->getCurWorkDir();
#endif
		strFileName += '/';
		if (!folder_name.empty())
		{
			strFileName += folder_name;
			strFileName += '/';
		}
		strFileName += ref_name;

		bool bLoadedResult = m_ChangeTexCB->OnSelectedi3sFilename(strFileName);
		cat->m_currSelectedi3sIndex = nIdx; 

		if (bLoadedResult)		// 새로 뜬 경우에 셀렉션 반영해주고 디스플레이도 반영..
		{
		
			m_Clonedi3sListCtrl.SetItemState(p->m_currCloneListSelIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED); 
			i3TDKDlgChangeTex::Process_Selection_i3sListCtrl( p->m_currCloneListSelIndex);

			if ( SetCurSel_PropGridCtrl(m_wndPropList, p->m_currLODIndex, p->m_currPropertySelIndex) == false)
			{
				this->DisplayTex_Orig(nullptr);
				this->DisplayTex_Target(nullptr);
			}
		}
	}

	i3TDKDlgChangeTex::UpdateDlgTitle();

}

void i3TDKDlgChangeTex::OnBnClickedTdkBtnSavepef()
{
	// TODO: Add your control notification handler code here
	//bool bResult =	
	Process_Save_All();
	//
}

void i3TDKDlgChangeTex::OnNMRClickRefI3sList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);	pNMItemActivate;
	// TODO: Add your control notification handler code here

	CPoint CurrentPosition;
	CPoint CurrentLocalPosition;
    ::GetCursorPos(&CurrentPosition);

	CurrentLocalPosition = CurrentPosition;
	Refi3sListCtrl& ListCtrl = *m_Refi3sListCtrl;
	ListCtrl.ScreenToClient(&CurrentLocalPosition);

    INT nIndex = -1;
    nIndex = ListCtrl.HitTest(CurrentLocalPosition);
	
	CMenu MenuTemp;
    CMenu *pContextMenu = nullptr;
    MenuTemp.LoadMenu(IDR_POPUP_CHANGETEX_EDIT_REF_I3S);
    pContextMenu = MenuTemp.GetSubMenu(0);
	
	if (-1 == nIndex)
    {
        // 아이템 영역이 아닌 곳에서 마우스 오른쪽 버튼을 선택한 경우 (삭제를 막음)
		pContextMenu->EnableMenuItem( ID_DELETE_REF_I3S, MF_DISABLED| MF_GRAYED);
    }
    else
    {
        // 아이템 영역에서 마우스 오른쪽 버튼을 선택한 경우(삽입/삭제 모두 켬)
		pContextMenu->EnableMenuItem( ID_DELETE_REF_I3S, MF_ENABLED);
    }
	
	if ( m_filename_from_filedlg.empty() )
	{
		pContextMenu->DeleteMenu(ID_ADD_REF_I3S, MF_BYCOMMAND);
	}
	else
	{
#ifdef _UNICODE
		i3::stack_wstring strRelPath;
#else
		i3::stack_string strRelPath;
#endif

		i3::make_relative_path(g_pTDKEnvSet->getCurWorkDir(), m_filename_from_filedlg, strRelPath);
		i3::make_unix_path(strRelPath);

#ifdef _UNICODE
		i3::stack_wstring str_menu;
#else
		i3::stack_string str_menu;
#endif
		i3::sprintf(str_menu, _T("Insert %s"), strRelPath);

		pContextMenu->ModifyMenu(ID_ADD_REF_I3S, MF_BYCOMMAND, ID_ADD_REF_I3S, str_menu.c_str());
	}

	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, CurrentPosition.x, CurrentPosition.y, this);

	*pResult = 0;
}



// 마우스로 직접 클릭된 경우에만 들어올것으로 보인다..



void i3TDKDlgChangeTex::OnBnClickedCheckChangetexLodSeperation()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	
	BuildProperties();
//	UpdateAttrList();
	DisplayTex_Orig(nullptr);
	DisplayTex_Target(nullptr);
	
	INT32 cur_sel = GetSelectedItemIndex(m_Clonedi3sListCtrl);
	
	MakeOrDestroy_i3RegKey_LOD(m_Clonedi3sListCtrl, cur_sel, m_MaxLODCount, IsCheckedLODSeperation());

	Process_Selection_i3sListCtrl(cur_sel);
}

void	i3TDKDlgChangeTex::SetCheckLODSeperation(bool b)
{
	this->CheckDlgButton(IDC_CHECK_CHANGETEX_LOD_SEPERATION, (b) ? BST_CHECKED : BST_UNCHECKED);
}

void	i3TDKDlgChangeTex::EnableLODSeperationCheck(bool b)
{
	this->GetDlgItem(IDC_CHECK_CHANGETEX_LOD_SEPERATION)->EnableWindow(b);
}

bool	i3TDKDlgChangeTex::IsCheckedLODSeperation() const
{
	UINT res = this->IsDlgButtonChecked(IDC_CHECK_CHANGETEX_LOD_SEPERATION);
	return (res == BST_CHECKED);
}

bool	i3TDKDlgChangeTex::IsEnabledLODSeperationCheck() const
{
	return (this->GetDlgItem(IDC_CHECK_CHANGETEX_LOD_SEPERATION)->IsWindowEnabled() != FALSE);
}

bool	i3TDKDlgChangeTex::IsSafeModifyCheckLODSeperation() const
{
	INT32 sel_idx =	GetSelectedItemIndex(m_Clonedi3sListCtrl);

	if (sel_idx == -1) 
		return false;

	i3RegKey* key = (i3RegKey*)m_Clonedi3sListCtrl.GetItemData(sel_idx);
	bool is_checked_sep = IsCheckedLODSeperation();

	if (!is_checked_sep)
	{
		I3ASSERT(key->getChildCount() == 0);
		
		const INT32 numData = key->GetDataCount();				// 저장 데이터가 하나라도 있으면 체크박스를 켤수 없게 해야함..
		
		return numData == 0;				
	}
	else
	{
		I3ASSERT(key->getChildCount() == (INT32)m_MaxLODCount);
		
		if (key->GetDataCount() > 0) return false;

		const INT32 numChild = key->getChildCount();

		for (INT32 i = 0 ; i < numChild ; ++i)
		{
			i3RegKey* lod_key = (i3RegKey*)key->getChild(i);
			
			if (lod_key->GetDataCount() > 0) 
				return false;
		}
		return true;
	}
}

void i3TDKDlgChangeTex::OnBnClickedButtonOpenRefI3s()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CFileDialog dlg(TRUE, _T("Reference I3S File"), nullptr, 
		OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_LONGNAMES | OFN_PATHMUSTEXIST,
		_T("Reference I3S Files(*.i3s)|*.i3s||"), this);

	if (dlg.DoModal() != IDOK)
		return;
	
	AddNewRefi3sItem(dlg.GetPathName() );
		
}

void	i3TDKDlgChangeTex::UpdateDlgTitle()
{
	int ref_idx = GetSelectedItemIndex(*m_Refi3sListCtrl);
	int clone_idx = GetSelectedItemIndex(m_Clonedi3sListCtrl);



#ifdef _UNICODE
	i3::stack_wstring strTitle(L"Change Texture");
	i3::stack_wstring strTemp;
#else
	i3::stack_string strTitle("Change Texture");
	i3::stack_string strTemp;
#endif
	
	if (ref_idx != -1 )
	{
		CString strItem = m_Refi3sListCtrl->GetItemText(ref_idx, 0);
		i3::sprintf(strTemp, _T(" [ REF : %s ]"), (LPCTSTR)strItem );
		strTitle += strTemp;		
	}

	if (clone_idx != -1)
	{
		CString strItem = m_Clonedi3sListCtrl.GetItemText(clone_idx, 0);
		i3::sprintf(strTemp, _T(" [ CLONE : %s ]"), (LPCTSTR)strItem );
		strTitle += strTemp;
	}

	this->SetWindowText(strTitle.c_str());
}



void i3TDKDlgChangeTex::OnNMClickChangetexPefList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);	pNMItemActivate;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	INT32 sel_idx = GetSelectedItemIndex(*m_ChangeTexPefListCtrl);
	i3TDKDlgChangeTex::Process_Selection_ChangeTexPefListCtrl(sel_idx);
	*pResult = 0;
}

void i3TDKDlgChangeTex::OnNMRClickChangetexPefList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);	pNMItemActivate;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CPoint CurrentPosition;
	CPoint CurrentLocalPosition;
	::GetCursorPos(&CurrentPosition);

	CurrentLocalPosition = CurrentPosition;
	CListCtrl &ListCtrl = *m_ChangeTexPefListCtrl;
	ListCtrl.ScreenToClient(&CurrentLocalPosition);

	INT nIndex = -1;
	nIndex = ListCtrl.HitTest(CurrentLocalPosition);

	CMenu MenuTemp;
	CMenu *pContextMenu = nullptr;
	MenuTemp.LoadMenu(IDR_POPUP_CHANGETEX_EDIT_PEFLIST);
	pContextMenu = MenuTemp.GetSubMenu(0);

	if (-1 == nIndex)
	{
		// 아이템 영역이 아닌 곳에서 마우스 오른쪽 버튼을 선택한 경우 (삭제를 막음)
		pContextMenu->EnableMenuItem( ID_DELETE_PEFFILE, MF_DISABLED| MF_GRAYED);
	}
	else
	{
		// 아이템 영역에서 마우스 오른쪽 버튼을 선택한 경우(삽입/삭제 모두 켬)
		pContextMenu->EnableMenuItem( ID_DELETE_PEFFILE, MF_ENABLED);
	}

	pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, CurrentPosition.x, CurrentPosition.y, this);

	*pResult = 0;
}

void i3TDKDlgChangeTex::OnLvnEndlabeleditChangetexPefList(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);		pDispInfo;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int nIdx = pDispInfo->item.iItem;	
	const TCHAR* szText = pDispInfo->item.pszText;			// 귀중한 문자열 정보..( 파일 이름...)

	size_t len = (szText) ? i3::generic_string_size(szText) : 0;		

	if (len == 0)			// 입력된게 없으면 무조건 취소한다..
	{
		m_ChangeTexPefListCtrl->DeleteItem(nIdx);			// 이걸로 끝...
	}
	else		// 문자열 유효한 경우... 어떻게든 반영해야한다..
	{
		i3::stack_string strText = szText;
		i3::stack_string strRelPath;

		i3::make_relative_path( g_pTDKEnvSet->getCurWorkDir(), strText, strRelPath);
		i3::make_unix_path(strRelPath);
		
		i3::stack_string strExt;
		i3::extract_fileext(strRelPath, strExt);

		if ( !strExt.empty() )
			i3::change_file_ext(strRelPath, "pef");							// 최대한 pef 포맷을 맞추어본다..
		else
			strRelPath += ".i3RegXML";
		
		i3TDKChangeTex_Pef* old_sel_pef = nullptr;
		if (m_curPefIdx != -1)
			old_sel_pef = m_pefList[m_curPefIdx];

		i3RegKey* pefRootKey =  CreatePefRootKey();			// 없으면 일단 새것이라도 만든다..	// 무조건 존재해야함..

		i3TDKChangeTex_Pef* new_pef = new i3TDKChangeTex_Pef(m_pefList, strRelPath, pefRootKey);
		
		pefRootKey->Release();

		INT32 idx = i3::vu::int_index_of(m_pefList, new_pef);
		
		m_ChangeTexPefListCtrl->DeleteItem(nIdx);						// 이동 개념..기존 것 지우고..
		m_ChangeTexPefListCtrl->InsertItem(idx, strRelPath.c_str());	// 새곳에 채움..
		m_ChangeTexPefListCtrl->SetItemData(idx, DWORD_PTR(new_pef));	
		
		if (old_sel_pef)
		{
			INT32 new_sel_idx = i3::vu::int_index_of(m_pefList, old_sel_pef);
			if (new_sel_idx != -1)
				m_curPefIdx = new_sel_idx;				// 삽입이나 삭제 후 셀렉션 인덱스가 바뀔 것 같으므로 그 변경에 대응..
		}

		i3TDKDlgChangeTex::Process_Selection_ChangeTexPefListCtrl(idx);
	}
	
	m_ChangeTexPefListCtrl->ModifyStyle( LVS_EDITLABELS, 0);

	*pResult = 0;
}
