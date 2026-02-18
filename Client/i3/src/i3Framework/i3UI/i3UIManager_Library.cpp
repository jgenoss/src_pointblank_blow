#include "i3FrameworkPCH.h"
#include "i3UIManager.h"

#include "i3UILTreeNode_Template.h"

#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/extract_fileext.h"
#include "i3Base/string/ext/make_relative_path.h"
#include "i3Base/string/ext/remove_end_slash.h"

#include "i3Base/string/compare/generic_ncompare.h"
#include "i3Base/string/compare/generic_compare.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/extract_filetitle_fileext.h"

#include "i3UIFilePathMgr.h"
#include "i3VTFileFormatUtil.h"

#include "i3UILibHdr.h"

#include "i3Base/string/system/prepare_directory.h"
#include "i3Base/string/ext/remove_ext.h"
#include "i3Base/string/system/delete_directory.h"
#include "i3Base/string/system/is_directory.h"
#include "i3Base/string/algorithm/replace_all.h"

#include <winbase.h>
#include <shellapi.h>

bool i3UIManager::OpenTemplateLibrary_Depricated(const char * pszName)
{
	if( pszName == nullptr)
		return false;

	i3::stack_string strUniquePath;
	i3UIFilePathMgr::i()->ObtainUniquePath(pszName, strUniquePath);


	bool bResult = false;
		

//#if defined( I3_DEBUG)
	i3XMLFile file;

	char pszFileExt[MAX_PATH];

 	i3::extract_fileext(strUniquePath, pszFileExt);

	if (i3::generic_ncompare(pszFileExt, "i3Uil", MAX_PATH -1))			// 확장자가 i3UIL이 아니면 강제로 바꿈..대소문자 구별도 복원시켜야함.
	{
		i3::change_file_ext(strUniquePath, "i3Uil");
	}
	
	if( m_pUILibrary != nullptr)	// 이미 로드된 라이브러리가 있으면 해당 라이브러리 사용.
	{
		i3::stack_string already_exist_lib_name;
		i3UIFilePathMgr::i()->ObtainUniquePath(m_pUILibrary->GetNameString(), already_exist_lib_name);

		if (i3::generic_is_iequal(already_exist_lib_name, strUniquePath))
		{
			return true;
		}
		else
		{
			::MessageBox(NULL, "UI Library는 동시에 1개만 사용할 수 있습니다.", "오류", MB_OK);
			return false;
		}
	}

		
	//
	// 여기서 판별을 잘 해야됨... VirtualTexture폴더와 VTexList 폴더 양쪽이 모두 존재하면...최신날짜 uil파일을 선택하도록 함..
	//
	i3::stack_string finalFullPath;
	bool			isVTexListFormat = false;

	bool bExist = vt_util::SelectUilFileByDate_InVTexDirectory(strUniquePath, finalFullPath, isVTexListFormat);

	if (bExist == false) // 예정된 디렉토리에 파일이 없으면..UIE파일이 있는 폴더를 뒤져본다?
	{
		i3::stack_string strUIEPath;
		i3::extract_directoryname(m_strCurrentScenePath, strUIEPath);

		i3::stack_string strFileNameOnly;
		i3::extract_filename(pszName, strFileNameOnly);

		if (!strUIEPath.empty())
			strUIEPath += '/';

		strUIEPath += strFileNameOnly;

		finalFullPath = strUIEPath;
	}

	if (i3System::IsFile(finalFullPath.c_str()))
	{
		bResult = file.Load(finalFullPath.c_str());
	}

	
//////////////////////////////////////////////////////////////////////////////
// 아래 주석처리된 코드들은 UI Libray 파일을 Binary로 읽어드리는 코드
// 추후 추가 작업이 필요함
/*#else
	i3ResourceFile file;

	char conv[MAX_PATH];
	char szTemplateLibraryPath[MAX_PATH];
	char pszFileExt[MAX_PATH];

	i3::extract_fileext(pszName, pszFileExt);

	if (i3::generic_nicompare(pszFileExt, "i3Uil", MAX_PATH -1))
	{
		i3::change_file_ext_copy(szTemplateLibraryPath ,pszName, "i3Uil");
	}
	else
	{
		i3::safe_string_copy(szTemplateLibraryPath, pszName,MAX_PATH);
	}

	if( m_pUILibrary != nullptr)	// 이미 로드된 라이브러리가 있으면 해당 라이브러리 사용.
	{
		i3::make_relative_path( i3ResourceFile::GetWorkingFolder(), m_pUILibrary->GetNameString(), conv);

		INT32 diff = i3::generic_nicompare( conv, szTemplateLibraryPath, MAX_PATH -1);
		if( diff != 0)
		{
			::MessageBox(NULL, "UI Library는 동시에 1개만 사용할 수 있습니다.", "오류", MB_OK);
			return false;
		}
		else
		{
			//I3TRACE("Already Loaded\n");
			return true;
		}
	}

	if( pszBasePath[0] != 0)
		i3::snprintf( szFull, sizeof( szFull), "%s/%s", pszBasePath, szTemplateLibraryPath);
	else
		i3::string_ncopy_nullpad( szFull, szTemplateLibraryPath, sizeof( szFull));
#endif*/

//	i3::remove_end_slash(szFull);
	/*
	if( i3System::IsFile(strFull.c_str()))
	{
		bResult = file.Load( strFull.c_str());
	}
	else
	{
		// I3S를 읽어들이고 있는 현재 Path에서 다시 한번 검색한다.
		char szPath[MAX_PATH], szFileName[MAX_PATH];

		i3::extract_directoryname_filename(szTemplateLibraryPath, szPath, szFileName);
			
		if( i3::generic_string_size( szPath) == 0)
		{
			strFull = szFileName;
		}
		else
		{
			i3::sprintf(strFull, "%s/%s", szPath, szFileName);
		}

		if( i3System::IsFile(strFull.c_str()) )
			bResult = file.Load(strFull.c_str() );
	}
	*/

	if( bResult)
	{
		i3UIFilePathMgr::i()->Enablei3VTexListFormat(isVTexListFormat);

		I3TRACE( "Open UI Library file OK!\n");
		i3UILibrary * pUILib = (i3UILibrary*)file.getKeyObject();

		setUILibrary( pUILib);

		I3ASSERT( m_pVirtualTexture == nullptr);

		i3::stack_string fileNameOnly;
		i3::extract_filename(pUILib->getVTexPath(), fileNameOnly);

		OpenVirtualTexture(fileNameOnly.c_str(), 2048, 2048);			// 파일 이름만으로도 찾을수 있어야될 모양이다. 
		
		i3UIManager::RefreshAllActiveTemplateForTool();		// UILib 교환의 경우 이 함수 호출이 필요하다..

		return true;
	}

	return false;
}

// i3UILibHdr / Uil 파일 모두 한방에 로딩 가능한 인터페이스로 처리..
namespace 
{
	bool Load_EachTemplate_TplVersion_Imp(i3UILibrary* pNewUILib, i3UILTreeNode_Template* pTemplateNode, const i3::stack_string& strTplFolderPath)
	{
		i3XMLFile file;

		i3::stack_string fullPath = strTplFolderPath;
		fullPath += '/';
		fullPath += pTemplateNode->GetNameString();
		fullPath += ".i3UITemplate";
		
		bool bResult = file.Load(fullPath.c_str());

		if (bResult == false)
		{
			I3PRINTLOG(I3LOG_FATAL, "File must exist! : %s", fullPath);
			return false;
		}

		i3UITemplate* pTemplate = static_cast<i3UITemplate*>(file.getKeyObject());

		if (pTemplate == nullptr)
		{
			I3PRINTLOG(I3LOG_FATAL, "i3UITemplate must exist! : %s", fullPath);
			return false;
		}
		pTemplateNode->SetTemplate(pTemplate);

		pNewUILib->AddElement(pTemplate, true);

		return true;
	}

	void Load_Template_TplVersion_Rec_Imp(i3UILibrary* pNewUILib, i3UILTreeNode_Filter* pRootNode, const i3::stack_string& strTplFolderPath)
	{
		i3::vector<i3::rc_string> vecDir;	i3::vector<i3::rc_string> vecFilename;
		bool bIsPackFile;
		vt_util::CollectSubDirectoryAndFilename(strTplFolderPath, vecDir, vecFilename, bIsPackFile);


		const size_t numFile = vecFilename.size();

		i3::stack_string strFileTitle;

		for (size_t i = 0; i < numFile; ++i)
		{
			i3UILTreeNode_Template* pNewTemplateNode = i3UILTreeNode_Template::new_object();

			i3::extract_filetitle(vecFilename[i], strFileTitle);

			pNewTemplateNode->SetName(strFileTitle);

			if (Load_EachTemplate_TplVersion_Imp(pNewUILib, pNewTemplateNode, strTplFolderPath) == true)
				pRootNode->AddChild(pNewTemplateNode);

			pNewTemplateNode->Release();
		}

		i3::stack_string strFullPath;
		const size_t numDir = vecDir.size();
		for (size_t i = 0; i < numDir; ++i)
		{
			i3UILTreeNode_Filter* pNewFilterNode = i3UILTreeNode_Filter::new_object();
			pNewFilterNode->SetName(vecDir[i]);

			strFullPath = strTplFolderPath;
			strFullPath += '/';
			strFullPath += vecDir[i];

			Load_Template_TplVersion_Rec_Imp(pNewUILib, pNewFilterNode, strFullPath);

			pRootNode->AddChild(pNewFilterNode);
			pNewFilterNode->Release();
		}
	}

	i3UILibrary* LoadTemplateLibrary_i3UILibHdr(const i3::stack_string& finalFullPath, const i3::rc_string& strVTexDir)
	{

		i3UILibHdr* pHdr = nullptr;

		{
			i3XMLFile file;
			bool bResult = file.Load(finalFullPath.c_str());		// UILibHdr확장자파일 로딩..간단한 파일로 경로 정보 2개만 얻는다.

			if (bResult)
			{
				pHdr = static_cast<i3UILibHdr*>(file.getKeyObject());
				if (pHdr)
					pHdr->AddRef();
			}
		}
		
		if (pHdr == nullptr)
			return nullptr;

		i3UILibrary* pNewUILib = i3UILibrary::new_object();
		pNewUILib->ApplyUILibHdr(pHdr);
		pNewUILib->Create();
		
		I3_SAFE_RELEASE(pHdr);

		i3::stack_string fileTitle;
		i3::extract_filetitle(pNewUILib->GetName(), fileTitle);

		i3::stack_string strTplFolderPath = strVTexDir;
		strTplFolderPath += "/Template_";			// 이 이름을 고정으로!
		strTplFolderPath += fileTitle;

		i3UILTreeNode_Filter* pRootNode = pNewUILib->getNewLibRoot();

		Load_Template_TplVersion_Rec_Imp(pNewUILib, pRootNode, strTplFolderPath);
			
		return pNewUILib;
	}

	i3UILibrary* LoadTemplateLibrary_i3Uil(const i3::stack_string& finalFullPath)
	{
		i3XMLFile file;
		bool bResult = false;

		if (i3System::IsFile(finalFullPath.c_str()))
		{
			bResult = file.Load(finalFullPath.c_str());
		}

		if (bResult)
		{
			I3TRACE("Open UI Library file OK!\n");
			i3UILibrary * pUILib = (i3UILibrary*)file.getKeyObject();
			pUILib->AddRef();
			return pUILib;
		}
		return nullptr;
	}
}


bool	i3UIManager::LoadTemplateLibrary(const char* pszName)
{
	if (pszName == nullptr)
		return false;

	i3::stack_string strUniquePath;
	i3UIFilePathMgr::i()->ObtainUniquePath(pszName, strUniquePath);
	
	if (m_pUILibrary != nullptr)	// 이미 로드된 라이브러리가 있으면 해당 라이브러리 사용.
	{
		i3::stack_string strUniquePathName = strUniquePath;
		i3::remove_ext(strUniquePathName);

		i3::stack_string already_exist_lib_name;
		i3UIFilePathMgr::i()->ObtainUniquePath(m_pUILibrary->GetNameString(), already_exist_lib_name);
		i3::remove_ext(already_exist_lib_name);

		if (i3::generic_is_iequal(already_exist_lib_name, strUniquePathName))
		{
			return true;
		}
		else
		{
			::MessageBox(NULL, "UI Library는 동시에 1개만 사용할 수 있습니다.", "오류", MB_OK);
			return false;
		}
	}

	i3::stack_string finalFullPath;				// i3Uil, i3UILibHdr파일 및 VirtualTexture/VTexList 폴더 구분을 모두 처리해야함.
	bool			isVTexListFormat = false;
	bool			isi3UILibHdrFile = false;

	bool bExist = vt_util::SelectUILibraryFileByDate_InVTexDirectory(strUniquePath, finalFullPath, isi3UILibHdrFile, isVTexListFormat);

	if (bExist == false) // 예정된 디렉토리에 파일이 없으면..UIE파일이 있는 폴더를 뒤져본다?
	{
		i3::stack_string strUIEPath;
		i3::extract_directoryname(m_strCurrentScenePath, strUIEPath);

		i3::stack_string strFileNameOnly;
		i3::extract_filename(pszName, strFileNameOnly);

		if (!strUIEPath.empty())
			strUIEPath += '/';

		strUIEPath += strFileNameOnly;

		finalFullPath = strUIEPath;
	}

	i3UILibrary* pNewUILibrary = nullptr;

	if (isi3UILibHdrFile == true)
	{
		i3::rc_string strVTexDir;

		if (isVTexListFormat)
			strVTexDir = i3UIFilePathMgr::i()->Retrieve_VTexDirectory_i3VTexListFormat(true);
		else
			strVTexDir = i3UIFilePathMgr::i()->Retrieve_VTexDirectory_i3VTexFormat(true);
		
		pNewUILibrary = LoadTemplateLibrary_i3UILibHdr(finalFullPath, strVTexDir);
	}
	else
	{
		pNewUILibrary = LoadTemplateLibrary_i3Uil(finalFullPath);
	}

	if (pNewUILibrary)
	{
		i3UIFilePathMgr::i()->Enablei3VTexListFormat(isVTexListFormat);

		i3UIManager::setUILibrary(pNewUILibrary);

	
		I3ASSERT(i3UIManager::getVirtualTexture() == nullptr);

		i3::stack_string fileNameOnly;
		i3::extract_filename(pNewUILibrary->getVTexPath(), fileNameOnly);

		i3UIManager::OpenVirtualTexture(fileNameOnly.c_str(), 2048, 2048);			// 파일 이름만으로도 찾을수 있어야될 모양이다. 

		if ( pNewUILibrary->IsNewTemplateLibraryFormat() )
			pNewUILibrary->Defragment();							// 신규포맷은 파일에서 옵셋을 얻지 않으므로 옵셋을 재구성한다.

		i3UIManager::RefreshAllActiveTemplateForTool();		// UILib 교환의 경우 이 함수 호출이 필요하다..

		pNewUILibrary->Release();

		return true;
	}
	return false;
}

bool	i3UIManager::LoadToolTemplateLibrary(const char* pszName)
{
	i3UILibrary* pNewUILibrary = nullptr;
	i3::rc_string strVTexDir;

	strVTexDir = i3UIFilePathMgr::i()->Retrieve_VTexDirectory_i3VTexListFormat(true);
	pNewUILibrary = LoadTemplateLibrary_i3UILibHdr(pszName, strVTexDir);

	if (pNewUILibrary)
	{
		i3UIFilePathMgr::i()->Enablei3VTexListFormat(true);

		i3UIManager::setUILibrary(pNewUILibrary);


		I3ASSERT(i3UIManager::getVirtualTexture() == nullptr);

		i3::stack_string fileNameOnly;
		i3::extract_filename(pNewUILibrary->getVTexPath(), fileNameOnly);

		i3UIManager::OpenVirtualTexture(fileNameOnly.c_str(), 2048, 2048);			// 파일 이름만으로도 찾을수 있어야될 모양이다. 

		if (pNewUILibrary->IsNewTemplateLibraryFormat())
			pNewUILibrary->Defragment();							// 신규포맷은 파일에서 옵셋을 얻지 않으므로 옵셋을 재구성한다.

		i3UIManager::RefreshAllActiveTemplateForTool();		// UILib 교환의 경우 이 함수 호출이 필요하다..

		pNewUILibrary->Release();

		return true;
	}
	return false;
}

void i3UIManager::MergeTemplateLibrary( const char * pszName)
{
	if( pszName == nullptr)
		return;

	bool bResult = false;

	i3XMLFile file;

	i3::stack_string strUniquePath;
	i3UIFilePathMgr::i()->ObtainUniquePath(pszName, strUniquePath);

	i3::rc_string strVTexDir = i3UIFilePathMgr::i()->Retrieve_Current_VTexDirectory(true);

	i3::stack_string fullPath = strVTexDir;
	fullPath += '/';
	fullPath += strUniquePath;
	
	if( i3System::IsFile(fullPath.c_str()))
	{
		bResult = file.Load(fullPath.c_str());
	}
	else
	{
		// I3S를 읽어들이고 있는 현재 Path에서 다시 한번 검색한다.

		i3::stack_string strUIEDir;
		i3::extract_directoryname(m_strCurrentScenePath, strUIEDir);

		i3::stack_string fileNameOnly;
		i3::extract_filename(pszName, fileNameOnly);

		fullPath = strUIEDir;
		if (!fullPath.empty())
			fullPath += '/';

		fullPath += fileNameOnly;

		if( i3System::IsFile(fullPath.c_str()))
			bResult = file.Load(fullPath.c_str());
	}

	if( bResult)
	{
		i3UILibrary * pUILib = (i3UILibrary*)file.getKeyObject();

		// 기존 Library에 추가

		// 같지 않은 것은 찾는다.
		INT32 j;
		i3::vector<i3UITemplate*> addTemplateList;

		for(INT32 i = 0; i < pUILib->getElementCount(); i++)
		{
			i3UITemplate * pSrc = pUILib->getElement( i);

			bool bSame = false;

			for( j = 0; j < m_pUILibrary->getElementCount(); j++)
			{
				i3UITemplate * pDest = m_pUILibrary->getElement( j);

				if( pSrc->meta() != pDest->meta())				// GetClassMeta는 스태틱함수라서, 이런 비교스킵이 안된다.(2012.08.08.수빈)
					continue;											// ->meta로 변경.
					
				if( i3::generic_is_iequal( pSrc->GetNameString(), pDest->GetNameString()) )
				{
					// 같다면 다음 것을 비교
					bSame = true;
					break;
				}
			}

			if( !bSame)
			{	// 다르군요..
				addTemplateList.push_back( pSrc);
			}
		}

		for( size_t i = 0; i < addTemplateList.size(); i++)
		{
			i3UITemplate * pAddTemplate = addTemplateList[ i];
			m_pUILibrary->AddElement( pAddTemplate);

			i3UILTreeNode_Template* pTNode = i3UILTreeNode_Template::new_object();

			pTNode->SetTemplate( pAddTemplate);
			pTNode->SetName( pAddTemplate->GetNameString());

			m_pUILibrary->getNewLibRoot()->AddChild( pTNode);

			pTNode->Release();
		}

		{
			// Virtaul Texture를 새로 만들어야 한다.
			// Texture 내용을 확인 하고 RefTexture를 맞춰서 다시 Build한다.
			MergeVirtualTexture( pUILib->getVTexPath());
		}

		// Texture Coord를 계산
		for(INT32 i = 0; i < m_pUILibrary->getElementCount(); i++)
		{
			i3UITemplate * pTemplate = m_pUILibrary->getElement( i);

			for( j = 0; j < pTemplate->getShapeCount(); j++)
			{
				i3UIShape * pShape = pTemplate->getShape( j);

				I3VT_TEXINFO * pTexInfo = m_pVirtualTexture->FindRefTexture( pShape->getTextureResPath());
				if(pTexInfo != nullptr)
				{
					pShape->setTexcoordOffset( pTexInfo->m_X, pTexInfo->m_Y);
				}
			}
		}

		i3UIManager::RefreshAllActiveTemplateForTool();
	}
}

void i3UIManager::CloseTemplateLibrary()
{
	if (m_pUILibrary != nullptr)
	{
		I3_SAFE_RELEASE(m_pUILibrary);
	}
}

//
// 일단은 m_pUILibrary의 이름 확보가 안되면 처리가 안되도록 처리하자..
// 기존 것은 엄청 간단하게 프로세싱됨.. (i3UILibrary포인터 넣고 곧바로 세이브하면 됨)
//
void	i3UIManager::SaveTemplateLibrary_UilVersion(const i3::literal_range& filename)
{
	if (m_pUILibrary == nullptr)
		return;

	if (filename.empty())
		return;

	i3UILibrary * pUILib = m_pUILibrary;
	
	i3XMLFile File;

	i3::stack_string strUniquePath;
	i3UIFilePathMgr::i()->ObtainUniquePath(filename, strUniquePath);

	i3::change_file_ext(strUniquePath, "i3Uil");			// 확장자 대소문자 구별을 복원시킴..
	
	pUILib->SetName(strUniquePath);
	pUILib->SelectNewTemplateLibraryFormat(false);

	i3::rc_string currVTexDir = i3UIFilePathMgr::i()->Retrieve_Current_VTexDirectory(true);
	i3::stack_string strFullPath = currVTexDir;
	strFullPath += '/';
	strFullPath += strUniquePath;

	File.setKeyObject(pUILib);
	if (File.Save(strFullPath.c_str()))
	{
		I3TRACE("Template Library Saved\n");
#if defined( I3_DEBUG)
		pUILib->setEdited(false);
#endif
	}
	else
	{
		I3PRINTLOG(I3LOG_WARN, "Couldn't save Template Library to %s\n", strFullPath);
	}

}

// 신규버전의 경우 더이상 i3UILibrary의 OnLoad / OnLoadXml이 통과되지 않아야 하며...
// 무조건, i3UITemplate외에는 루트로 삼으면 안됨.. 또한 i3UILTreeNode_Template,Filter 역시 저장되어선 안된다.
//

namespace
{
	void Save_EachTemplate_TplVersion_Imp(i3UILTreeNode_Template* pTemplateNode, const i3::stack_string& strTplFolderPath)
	{
		i3XMLFile File;

		i3UITemplate* pUITemplate =	pTemplateNode->getTemplate();

		I3ASSERT(pUITemplate != nullptr);

		File.setKeyObject(pUITemplate);		// 이걸로 충분? .. 그리고, Name이 곧 fileName...
		
		const i3::rc_string& tplName =	pUITemplate->GetNameString();

		i3::stack_string fullPath = strTplFolderPath;
		fullPath += '/';
		fullPath += tplName;
		fullPath += ".i3UITemplate";

		if (File.Save(fullPath.c_str()))
		{
			I3TRACE("Template file Saved : %s\n", fullPath);
		}
		else
		{
			I3PRINTLOG(I3LOG_WARN, "Couldn't save Template file : %s\n", fullPath);
		}
	}
	
	void Save_Template_TplVersion_Rec_Imp(i3UILTreeNode_Filter* pRootNode, const i3::stack_string& strTplFolderPath)
	{
		bool bMakeDirOK = i3::prepare_directory(strTplFolderPath);			
		I3ASSERT(bMakeDirOK);

		const INT32 num_child = pRootNode->getChildCount();
			
		for (INT32 i = 0; i < num_child; ++i)
		{
			i3TreeNode* pChildNode = pRootNode->getChild(i);

			if (i3::same_of<i3UILTreeNode_Filter*>(pChildNode))
			{
				i3UILTreeNode_Filter* pChildFilter = static_cast<i3UILTreeNode_Filter*>(pChildNode);
				const i3::rc_string& folder_name = pChildFilter->GetNameString();

				i3::stack_string strChildFolderPath = strTplFolderPath;
				strChildFolderPath += '/';
				strChildFolderPath += folder_name;

				Save_Template_TplVersion_Rec_Imp(pChildFilter, strChildFolderPath);
			}
			else
			if (i3::same_of<i3UILTreeNode_Template*>(pChildNode))
			{
				i3UILTreeNode_Template* pChildTemplateNode = static_cast<i3UILTreeNode_Template*>(pChildNode);
				Save_EachTemplate_TplVersion_Imp(pChildTemplateNode, strTplFolderPath);
			}
			else
			{
				I3PRINTLOG(I3LOG_FATAL, "Wrong i3TreeNode in i3UILTreeNode_Filter : %s", pChildNode->GetNameString());
			}
		}
	}
}


void	i3UIManager::SaveTemplateLibrary_TplVersion(const i3::literal_range& filename)
{
	if (m_pUILibrary == nullptr)
		return;

	if (filename.empty())
		return;

	i3UILibrary * pUILib = m_pUILibrary;

	pUILib->SelectNewTemplateLibraryFormat(true);

	i3::stack_string strUniquePath;

	{
		i3UIFilePathMgr::i()->ObtainUniquePath(pUILib->GetName(), strUniquePath);

		i3::stack_string strExt;
		i3::extract_fileext(strUniquePath, strExt);

		if (strExt.empty())
		{
			strUniquePath += ".i3UILibHdr";
		}
		else
		{
			i3::change_file_ext(strUniquePath, "i3UILibHdr");
		}
	}
	
	i3::stack_string fileTitle;
	i3::extract_filetitle(strUniquePath, fileTitle);

	i3::rc_string currVTexDir = i3UIFilePathMgr::i()->Retrieve_Current_VTexDirectory(true);
	i3::stack_string strTplFolderPath = currVTexDir;
	strTplFolderPath += "/Template_";			// 이 이름을 고정으로!
	strTplFolderPath += fileTitle;
		
	if (i3::is_directory(strTplFolderPath))
	{
		i3::stack_string strTplFolderPathToOrig = strTplFolderPath;
		i3::stack_string strTplFolderPathToDelete = strTplFolderPath;
		strTplFolderPathToDelete += "_ToDelete";

		i3::replace_all(strTplFolderPathToOrig, "/", "\\");
		i3::replace_all(strTplFolderPathToDelete, "/", "\\");

		strTplFolderPathToOrig += '\0';			// 널문자 2개가 있어야 처리된다..
		strTplFolderPathToDelete += '\0';		// 널문자 2개가 있어야 처리된다..

		SHFILEOPSTRUCTA fo = { 0 };

		fo.wFunc = FO_RENAME;
		fo.pFrom = strTplFolderPathToOrig.c_str();
		fo.pTo = strTplFolderPathToDelete.c_str();
		fo.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
		::SHFileOperation(&fo);			// 서브디렉토리가 파일 탐색기에 걸려있는 경우 , 
										// MoveFile함수로의 Rename이 실패하기 때문에 이 API로 변경..
		
		// 서브폴더를 다 지우고, 저장하도록 한다!!
		strTplFolderPathToDelete.resize(strTplFolderPathToDelete.size() - 1);

		i3::delete_directory(strTplFolderPathToDelete);
	}

	i3UILTreeNode_Filter* pRootNode = pUILib->getNewLibRoot();

	Save_Template_TplVersion_Rec_Imp(pRootNode, strTplFolderPath);
	
	pUILib->SetName(strUniquePath);

	i3UILibHdr* pHdr = pUILib->CreateUILibHdr();

	{
		i3XMLFile File;
		File.setKeyObject(pHdr);

		i3::stack_string strFullPath = currVTexDir;
		strFullPath += '/';
		strFullPath += pUILib->GetNameString();

		if (File.Save(strFullPath.c_str()))
		{
			I3TRACE("UILibHdr File Saved\n");
#if defined( I3_DEBUG)
			pUILib->setEdited(false);
#endif
		}
	}

	pHdr->Release();
}
