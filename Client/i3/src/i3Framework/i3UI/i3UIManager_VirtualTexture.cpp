#include "i3FrameworkPCH.h"
#include "i3UIManager.h"
#include "../i3Framework.h"

#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/remove_end_slash.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/compare/generic_compare.h"
#include "i3Base/itl/range/generic_empty.h"
#include "i3VirtualTextureSystem.h"

#include "i3VTFileFormat.h"
#include "i3VTFileFormatUtil.h"

#include "i3UIFilePathMgr.h"

static INT32	s_CacheCX = -1;
static INT32	s_CacheCY = -1;

void i3UIManager::SetVirtualTexture( i3VirtualTexture * pTexture, INT32 cache_cx, INT32 cache_cy)
{
	I3_REF_CHANGE( m_pVirtualTexture, pTexture);

	if( s_CacheCX != -1)
	{
		cache_cx = s_CacheCX;
		cache_cy = s_CacheCY;
	}
	else
	{
		s_CacheCX = cache_cx;
		s_CacheCY = cache_cy;
	}

	if( m_pVirtualTexture != nullptr && g_pUIRenderer != nullptr)
	{
		m_pVirtualTexture->PrepareForRendering( cache_cx, cache_cy, g_pUIRenderer->getLegacyModeState());
		I3ASSERT(g_pUIRenderer != nullptr);
		g_pUIRenderer->SetTexture(m_pVirtualTexture);
	}


}

bool i3UIManager::ResizeVRAMCache( INT32 cx, INT32 cy)
{
	if( m_pVirtualTexture != nullptr)
	{
		if( s_CacheCX != cx || s_CacheCY != cy)
		{
			m_pVirtualTexture->PrepareForRendering( cx, cy, g_pUIRenderer->getLegacyModeState());

			// 다시 로딩되게..
			g_pUIRenderer->OnResetVRAMCache();
		}
	}

	s_CacheCX = cx;
	s_CacheCY = cy;

	return true;
}

void i3UIManager::AdjustVirtualTexture(bool isNewFormat)
{
	if( m_pVirtualTexture != nullptr)
	{
		// 콘트롤러 변환 여부를 체크해서 변경해야함...
		i3UIFilePathMgr::i()->Enablei3VTexListFormat(isNewFormat);

		i3VTFileFormat* pFF = m_pVirtualTexture->GetFileFormat();

		if (pFF->IsControllerNewFormat() != isNewFormat)
		{
			pFF->ConvertController();
		}

		pFF->Defragment();
		pFF->Flush();
	}

	if( m_pUILibrary != nullptr)
	{
		m_pUILibrary->Defragment();
	}

	// Template가 변경된 것에 맞추어, 이미 생성되어 있던 모든 Control들의 Shape::Sprite 들을
	// 갱신해 주어야 한다.
	//_RefreshAllControls();	// 현재 정상작동 안하는 함수..정상작동해도, 내부 구현루틴이 안좋아서 안쓰는게 나을듯..

	i3UIManager::RefreshAllActiveTemplateForTool();
}

bool i3UIManager::OpenVirtualTexture( const char * pszname, INT32 cache_cx, INT32 cache_cy)
{
	// virtual texture는 동시에 둘 이상 존재할 수 없다.따라서 닫는다.
	CloseVirtualTexture();

	i3::stack_string strUniquePath;
	i3UIFilePathMgr::i()->ObtainUniquePath(pszname, strUniquePath);


	i3VirtualTexture * pTexture = i3VirtualTexture::new_object();
	I3ASSERT( pTexture != nullptr);

	bool bResult = false;

	// 결정 루틴이 아직 구현 안되어 있다. 일단은 구버전으로 구현하고, 얼른 호환성 있는 처리를 해야함...

	i3::rc_string strVTexPath = i3UIFilePathMgr::i()->Retrieve_Current_VTexDirectory(true);
	bool isVTexListFormat = i3UIFilePathMgr::i()->Isi3VTexListFormat();

	i3::stack_string strFull = strVTexPath;
	strFull += '/';
	strFull += strUniquePath;
	
	if (vt_util::IsVirtualTextureFileInHardDisk(strFull, isVTexListFormat))
	{
		bResult = pTexture->GetFileFormat()->Open(strFull.c_str());
	}
	else
	{
		// I3S를 읽어들이고 있는 현재 Path에서 다시 한번 검색한다.
		char szPath[MAX_PATH];
		i3::extract_directoryname(m_strCurrentScenePath, szPath);

		char conv[MAX_PATH];
		i3::extract_filename(strUniquePath, conv);

		if (szPath[0] != 0)
			i3::sprintf(strFull, "%s/%s", szPath, conv);
		else
			strFull = conv;
		
		if (vt_util::IsVirtualTextureFileInHardDisk(strFull, isVTexListFormat))
		{
			bResult = pTexture->GetFileFormat()->Open(strFull.c_str() );
		}
	}

	
	if( bResult == false)
	{
		I3PRINTLOG(I3LOG_FATAL, "Couldn't open Virtual Texture file: %s", strUniquePath);
		I3_SAFE_RELEASE( pTexture);
		return false;
	}

	SetVirtualTexture( pTexture, cache_cx, cache_cy);
	I3_MUST_RELEASE( pTexture);

	return true;
}

void i3UIManager::CloseVirtualTexture(void)
{
	/*
	i3VirtualTextureIO * pIOManager = i3VirtualTextureSystem::getIOManager();
	while (!pIOManager->IsEmpty())
	{
		Sleep(1);
	}
	*/

	if( g_pUIRenderer != nullptr)
		g_pUIRenderer->SetTexture( nullptr);
	
	I3_SAFE_RELEASE(m_pVirtualTexture);
}

void i3UIManager::MergeVirtualTexture( const char * pszName)
{
	i3VirtualTexture * pTexture = i3VirtualTexture::new_object_ref();
	I3ASSERT( pTexture != nullptr);

	bool bResult = false;

	i3::stack_string strUniquePath;
	i3UIFilePathMgr::i()->ObtainUniquePath(pszName, strUniquePath);

	i3::rc_string strVTexDir = i3UIFilePathMgr::i()->Retrieve_Current_VTexDirectory(true);

	i3::stack_string fullPath = strVTexDir;
	fullPath += '/';
	fullPath += strUniquePath;

	bool isVTexListFormat = i3UIFilePathMgr::i()->Isi3VTexListFormat();

	if (vt_util::IsVirtualTextureFileInHardDisk(fullPath, isVTexListFormat) )
	{
		bResult = pTexture->GetFileFormat()->Open(fullPath.c_str());
	}
	else
	{
		// I3S를 읽어들이고 있는 현재 Path에서 다시 한번 검색한다.
		char szPath[MAX_PATH];
	//	i3String::SplitPath( m_strCurrentScenePath.c_str(), szPath);
		i3::extract_directoryname(m_strCurrentScenePath, szPath);

		char conv[MAX_PATH];
		i3::extract_filename(pszName, conv);

		if( szPath[0] != 0)
			i3::sprintf(fullPath, "%s/%s", szPath, conv);
		else
			fullPath = conv;
		
		i3::remove_end_slash(fullPath);

//		if( i3System::IsFile( szFull))
		if (vt_util::IsVirtualTextureFileInHardDisk(fullPath, isVTexListFormat))
		{
			bResult = pTexture->GetFileFormat()->Open(fullPath.c_str());
		}
	}

	if( bResult)
	{
		i3::vector<I3VT_TEXINFO*> addTextureList;

		INT32 j;

		// 추가된 texture를 찾는다.
		for(INT32 i = 0; i < pTexture->getRefTextureCount(); i++)
		{
			I3VT_TEXINFO * pSrc = pTexture->getRefTexture( i);

			bool bSame = false;

			for( j = 0; j < m_pVirtualTexture->getRefTextureCount(); j++)
			{
				I3VT_TEXINFO * pDest = m_pVirtualTexture->getRefTexture( j);

				if( pSrc->m_CODE != pDest->m_CODE)
					continue;

				if( i3::generic_is_iequal( pSrc->m_strName, pDest->m_strName) )
				{
					bSame = true;
					break;
				}
			}

			if( !bSame)
			{
				addTextureList.push_back( pSrc);
			}
		}

		INT32 x, y;
		i3Framework * pFramework = i3Framework::getCurrentFramework();
		i3ResourceManager * pResManager = pFramework->GetResourceManager();

		for(size_t i = 0; i < addTextureList.size(); i++)
		{
			I3VT_TEXINFO * pAddTexInfo = addTextureList[ i];

			i3Texture * pAddTexture = pResManager->LoadTexture( pAddTexInfo->m_strName.c_str());
			m_pVirtualTexture->GetFileFormat()->PlaceTexture( pAddTexture, &x, &y);
		}
	}
}

