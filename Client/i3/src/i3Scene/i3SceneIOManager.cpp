#include "i3SceneDef.h"
#include "i3SceneIOManager.h"
#include "i3SceneGraphInfo.h"
#include "i3SceneIOThread.h"

#include "i3RenderAttr.h"
#include "i3TextureBindAttr.h"
#include "i3SceneUtil.h"

#include "i3Base/string/ext/make_unix_path.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/generic_string_size.h"

I3_CLASS_INSTANCE( i3SceneIOManager);

static i3SceneIOManager *	s_pSceneIOManager = nullptr;

i3SceneIOManager::i3SceneIOManager(void)
{
	s_pSceneIOManager = this;
	
	m_CommonResList.reserve( 64);
	m_ResList.reserve( 32);
}

i3SceneIOManager::~i3SceneIOManager(void)
{
	Destroy();

	s_pSceneIOManager = nullptr;
}


void i3SceneIOManager::Destroy()		
{
	DestroyResource( true);

	I3_SAFE_RELEASE( m_pThread);
	I3_SAFE_RELEASE( m_pQueue);
}

void i3SceneIOManager::DestroyResource( bool bDeleteCommon)
{
	UINT32 i;
	m_bEnabled = false;

	// Que된 ResFile도 처리해야한다.
	I3ASSERT( m_pQueue != nullptr);
	m_pQueue->FlushTicket();

	if( bDeleteCommon)
	{
		for( i = 0; i < m_CommonResList.size(); i++)
		{
			i3SceneGraphInfo * pInfo = m_CommonResList.at(i);
			I3_SAFE_RELEASE( pInfo);
		}
		m_CommonResList.clear();
	}
	
	for( i = 0; i < m_ResList.size(); i++)
	{
		RESINFO * pInfo = m_ResList[i];
		I3ASSERT( pInfo->_pSGInfo != nullptr);

		I3ASSERT( pInfo->_pSGInfo->GetRefCount() == 1);
		I3_SAFE_RELEASE( pInfo->_pSGInfo);
		i3::static_pool_free( pInfo);
	}

	m_ResList.clear();
}

bool i3SceneIOManager::Create( void)
{
	// Queue
	{
		m_pQueue = i3SceneIOQueue::new_object();
		m_pQueue->Create();
	}

	// Thread
	{
		m_pThread = i3SceneIOThread::new_object();

		m_pThread->SetQueue( m_pQueue);

		if (m_pThread->Create("SceneIOThread") == false)
			return false;
	}

	m_ResList.reserve( 128);

	return true;
}

INT32 i3SceneIOManager::Request( const char * pszPath, bool bLoadedDX, bool bCommonRes)
{
	return m_pQueue->PushIO( pszPath, bLoadedDX, bCommonRes);
}

LOADSTATE i3SceneIOManager::getLoadState( INT32 hndl)
{
	return m_pQueue->getLoadState( hndl);
}

void i3SceneIOManager::AddRes( i3SceneGraphInfo * pSgInfo, UINT32 time)
{
	if( m_bCommonRes)
	{
		m_CommonResList.push_back( pSgInfo);
	}
	else
	{
		RESINFO * pInfo = static_cast<RESINFO*>(i3::static_pool_malloc( sizeof( RESINFO)));
		pInfo->_pSGInfo = pSgInfo;
		pInfo->_UsageCount = 1;
		pInfo->_UsageFrameID = time;

		m_ResList.push_back( pInfo);
	}
}

void i3SceneIOManager::TrashRes( bool bDeleteTexture)
{
	i3Monitor2 mon( &m_Mutex);

	// 가장 예전에 Loading되었던 Scene을 삭제한다.
	for( UINT32 i = 0; i < m_ResList.size(); i++)
	{
		RESINFO * pInfo = m_ResList[i];
		I3ASSERT( pInfo->_pSGInfo != nullptr);

		if( pInfo->_pSGInfo->GetRefCount() == 1 )
		{
			//I3TRACE( "Delete sgInfo %s\n", pInfo->_pSGInfo->GetName());

			// extern 텍스쳐 삭제
			if( bDeleteTexture )
			{
				i3::vector<i3RenderAttr*> list;
				i3Scene::FindAttrByType( pInfo->_pSGInfo->getInstanceSg(), i3TextureBindAttr::static_meta(), list);
				for( UINT32 j = 0; j < list.size(); j++)
				{
					i3TextureBindAttr * pAttr = static_cast<i3TextureBindAttr*>(list.at(j));
					if( pAttr->GetTexture() != nullptr &&
						pAttr->GetTexture()->GetRefCount() == 3)
					{
						// 메니져에만 있는 것 삭제!!
						i3NamedResourceManager * pManager = i3ResourceFile::GetResourceManager( i3Texture::static_meta());
						for( UINT32 i = 0; i < pManager->GetResourceCount(); i++)
						{
							if( pAttr->GetTexture() == pManager->GetResource( i) )
							{
								I3_MUST_RELEASE(pAttr->GetTexture());	// TextureResourceManager에서 AddRef 한것을 줄인다.
								pManager->DeleteResource( i);	// NamedResourceManager에서 AddRef 한것을 줄인다.
								pAttr->SetTexture(nullptr);
							}
						}
					}
				}
			}

			I3_SAFE_RELEASE( pInfo->_pSGInfo);
			i3::static_pool_free( pInfo);

			m_ResList.erase(m_ResList.begin() + i);
			i--;
		}
	}
}

INT32 i3SceneIOManager::_FindLoadedScene( i3SceneGraphInfo * pInfo)
{
	INT32 i;

	for( i = 0; i < (INT32) m_ResList.size(); i++)
	{
		RESINFO * pSrcInfo = m_ResList[i];
		I3ASSERT( pSrcInfo->_pSGInfo != nullptr);

		if( pSrcInfo->_pSGInfo->getChecksum() == pInfo->getChecksum())
			return i;
	}

	return -1;
}

INT32 i3SceneIOManager::_FindLoadedSceneFromCommon( i3SceneGraphInfo * pInfo)
{
	// find from common list
	for( INT32 i = 0; i < (INT32) m_CommonResList.size(); i++)
	{
		i3SceneGraphInfo * pSrc = m_CommonResList.at(i);
		if( pSrc->getChecksum() == pInfo->getChecksum())
			return i;
	}

	return -1;
}

i3SceneGraphInfo *	i3SceneIOManager::PopLoadedScene( INT32 hndl, UINT32 time)
{
	i3Monitor2 mon( &m_Mutex);

	i3SceneGraphInfo * pSgInfo = m_pQueue->PopLoadedScene( hndl);
	I3ASSERT( pSgInfo != nullptr);

	bool bCommon = true;
	INT32 findIdx = _FindLoadedSceneFromCommon( pSgInfo);

	if( findIdx == -1)
	{
		findIdx = _FindLoadedScene( pSgInfo);
		bCommon = false;
	}

	if( findIdx == -1)
		AddRes( pSgInfo, time);
	else
	{
		// 읽은 파일은 삭제
		m_pQueue->ReleaseSceneGraphInfo( pSgInfo);
		
		// 이전에 읽어 놓은 버퍼를 반환
		if( bCommon)
		{
			pSgInfo = m_CommonResList[ findIdx];
		}
		else
		{
			RESINFO * pInfo = m_ResList[ findIdx];
			pInfo->_UsageFrameID = time;
			pInfo->_UsageCount++;
			pSgInfo = pInfo->_pSGInfo;
		}
	}

	I3_SAFE_ADDREF( pSgInfo);

	return pSgInfo;
}

i3SceneGraphInfo *	i3SceneIOManager::GetLoadedScene( const char * pszPath)
{
	i3Monitor2 mon( &m_Mutex);

	UINT128 code;
	char conv[ MAX_PATH];

	i3::safe_string_copy( conv, pszPath, _countof( conv));
	i3::make_unix_path( conv);
	i3::to_upper( conv);

	i3MD5::Gen( conv, i3::generic_string_size( conv), (char *) &code);

	for( UINT32 i = 0; i < m_CommonResList.size(); i++)
	{
		i3SceneGraphInfo * pSgInfo = m_CommonResList[i];
		UINT128 pSgCode = pSgInfo->getChecksum();
		if( pSgCode == code)
		{
			I3_MUST_ADDREF(pSgInfo);
			return pSgInfo;
		}
	}

	for( UINT32 i = 0; i < m_ResList.size(); i++)
	{
		RESINFO * pInfo = m_ResList[ i];
		I3ASSERT( pInfo->_pSGInfo != nullptr);

		UINT128 pSgCode = pInfo->_pSGInfo->getChecksum();

		if( pSgCode == code)
		{
			I3_MUST_ADDREF(pInfo->_pSGInfo);
			return pInfo->_pSGInfo;
		}
	}

	m_bEnabled = true;

	return nullptr;
}

i3SceneIOManager *	i3SceneIOManager::getInstance(void)
{
	return s_pSceneIOManager;
}
