#include "i3GfxType.h"
#include "i3TextureResManager.h"
#include "i3Texture.h"
#include "i3ImageFile.h"
#include "i3Base/string/ext/change_file_ext.h"
#include "i3Base/string/ext/extract_filename.h"
#include "i3Base/string/ext/extract_directoryname.h"
#include "i3Base/string/ext/extract_fileext.h"

I3_CLASS_INSTANCE( i3TextureResManager);

i3TextureResManager::i3TextureResManager(void)
{
	m_CPUCount = i3CPUGetCount();

	if( m_CPUCount > 1)
	{
		m_pBGLoader = i3ImageFileBGLoader::new_object();
		m_pBGLoader->Create( "ImageBGLoader");
	}
}

i3TextureResManager::~i3TextureResManager(void)
{
	while( m_SplitLoadQue.size() > 0)
	{
		tagSPLITTEX_LOADINFO * pInfo = m_SplitLoadQue.front();
		pInfo->_pTempTex = nullptr;
		I3_SAFE_RELEASE( pInfo->_pLoadableTex);
		I3MEM_SAFE_FREE( pInfo);
		m_SplitLoadQue.pop();
	}

	I3_SAFE_RELEASE( m_pBGLoader);
}

i3Texture *	 i3TextureResManager::_TryToLoad( const char * pszPath, bool bBackgroundLoad)
{
	i3ImageFile file;
	i3Texture * pTex = nullptr;
	char szExt[128], szFull[ MAX_PATH];

	if( i3FileStream::isFile( (char *)pszPath))
	{
		// 듀얼core 이상에서 background loading을 하자
		pTex = file.Load( pszPath, bBackgroundLoad);
		I3_SAFE_ADDREF( pTex);
	}
	else 
	{
		// 확장자가 I3I가 아닌 경우, I3I로 대체해 검사해본다.
	//	i3String::SplitFileExt( pszPath, szExt, sizeof(szExt) - 1);
		i3::extract_fileext( pszPath, szExt );

		if( i3::generic_is_iequal( szExt, "I3I") == false)
		{
			i3::safe_string_copy( szFull, pszPath, MAX_PATH);
		//	i3String::SwapExt( szFull, MAX_PATH, szFull, "I3I");
			i3::change_file_ext(szFull, "I3I");

			if( i3FileStream::isFile( szFull))
			{
				pTex = file.Load( szFull, bBackgroundLoad);
				I3_SAFE_ADDREF( pTex);
			}
		}
	}

	return pTex;
}

i3ResourceObject*	i3TextureResManager::GetResource( const char * pszResFile, const char * pszBasePath,
													 const char *szName, bool bBackgroundLoad )
{
	i3Texture * pTex;
	char szFull[ MAX_PATH];
	i3ImageFile file;

	pTex = (i3Texture *) i3NamedResourceManager::GetResource( pszResFile, pszBasePath, szName);
	if( pTex != nullptr)
		return pTex;

	if( pszBasePath != nullptr && pszBasePath[0] != 0)
		sprintf( szFull, "%s/%s", pszBasePath, szName);
	else
		strncpy( szFull, szName, sizeof( szFull)-1);
	
	pTex = _TryToLoad( szFull, bBackgroundLoad && (m_CPUCount>1));
	if( pTex == nullptr)
	{
		// I3S를 읽어들이고 있는 현재 Path에서 다시 한번 검색한다.
		char szPath[MAX_PATH], szFileName[MAX_PATH];

		i3::extract_directoryname(pszResFile, szPath);
		i3::extract_filename( szName, szFileName);

		if( i3::generic_string_size( szPath) == 0)
		{
			sprintf( szFull, "%s", szFileName);
		}
		else
		{
			sprintf( szFull, "%s/%s", szPath, szFileName);
		}

		pTex = _TryToLoad( szFull, bBackgroundLoad && (m_CPUCount>1));

		if( pTex != nullptr)
		{
			pTex->SetName( szFull);
		}
	}

	if( pTex != nullptr)
	{
		//I3TRACE( "Load Texture : %s\n", szFull);

		AddResource( pTex, szName);
		
		I3_MUST_RELEASE(pTex);

		/////////////////////////////////////////////////////////////////////////////
		// Texture 와 Sound의 Extern Res인경우
		// OnLoad가 종료된 시점에서 Intern Res와 RefCount가 서로 다르게됨
		// ( Extern : 2( File Load : 1, AddResource : 1 ),  Intern : 0 )
		// AddedResource완료 시점에서 RefCount가 1이되도록함
		{
			I3ASSERT( pTex->GetRefCount() == 2);
			I3_MUST_RELEASE(pTex);
		}
	}

	return pTex;
}

UINT32				i3TextureResManager::SaveResource( const char * pszBasePath, i3ResourceObject * pObj)
{
	i3Texture * pTex = (i3Texture *) pObj;
	char szFull[ MAX_PATH];
	i3ImageFile file;
	UINT32 Rc;

	I3ASSERT( pObj != nullptr);
	I3ASSERT( pObj->hasName());

	if( pszBasePath[0] != 0)
		i3::snprintf( szFull, MAX_PATH, "%s/%s", pszBasePath, pObj->GetNameString());
	else
		i3::safe_string_copy( szFull, pObj->GetNameString(), MAX_PATH);

	Rc = file.Save( szFull, pTex);

	return Rc;
}

void i3TextureResManager::UpdateSplitLoadQue( void)
{
	if( m_pBGLoader != nullptr)
	{
		if( !m_SplitLoadQue.empty())
		{
			if( m_pBGLoader->IsEmpty() )
			{
				tagSPLITTEX_LOADINFO * pInfo = m_SplitLoadQue.front();
				m_pBGLoader->RequestLoad( pInfo);

				pInfo->_pTempTex = nullptr;
				I3_SAFE_RELEASE( pInfo->_pLoadableTex);
				I3MEM_SAFE_FREE( pInfo);
				m_SplitLoadQue.pop();
			}
		}
	}
}

void i3TextureResManager::RequestSplitLoad( i3Texture * pTempTex, UINT16 w, UINT16 h, UINT16 lv, I3G_IMAGE_FORMAT format, UINT32 usage)
{
	I3ASSERT( m_pBGLoader != nullptr);

	tagSPLITTEX_LOADINFO * pNewInfo = (tagSPLITTEX_LOADINFO*) i3MemAlloc( sizeof(tagSPLITTEX_LOADINFO));
	pNewInfo->_pTempTex = pTempTex;
	pTempTex->SetPreload( true);

	pNewInfo->_pLoadableTex = i3Texture::new_object();
#if defined( I3_DEBUG)
	pNewInfo->_pLoadableTex->SetName( "SplitLoadTex");
#endif

	m_SplitLoadQue.push( pNewInfo);
}

